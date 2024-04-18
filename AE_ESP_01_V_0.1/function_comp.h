/*|---------------------------------------------------------------------------------------|
             Insterion of TAB pin.h [ Libraries + Pin Defination and Declarations ]
  |---------------------------------------------------------------------------------------|*/
#include "pin.h"

/*|---------------------------------------------------------------------------------------|
             RPM CALCULATION INTERUPTS
  |---------------------------------------------------------------------------------------|*/

// ISR for Motor 1
void IRAM_ATTR handleInterruptMotor1() {
  if (calculateRPM) {
    pulseCountMotor1++;
  }
}

// ISR for Motor 2
void IRAM_ATTR handleInterruptMotor2() {
  if (calculateRPM) {
    pulseCountMotor2++;
  }
}

// ISR for Motor 3
void IRAM_ATTR handleInterruptMotor3() {
  if (calculateRPM) {
    pulseCountMotor3++;
  }
}

hw_timer_t* timer = NULL;

/*|---------------------------------------------------------------------------------------|
              Timer interrupt to calculate RPM for all motors
  |---------------------------------------------------------------------------------------|*/

void IRAM_ATTR onTimer() {
  if (calculateRPM) {
    // Calculate RPM for Motor 1
    RPM_T1 = pulseCountMotor1 / 5;  // RPM is pulse count divided by 5

    // Calculate RPM for Motor 2
    RPM_T2 = pulseCountMotor2 / 5;  // RPM is pulse count divided by 5

    // Calculate RPM for Motor 3
    RPM_T3 = pulseCountMotor3 / 5;  // RPM is pulse count divided by 5

    // Reset pulse counts for the next second
    pulseCountMotor1 = 0;
    pulseCountMotor2 = 0;
    pulseCountMotor3 = 0;
  }
}
/*|---------------------------------------------------------------------------------------|
              RPM CALCULATION FUNCTION 
  |---------------------------------------------------------------------------------------|*/

void RPM_CALCULATION() {
  if (calculateRPM) {
    // Display the RPM of each motor
    Serial.print("Motor 1 RPM: ");
    Serial.println(RPM_T1);
    Serial.print("Motor 2 RPM: ");
    Serial.println(RPM_T2);
    Serial.print("Motor 3 RPM: ");
    Serial.println(RPM_T3);
  }

  // Add your condition to stop RPM calculation
  // For example, after 10 seconds
  if (millis() > 10000) {
    calculateRPM = false;
  }
}

/*|---------------------------------------------------------------------------------------|
              WIND SPEED FUNCTION
  |---------------------------------------------------------------------------------------|*/

void WIND_SPEED() {
  windSpeedReading = analogRead(Wind_Speed_Pin) * (3.3 / 4095.0);  // Convert the value to voltage (0-3.3V)
  //Wind_Speed = 7.807121 * windSpeedReading - 2.224219; // Apply the calibration equation
  Wind_Speed = -0.5798437 + 5.148 * windSpeedReading + 0.867 * windSpeedReading * windSpeedReading;
  Wind_Speed *= 1.3;
  Wind_Speed += 0.75;
  if (Wind_Speed < 0)
    Wind_Speed = 0;

  // Print the wind speed to the Serial Monitor
  Serial.print("Wind_Speed: ");
  Serial.print(Wind_Speed);
  Serial.println(" m/s");
  delay(1000);  // Wait for a second before reading the value again
}

/*|---------------------------------------------------------------------------------------|
              WIND CURRENT FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Wind_Current() {
  W_CurrentReading = analogRead(W_Current_Pin);  // Read the analog value from pin 35

  delay(1000);  // Wait for a second before reading again
  W_Current = (W_CurrentReading - 1297) / 13.7;
  W_Current -= 41.4;
  W_Current /= 1.7;

  Serial.println("");
  Serial.print("W_Current : ");
  Serial.println(W_Current);
}

/*|---------------------------------------------------------------------------------------|
              SOLAR CURRENT FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Solar_Current() {
  S_CurrentReading = analogRead(S_Current_Pin);  // Read the analog value from pin 35

  delay(1000);  // Wait for a second before reading again
  S_Current = (S_CurrentReading - 1297) / 13.7;
  S_Current -= 41.4;
  S_Current /= 1.7;

  Serial.println("");
  Serial.print("S_Current : ");
  Serial.println(S_Current);
}

/*|---------------------------------------------------------------------------------------|
              SOLAR VOLTAGE FUNCTION
  |---------------------------------------------------------------------------------------|*/


void Solar_Voltage_1() {
  S_VoltageReading = analogRead(S_Voltage_Pin);
  S_Voltage = 0.01743626 * S_VoltageReading + 2.586991;
  Serial.println("");
  Serial.print("Solar Volatege_1: ");
  Serial.println(S_Voltage);
  delay(1000);
}

/*|---------------------------------------------------------------------------------------|
              WIND VOLTAGE FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Wind_Voltage() {
  W_VoltageReading = analogRead(W_Voltage_Pin);
  W_Voltage = 0.01743626 * W_VoltageReading + 2.586991;
  Serial.println("");
  Serial.print("Wind Volatege: ");
  Serial.println(W_Voltage);
  delay(1000);
}

/*|---------------------------------------------------------------------------------------|
              POWER FUNCTION
  |---------------------------------------------------------------------------------------|*/

void Power() {

  S_Power = S_Voltage * S_Current;
  Serial.println("");
  Serial.print("Solar Power : ");
  Serial.println(S_Power);

  W_Power = W_Voltage * W_Current;
  Serial.println("");
  Serial.print("Wind Power : ");
  Serial.println(W_Power);
}

/*|---------------------------------------------------------------------------------------|
              DUMMY DATA FUNCTION
  |---------------------------------------------------------------------------------------|*/

void DUMMY_DATA() {
  float Avg_RPM = (RPM_T1 + RPM_T2 + RPM_T3) / 3;

  float D_Power = 39.23169 * Avg_RPM - 369.4152;

  float D_Current = D_Power / W_Voltage;

  Serial.println("");
  Serial.print("D_Power : ");
  Serial.println(D_Power);

  Serial.println("");
  Serial.print("D_Current : ");
  Serial.println(D_Current);
}

/*|---------------------------------------------------------------------------------------|
              MODE SWITCHING FUNCTION 
  |---------------------------------------------------------------------------------------|*/

void MODE_SWITCHING() {

  if (RPM_T1 > 10 || RPM_T2 > 10 || RPM_T3 > 10 )  // DEFAULT - ALL PINS LOW
  {
    Serial.println("GEN_MODE + Power -> MODE ");
    SM_1_1 = LOW;  //  [SWITCH:T1 - (1) Motor Mode & (0) Gen Moode]
    SM_1_2 = LOW;  //  [SWITCH:T1 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    SM_2_1 = LOW;  //  [SWITCH:T2 - (1) Motor Mode & (0) Gen Moode]
    SM_2_2 = LOW;  //  [SWITCH:T2 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    SM_3_1 = LOW;  //  [SWITCH:T3 - (1) Motor Mode & (0) Gen Moode]
    SM_3_2 = LOW;  //  [SWITCH:T3 - (1) Power to Batt. Mode & (0) Dump Power Mode]
  }

  if (RPM_T1 < 3 || RPM_T2 < 3 || RPM_T3 < 3 && Wind_Speed > 3)  //  first pin of all 3 module are high for 20 seconds $$$$$$$$
  {
    Serial.println("MOTOR_MODE");

    SM_1_1 = LOW;  //  [SWITCH:T1 - (1) Motor Mode & (0) Gen Moode]
    SM_1_2 = LOW;  //  [SWITCH:T1 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    SM_2_1 = LOW;  //  [SWITCH:T2 - (1) Motor Mode & (0) Gen Moode]
    SM_2_2 = LOW;  //  [SWITCH:T2 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    SM_3_1 = LOW;  //  [SWITCH:T3 - (1) Motor Mode & (0) Gen Moode]
    SM_3_2 = LOW;  //  [SWITCH:T3 - (1) Power to Batt. Mode & (0) Dump Power Mode]

    // Variables to track switch timing
    // unsigned long switchStartTime = millis();
    // const unsigned long switchDuration = 20000;  // 20 seconds in milliseconds
    // if (millis() - switchStartTime <= switchDuration) {
    //   // Turn on the switch
    //   digitalWrite(switchPin, HIGH);
    // } else {
    //   // Turn off the switch if the duration has elapsed
    //   digitalWrite(switchPin, LOW);
    // }
  }

  if ((RPM_T1 > 150 || RPM_T2 > 150 || RPM_T3 > 150) || Wind_Speed > 20 || W_Voltage > 100)  // 2n dpin high for all three mode 
  {
    Serial.println("DUMP_MODE");

    SM_1_1 = LOW;  //  [SWITCH:T1 - (1) Motor Mode & (0) Gen Moode]
    SM_1_2 = HIGH;  //  [SWITCH:T1 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    SM_2_1 = LOW;  //  [SWITCH:T2 - (1) Motor Mode & (0) Gen Moode]
    SM_2_2 = HIGH;  //  [SWITCH:T2 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    SM_3_1 = LOW;  //  [SWITCH:T3 - (1) Motor Mode & (0) Gen Moode]
    SM_3_2 = HIGH;  //  [SWITCH:T3 - (1) Power to Batt. Mode & (0) Dump Power Mode]
  }
}

/*|---------------------------------------------------------------------------------------|
              Router Reset  
  |---------------------------------------------------------------------------------------|*/

void ROUTER_RESET()
{
  if (j == 3) {
    bool internet_check = Ping.ping(REMOTE_HOST, 2);
    if ((!internet_check)) 
    {
      digitalWrite(RELAY_2, LOW);
      delay(5000);
      digitalWrite(RELAY_2, HIGH);
    }
  }
  else
  {
    return;
  }
}
