/*|-----------------------------------------------------------------------------|
                  Including Attaching TAB Libraries & Defination
  |-----------------------------------------------------------------------------|*/
#include "Arduino.h"
#define pin_h

/*|-----------------------------------------------------------------------------|
                           Including Libraries Used
  |-----------------------------------------------------------------------------|*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <DNSServer.h>
#include <string.h>     // V_5.13.2  //Arduino JSON Version 6.15.2
#include <NTPClient.h>  // V_3.1.0
#include <WiFiUdp.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include "time.h"
#include <WiFiClientSecure.h>
#include "esp_system.h"
#include <Adafruit_NeoPixel.h>


/* | ---------------------------------------------------------------------------- - |
                              Defination - Wi-Fi and NTP
  | ---------------------------------------------------------------------------- - |*/

// Define NTP server and UTC offset
#define NTP_SERVER "0.in.pool.ntp.org"
const long utcOffsetInSeconds = 19800;

// Set up UDP client for NTP and initialize NTP client
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19800);

DynamicJsonDocument doc(4000);

WiFiClient espClient;

ThingsBoard tb(espClient);

IPAddress myIP;

WebServer server(80);        // Object of WebServer(HTTP port, 80 is defult)

HTTPClient http;

//SoftwareSerial s(D6, D5);  //RX ,TX


/*|---------------------------------------------------------------------------------------|
      Configuration Variables  -- [NEED UPDATE CONFIGURABLE]
  |---------------------------------------------------------------------------------------|*/

int sizes = 0;    //Size of SSID
int sizep = 0;    //Size of Password

char ssid1[30];
char pass1[30];

String sssid;
String pass;

int ERR_CODE = 0;   // 0 = ALL OKEY | 1 = SENSOR DATA ERROR | 2 = SENSOR TIME OUT | 3 = SD CARD FAILED | 4 = RTC ERROR |

/*|---------------------------------------------------------------------------------------|
      Default Values and Server Details
  |---------------------------------------------------------------------------------------|*/

const char* Default_STA_SSID  = "Apeiro E";                     //Station mode credentials
const char* Default_STA_PASS  = "12345678";

const char* ap_ssid         = "Apeiro_E_V2.0";                     //Acess Point Mode credentials
const char* ap_password     = "12345678";

#define WIFI_AP             "Apeiro_E"
#define WIFI_PASSWORD       "12345678"

#define REMOTE_HOST       "www.google.com"               //Enter the host name to ping for checking the internet


/*|---------------------------------------------------------------------------------------|
      Updateable Data Device Wise -   
  |---------------------------------------------------------------------------------------|*/

// Define ThingsBoard server address and token
#define TOKEN "O0EIL1ZiNNBVc3LAs5Ce"
char thingsboardServer[] = "thingsboard.cloud";

#define URL_fw_Version    "https://raw.githubusercontent.com/citygreenstech2019/wadda/master/bin_version.txt"  //Enter the web-address of your stored firmware version on the github
#define URL_fw_Bin        "https://raw.githubusercontent.com/citygreenstech2019/wadda/master/fw.bin"           //Enter the web-address of your stored firmware binary on the github

String FirmwareVer = "0.01";   //FW VERSION
String HardwareVer = "2.0";    //HW VERSION

String DeviceID = "AE0002";    //Device ID 
//String DeviceID = "IH0019";    //Device ID 

float FW_VER = FirmwareVer.toFloat();
float HW_VER = HardwareVer.toFloat();


/*|---------------------------------------------------------------------------------------|
        Conditional Variables [MISC]
  |---------------------------------------------------------------------------------------|*/

bool intr_check = 0;
int j;
int k;
float Wi_Fi_Strength;
float Wi_Fi_Strength_P;
unsigned long WIFI_CONNECT_time_now = 0;
bool isInConfigurationMode = false;
unsigned long ST_TO_CLOUD_time_now = 0;

unsigned long motorModeStartTime = 0;
const unsigned long motorModeDuration = 20000; // 20 seconds in milliseconds




/* | ---------------------------------------------------------------------------- - |
                             Pins Declarations : OUTPUT
  | ---------------------------------------------------------------------------- - |*/

int RELAY_1 = 17;  // (12V) FAN:       ON - 1 || OFF - 0  [Enable/Disable Cooling Fan ]
int RELAY_2 = 4;   // (12V) DONGLE:    ON - 1 || OFF - 0  [Enable/Disable Dongle Power]
int RELAY_3 = 16;  // (5V)  NANO PCB : ON - 1 || OFF - 0  [Turbine Runs in Motor Mode ]

int SM_1_1 = 23;  // (12V) SM1-1: ON - 1 || OFF - 0  [SWITCH:T1 - (1) Motor Mode & (0) Gen Moode]
int SM_1_2 = 22;  // (12V) SM1-2: ON - 1 || OFF - 0  [SWITCH:T1 - (1) Power to Batt. Mode & (0) Dump Power Mode]
int SM_2_1 = 21;  // (12V) SM2-1: ON - 1 || OFF - 0  [SWITCH:T2 - (1) Motor Mode & (0) Gen Moode]
int SM_2_2 = 19;  // (12V) SM2-2: ON - 1 || OFF - 0  [SWITCH:T2 - (1) Power to Batt. Mode & (0) Dump Power Mode]
int SM_3_1 = 18;  // (12V) SM3-1: ON - 1 || OFF - 0  [SWITCH:T3 - (1) Motor Mode & (0) Gen Moode]
int SM_3_2 = 5;   // (12V) SM3-2: ON - 1 || OFF - 0  [SWITCH:T3 - (1) Power to Batt. Mode & (0) Dump Power Mode]

#define RGB_LED  26   // RGB LED COLOURS FOR CONNECTIVITY STATE
#define NUMPIXELS   1
Adafruit_NeoPixel pixels(NUMPIXELS, RGB_LED, NEO_GRB + NEO_KHZ800);

#define INTR_PIN 2  // CONFIG INTR

/* | ---------------------------------------------------------------------------- - |
                             Pins Declarations : INPUT
  | ---------------------------------------------------------------------------- - |*/

const int S_Voltage_Pin = 39;  // [Analog Value: 0V - 3V]
const int S_Current_Pin = 36;  // [Analog Value: 1.6V - 3V]

const int W_Voltage_Pin = 34;  // [Analog Value: 0V - 3V]
const int W_Current_Pin = 35;  // [Analog Value: 1.6V - 3V]

const int RPM_T1_Pin = 25;     // [High/Low Pulse input Interupt]
const int RPM_T2_Pin = 33;     // [High/Low Pulse input Interupt]
const int RPM_T3_Pin = 32;     // [High/Low Pulse input Interupt]

const int Wind_Speed_Pin = 27; // [Analog Value: 0V - 3V]


/* | ---------------------------------------------------------------------------- - |
                             Data Variables - RAW 
  | ---------------------------------------------------------------------------- - |*/

float S_VoltageReading;  // Variable to store solar voltage reading
float S_CurrentReading;  // Variable to store solar current reading

float W_VoltageReading;   // Variable to store wind voltage reading
float W_CurrentReading;   // Variable to store wind current reading

volatile int turbine1RPM;   // Variable to store turbine 1 RPM reading
volatile int turbine2RPM;   // Variable to store turbine 2 RPM reading
volatile int turbine3RPM;   // Variable to store turbine 3 RPM reading

float windSpeedReading;     // Variable to store wind speed reading

/* | ---------------------------------------------------------------------------- - |
                             Data Variables - ACTUAL -> Post Conversion 
  | ---------------------------------------------------------------------------- - |*/

float S_Voltage = 0;  // Variable to store solar voltage reading
float S_Current = 0;  // Variable to store solar current reading

float W_Voltage = 0;   // Variable to store wind voltage reading
float W_Current = 0;   // Variable to store wind current reading

volatile int pulseCountMotor1 = 0;
volatile int pulseCountMotor2 = 0;
volatile int pulseCountMotor3 = 0;

int RPM_T1;   // Variable to store turbine 1 RPM reading
int RPM_T2;   // Variable to store turbine 2 RPM reading
int RPM_T3;   // Variable to store turbine 3 RPM reading

float S_Power;
float W_Power;

float D_Power;
float D_Current;

bool calculateRPM = true; // Flag to control RPM calculation

float Wind_Speed = 0.0;     // Variable to store wind speed reading
