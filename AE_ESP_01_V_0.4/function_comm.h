/*|---------------------------------------------------------------------------------------|
      Insterion of TAB pin.h [ Libraries + Pin Defination and Declarations ]
    |---------------------------------------------------------------------------------------|*/

#include "function_comp.h"
#include "Root_Certificate.h"

/*|---------------------------------------------------------------------------------------|
     Recalling all the functions at start so that it compile without error
  |---------------------------------------------------------------------------------------|*/

void CONFIG_INTR(void); // Configuration mode interrupt
void ASSIGN_CRED(void); // Assigns Wi-Fi credentials
void RECONNECT(void); // Reconnects to Wi-Fi if connection lost
void HANDLE_ROOT(void); // Handles root HTTP request
void CONFIG_MODE(void); // Enters configuration mode
void FIRMWARE_UPDATE(void); // Updates firmware
int FW_VERSION_CHECK(void); // Checks firmware version
void DEVICE_DETAILS_TO_CLOUD(void); // Sends device details to cloud
void STATUS_TO_CLOUD(void); // Sends status to cloud
void GET_DATA(void); // Gets data
void DATA_TO_CLOUD(void); // Sends data to cloud

/*|---------------------------------------------------------------------------------------|
       Intrupt - 1 - To Configuration Mode  [NOT OPERATING]
  |---------------------------------------------------------------------------------------|*/

void IRAM_ATTR CONFIG_INTR(void) {
  intr_check = 1;
}

/*|---------------------------------------------------------------------------------------|
      Function to assign Wi-Fi credentials[NOT OPERATING]
  |---------------------------------------------------------------------------------------|*/

void ASSIGN_CRED() {
 
  Serial.println(F("Assigning Credential at Global Level"));
  sizes = EEPROM.read(1);
  sizep = EEPROM.read(2);

  if (sizes == 0 || sizep == 0) {
    Serial.println(F("    "));
    Serial.println(F("First Time Device Started"));
  }

  ssid1[sizes + 1];
  pass1[sizep + 1];

  for (int i = 0; i < sizes + 1; i++) {
    ssid1[i] = EEPROM.read(5 + i);
  }

  for (int i = 0; i < sizep + 1; i++) {
    pass1[i] = EEPROM.read(5 + i + sizes);
  }

  Serial.print(F("WIFI SSID:"));
  Serial.println(ssid1);
  Serial.print(F("Password:"));
  Serial.println(pass1);
}

/*|---------------------------------------------------------------------------------------|
      Function to reconnect to Wi-Fi [In case of Wi-Fi Loss]
  |---------------------------------------------------------------------------------------|*/

void RECONNECT() {
 
  if (WiFi.status() != WL_CONNECTED) {
    WIFI_CONNECT_time_now = millis();
    Serial.print("Connecting time = ");
    Serial.println(WIFI_CONNECT_time_now);
    //WiFi.begin(ssid1, pass1);
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    //-DEFAULT CONNECTION ENABLEMENT

    /********************************************************/
    while (WiFi.status() != WL_CONNECTED && millis() - WIFI_CONNECT_time_now <= 10000)  //scans for wifi for 2 seconds
    {
      Serial.print(F("Time Lapsed = "));
      Serial.println((millis() - WIFI_CONNECT_time_now) / 1000);
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Green color Blink while connecting in progress
      pixels.show();
      delay(500);
      Serial.println(F(" CONNECTING ... "));
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
      delay(500);
      Serial.print(F("."));

      // if (intr_check == 1)  //For configuration
      // {
      //   intr_check = 0;
      //   delay(3000);
      //   if (digitalRead(2) == HIGH) {
      //     CONFIG_MODE();
      //   }
      // }
    }

    if (WiFi.status() == WL_CONNECTED) {
      j = 6;
      Serial.println(F("Wi-Fi Network Connected"));
    } else {
      j = 3;
      Serial.println(F("Wi-Fi Network [NOT] Connected "));
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));  // Red color
      pixels.show();
      Serial.println("");
      Serial.print("j = ");
      Serial.println(j);
    }
  }

  else if (j == 6) {
    Serial.println(F("Wi-Fi NETWORK: [CONNECTED] - Checking Internet Accessiability ?"));
    bool internet_check = Ping.ping(REMOTE_HOST, 2);
    // float avg_time_ms = Ping.averageTime();
    if ((internet_check)) {
      j = 6;
      Serial.println("");
      Serial.print("j = ");
      Serial.println(j);
      //if connected exits the function altogether
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Green color
      pixels.show();
      Serial.println(F("CLOUD NETWORK: [CONNECTED] -> Internet Accessiable"));
      Serial.print(("IP address: "));
      Serial.println(WiFi.localIP());

      if (!tb.connected()) {
        tb.connect(thingsboardServer, TOKEN);
      }

    } else {
      // Serial.println(F("NETWORK: [CONNECTED] -> Internet [NOT] Accessiable"));
      j = 3;
      Serial.println("");
      Serial.print("j = ");
      Serial.println(j);
    }
  }
}

/*|------------------------------------------------------------------------------------|
     Function to handle root HTTP request and store inputs in EEPROM  [NOT OPERATING]
  |------------------------------------------------------------------------------------|*/

void HANDLE_ROOT() {
  // 
  String webpage;
  String IPaddress = myIP.toString();
  webpage = "<!DOCTYPE html><html> <head> <title>Configuration</title> <style>body {background-color: #9E2E2E;font-family: Sans-Serif, Sans-Serif, Sans-Serif;Color: white;}.header {font-family: Consolas;color: lightgoldenrodyellow;}.main {border: 1px solid white;border-radius: 10px;padding: 30px;width: 60%;height: auto;}label {text-align: right;padding: 10px;}.textbox, .check {border-radius: 3px;border: 1px solid grey;margin: 10px;transition: all 0.5s;height: 20px;outline: none;padding: 5px;}.textbox:hover, .check:hover {box-shadow: 0px 0px 10px white;}.button {border-radius: 5px;background: #000;border: 1px solid white;padding: 5px;margin: 10px;display: block;color: white;font-size: 16px;transition: all 0.5s;padding: 5px 10px 5px 10px;align-self: center;}.button:hover {background: #fff;color: black;}input[type=checkbox] {width: 10px;height: 10px;border: 1px solid black;background: white;}</style> </head> <body> <center> <div class=\"header\"> <h1> City Greens tech </h1> </div> <div class=\"main\"> <h3>IoT Device Configuration</h3> <hr> <form action=\"http://" + IPaddress + "\" method=\"POST\"> <table class=\"table\"> <tr> <td><label for=\"ssid\">SSID:</label></td> <td><input type=\"text\" name=\"ssid\" class=\"textbox\" placeholder=\"Enter SSID\" id=\"ssid\"></td> </tr> <tr> <td><label for=\"password\">Password: </label></td> <td><input type=\"password\" name=\"password\" class=\"textbox\" placeholder=\"Enter Password\" id=\"password\"></td> </tr><tr> <td><label for=\"Interval\">Time_Interval:</label></td> <td><input type=\"number\" name=\"Time_Interval\" class=\"textbox\" placeholder=\"Interval in Minutes\" id=\"Time_Interval\"></td> </td> </tr> </table> <button type=\"submit\" value=\"Submit\" class=\"button\">Submit</button></form> </div> </center> </body></html>";
  server.send(200, "text/html", webpage);
  if (server.args() > 0) {
    sssid = server.arg(0);
    pass = server.arg(1);

    delay(100);

    Serial.print("SSID:");
    Serial.println(sssid);
    Serial.print("Password:");
    Serial.println(pass);

    char ssid1[sssid.length() + 1];
    sssid.toCharArray(ssid1, sssid.length() + 1);
    char pass1[pass.length() + 1];
    pass.toCharArray(pass1, pass.length() + 1);


    EEPROM.write(1, sizeof(ssid1));
    EEPROM.commit();
    EEPROM.write(2, sizeof(pass1));
    EEPROM.commit();



    for (int i = 0; i < sizeof(ssid1); i++) {
      EEPROM.write(5 + i, ssid1[i]);
      Serial.print(ssid1[i]);
      EEPROM.commit();
    }
    for (int i = 0; i < sizeof(pass1); i++) {
      EEPROM.write(5 + sizeof(ssid1) + i, pass1[i]);
      Serial.print(pass1[i]);
      EEPROM.commit();
    }


    delay(100);

    ESP.restart();
  }
}

/*|---------------------------------------------------------------------------------------|
      Function to enter configuration mode [NOT OPERATING]
  |---------------------------------------------------------------------------------------|*/

void CONFIG_MODE() {
  
  intr_check = 0;

  isInConfigurationMode = true;

  Serial.println(" ");
  Serial.print(F("FUNCTION: "));
  Serial.println(F("CONFIG_MODE"));
  Serial.println(" ");

  Serial.println(F(" "));
  Serial.println(F("Setting up Access Point"));
  WiFi.softAP(ap_ssid, ap_password);

  myIP = WiFi.softAPIP();
  Serial.print(F(" HOTSPOT NAME:  "));
  Serial.println(ap_ssid);
  Serial.print(F(" HOTSPOT PASS:  "));
  Serial.println(ap_password);
  Serial.print(F("AP IP Address is : "));
  Serial.println(myIP);

  server.begin();
  server.on("/", HANDLE_ROOT);  //hosts the webpage

  while (1)
    server.handleClient();
}


/*|-----------------------------------------------------------------------------|
      Function to update firmware
  |-----------------------------------------------------------------------------|*/

void FIRMWARE_UPDATE() {
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : FIRMWARE_UPDATE "));

  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf(" HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
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
    Function to check firmware version
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
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK)  // if version received
      {
        payload = https.getString();  // save received version
      } else {
        Serial.print(" error in downloading version file: ");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK)  // if version received
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
     Function to send device details to cloud
  |---------------------------------------------------------------------------------------|*/

void DEVICE_DETAILS_TO_CLOUD() {
  
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
       STATUS TO CLOUD - > SENDING SWITCH STATUS AND DEVICE UPLINK STATUS
  |-----------------------------------------------------------------------------|*/

void STATUS_TO_CLOUD() {
  // Function to send status to cloud
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : STATUS_TO_CLOUD "));

  if (j == 6) {
    float Wi_Fi_Strength = abs(WiFi.RSSI());
    float Wi_Fi_Strength_P = map(Wi_Fi_Strength, 100, 10, 0, 100);
    Serial.print(F("Wi-FI Strength (%):   "));
    Serial.println(Wi_Fi_Strength_P);
    tb.sendTelemetryData("W_LAN", Wi_Fi_Strength_P);
    tb.sendTelemetryData("ERR_CODE", ERR_CODE);
    Serial.println(F("STATUS sent to Server !"));
  }
}

/*|---------------------------------------------------------------------------------------|
        Function to send data to cloud
  |---------------------------------------------------------------------------------------|*/

void DATA_TO_CLOUD() {
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : DATA_TO_CLOUD "));

  if (j == 6) {
    bool internet_check = Ping.ping(REMOTE_HOST, 2);
    if ((internet_check)) {
      char buf2[500];
      sprintf(buf2, "{\"W_Speed\":%f,\"RPM_T1\":%.1f,\"RPM_T2\":%.1f,\"RPM_T3\":%.1f,\"S_Voltage_1\":%.1f,\"S_Current\":%.1f,\"W_Voltage\":%.1f,\"W_Current\":%.1f,\"S_Power\":%.1f,\"W_Power\":%.1f}", Wind_Speed, RPM_T1, RPM_T2, RPM_T3, S_Voltage, S_Current, W_Voltage, W_Current, S_Power, W_Power);
      tb.sendTelemetryJson(buf2);
      tb.sendTelemetryData("ERR_CODE", ERR_CODE);
      Serial.println(buf2);
      Serial.println(F(" DATA SENT TO SERVER"));
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // Blue color
      pixels.show();
      delay(500);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));  // No color
      pixels.show();
      delay(10);
    } else {
      j=3;
    }
  }
  RECONNECT();
}

/*|---------------------------------------------------------------------------------------|
       Function to send dummy data to cloud
  |---------------------------------------------------------------------------------------|*/

void D_DATA_TO_CLOUD() {
  Serial.println(F(" "));
  Serial.println(F(" ---> FUNCTION : D_DATA_TO_CLOUD "));

  if (j == 6) {
    bool internet_check = Ping.ping(REMOTE_HOST, 2);
    if ((internet_check)) {
      char buf3[500];
      sprintf(buf3, "{\"D_Power\":%f,\"D_Current\":%.1f}", D_Power, D_Current);
      tb.sendTelemetryJson(buf3);
      tb.sendTelemetryData("ERR_CODE", ERR_CODE);
      Serial.println(buf3);
      Serial.println(F(" DATA SENT TO SERVER"));
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // Blue color
      pixels.show();
      delay(500);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));  // No color
      pixels.show();
      delay(10);
    } else {
      return;
    }
  }
}
