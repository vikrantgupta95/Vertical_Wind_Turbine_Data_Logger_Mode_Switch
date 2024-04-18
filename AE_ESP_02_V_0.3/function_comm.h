/*|---------------------------------------------------------------------------------------|
             Insertion of TAB pin.h [ Libraries + Pin Definition and Declarations ]
    |---------------------------------------------------------------------------------------|*/

#include "function_comp.h"        // Include user-defined function components
#include "Root_Certificate.h"     // Include root certificate for secure connection

/*|---------------------------------------------------------------------------------------|
            Recalling all the functions at start so that it compiles without error
  |---------------------------------------------------------------------------------------|*/

void CONFIG_INTR(void);           // Function declaration for configuration interrupt
void ASSIGN_CRED(void);           // Function declaration for assigning credentials
void RECONNECT(void);             // Function declaration for reconnecting to WiFi
void HANDLE_ROOT(void);           // Function declaration for handling root requests
void CONFIG_MODE(void);           // Function declaration for configuring the mode

void FIRMWARE_UPDATE(void);       // Function declaration for firmware update
int FW_VERSION_CHECK(void);       // Function declaration for firmware version check

void DEVICE_DETAILS_TO_CLOUD(void);  // Function declaration for sending device details to the cloud
void STATUS_TO_CLOUD(void);           // Function declaration for sending status to the cloud
void DATA_TO_CLOUD(void);             // Function declaration for sending data to the cloud

void SUBSCRIBE_RPC(void);             // Function declaration for subscribing to RPC

/*|---------------------------------------------------------------------------------------|
                        Interrupt - 1 - Configuration Mode [NOT OPERATING]
  |---------------------------------------------------------------------------------------|*/

void IRAM_ATTR CONFIG_INTR(void) {
  intr_check = 1;   // Set interrupt check flag to 1
}

/*|---------------------------------------------------------------------------------------|
    Credential Assigning for Wi-Fi Connectivity at Global Variables [NOT OPERATING]
  |---------------------------------------------------------------------------------------|*/

void ASSIGN_CRED() {
  Serial.println(F("Assigning Credentials at Global Level"));  // Print status message
  sizes = EEPROM.read(1);        // Read Wi-Fi SSID size from EEPROM
  sizep = EEPROM.read(2);        // Read Wi-Fi password size from EEPROM

  if (sizes == 0 || sizep == 0) {
    Serial.println(F("    "));
    Serial.println(F("First Time Device Started"));   // Print if it's the first time device is started
  }

  ssid1[sizes + 1];   // Initialize Wi-Fi SSID array
  pass1[sizep + 1];   // Initialize Wi-Fi password array

  for (int i = 0; i < sizes + 1; i++) {
    ssid1[i] = EEPROM.read(5 + i);   // Read Wi-Fi SSID from EEPROM
  }

  for (int i = 0; i < sizep + 1; i++) {
    pass1[i] = EEPROM.read(5 + i + sizes);   // Read Wi-Fi password from EEPROM
  }

  Serial.print(F("Wi-Fi SSID:"));   // Print Wi-Fi SSID
  Serial.println(ssid1);
  Serial.print(F("Password:"));      // Print Wi-Fi password
  Serial.println(pass1);
}

/*|---------------------------------------------------------------------------------------|
                        Reconnect Function [In Case of Wi-Fi Loss]
  |---------------------------------------------------------------------------------------|*/

void RECONNECT() {
  if (WiFi.status() != WL_CONNECTED) {   // If not connected to Wi-Fi
    WIFI_CONNECT_time_now = millis();    // Get current time
    Serial.print("Connecting time = ");
    Serial.println(WIFI_CONNECT_time_now);
    // WiFi.begin(ssid1, pass1);
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);   // Begin Wi-Fi connection with AP and password

    while (WiFi.status() != WL_CONNECTED && millis() - WIFI_CONNECT_time_now <= 10000) {
      Serial.print(F("Time Lapsed = "));
      Serial.println((millis() - WIFI_CONNECT_time_now) / 1000);
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Set LED to green while connecting
      pixels.show();
      delay(500);
      Serial.println(F(" CONNECTING ... "));
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));    // Set LED to off
      pixels.show();
      delay(500);
      Serial.print(F("."));
    }

    if (WiFi.status() == WL_CONNECTED) {
      j = 6;
      Serial.println(F("Wi-Fi Network Connected"));   // Print Wi-Fi connected message
    } else {
      j = 3;
      Serial.println(F("Wi-Fi Network [NOT] Connected "));  // Print Wi-Fi not connected message
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));    // Set LED to red
      pixels.show();
      Serial.println("");
      Serial.print("j = ");
      Serial.println(j);
    }
  } else if (j == 6) {
    Serial.println(F("Wi-Fi NETWORK: [CONNECTED] - Checking Internet Accessibility ?"));
    bool internet_check = Ping.ping(REMOTE_HOST, 2);   // Check internet connectivity

    if (internet_check) {   // If internet is accessible
      j = 6;
      Serial.println("");
      Serial.print("j = ");
      Serial.println(j);
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Set LED to green
      pixels.show();
      Serial.println(F("CLOUD NETWORK: [CONNECTED] -> Internet Accessible"));
      Serial.print(("IP address: "));
      Serial.println(WiFi.localIP());   // Print local IP address

      if (!tb.connected()) {
        tb.connect(thingsboardServer, TOKEN);   // Connect to Thingsboard server
      }
    } else {
      j = 3;
      Serial.println("");
      Serial.print("j = ");
      Serial.println(j);
    }
  }
}

/*|------------------------------------------------------------------------------------|
     HandleClient() Function for HTML Page and Storing Inputs from That in EEPROM [NOT OPERATING]
  |------------------------------------------------------------------------------------|*/

void HANDLE_ROOT() {
  String webpage;   // Define string for webpage
  String IPaddress = myIP.toString();   // Get IP address as string
  webpage = "<!DOCTYPE html><html> <head> <title>Configuration</title> <style>body {background-color: #9E2E2E;font-family: Sans-Serif, Sans-Serif, Sans-Serif;Color: white;}.header {font-family: Consolas;color: lightgoldenrodyellow;}.main {border: 1px solid white;border-radius: 10px;padding: 30px;width: 60%;height: auto;}label {text-align: right;padding: 10px;}.textbox, .check {border-radius: 3px;border: 1px solid grey;margin: 10px;transition: all 0.5s;height: 20px;outline: none;padding: 5px;}.textbox:hover, .check:hover {box-shadow: 0px 0px 10px white;}.button {border-radius: 5px;background: #000;border: 1px solid white;padding: 5px;margin: 10px;display: block;color: white;font-size: 16px;transition: all 0.5s;padding: 5px 10px 5px 10px;align-self: center;}.button:hover {background: #fff;color: black;}input[type=checkbox] {width: 10px;height: 10px;border: 1px solid black;background: white;}</style> </head> <body> <center> <div class=\"header\"> <h1> City Greens tech </h1> </div> <div class=\"main\"> <h3>IoT Device Configuration</h3> <hr> <form action=\"http://" + IPaddress + "\" method=\"POST\"> <table class=\"table\"> <tr> <td><label for=\"ssid\">SSID:</label></td> <td><input type=\"text\" name=\"ssid\" class=\"textbox\" placeholder=\"Enter SSID\" id=\"ssid\"></td> </tr> <tr> <td><label for=\"password\">Password: </label></td> <td><input type=\"password\" name=\"password\" class=\"textbox\" placeholder=\"Enter Password\" id=\"password\"></td> </tr><tr> <td><label for=\"Interval\">Time_Interval:</label></td> <td><input type=\"number\" name=\"Time_Interval\" class=\"textbox\" placeholder=\"Interval in Minutes\" id=\"Time_Interval\"></td> </td> </tr> </table> <button type=\"submit\" value=\"Submit\" class=\"button\">Submit</button></form> </div> </center> </body></html>";
  server.send(200, "text/html", webpage);   // Send HTML webpage to client

  if (server.args() > 0) {   // If there are arguments from the client
    sssid = server.arg(0);   // Get SSID from client
    pass = server.arg(1);     // Get password from client

    delay(100);

    Serial.print("SSID:");
    Serial.println(sssid);
    Serial.print("Password:");
    Serial.println(pass);

    char ssid1[sssid.length() + 1];   // Define SSID character array
    sssid.toCharArray(ssid1, sssid.length() + 1);   // Convert SSID to character array
    char pass1[pass.length() + 1];     // Define password character array
    pass.toCharArray(pass1, pass.length() + 1);     // Convert password to character array

    EEPROM.write(1, sizeof(ssid1));    // Write SSID size to EEPROM
    EEPROM.commit();
    EEPROM.write(2, sizeof(pass1));    // Write password size to EEPROM
    EEPROM.commit();

    for (int i = 0; i < sizeof(ssid1); i++) {
      EEPROM.write(5 + i, ssid1[i]);   // Write SSID to EEPROM
      Serial.print(ssid1[i]);
      EEPROM.commit();
    }
    for (int i = 0; i < sizeof(pass1); i++) {
      EEPROM.write(5 + sizeof(ssid1) + i, pass1[i]);   // Write password to EEPROM
      Serial.print(pass1[i]);
      EEPROM.commit();
    }

    delay(100);

    ESP.restart();   // Restart ESP module
  }
}

/*|---------------------------------------------------------------------------------------|
                   Config_Mode - Program Goes into This On Interrupt [NOT OPERATING]
  |---------------------------------------------------------------------------------------|*/

void CONFIG_MODE() {
  intr_check = 0;   // Reset interrupt check flag

  isInConfigurationMode = true;   // Set configuration mode flag

  Serial.println(" ");
  Serial.print(F("FUNCTION: "));
  Serial.println(F("CONFIG_MODE"));
  Serial.println(" ");

  Serial.println(F(" "));
  Serial.println(F("Setting up Access Point"));   // Print setting up access point message
  WiFi.softAP(ap_ssid, ap_password);   // Set up soft access point with SSID and password

  myIP = WiFi.softAPIP();   // Get soft access point IP address
  Serial.print(F(" HOTSPOT NAME:  "));
  Serial.println(ap_ssid);
  Serial.print(F(" HOTSPOT PASS:  "));
  Serial.println(ap_password);
  Serial.print(F("AP IP Address is : "));
  Serial.println(myIP);

  server.begin();   // Begin server

  server.on("/", HANDLE_ROOT);  // Host the webpage

  while (1)   // Infinite loop to handle client requests
    server.handleClient();
}

/*|-----------------------------------------------------------------------------|
   Firmware Update
  |-----------------------------------------------------------------------------|*/

void FIRMWARE_UPDATE() {
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : FIRMWARE_UPDATE "));

  WiFiClientSecure client;   // Initialize secure client for firmware update
  client.setCACert(rootCACertificate);   // Set root CA certificate for secure connection
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);   // Perform firmware update

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf(" HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println(" HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println(" HTTP_UPDATE_OK");
      break;
  }
}

/*|-----------------------------------------------------------------------------|
    Firmware Version Check
  |-----------------------------------------------------------------------------|*/

int FW_VERSION_CHECK(void) {
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : FW_VERSION_CHECK "));

  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  WiFiClientSecure *client = new WiFiClientSecure;

  if (client) {
    client->setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
    HTTPClient https;

    if (https.begin(*client, fwurl)) {
      // Start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK)  // If version received
      {
        payload = https.getString();  // Save received version
      } else {
        Serial.print(" error in downloading version file: ");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK)  // If version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.print(F(" Latest Firmware Exist-> Firmware Version: "));
      Serial.println(FirmwareVer);
      return 0;
    } else {
      Serial.print("New firmware found: ");
      Serial.println(payload);
      Serial.println("Uploading new firmware");
      return 1;
    }
  }
  return 0;
}

/*|---------------------------------------------------------------------------------------|
    Device Details to Cloud
  |---------------------------------------------------------------------------------------|*/

void DEVICE_DETAILS_TO_CLOUD()  // Type - 1
{
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : DEVICE_DETAILS_TO_CLOUD "));

  if (j == 6) {
    char buf4[300];
    tb.sendTelemetryData("D_ID", "AE_0002");
    tb.sendTelemetryData("HW_VER", HW_VER);
    tb.sendTelemetryData("FW_VER", FW_VER);

    Serial.println(F("[DEVICE DETAILS]: DATA SENT TO SERVER"));
  } else if (j == 3) {
    Serial.println("[Internet Not available] : Failed Sending Device Detail : ");
  }
}

/*|-----------------------------------------------------------------------------|
       Status to Cloud - Sending Switch Status and Device Uplink Status
  |-----------------------------------------------------------------------------|*/

void STATUS_TO_CLOUD()  // Type: 5
{
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : STATUS_TO_CLOUD "));

  if (j == 6) {
    float Wi_Fi_Strength = abs(WiFi.RSSI());
    float Wi_Fi_Strength_P = map(Wi_Fi_Strength, 100, 10, 0, 100);
    Serial.print(F("Wi-Fi Strength (%):   "));
    Serial.println(Wi_Fi_Strength_P);
    tb.sendTelemetryData("W_LAN", Wi_Fi_Strength_P);
    tb.sendTelemetryData("ERR_CODE", ERR_CODE);
    Serial.println(F("STATUS sent to Server !"));
  }
}

/*|---------------------------------------------------------------------------------------|
       Data to Cloud
  |---------------------------------------------------------------------------------------|*/

void DATA_TO_CLOUD() {
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : DATA_TO_CLOUD "));

  if (j == 6) {
    bool internet_check = Ping.ping(REMOTE_HOST, 2);
    if ((internet_check)) {
      char buf3[500];
      sprintf(buf3, "{\"S_Voltage_2\":%f,\"S_Current_2\":%.1f}", S_Voltage_2, S_Current_2);
      tb.sendTelemetryJson(buf3);
      tb.sendTelemetryData("ERR_CODE", ERR_CODE);
      Serial.println(buf3);
      Serial.println(F(" DATA SENT TO SERVER"));
    } else {
      j = 3;
    }
  }
  RECONNECT();   // Reconnect to Wi-Fi
}

/*|---------------------------------------------------------------------------------------|
       RPC Call Function for Switch State - Remote Switch
  |---------------------------------------------------------------------------------------|*/

RPC_Response processSetGpioState(const RPC_Data &data) {
  Serial.println("Received the set GPIO RPC method");

  int pin = data["pin"];
  bool enabled = data["enabled"];

  Serial.print("Setting GPIO ");
  Serial.print(GPIO_21);
  Serial.print(" to state ");
  Serial.println(enabled);

  digitalWrite(GPIO_21, enabled);   // Set GPIO state

  return RPC_Response(data["pin"], (bool)data["enabled"]);   // Return response
}

RPC_Callback callbacks[] = {
  { "setState", processSetGpioState },
};

/*|---------------------------------------------------------------------------------------|
       RPC Switching Function in Loop
  |---------------------------------------------------------------------------------------|*/

void SUBSCRIBE_RPC() {
  if (!tb.connected()) {
    subscribed = false;

    if (!tb.connect(thingsboardServer, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  // Subscribe for RPC, if needed
  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }
  
  // Process messages
  tb.loop();
}
