//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2018-11-02 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef __SENS_VEML6030_h__
#define __SENS_VEML6030_h__

#include <Sensors.h>
#include <Wire.h>
#include "VEML6030.h"

#define POWERSAVEMODE 4
#define GAIN 1
#define INTEGTIME 200
//scaling factors to map illuminance to 8-bit-value on logarithmic scale
#define RESOLUTION 0.0036 * 800.0 / INTEGTIME * 2.0 / GAIN *1000
#define F1 (255.0 / log10(RESOLUTION  * 65536.0))
#define F2 F1 * log10(RESOLUTION)
#define F3 255.0 / (255.0 - F2)



namespace as {


template <uint8_t ADDRESS=0x48>
class Sens_VEML6030 : public Sensor {
  VEML6030 _veml6030;
  uint32_t _illuminance;

public:
  Sens_VEML6030 () : _veml6030(ADDRESS) , _illuminance(0) {}

  void init () {
    Wire.begin();
    _present = _veml6030.begin();
    DPRINT(F("VEML6030 "));
    if (_present) {
      DPRINTLN(F("OK"));
      _veml6030.setPowSavMode(POWERSAVEMODE);
      _veml6030.enablePowSave();
      _veml6030.setGain(GAIN);
      _veml6030.setIntegTime(INTEGTIME);
      DPRINT("VEML6030 Gain               : ");DDECLN(_veml6030.readGain());
      DPRINT("VEML6030 Integration Time   : ");DDECLN(_veml6030.readIntegTime());
      DPRINT("VEML6030 Power Save Enabled : ");DDECLN(_veml6030.readPowSavEnabled());
      DPRINT("VEML6030 Power Save Mode    : ");DDECLN(_veml6030.readPowSavMode());
    } else {
      DPRINTLN(F("ERROR"));
    }
  }


  bool measure () {
    if( present() == true ) {
      //_veml6030.powerOn();
      //delay(150);
      _illuminance = _veml6030.readMilliLux();
      //_veml6030.shutDown();
      return true;
    }
    return false;
  }


  uint32_t milliLux() {
    return _illuminance;
  }

  uint8_t brightness() {
    int32_t brightness = round((log10((float)_illuminance + 1) * F1 - F2) * F3); //reading + 1 mLux to avoid log(0)
    brightness = min(max(brightness,0),255);
    return brightness;
  }

};

}

#endif
