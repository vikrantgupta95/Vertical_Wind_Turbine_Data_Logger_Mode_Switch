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
  
  Serial.println("");

  Serial.print("Motor 1 RPM: ");
  Serial.println(RPM_T1);
  Serial.println("");
  Serial.print("Motor 2 RPM: ");
  Serial.println(RPM_T2);
  Serial.println("");
  Serial.print("Motor 3 RPM: ");
  Serial.println(RPM_T3);
}

/*|---------------------------------------------------------------------------------------|
              WIND SPEED FUNCTION - calculate wind speed
  |---------------------------------------------------------------------------------------|*/

void WIND_SPEED() {
  int sensor_value = analogRead(Wind_Speed_Pin);  // Read analog value from wind speed sensor
  windSpeedReading = sensor_value * (3.3 / 4095.0);  // Convert value to voltage (0-3.3V)
  // Apply calibration equation to get wind speed in m/s
  Wind_Speed = -0.5798437 + 5.148 * windSpeedReading + 0.867 * windSpeedReading * windSpeedReading;
  Wind_Speed *= 1.3;  // Apply scaling factor
  Wind_Speed += 0.75; // Add offset

  // Print wind speed to Serial Monitor
  Serial.println("");
  Serial.print("Wind_Speed: ");
  Serial.print(Wind_Speed);
  Serial.println(" m/s");
  delay(1000);  // Wait for a second before reading again
}

/*|---------------------------------------------------------------------------------------|
              WIND CURRENT FUNCTION - read wind current
  |---------------------------------------------------------------------------------------|*/

void Wind_Current() {
  W_CurrentReading = analogRead(W_Current_Pin);  // Read analog value from wind current sensor

  delay(1000);  // Wait for a second before reading again
  W_Current = (W_CurrentReading - 1297) / 13.7;  // Apply conversion equation
  W_Current -= 41.4;
  W_Current /= 1.7;

  // Print wind current to Serial Monitor
  Serial.println("");
  Serial.print("W_Current : ");
  Serial.println(W_Current);
}

/*|---------------------------------------------------------------------------------------|
              SOLAR CURRENT FUNCTION - read solar current
  |---------------------------------------------------------------------------------------|*/

void Solar_Current() {
  S_CurrentReading = analogRead(S_Current_Pin);  // Read analog value from solar current sensor

  delay(1000);  // Wait for a second before reading again
  S_Current = (S_CurrentReading - 1297) / 13.7;  // Apply conversion equation
  S_Current -= 41.4;
  S_Current /= 1.7;

  // Print solar current to Serial Monitor
  Serial.println("");
  Serial.print("S_Current : ");
  Serial.println(S_Current);
}

/*|---------------------------------------------------------------------------------------|
              SOLAR VOLTAGE FUNCTION - to read solar voltage
  |---------------------------------------------------------------------------------------|*/

void Solar_Voltage_1() {
  S_VoltageReading = analogRead(S_Voltage_Pin);  // Read analog value from solar voltage sensor
  S_Voltage = 0.01743626 * S_VoltageReading + 2.586991;  // Apply conversion equation

  // Print solar voltage to Serial Monitor
  Serial.println("");
  Serial.print("Solar Voltage_1: ");
  Serial.println(S_Voltage);
  delay(1000);
}

/*|---------------------------------------------------------------------------------------|
         WIND VOLTAGE FUNCTION - Function to read wind voltage
  |---------------------------------------------------------------------------------------|*/

void Wind_Voltage() {
  W_Voltage = S_Voltage;  // Set wind voltage equal to solar voltage
  // Print wind voltage to Serial Monitor
  Serial.println("");
  Serial.print("Wind Voltage: ");
  Serial.println(W_Voltage);
  delay(1000);
}

/*|---------------------------------------------------------------------------------------|
              POWER FUNCTION - Function to calculate power
  |---------------------------------------------------------------------------------------|*/

void Power() {
  // Calculate solar power
  S_Power = S_Voltage * S_Current;
  Serial.println("");
  Serial.print("Solar Power : ");
  Serial.println(S_Power);

  // Calculate wind power
  W_Power = W_Voltage * W_Current;
  Serial.println("");
  Serial.print("Wind Power : ");
  Serial.println(W_Power);
}

/*|---------------------------------------------------------------------------------------|
              DUMMY DATA FUNCTION - Function to generate dummy data
  |---------------------------------------------------------------------------------------|*/

void DUMMY_DATA() {
  float Avg_RPM = (RPM_T1 + RPM_T2 + RPM_T3) / 3;  // Calculate average RPM

  float D_Power = 39.23169 * Avg_RPM - 369.4152;  // Calculate dummy power

  float D_Current = D_Power / W_Voltage;  // Calculate dummy current

  // Print dummy data to Serial Monitor
  Serial.println("");
  Serial.print("D_Power : ");
  Serial.println(D_Power);

  Serial.println("");
  Serial.print("D_Current : ");
  Serial.println(D_Current);
}

/*|---------------------------------------------------------------------------------------|
              MODE SWITCHING FUNCTION - Function to switch between modes
  |---------------------------------------------------------------------------------------|*/

void MODE_SWITCHING() {
  // Check conditions and switch modes accordingly

  // Default mode: All pins low
  if (RPM_T1 > 10 || RPM_T2 > 10 || RPM_T3 > 10) {
    Serial.println("GEN_MODE + Power -> MODE ");
    digitalWrite(SM_1_1, LOW);
    digitalWrite(SM_1_2, LOW);
    digitalWrite(SM_2_1, LOW);
    digitalWrite(SM_2_2, LOW);
    digitalWrite(SM_3_1, LOW);
    digitalWrite(SM_3_2, LOW);
    digitalWrite(RELAY_3, LOW);
    // Reset stored states
    STSM_1_2 = LOW;
    STSM_2_2 = LOW;
    STSM_3_2 = LOW;
    STSM_1_1 = LOW;
    STSM_2_1 = LOW;
    STSM_3_2 = LOW;
    Serial.println("ALL MODULE + RELAY_3 -> LOW");

    // Reset motor mode timers
    motorModeOnTime = 0;
    motorModeOffTime = 0;
    return;
  }

  // Motor Mode
  if (millis() >= motorModeCooldownEnd) {
    Serial.println("COOL DOWN FINISHED");
    if ((RPM_T1 < 3 || RPM_T2 < 3 || RPM_T3 < 3) && Wind_Speed > 1.5) {
      Serial.println("MOTOR_MODE");
      if (motorModeOnTime == 0) {  // If motor mode is just turned on
        motorModeOnTime = millis();
        motorModeOffTime = motorModeOnTime + motorModeDuration;
        motorModeCooldownEnd = millis() + motorModeCooldownDuration + motorModeDuration;  // Set cooldown end time including motor run time 
        Serial.println("ALL THE TIME ARE SET FOR ACTIONS");
      }
      // Motor mode actions (keep motor mode pins and relay pin HIGH)
      digitalWrite(SM_1_1, HIGH);
      digitalWrite(SM_2_1, HIGH);
      digitalWrite(SM_3_1, HIGH);
      digitalWrite(RELAY_3, HIGH);
      // Store states
      STSM_1_1 = HIGH;
      STSM_2_1 = HIGH;
      STSM_3_1 = HIGH;
      STSM_1_2 = LOW;
      STSM_2_2 = LOW;
      STSM_3_2 = LOW;
      Serial.println("SWITCH MODULE PIN ONE -> HIGH + RELAY_3 -> HIGH");
    }
  }

  // Dump Mode
  else if ((RPM_T1 > 150 || RPM_T2 > 150 || RPM_T3 > 150) || Wind_Speed > 20 || W_Voltage > 100) {
    Serial.println("DUMP_MODE");
    digitalWrite(SM_1_1, LOW);
    digitalWrite(SM_1_2, HIGH);
    digitalWrite(SM_2_1, LOW);
    digitalWrite(SM_2_2, HIGH);
    digitalWrite(SM_3_1, LOW);
    digitalWrite(SM_3_2, HIGH);
    digitalWrite(RELAY_3, LOW);
    // Store states
    STSM_1_1 = LOW;
    STSM_2_1 = LOW;
    STSM_3_1 = LOW;
    STSM_1_2 = HIGH;
    STSM_2_2 = HIGH;
    STSM_3_2 = HIGH;
    Serial.println("SWITCH MODULE PIN 2 -> HIGH + RELAY_3 -> LOW");

    // Reset motor mode timers
    motorModeOnTime = 0;
    motorModeOffTime = 0;
  }

  // Turn off motor mode if it's time
  if (motorModeOffTime != 0 && millis() >= motorModeOffTime) {
    digitalWrite(SM_1_1, LOW);
    digitalWrite(SM_2_1, LOW);
    digitalWrite(SM_3_1, LOW);
    digitalWrite(RELAY_3, LOW);
    // Reset motor mode timers
    motorModeOnTime = 0;
    motorModeOffTime = 0;
    Serial.println("MOTOR SWITCH OFF MET TURNING OFF MOTOR");
    Serial.println("SWITCH MODULE PIN 1 -> LOW + RELAY_3 -> LOW");
    Serial.println("NEXT MOTOR MODE WILL COME AFTER COOL DOWN TIME");
    Serial.println("COOL DOWN STARTED");
  }
}

/*|---------------------------------------------------------------------------------------|
              Function to reset the router 
  |---------------------------------------------------------------------------------------|*/

void ROUTER_RESET() {
  if (j == 3) {
    bool internet_check = Ping.ping(REMOTE_HOST, 2);  // Ping remote host to check internet connection
    if ((!internet_check)) {
      digitalWrite(RELAY_2, LOW);  // Toggle relay to reset router
      delay(5000);
      digitalWrite(RELAY_2, HIGH);
    }
  } else {
    return;
  }
}
