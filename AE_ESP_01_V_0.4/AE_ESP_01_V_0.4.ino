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

  Section 1: ESP_01 [ Data Gathering and Sending to cloud + Switch Module Automation]
*/

/*|---------------------------------------------------------------------------------------|
            Insterion of TAB pin.h [ Libraries + Pin Defination and Declarations ]
   |---------------------------------------------------------------------------------------|*/

#include "function_comm.h"
#include "Root_Certificate.h"

/*|---------------------------------------------------------------------------------------|
      SET UP FUNCTION 
   |---------------------------------------------------------------------------------------|*/

void setup() {

  /*--------1. Basic Initialization --------------------*/

  // Begin serial communication
  Serial.begin(9600);

  // Initialize EEPROM memory
  EEPROM.begin(4096);

  // Set Wind Speed Pin as input
  pinMode(Wind_Speed_Pin, INPUT);

  /*--------2. RGB Initialization --------------------*/

  // Initialize RGB LEDs
  pixels.begin();

  /*------ 3. Defining the interrupt-------------------*/

  // Set relay pins as output and turn them off initially
  pinMode(RELAY_1, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(RELAY_2, HIGH);
  pinMode(RELAY_3, OUTPUT);

  // Set  motor pins as output and turn them off initially
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

  /*------ 4. Set up the interrupts on the GPIO pins-------------------*/

  // Set RPM sensor pins as input with pull-up resistors and attach interrupts
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

  // Initialize the Serial Monitor
  Serial.println("_ALL SET UP -> [DONE] -> Connecting to Network");

  // Attempt to reconnect to the network
  RECONNECT();

  // Read device details and send them to the cloud
  DEVICE_DETAILS_TO_CLOUD();
}

/*|---------------------------------------------------------------------------------------|
       LOOP FUNCTION 
   |---------------------------------------------------------------------------------------|*/
void loop() {

  // Read wind speed
  WIND_SPEED();

  // Calculate RPM
  RPM_CALCULATION();

  // Read solar voltage
  Solar_Voltage_1();

  // Read wind voltage
  Wind_Voltage();

  // Read wind current
  Wind_Current();

  // Read solar current
  Solar_Current();

  // Calculate power
  Power();

  // Generate dummy data
  DUMMY_DATA();

  // Switch between modes
  MODE_SWITCHING();

  // Delay to prevent rapid data transmission
  delay(1000);

  // Send sensor data to the cloud
  DATA_TO_CLOUD();

  // Send dummy data to the cloud
  D_DATA_TO_CLOUD();

  // Check if it's time to send status data to the cloud
  if (millis() > ST_TO_CLOUD_time_now + 60000) {
    // Attempt to reconnect and send status data
    RECONNECT();
    STATUS_TO_CLOUD();
    // Reset router if needed
    ROUTER_RESET();
    ST_TO_CLOUD_time_now = millis();
  }
}
