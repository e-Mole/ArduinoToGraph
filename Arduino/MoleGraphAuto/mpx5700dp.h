#ifndef MPX5700DP_h
#define MPX5700DP_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class MPX5700DP : public Sensor {
  public:
    MPX5700DP(uint8_t, uint32_t, uint8_t);
    virtual bool process();
    virtual void calibrate(); 
  private:
    uint8_t   pin;
    int32_t   offset = 0;
    float     scale; 
};


#endif
