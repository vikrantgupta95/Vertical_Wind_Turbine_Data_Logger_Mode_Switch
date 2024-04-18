/*
  Author: Vikrant_Gupta
  Email: vikrantgupta95@gmail.com
  LinkedIn: https://www.linkedin.com/in/vikrantgupta95/
  Instagram: https://www.instagram.com/crazyvikku/
  GitHub: https://github.com/vikrantgupta95
  Created For: Apeiro Energy 

  Description:
  The 3-phase wind turbine is equipped with sensors to gather power, voltage, current, RPM, and
  wind speed data. It smoothly switches between modes to optimize performance, connecting to 
  the cloud for remote monitoring and data collection, ensuring efficient operation and maintenance.

  Last updated: [11-04-2024]

  Section 1: ESP_01 [ Only Data Gathering and Sending to cloud ]
*/


/*|---------------------------------------------------------------------------------------|
            Insterion of TAB pin.h [ Libraries + Pin Defination and Declarations ]
   |---------------------------------------------------------------------------------------|*/

#include "function_comm.h"
#include "Root_Certificate.h"


void setup() {

  /*--------1. Basic Initilisation --------------------*/

  Serial.begin(9600);
  EEPROM.begin(4096);
  analogReadResolution(12);  // Set the resolution of analog read to 12 bits



  /*--------2. RGB Initilisation --------------------*/

  pixels.begin();

  /*------ 3.Defining the interrupt-------------------*/

  pinMode(INTR_PIN, INPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(RELAY_2, HIGH);
  pinMode(RELAY_3, OUTPUT);

  pinMode(SM_1_1, OUTPUT);
  pinMode(SM_1_2, OUTPUT);
  pinMode(SM_2_1, OUTPUT);
  pinMode(SM_2_2, OUTPUT);
  pinMode(SM_3_1, OUTPUT);
  pinMode(SM_3_2, OUTPUT);

  digitalWrite(SM_1_1, LOW);
  digitalWrite(SM_1_2, LOW);
  digitalWrite(SM_2_1, LOW);
  digitalWrite(SM_2_2, LOW);
  digitalWrite(SM_3_1, LOW);
  digitalWrite(SM_3_2, LOW);


  // attachInterrupt(INTR_PIN, CONFIG_INTR, HIGH);

  /*------ 4.Set up the interrupts on the GPIO pins-------------------*/

  pinMode(RPM_T1_Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_T1_Pin), handleInterruptMotor1, FALLING);

  pinMode(RPM_T2_Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_T2_Pin), handleInterruptMotor2, FALLING);

  pinMode(RPM_T3_Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_T3_Pin), handleInterruptMotor3, FALLING);

  // Set up a timer to call the onTimer function every second
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  /*------- 5. Interrupt Calling for Config Mode------------*/

  // if (intr_check == 1)  //For configuration
  // {
  //   intr_check = 0;
  //   delay(3000);
  //   if (digitalRead(2) == HIGH) {
  //     CONFIG_MODE();
  //   }
  // }

  /*------- 6. Wind Speed Declarations------------*/


  //ASSIGN_CRED();

  Serial.println("_ALL SET UP -> [DONE] -> Connecting to Network");

  RECONNECT();

  // if (j == 6 && FW_VERSION_CHECK()) {
  //   Serial.println("Network - [CONNECTED]");
  //   Serial.println("Executing Over the air Updates");
  //   FIRMWARE_UPDATE();
  // } else if (j != 6) {
  //   Serial.println(F("Couldn't Get Wi-Fi  ->  Skipped OTA Update"));
  // }

  DEVICE_DETAILS_TO_CLOUD();
}

void loop() {
  WIND_SPEED();

  RPM_CALCULATION();

  Solar_Voltage_1();

  Wind_Voltage();

  Wind_Current();

  Solar_Current();

  Power();

  DUMMY_DATA();

  MODE_SWITCHING();

  DATA_TO_CLOUD();

  D_DATA_TO_CLOUD();

  if (millis() > ST_TO_CLOUD_time_now + 30000) {
    RECONNECT();
    STATUS_TO_CLOUD();
    // ROUTER_RESET();
  }
}
