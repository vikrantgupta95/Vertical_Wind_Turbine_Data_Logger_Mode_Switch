/*|---------------------------------------------------------------------------------------|
             Insterion of TAB pin.h [ Libraries + Pin Defination and Declarations ]
  |---------------------------------------------------------------------------------------|*/
#include "pin.h"


/*|---------------------------------------------------------------------------------------|
              SOLAR VOLTAGE_2 FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Solar_Voltage_2() {
  S_VoltageReading_2 = analogRead(S_Voltage_2_Pin);
  S_Voltage_2 = 0.01743626 * S_VoltageReading_2 + 2.586991;
  Serial.println("");
  Serial.print("Solar Volatege_2: ");
  Serial.println(S_Voltage_2);
  delay(1000);
}


/*|---------------------------------------------------------------------------------------|
              SOLAR CURRENT_2 FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Solar_Current_2() {
  S_CurrentReading_2 = analogRead(S_Current_2_Pin);  // Read the analog value from pin 35

  delay(1000);  // Wait for a second before reading again
  S_Current_2 = (S_CurrentReading_2 - 1297) / 13.7;
  S_Current_2 -= 41.4;
  S_Current_2 /= 1.7;

  Serial.println("");
  Serial.print("S_Current_2 : ");
  Serial.println(S_Current_2);
}


/*|---------------------------------------------------------------------------------------|
              POWER FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Power_2() {

  S_Power_2 = S_Voltage_2 * S_Current_2;
  Serial.println("");
  Serial.print("Solar Power_2 : ");
  Serial.println(S_Power_2);
}
