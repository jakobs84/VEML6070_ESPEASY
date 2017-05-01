//#######################################################################################################
//#################################### Plugin-122: LuxRead   ############################################
//#######################################################################################################

#define PLUGIN_034
#define PLUGIN_ID_034         34
#define PLUGIN_NAME_034       "VEML6070"
#define PLUGIN_VALUENAME1_034 "UV"

#define VEML6070_ADDRESS    0x38
boolean Plugin_034_init = false;

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>

//Integration Time
#define VEML6070_IT_1_2 0x0 //1/2T
#define VEML6070_IT_1   0x1 //1T
#define VEML6070_IT_2   0x2 //2T
#define VEML6070_IT_4   0x3 //4T

/*

  Source https://github.com/watterott/VEML6070-Breakout

  Example sketch for VEML6070-Breakout (Digital UV Light Sensor).
  Rset=270k on breakout, UVA sensitivity: 5.625 uW/cm²/step
  Board           I2C/TWI Pins
                  SDA, SCL
  ----------------------------
  Uno, Ethernet    A4, A5
  Mega             20, 21
  Leonardo          2,  3
  Due              20, 21
  Integration Times and UVA Sensitivity:
    Rset=240k -> 1T=100.0ms ->   5.000 uW/cm²/step
    Rset=270k -> 1T=112.5ms ->   5.625 uW/cm²/step
    Rset=300k -> 1T=125.0ms ->   6.250 uW/cm²/step
    Rset=600k -> 1T=250.0ms ->  12.500 uW/cm²/step
*/


boolean Plugin_034(byte function, struct EventStruct *event, String& string)
  {
  boolean success=false;

  switch(function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_034;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_034);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_034));
        break;
      }


  case PLUGIN_READ:
    {
      if (!Plugin_034_init)
        {

          Wire.begin();

          Wire.beginTransmission(VEML6070_ADDRESS);
          Wire.write((VEML6070_IT_1<<2) | 0x02);
          Wire.endTransmission();
          delay(500);

          Plugin_034_init = true;
        }
        if(Plugin_034_init)
        {

          byte msb=0, lsb=0;
          uint16_t uv;

          Wire.requestFrom(VEML6070_ADDRESS+1, 1); //MSB
          delay(1);
          if(Wire.available())
            msb = Wire.read();

          Wire.requestFrom(VEML6070_ADDRESS+0, 1); //LSB
          delay(1);
          if(Wire.available())
            lsb = Wire.read();

          uv = (msb<<8) | lsb;
        //Serial.println(uv, DEC); //output in steps (16bit)
        UserVar[event->BaseVarIndex] = hexToDec(String(uv));
        String log = F("UV   : VEML6070: ");
        log += UserVar[event->BaseVarIndex];
        addLog(LOG_LEVEL_INFO,log);
        success=true;
        }
      break;
    }
  }
  return success;
}

unsigned int hexToDec(String hexString) {

  // Source https://github.com/benrugg/Arduino-Hex-Decimal-Conversion/blob/master/hex_dec.ino

  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++) {

    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);

    decValue = (decValue * 16) + nextInt;
  }

  return decValue;
}
