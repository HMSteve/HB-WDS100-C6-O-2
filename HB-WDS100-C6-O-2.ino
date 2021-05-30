//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2016-10-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-02-28 jp112sdl (Creative Commons)
//- -----------------------------------------------------------------------------------------------------------------------
// 2021-01-26 HB drop-in replacement for HM-WDS100-C6-O-2, HMSteve (Creative Commons by-nc-sa)
//- -----------------------------------------------------------------------------------------------------------------------

#define NDEBUG   // disable all serial debug messages 

#define EI_NOTEXTERNAL

#include <EnableInterrupt.h>
#include <AskSinPP.h>
#include <LowPower.h>
#include <Register.h>
#include <MultiChannelDevice.h>
#include "Wds100ConfigButton.h"
#include "PCF8593.h"
#include "sensors/Sens_SHT31.h"
#include "sensors/Sens_VEML6030.h"
#include "sensors/Sens_AS5600.h"
#include "sensors/tmBattery.h"  

#define CC1101_CS_PIN        4
#define CC1101_GDO0_PIN      2
#define CC1101_SCK_PIN       7 
#define CC1101_MOSI_PIN      5 
#define CC1101_MISO_PIN      6 
#define LED_PIN             13              
#define LED_PIN2            14
#define CONFIG_BUTTON_PIN   15
#define CC1101_PWR_SW_PIN   27
#define AS5600PWRSW_PIN     28 
#define RAINDETECTOR_PIN    18 
#define RAINCOUNTER_PIN     19  

#define BAT_VOLT_LOW                32    //3.2V for 3x Alkaline with 3V3 LDO
#define BAT_VOLT_CRITICAL           30  
#define PEERS_PER_CHANNEL            6
#define UPDATE_INTERVAL            183    //seconds. message transmission interval
#define HIGHFREQ_MEASURE_INTERVAL    5    //seconds. measurement interval wind speed and brightness
#define ANEMOMETER_CALIB_FACTOR     28    //empirical calib value ag. Windboss anemometer
#define NORTH_ANGLE_DEFAULT        140    //individual value if mounting direction is known, no need to change though

//#define CLOCK_SYSCLOCK
#define CLOCK_RTC


#ifdef CLOCK_SYSCLOCK
#define CLOCK sysclock
#define SAVEPWR_MODE Sleep<>
#elif defined CLOCK_RTC
#define CLOCK rtc
#define SAVEPWR_MODE SleepRTC
#undef seconds2ticks
#define seconds2ticks(tcks) (tcks)
#else
#error INVALID CLOCK OPTION
#endif



// tmBatteryLoad: sense pin, activation pin, Faktor = Rges/Rlow*1000, z.B. 10/30 Ohm, Faktor 40/10*1000 = 4000, 200ms Belastung vor Messung
// 1248p has 2.56V ARef, 328p has 1.1V ARef
#define BATT_SENSOR tmBatteryLoad<A6, 12, (uint16_t)(4000.0*2.56/1.1), 200>  



// all library classes are placed in the namespace 'as'
using namespace as;

volatile uint8_t _raindetector_isr_indicator = 0;
volatile uint32_t _raincounter_isr_counter = 0;



void raindetectorISR() {
  _raindetector_isr_indicator++;
  //save power by avoiding multiple interrupts during one measuring cycle, reenable in clock trigger function
  if (digitalPinToInterrupt(RAINDETECTOR_PIN) == NOT_AN_INTERRUPT) disableInterrupt(RAINDETECTOR_PIN); else detachInterrupt(digitalPinToInterrupt(RAINDETECTOR_PIN));  
}


void raincounterISR() {
  _raincounter_isr_counter++;
}




// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
  {0xF8, 0x23, 0x01},     // Device ID
  "SGSENWDS01",           // Device Serial
  {0x00, 0xAE},           // Device Model HM-WDS100-C6-O-2
  0x10,                   // Firmware Version
  as::DeviceType::THSensor, // Device Type
  {0x01, 0x00}            // Info Bytes
};


// Configure the used hardware
typedef AvrSPI<CC1101_CS_PIN, CC1101_MOSI_PIN, CC1101_MISO_PIN, CC1101_SCK_PIN> SPIType;
typedef Radio<SPIType, CC1101_GDO0_PIN> RadioType;
typedef DualStatusLed<LED_PIN, LED_PIN2> LedType;
typedef AskSin<LedType, BATT_SENSOR, RadioType> BaseHal;

class Hal : public BaseHal {
  public:
    void init (const HMID& id) {
      BaseHal::init(id);
#ifdef CLOCK_RTC
      rtc.init();    // init real time clock - 1 tick per second
#endif
      // measure battery every a*b*c seconds
      battery.init(seconds2ticks(60UL * 60 * 12), CLOCK);  // 60UL * 60 for 1hour
      battery.low(BAT_VOLT_LOW);
      battery.critical(BAT_VOLT_CRITICAL);
    }

    bool runready () {
      return CLOCK.runready() || BaseHal::runready();
    }
} hal;

  
DEFREGISTER(Reg0, MASTERID_REGS, DREG_LEDMODE, DREG_TRANSMITTRYMAX, 0x01, 0x11, 0x18)
class Wds100List0 : public RegList0<Reg0> {
  public:
    Wds100List0(uint16_t addr) : RegList0<Reg0>(addr) {}

    bool liveModeRx (bool value) const {
      return this->writeRegister(0x01, value & 0xff);
    }
    bool liveModeRx () const {
      return this->readRegister(0x01, 0);
    }    

    bool cycleInfoMsgDis (uint8_t value) const {
      return this->writeRegister(0x11, value & 0xff);
    }
    uint8_t cycleInfoMsgDis () const {
      return this->readRegister(0x11, 0);
    }     

    bool localResetDisable (bool value) const {
      return this->writeRegister(0x18, value & 0xff);
    }
    bool localResetDisable () const {
      return this->readRegister(0x18, 0);
    }    
    
    void defaults () {
      clear();
      ledMode(0);
      transmitDevTryMax(6);     
      liveModeRx(false);
      localResetDisable(false);
      cycleInfoMsgDis(0);   
    }
};


DEFREGISTER(Reg1, 0x05, 0x06, 0x07, 0x0A, 0x20, 0x21)
class Wds100List1 : public RegList1<Reg1> {
  public:
    Wds100List1 (uint16_t addr) : RegList1<Reg1>(addr) {}

    bool sunshineThreshold (uint8_t value) const {
      return this->writeRegister(0x05, value & 0xff);
    }
    
    uint8_t sunshineThreshold () const {
      return this->readRegister(0x05, 0);
    } 
    
    bool stormUpperThreshold (uint8_t value) const {
      return this->writeRegister(0x06, value & 0xff);
    }
    
    uint8_t stormUpperThreshold () const {
      return this->readRegister(0x06, 0);
    }    
    
    bool stormLowerThreshold (uint8_t value) const {
      return this->writeRegister(0x07, value & 0xff);
    }
    
    uint8_t stormLowerThreshold () const {
      return this->readRegister(0x07, 0);
    }   
    
    bool windSpeedResultSource (uint8_t value) const {
      return this->writeRegister(0x0A, value & 0xff);
    }
    
    uint8_t windSpeedResultSource () const {
      return this->readRegister(0x0A, 0);
    }     

    bool northAngle (uint16_t value) const {
      return this->writeRegister(0x20, (value >> 8) & 0xff) && this->writeRegister(0x21, value & 0xff);
    }
    
    uint16_t northAngle () const {
      return (this->readRegister(0x20, 0) << 8) + this->readRegister(0x21, 0);
    }

    void defaults () {
      clear();
      sunshineThreshold(0);
      stormUpperThreshold(0);
      stormLowerThreshold(0);
      windSpeedResultSource(0);
      northAngle(NORTH_ANGLE_DEFAULT);
    }
};     


class Wds100EventMsg : public Message {
  public:
    void init(uint8_t msgcnt, int16_t temperature, uint8_t humidity, uint8_t raining, uint16_t rain_counter, uint16_t wind_speed, uint8_t wind_direction, uint8_t wind_direction_range, uint8_t sunshineduration, uint8_t brightness, bool batlow) {
      uint8_t t1 = (temperature >> 8) & 0x7f;
      uint8_t t2 = temperature & 0xff;
      if ( batlow == true ) {
        t1 |= 0x80; // set bat low bit
      }
      uint8_t flags = 0;
      // Message Length (first byte param.): 11 + payload. Max. payload: 17 Bytes (https://www.youtube.com/watch?v=uAyzimU60jw)
      Message::init(19, msgcnt, 0x70, flags, t1, t2);
      pload[0] = humidity & 0xff;
      pload[1] = ((rain_counter >> 8) & 0x7f) | (raining << 7);
      pload[2] = (rain_counter) & 0xff;      
      pload[3] = ((wind_speed >> 8) & 0x3f) | (wind_direction_range << 6);
      pload[4] = (wind_speed) & 0xff;        
      pload[5] = wind_direction & 0xff;         
      pload[6] = sunshineduration & 0xff;
      pload[7] = brightness & 0xff;
    }
};


class Wds100Channel : public Channel<Hal, Wds100List1, EmptyList, List4, PEERS_PER_CHANNEL, Wds100List0>, public Alarm {
    Sens_SHT31<0x44>    sht31;
    Sens_VEML6030<0x10> veml6030;
    Sens_AS5600         as5600; 
    PCF8593             pcf8593;  
    int16_t  temperature = 0;
    uint8_t  humidity = 0;
    uint8_t  raining = 0;    
    uint16_t rain_counter = 0; 
    uint16_t wind_speed = 0; 
    uint8_t  wind_direction = 0;  
    uint8_t  wind_direction_range = 0;  
    uint8_t  sunshineduration = 0;  
    uint8_t  brightness = 0;   
    uint16_t ws_total = 0; 
    uint16_t ws_measure_count = 0;  
    uint16_t wind_speed_avg = 0; 
    uint16_t wind_speed_gust = 0;      
    uint16_t  stormUpperThreshold;
    uint16_t  stormLowerThreshold;        
                           
  public:
    Wds100Channel () : Channel(), Alarm(10), hfMeasureAlarm(*this) {}
    
    virtual ~Wds100Channel () {}

    class HighFreqMeasureAlarm : public Alarm {
        Wds100Channel& chan;
        uint16_t ssh_total;
        
      public:
        HighFreqMeasureAlarm (Wds100Channel& c) : Alarm (seconds2ticks(HIGHFREQ_MEASURE_INTERVAL)), chan(c), ssh_total(0) {}
        
        virtual ~HighFreqMeasureAlarm () {}

        void trigger (__attribute__ ((unused)) AlarmClock& clock)  {
          chan.measure_windspeed();

          chan.veml6030.measure();
          chan.brightness = chan.veml6030.brightness();  
          if (chan.brightness >= chan.getList1().sunshineThreshold()) {
            if (ssh_total < 255 * 60 - HIGHFREQ_MEASURE_INTERVAL) {
              ssh_total += HIGHFREQ_MEASURE_INTERVAL;
            }
            else {
              ssh_total = (HIGHFREQ_MEASURE_INTERVAL - (255 * 60 - ssh_total));
            }
          }
          // guess it's night when dark, so reset sunshine duration counter
          if (chan.veml6030.brightness() < 1) {
            ssh_total = 0;
          }
          chan.sunshineduration = ssh_total / 60;
          DPRINT("mLux / 8-bit brightness = ");DDEC(chan.veml6030.milliLux()); DPRINT(" / "); DDECLN(chan.veml6030.brightness());

          chan.pcf8593.resetCounter();
          tick = (seconds2ticks(HIGHFREQ_MEASURE_INTERVAL));
          clock.add(*this);
        }
    } hfMeasureAlarm;


    virtual void trigger (AlarmClock& clock) {
      sht31.measure();
      temperature = sht31.temperature();
      humidity = sht31.humidity();        
      measure_winddir();
      evaluate_windspeed();   
      measure_raining();   
      rain_counter = _raincounter_isr_counter;
      DPRINT("wind dir angle / corrected wind dir  = ");DDEC(as5600.angle()); DPRINT(" / "); DDECLN(wind_direction * 5);
      DPRINT("windspeed avg / gust / reported km/h = ");DDEC(wind_speed_avg); DPRINT(" / ");DDEC(wind_speed_gust); DPRINT(" / ");DDECLN(wind_speed);
      DPRINT("raining =                              ");DDECLN(raining);
      Wds100EventMsg& msg = (Wds100EventMsg&)device().message();
      uint8_t msgcnt = device().nextcount();      
      msg.init( msgcnt, temperature, humidity, raining, rain_counter, wind_speed, wind_direction, wind_direction_range, sunshineduration, brightness, device().battery().low());
      // BIDI|WKMEUP every 20th msg      
      if ((msgcnt % 20) == 1) {
          DPRINTLN("sendMasterEvent");
          msg.flags(Message::BIDI | Message::WKMEUP);
          device().sendMasterEvent(msg);
      } else {
          DPRINTLN("broadcastEvent");
          msg.flags(Message::BCAST);  
          device().broadcastEvent(msg);
      }
      
      wind_speed_gust = 0;
      set(seconds2ticks(UPDATE_INTERVAL * (this->device().getList0().cycleInfoMsgDis() + 1)));
      clock.add(*this);
    }

    void measure_windspeed() {
      uint16_t ws = pcf8593.getCount();
      ws_total += ws;
      ws = uint16_t(ANEMOMETER_CALIB_FACTOR * ws / HIGHFREQ_MEASURE_INTERVAL);     
      if (ws > wind_speed_gust) {
        wind_speed_gust = ws;
      } 
      ws_measure_count++;   
      DPRINT("current windspeed km/h * 10 = ");DDECLN(ws);

      static uint8_t STORM_COND_VALUE_Last = 0;
      static uint8_t STORM_COND_VALUE      = 0;

      if (stormUpperThreshold > 0) {
        if (ws >= stormUpperThreshold || ws <= stormLowerThreshold) {
          static uint8_t evcnt = 0;
          if (ws >= stormUpperThreshold) STORM_COND_VALUE = 1;
          if (ws <= stormLowerThreshold) STORM_COND_VALUE = 0;          

          if (STORM_COND_VALUE != STORM_COND_VALUE_Last) {
            SensorEventMsg& rmsg = (SensorEventMsg&)device().message();
            DPRINT(F("PEER THRESHOLD DETECTED : ")); DDECLN(STORM_COND_VALUE);
            rmsg.init(device().nextcount(), number(), evcnt++, ws / 10, false , false);           
            rmsg.flags(Message::BIDI | Message::WKMEUP);
            device().sendPeerEvent(rmsg, *this);
          }
          STORM_COND_VALUE_Last = STORM_COND_VALUE;
        }
      }
    }


    void evaluate_windspeed() {
      wind_speed_avg = uint16_t(ANEMOMETER_CALIB_FACTOR * ws_total / HIGHFREQ_MEASURE_INTERVAL / ws_measure_count);
      if (this->getList1().windSpeedResultSource() == 1) {
        wind_speed = wind_speed_gust;
      }
      else {
        wind_speed = wind_speed_avg;        
      }
      ws_total = 0;
      ws_measure_count = 0;      
    }


    void measure_winddir() {
      int16_t wdr = 0;
      uint16_t wdold = as5600.angle();
      pinMode(AS5600PWRSW_PIN, OUTPUT);   
      digitalWrite(AS5600PWRSW_PIN, LOW);
      delay(10); //power-up time       
      as5600.init(); 
      as5600.measure(); 
      pinMode(AS5600PWRSW_PIN, INPUT); 
      if (as5600.angle() > this->getList1().northAngle()) {
        wind_direction = (as5600.angle() - this->getList1().northAngle()) / 5;    
      } else {
        wind_direction = (as5600.angle() +360 - this->getList1().northAngle()) / 5;
      }
      wdr = as5600.angle() - wdold;
      wdr = abs(wdr);
      if (wdr > 180) {
        wdr = 360 - wdr;                    
      }
      if (wdr < 22) {
        wind_direction_range = 0;
      } else if ((wdr >= 22) and (wdr <45)) {
        wind_direction_range = 1;  
      } else if ((wdr >= 45) and (wdr <67)) {
        wind_direction_range = 2;  
      } else {
        wind_direction_range = 3;           
      }
      DPRINT("Wind dir old / new / range / sent val : ");DDEC(wdold);DPRINT(" / ");DDEC(as5600.angle());DPRINT(" / ");DDEC(wdr);DPRINT(" / ");DDECLN(wind_direction_range);
    }


    uint16_t get_winddir_angle() {
      measure_winddir();
      return as5600.angle();
    }


    void measure_raining(){
      raining = _raindetector_isr_indicator;
      if(digitalRead(RAINDETECTOR_PIN) == 1) {
        _raindetector_isr_indicator = 0;
        if ( digitalPinToInterrupt(RAINDETECTOR_PIN) == NOT_AN_INTERRUPT ) enableInterrupt(RAINDETECTOR_PIN, raindetectorISR, FALLING); else attachInterrupt(digitalPinToInterrupt(RAINDETECTOR_PIN), raindetectorISR, FALLING);  
      }      
    }

    
    void setup(Device<Hal, Wds100List0>* dev, uint8_t number, uint16_t addr) {
      Channel::setup(dev, number, addr);
      sht31.init();
      veml6030.init();
      pcf8593.init();
      set(seconds2ticks(2));  // first message in 2 sec.      
      CLOCK.add(*this);
      CLOCK.add(hfMeasureAlarm);      
    }

    uint8_t status () const {
      return 0;
    }

    uint8_t flags () const {
      return 0;
    }

    void configChanged() {
      Channel::configChanged();
      stormUpperThreshold = this->getList1().stormUpperThreshold() * 10;
      stormLowerThreshold = this->getList1().stormLowerThreshold() * 10;      
      DPRINTLN("* Channel Conf Changed : Wds100List1");
      DPRINT(F("* Sonnescheinschwelle  : ")); DDECLN(this->getList1().sunshineThreshold()); 
      DPRINT(F("* Sturm obere Schwelle : ")); DDECLN(this->getList1().stormUpperThreshold());  
      DPRINT(F("* Sturm untere Schwelle: ")); DDECLN(this->getList1().stormLowerThreshold());       
      DPRINT(F("* Art Windgeschw.      : ")); DDECLN(this->getList1().windSpeedResultSource());
      DPRINT(F("* Winkel Nord          : ")); DDECLN(this->getList1().northAngle());      
    }
};


class Wds100Device : public MultiChannelDevice<Hal, Wds100Channel, 1, Wds100List0> {
  public:
    typedef MultiChannelDevice<Hal, Wds100Channel, 1, Wds100List0> TSDevice;
    
    Wds100Device(const DeviceInfo& info, uint16_t addr) : TSDevice(info, addr) {}
    
    virtual ~Wds100Device () {}

    virtual void configChanged () {
      TSDevice::configChanged();
      DPRINTLN("* Device Conf Changed  : Wds100List0");
      DPRINT(F("* LED Mode             : ")); DDECLN(this->getList0().ledMode());    
      DPRINT(F("* Sendeversuche        : ")); DDECLN(this->getList0().transmitDevTryMax());          
      DPRINT(F("* Live Mode            : ")); DDECLN(this->getList0().liveModeRx());     
      DPRINT(F("* Auszul. Meldungen    : ")); DDECLN(this->getList0().cycleInfoMsgDis());          
      DPRINT(F("* Local Reset Disable  : ")); DDECLN(this->getList0().localResetDisable());  
    }

    void setNorth () {
      uint16_t north = Wds100Channel().get_winddir_angle();
      this->channel(0).getList1().northAngle(north);     
      DPRINT("North angle stored in List 1 : "); DDECLN(this->channel(0).getList1().northAngle());       
    }
};



Wds100Device sdev(devinfo, 0x20);
Wds100ConfigButton<Wds100Device> cfgBtn(sdev);


void setup () {
  //switch on MOSFET to power CC1101
  pinMode(CC1101_PWR_SW_PIN, OUTPUT); 
  digitalWrite (CC1101_PWR_SW_PIN, LOW);
  DINIT(57600, ASKSIN_PLUS_PLUS_IDENTIFIER);
  sdev.init(hal);
   
  buttonISR(cfgBtn, CONFIG_BUTTON_PIN);
  sdev.initDone();
  //sdev.getList0().dump();
  //sdev.channel(0).getList1().dump();
  
  pinMode(RAINDETECTOR_PIN, INPUT); //large ext pullup to save power during rain
  pinMode(RAINCOUNTER_PIN, INPUT);  
  if ( digitalPinToInterrupt(RAINCOUNTER_PIN) == NOT_AN_INTERRUPT ) enableInterrupt(RAINCOUNTER_PIN, raincounterISR, CHANGE); else attachInterrupt(digitalPinToInterrupt(RAINCOUNTER_PIN), raincounterISR, CHANGE);  
}


void loop() {
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if ( worked == false && poll == false ) {
    hal.activity.savePower<SAVEPWR_MODE>(hal);
  }
}
