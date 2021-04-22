#include <Button.h>

namespace as {

  template <class DEVTYPE,uint8_t OFFSTATE=HIGH,uint8_t ONSTATE=LOW,WiringPinMode MODE=INPUT_PULLUP>
  class Wds100ConfigButton : public StateButton<OFFSTATE,ONSTATE,MODE> {
    DEVTYPE& device;
    bool setNorthState = false;
  public:
    typedef StateButton<OFFSTATE,ONSTATE,MODE> ButtonType;
  
    Wds100ConfigButton (DEVTYPE& dev,uint8_t longpresstime=3) : device(dev) {
      this->setLongPressTime(seconds2ticks(longpresstime));
    }
    
    virtual ~Wds100ConfigButton () {}
    
    void state (uint8_t s) {
      uint8_t old = ButtonType::state();
      ButtonType::state(s);
      if( s == ButtonType::released ) {
        if (setNorthState) {
          device.setNorth();
          setNorthState = false;
          device.led().ledOn(200,0);
        }
        else {
          device.startPairing();        
        }
      }
      else if( s == ButtonType::longpressed ) {
        if( old == ButtonType::longpressed ) {
          if( device.getList0().localResetDisable() == false ) {
            device.reset(); // long pressed again - reset
          }
        }
        else {
          setNorthState = true;
          device.led().set(LedStates::key_long);
          DPRINTLN("+++short press to store north angle");
        }
      }
    }
    
  };

}
