/* |---------------------------------------------------------------------------------------|
               Inclusion of TAB pin.h [ Libraries + Pin Definition and Declarations ]
   |---------------------------------------------------------------------------------------| */
#include "pin.h"

// Function prototypes
void Solar_Voltage_2(void);  // Function prototype for measuring solar voltage_2
void Solar_Current_2(void);  // Function prototype for measuring solar current_2
void Power_2(void);          // Function prototype for calculating solar power_2

/* |---------------------------------------------------------------------------------------|
                  SOLAR VOLTAGE_2 FUNCTION
   |---------------------------------------------------------------------------------------| */
void Solar_Voltage_2() {
  // Read analog value from pin S_Voltage_2_Pin
  S_VoltageReading_2 = analogRead(S_Voltage_2_Pin);

  // Convert analog reading to voltage using calibration formula
  S_Voltage_2 = 0.01743626 * S_VoltageReading_2 + 2.586991;

  // Print the measured solar voltage_2
  Serial.println("");
  Serial.print("Solar Voltage_2: ");
  Serial.println(S_Voltage_2);

  // Delay for stability before next reading
  delay(1000);
}


/* |---------------------------------------------------------------------------------------|
                  SOLAR CURRENT_2 FUNCTION
   |---------------------------------------------------------------------------------------| */
void Solar_Current_2() {
  // Read analog value from pin S_Current_2_Pin
  S_CurrentReading_2 = analogRead(S_Current_2_Pin);

  // Delay for stability before next reading
  delay(1000);

  // Convert analog reading to current using calibration formula
  S_Current_2 = (S_CurrentReading_2 - 1297) / 13.7;  // Adjusted reading
  S_Current_2 -= 41.4;                                // Offset adjustment
  S_Current_2 /= 1.7;                                 // Calibration factor adjustment

  // Print the measured solar current_2
  Serial.println("");
  Serial.print("Solar Current_2: ");
  Serial.println(S_Current_2);
}


/* |---------------------------------------------------------------------------------------|
                  POWER FUNCTION
   |---------------------------------------------------------------------------------------| */
void Power_2() {
  // Calculate power by multiplying voltage and current
  S_Power_2 = S_Voltage_2 * S_Current_2;

  // Print the calculated solar power_2
  Serial.println("");
  Serial.print("Solar Power_2: ");
  Serial.println(S_Power_2);
}
