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


  /*--------2. RGB Initilisation --------------------*/

  pixels.begin();

  /*------ 3.Defining the interrupt-------------------*/

  pinMode(INTR_PIN, INPUT);
 

  /*------- 6. Wind Speed Declarations------------*/

  analogReadResolution(12); // Set the resolution of analog read to 12 bits

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

void loop() 
{
  Solar_Voltage_2();

  Solar_Current_2();

  DATA_TO_CLOUD();

  if (millis() > ST_TO_CLOUD_time_now + 30000) {
    RECONNECT();
    STATUS_TO_CLOUD();
   // ROUTER_RESET();
  }
}


