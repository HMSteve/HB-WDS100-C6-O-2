//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2018-11-02 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef __SENS_VEML6030_h__
#define __SENS_VEML6030_h__

#include <Sensors.h>
#include <Wire.h>
#include <SparkFun_VEML6030_Ambient_Light_Sensor.h>

#define GAIN 0.125
#define INTEGTIME 100

namespace as {


template <uint8_t ADDRESS=0x48>
class Sens_VEML6030 : public Sensor {
  SparkFun_Ambient_Light _veml6030;
  uint32_t   _illuminance;


public:
  Sens_VEML6030 ()
  : _veml6030(ADDRESS)
  , _illuminance(0)
{
}

  void init () {
    Wire.begin();
    _present = _veml6030.begin();
    DPRINT(F("VEML6030 "));
    if (_present) {
      DPRINTLN(F("OK"));
      _veml6030.setGain(GAIN);
      _veml6030.setIntegTime(INTEGTIME);
      DPRINT("VEML6030 Gain             : ");DDECLN(_veml6030.readGain());
      DPRINT("VEML6030 Integration Time : ");DDECLN(_veml6030.readIntegTime());
    } else {
      DPRINTLN(F("ERROR"));
    }
  }


  bool measure (__attribute__((unused)) bool async=false) {
    if( present() == true ) {
      _veml6030.powerOn();
      delay(150);
      _illuminance = _veml6030.readLight();
      DPRINT("VEML6030 Illumination (Lux)   : "); DDECLN(_illuminance);
      _veml6030.shutDown();
      return true;
    }
    return false;
  }




  int32_t illuminance() { return _illuminance; }
};

}

#endif
