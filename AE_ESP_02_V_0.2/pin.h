/* |-----------------------------------------------------------------------------|
                    Including Attaching TAB Libraries & Definition
   |-----------------------------------------------------------------------------| */
#include "Arduino.h"
#define pin_h

/* |-----------------------------------------------------------------------------|
                            Including Libraries Used
   |-----------------------------------------------------------------------------| */
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <DNSServer.h>
#include <string.h>       // Version 5.13.2  //Arduino JSON Version 6.15.2
#include <NTPClient.h>    // Version 3.1.0
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

/* |------------------------------------------------------------------------------|
                             Definition - Wi-Fi and NTP
   |------------------------------------------------------------------------------| */
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
WebServer server(80);         // Object of WebServer(HTTP port, 80 is default)
HTTPClient http;
// SoftwareSerial s(D6, D5);  //RX ,TX

/* |-----------------------------------------------------------------------------|
                       Configuration Variables  -- [NEED UPDATE CONFIGURABLE]
   |-----------------------------------------------------------------------------| */
int sizes = 0;     // Size of SSID
int sizep = 0;     // Size of Password
char ssid1[30];
char pass1[30];
String sssid;
String pass;
int ERR_CODE = 0;    // 0 = ALL OKAY | 1 = SENSOR DATA ERROR | 2 = SENSOR TIMEOUT | 3 = SD CARD FAILED | 4 = RTC ERROR |

/* |-----------------------------------------------------------------------------|
                       Default Values and Server Details
   |-----------------------------------------------------------------------------| */
const char* Default_STA_SSID = "Apeiro E";                         // Station mode credentials
const char* Default_STA_PASS = "12345678";
const char* ap_ssid = "Apeiro_E_V2.0";                             // Access Point Mode credentials
const char* ap_password = "12345678";
#define WIFI_AP "apeiroenergy"
#define WIFI_PASSWORD "12345678"
#define REMOTE_HOST "www.google.com"               // Host name for internet connectivity check

/* |-----------------------------------------------------------------------------|
                       Updateable Data Device Wise -
   |-----------------------------------------------------------------------------| */
// Define ThingsBoard server address + token + OTA Details 
#define TOKEN "54ON2zZvrJ66zUxzLjWB"
char thingsboardServer[] = "thingsboard.cloud";
#define URL_fw_Version "https://raw.githubusercontent.com/citygreenstech2019/wadda/master/bin_version.txt"  // URL for firmware version
#define URL_fw_Bin "https://raw.githubusercontent.com/citygreenstech2019/wadda/master/fw.bin"                 // URL for firmware binary
String FirmwareVer = "0.01";   // Firmware VERSION
String HardwareVer = "2.0";    // Hardware VERSION
String DeviceID = "AE0002";    // Device ID 
float FW_VER = FirmwareVer.toFloat();
float HW_VER = HardwareVer.toFloat();

/* |-----------------------------------------------------------------------------|
                       Switch Control Declaration and Definition
   |-----------------------------------------------------------------------------| */
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
int GPIO_21 = 2;   // Remote Control Switch
bool subscribed = false;

/* |-----------------------------------------------------------------------------|
                           Conditional Variables [MISC]
   |-----------------------------------------------------------------------------| */
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

/* |-----------------------------------------------------------------------------|
                                Pins Declarations: OUTPUT
   |-----------------------------------------------------------------------------| */
#define RGB_LED  26   // RGB LED COLORS FOR CONNECTIVITY STATE
#define NUMPIXELS   1
Adafruit_NeoPixel pixels(NUMPIXELS, RGB_LED, NEO_GRB + NEO_KHZ800);
#define INTR_PIN 2  // CONFIG INTR

/* |-----------------------------------------------------------------------------|
                                Pins Declarations: INPUT
   |-----------------------------------------------------------------------------| */
const int S_Voltage_2_Pin = 27;   // Solar voltage analog pin
const int S_Current_2_Pin = 34;   // Solar current analog pin

/* |-----------------------------------------------------------------------------|
                          Data Variables - RAW
   |-----------------------------------------------------------------------------| */
float S_VoltageReading_2;   // Variable to store solar voltage reading
float S_CurrentReading_2;   // Variable to store solar current reading

/* |-----------------------------------------------------------------------------|
                         Data Variables - ACTUAL -> Post Conversion
   |-----------------------------------------------------------------------------| */
float S_Voltage_2 = 0;   // Solar voltage
float S_Current_2 = 0;   // Solar current
float S_Power_2 = 0;     // Solar power
