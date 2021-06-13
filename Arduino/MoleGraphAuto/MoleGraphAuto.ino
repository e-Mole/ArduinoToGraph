#include "core.h"
#include <OneWire.h>
#include <Wire.h>

#define VERSION "ATG_5" //arduino to graph version

void serialProcess() {
  uint8_t instruction = Serial.read();

  switch (instruction) {
    case INS_GET_VERSION:
          Serial.write(VERSION);
          break;
    case INS_SET_TIME:
          setPeriod(0);
          break;
    case INS_SET_FREQUENCY:
          setPeriod(1);
          break;
    case INS_ENABLED_CHANNELS:
          enableChannels();
          break;
    case INS_START:
          start();
          break;
    case INS_STOP:
          stop();
          break;
    case INS_CONTINUE:
          start();
          break;
    case INS_PAUSE:
          stop();
          break;
    case INS_SET_TYPE:
          setScanType();
          break;
    case INS_GET_SAMPLE:
          getSample();
          break;
    case INS_INITIALIZE: //when desktop application crash, this stay in sending data
          stop();   // doplnit uvolneni vsech portu a kanalu
          break;
    case INS_SET_SENSOR:
          setSensor();
          break;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Timer_Init();
  SystemInit();

/*
  clean();
  addChannelSensor(0, SENSOR_AD, 0, 1, 0);
  channels[0].Enable();
  addChannelSensor(1, SENSOR_LED, 0, 0, 0);
  channels[1].Enable();
  uint16_t x = 10;
  period = (uint32_t)((1 / TIME_BASE) / x);    // nastaveni periody vzorkovani v 0.5 us [zadano v Hz] 
//  period = (uint32_t)x * (1000 * TICK_PER_MS);    // nastaveni periody vzorkovani v 0.5 us [zadano v s] 
  running = 1;
  start(); 
*/
}

void loop() {
  newTime = getTime();

  if (StartStop) {
    if (running) {
      bool dataReady = update();
      if (scanType == PERIODICAL) {
        //DEBUG_MSG("scanType == PERIODICAL: %d, %d", newTime - time, period)
        if ((firstSample && dataReady) || (uint32_t)(newTime - time) >= period) {
          firstSample = false;
          if ((uint32_t)(newTime - time) >= period){
            time += period;
          }

          scan();
          sendValues();
        }
      } else {
          running = 0;
          scan();
          sendValues();
      }
    }
  }

  if (Serial.available()) {
    serialProcess();
  }

  SystemProcess(newTime);

  if (button) {
    calibrate(button);
  }

}
