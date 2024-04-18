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

  Section 1: ESP_02 [ Only Data Gathering and Sending to cloud ]
*/


/*|---------------------------------------------------------------------------------------|
            Insterion of TAB pin.h [ Libraries + Pin Defination and Declarations ]
   |---------------------------------------------------------------------------------------|*/

#include "function_comm.h"
#include "Root_Certificate.h"

/*|---------------------------------------------------------------------------------------|
            SET UP FUNCTIONS 
   |---------------------------------------------------------------------------------------|*/

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize EEPROM
  EEPROM.begin(4096);

  // Initialize RGB LEDs
  pixels.begin();

  // Configure pins
  pinMode(GPIO_21, OUTPUT); // Example pin configuration

  // Set analog read resolution to 12 bits
  analogReadResolution(12);

  // Print setup completion message
  Serial.println("_ALL SET UP -> [DONE] -> Connecting to Network");

  // Connect to the network
  RECONNECT();

  // Send device details to the cloud
  DEVICE_DETAILS_TO_CLOUD();
}

/*|---------------------------------------------------------------------------------------|
            MAIN LOOP
   |---------------------------------------------------------------------------------------|*/

void loop() {
  // Gather solar voltage data
  Solar_Voltage_2();

  // Gather solar current data
  Solar_Current_2();

  // Send gathered data to the cloud
  DATA_TO_CLOUD();

  // Subscribe to remote procedure calls
  SUBSCRIBE_RPC();

  // Reconnect to the network and send status to the cloud every minute
  if (millis() > ST_TO_CLOUD_time_now + 60000) {
    RECONNECT();
    STATUS_TO_CLOUD();
    ST_TO_CLOUD_time_now = millis();
  }
}