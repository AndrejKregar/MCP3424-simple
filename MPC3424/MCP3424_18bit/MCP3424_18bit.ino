/*
 * 18 bit example
 * see MCP3424 datasheet for information on paramters and setting register bits
 * (c) Andrej Kregar
*/

#include <Wire.h>

#define MCP3424_naslov 0x68

#define MCP3424_1x     0x00              // 1 krat
#define MCP3424_2x     0x01              // 2 krat
#define MCP3424_4x     0x02              // 4 krat
#define MCP3424_8x     0x03              // 8 krat

#define MCP3424_18bit  0x03              // 18 bit/3,75 sps   DELA SMO PRI TEJ RESOLUCIJI

#define kanal_1        0x00
#define kanal_2        0x01
#define kanal_3        0x02
#define kanal_4        0x03

byte config_byte;
byte zg_byte;
byte sr_byte;
byte sp_byte;

int32_t rezultat;

uint8_t ADCconfig;

uint32_t test_sp = pow(2, 23);
uint32_t test_zg = pow(2, 24);           // združujemo 3×8=24 bitov




void setup()
{
  Serial.begin(115200);
  Wire.begin();

  ADCconfig  &= 0;                                  // pobrišemo vse
  delay(1);
  ADCconfig  |= 0x01 << 7 | 0x00 << 5 | 0x00 << 4 | MCP3424_18bit << 2 | MCP3424_1x;     // in vpišemo novo konfiguracijo
  //            start       kanal       mode        ločljivost           ojačanje
  delay(1);
}


void loop()
{
  beri(kanal_2);                                    // beremo 2. kanal, vrne rezultat

  Serial.print("CH2 ");
  Serial.print(rezultat);
  Serial.println();
}



// funkcija za branje
int32_t beri(int8_t kanal)
{
  ADCconfig |= kanal << 5;                          // izberemo kanal
  nastavi(ADCconfig);

  //  config_byte = 0x00;                           // izbrišemo konfiguracijo
  config_byte |= 1 << 7;                            // sprožimo meritev

  do                                                // dokler se ne konča zadnja pretvorba
  {
    Wire.requestFrom(MCP3424_naslov, 4);            // beremo tri byte z rezultati in configbyt
    if (Wire.available() == 4)
    {
      while (Wire.available())
      {
        zg_byte = Wire.read();
        sr_byte = Wire.read();
        sp_byte = Wire.read();
        config_byte = Wire.read();                  // preverimo konec
      }
      rezultat = (zg_byte << 16) + (sr_byte << 8) + sp_byte;         // spojimo vse tri byte
      if (rezultat >= test_sp)                      // da obvlada negativne vrednosti
      {
        return rezultat = rezultat - test_zg;
      }
      else   return rezultat;
    }
    else
    {
      return false;
      Serial.println("NAPAKA");
    }
  }
  while ((config_byte & (1 << 7)));                 // dokler ne preberemo vsega
}

void nastavi(uint8_t konfiguracija)                 // vpišemo konfuguracijo
{
  Wire.beginTransmission(MCP3424_naslov);
  Wire.write(konfiguracija);
  Wire.endTransmission();
}
