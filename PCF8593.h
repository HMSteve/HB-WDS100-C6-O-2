#include "PCF8583.h"

namespace as
{
  class PCF8593 {
    PCF8583 _pcf8593;
    
  public:
    PCF8593() : _pcf8593(0xA2) {};

    void init() {
      _pcf8593.setRegister(LOCATION_CONTROL, 0x80);     
      _pcf8593.setCount(0);       
      _pcf8593.setRegister(LOCATION_CONTROL, MODE_EVENT_COUNTER);
      if ((_pcf8593.getRegister(LOCATION_CONTROL)) && 0x20 == 0x20) {
        DPRINTLN("PCF8593 init done, event counter mode set.");
      }
      else {
        DPRINTLN("PCF8593 init: setting event counter mode FAILED!");       
      }      
    }


    uint16_t getCount() {
      return _pcf8593.getCount();
    }


    void resetCounter() {
      uint8_t controlReg = _pcf8593.getRegister(LOCATION_CONTROL);
      controlReg |= 0x80;
      //datasheet recommends stopping counter before setting value
      _pcf8593.setRegister(LOCATION_CONTROL, controlReg);  
      _pcf8593.setCount(0);
      controlReg &= 0x7F;      
      _pcf8593.setRegister(LOCATION_CONTROL, controlReg);  
    }
    
  };
  
}
