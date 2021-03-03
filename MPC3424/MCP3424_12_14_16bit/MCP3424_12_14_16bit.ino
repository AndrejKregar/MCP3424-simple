/*
 * 12/16/16 bit example
 * see MCP3424 datasheet for information on paramters and setting register bits
 * (c) Andrej Kregar
*/

#include <Wire.h>

#define MCP3424_naslov 0x68

#define MCP3424_1x     0x00              // 1 krat ojačanje
#define MCP3424_2x     0x01              // 2 krat
#define MCP3424_4x     0x02              // 4 krat
#define MCP3424_8x     0x03              // 8 krat

#define MCP3424_12bit  0x00              // 240SPS hitrost/ločljivost
#define MCP3424_14bit  0x01              // 60 SPS
#define MCP3424_16bit  0x02              // 15 SPS

#define kanal_1        0x00              // kanal
#define kanal_2        0x01
#define kanal_3        0x02
#define kanal_4        0x03

byte config_byte;
byte sr_byte;
byte sp_byte;

int32_t rezultat;

uint8_t ADCconfig;

int32_t test_sp = pow(2, 15);
int32_t test_zg = pow(2, 16);            // združujemo 2×8=16 bitov



void setup()
{
  Serial.begin(112500);
  Wire.begin();

  ADCconfig  &= 0;                                 // pobrišemo vse
  delay(1);
  ADCconfig  |= 0x01 << 7 | 0x00 << 5 | 0x00 << 4 | MCP3424_12bit << 2 | MCP3424_1x;     // in vpišemo novo konfiguracijo
  //            start       kanal       mode        ločljivost           ojačanje
  delay(1);
}


void loop()
{
  beri(kanal_2);                                   // beremo 2. kanal, vrne rezultat

  Serial.print("CH2 ");
  Serial.print(rezultat);
  Serial.println();
}



// funkcija za branje
int32_t beri(int8_t kanal)
{
  ADCconfig |= kanal << 5;                         // izberemo kanal
  nastavi(ADCconfig);

  config_byte |= 1 << 7;                           // sprožimo meritev

  do                                               // dokler se ne konča zadnja pretvorba
  {
    Wire.requestFrom(MCP3424_naslov, 3);           // beremo oba byta z rezultati in configbyt
    if (Wire.available() == 3)
    {
      while (Wire.available())
      {
        sr_byte = Wire.read();
        sp_byte = Wire.read();
        config_byte = Wire.read();                 // preverimo konec
      }
      rezultat = (sr_byte << 8) + sp_byte;         // spojimo oba byta
      if (rezultat >= test_sp)                     // da obvlada negativne vrednosti
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
  while ((config_byte & (1 << 7)));                // dokler ne preberemo vsega
}

void nastavi(uint8_t konfiguracija)                // vpišemo konfuguracijo
{
  Wire.beginTransmission(MCP3424_naslov);
  Wire.write(konfiguracija);
  Wire.endTransmission();
}
