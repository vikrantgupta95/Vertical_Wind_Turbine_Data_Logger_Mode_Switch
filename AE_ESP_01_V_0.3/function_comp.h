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
  //if (calculateRPM) {
  // Display the RPM of each motor
  Serial.println("");

  Serial.print("Motor 1 RPM: ");
  Serial.println(RPM_T1);
  Serial.println("");
  Serial.print("Motor 2 RPM: ");
  Serial.println(RPM_T2);
  Serial.println("");
  Serial.print("Motor 3 RPM: ");
  Serial.println(RPM_T3);
  // }

  // Add your condition to stop RPM calculation
  // For example, after 10 seconds
  //  if (millis() > 10000) {
  //    calculateRPM = false;
  //  }
}

/*|---------------------------------------------------------------------------------------|
              WIND SPEED FUNCTION
  |---------------------------------------------------------------------------------------|*/

void WIND_SPEED() {
  int sensor_value = analogRead(Wind_Speed_Pin);
  windSpeedReading = sensor_value * (3.3 / 4095.0);  // Convert the value to voltage (0-3.3V)
  //Wind_Speed = 7.807121 * windSpeedReading - 2.224219; // Apply the calibration equation
  Wind_Speed = -0.5798437 + 5.148 * windSpeedReading + 0.867 * windSpeedReading * windSpeedReading;
  Wind_Speed *= 1.3;
  Wind_Speed += 0.75;
  //  if (Wind_Speed < 0)
  //    Wind_Speed = 0;


  // Print the wind speed to the Serial Monitor
  Serial.println("");
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
  //W_VoltageReading = analogRead(W_Voltage_Pin);
  //W_Voltage = 0.01743626 * W_VoltageReading + 2.586991;
  W_Voltage = S_Voltage;
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

  if (RPM_T1 > 10 || RPM_T2 > 10 || RPM_T3 > 10)  // DEFAULT - ALL PINS LOW
  {
    Serial.println("GEN_MODE + Power -> MODE ");
    digitalWrite(SM_1_1, LOW);  //  [SWITCH:T1 - (1) Motor Mode & (0) Gen Moode]
    digitalWrite(SM_1_2, LOW);  //  [SWITCH:T1 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    digitalWrite(SM_2_1, LOW);  //  [SWITCH:T2 - (1) Motor Mode & (0) Gen Moode]
    digitalWrite(SM_2_2, LOW);  //  [SWITCH:T2 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    digitalWrite(SM_3_1, LOW);  //  [SWITCH:T3 - (1) Motor Mode & (0) Gen Moode]
    digitalWrite(SM_3_2, LOW);  //  [SWITCH:T3 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    digitalWrite(RELAY_3, LOW);
    STSM_1_2 = LOW;
    STSM_2_2 = LOW;
    STSM_3_2 = LOW;
    STSM_1_1 = LOW;
    STSM_2_1 = LOW;
    STSM_3_2 = LOW;
    
    Serial.println("ALL MODULE + RELAY_3 -> LOW");

    motorModeOnTime = 0;   // Reset motor mode on time
    motorModeOffTime = 0;  // Reset motor mode off time
    return;
  }

  if (millis() >= motorModeCooldownEnd) {

    Serial.println("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    // Motor Mode
    Serial.println("COOL DOWN FINISHED");
    if ((RPM_T1 < 3 || RPM_T2 < 3 || RPM_T3 < 3) && Wind_Speed > 1) {
      Serial.println("MOTOR_MODE");

      if (motorModeOnTime == 0) {  // If motor mode is just turned on
        motorModeOnTime = millis();
        motorModeOffTime = motorModeOnTime + motorModeDuration;
        motorModeCooldownEnd = millis() + motorModeCooldownDuration + motorModeDuration;  // Set cooldown end time including motor run time 
        Serial.println("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        Serial.println("ALL THE THE TIME ARE SET FOR ACTIONS");
      }

      // Motor mode actions (keep motor mode pins and relay pin HIGH)
      digitalWrite(SM_1_1, HIGH);
      digitalWrite(SM_2_1, HIGH);
      digitalWrite(SM_3_1, HIGH);
      digitalWrite(RELAY_3, HIGH);
      STSM_1_1 = HIGH;
      STSM_2_1 = HIGH;
      STSM_3_1 = HIGH;
      STSM_1_2 = LOW;
      STSM_2_2 = LOW;
      STSM_3_2 = LOW;
      Serial.println("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
      Serial.println("SWITHC MODULE PIN ONE ARE -> HIGH + RELAY_3 -> HIGH");
    }
  }

  // Dump Mode
  else if ((RPM_T1 > 150 || RPM_T2 > 150 || RPM_T3 > 150) || Wind_Speed > 20 || W_Voltage > 100)  // 2n dpin high for all three mode
  {
    Serial.println("DUMP_MODE");

    digitalWrite(SM_1_1, LOW);   //  [SWITCH:T1 - (1) Motor Mode & (0) Gen Moode]
    digitalWrite(SM_1_2, HIGH);  //  [SWITCH:T1 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    digitalWrite(SM_2_1, LOW);   //  [SWITCH:T2 - (1) Motor Mode & (0) Gen Moode]
    digitalWrite(SM_2_2, HIGH);  //  [SWITCH:T2 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    digitalWrite(SM_3_1, LOW);   //  [SWITCH:T3 - (1) Motor Mode & (0) Gen Moode]
    digitalWrite(SM_3_2, HIGH);  //  [SWITCH:T3 - (1) Power to Batt. Mode & (0) Dump Power Mode]
    digitalWrite(RELAY_3, LOW);
    STSM_1_1 = LOW;
    STSM_2_1 = LOW;
    STSM_3_1 = LOW;
    STSM_1_2 = HIGH;
    STSM_2_2 = HIGH;
    STSM_3_2 = HIGH;
    Serial.println("SWITHC MODULE PIN 2 -> HIGH + RELAY_3 -> LOW");

    motorModeOnTime = 0;   // Reset motor mode on time
    motorModeOffTime = 0;  // Reset motor mode off time
  }


  if (motorModeOffTime != 0 && millis() >= motorModeOffTime) {
    digitalWrite(SM_1_1, LOW);
    digitalWrite(SM_2_1, LOW);
    digitalWrite(SM_3_1, LOW);
    digitalWrite(RELAY_3, LOW);
    motorModeOnTime = 0;   // Reset motor mode on time
    motorModeOffTime = 0;  // Reset motor mode off time

    Serial.println("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    Serial.println("MOTOR SWITH OFF MET TURNING OFF MOTOR");
    Serial.println("SWITHC MODULE PIN 1 -> LOW + RELAY_3 -> LOW");
    Serial.println("NEXT MOTOR MODE WILL COME AFTER COOL DOWN TIME -> -> -> ");
    Serial.println("->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    Serial.println("COOL DOWN STARTED ->>>>>>>>>>>>>>>>>>>>");
  }
}

/*|---------------------------------------------------------------------------------------|
              Router Reset
  |---------------------------------------------------------------------------------------|*/

void ROUTER_RESET() {
  if (j == 3) {
    bool internet_check = Ping.ping(REMOTE_HOST, 2);
    if ((!internet_check)) {
      digitalWrite(RELAY_2, LOW);
      delay(5000);
      digitalWrite(RELAY_2, HIGH);
    }
  } else {
    return;
  }
}

void OUTPUT_TEST() {
  digitalWrite(SM_1_1, HIGH);
  digitalWrite(SM_1_2, HIGH);
  digitalWrite(SM_2_1, HIGH);
  digitalWrite(SM_2_2, HIGH);
  digitalWrite(SM_3_1, HIGH);
  digitalWrite(SM_3_2, HIGH);

  delay(2000);

  digitalWrite(SM_1_1, LOW);
  digitalWrite(SM_1_2, LOW);
  digitalWrite(SM_2_1, LOW);
  digitalWrite(SM_2_2, LOW);
  digitalWrite(SM_3_1, LOW);
  digitalWrite(SM_3_2, LOW);

  delay(2000);

  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);

  delay(2000);

  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);

  delay(2000);
}
