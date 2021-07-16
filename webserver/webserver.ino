#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>
#define BLYNK_PRINT Serial
#include"html.h";
#include"dongbo.h";
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>
ESP8266WebServer webServer(80);
DNSServer dnsServer;
char* ssid_ap = "STAUG";
char* pass_ap = "12345678";
const IPAddress apIP(192, 168, 1, 1);
IPAddress subnet_ap(255, 255, 255, 0);
String statusD1 = "1", statusD2 = "1", statusD3 = "1", statusD4 = "1";
String ssid;
String pass;
String auth;
boolean wifimode;
BlynkTimer timer;
void setup() {
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);
  digitalWrite(ledPin3, ledState3);
  digitalWrite(ledPin4, ledState4);
  digitalWrite(ledPin5, ledState5);
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT_PULLUP);
  pinMode(btnPin4, INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  EEPROM.begin(512);       //Khởi tạo bộ nhớ EEPROM
  delay(10);
  if (read_EEPROM()) {
    if (checkConnection()) {
      Blynk.begin(auth.c_str(), ssid.c_str(), pass.c_str());
      wifimode = 0;
    }
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  EEPROM.begin(512);
  WiFi.softAP(ssid_ap, pass_ap);
  dnsServer.start(53, "*", apIP);
  wifimode = 1;
  webServer.on("/", mainpage);
  webServer.on("/getSTATUSD", get_STATUSD);
  webServer.on("/D1on", D1_on);
  webServer.on("/D1off", D1_off);
  webServer.on("/D2on", D2_on);
  webServer.on("/D2off", D2_off);
  webServer.on("/D3on", D3_on);
  webServer.on("/D3off", D3_off);
  webServer.on("/D4on", D4_on);
  webServer.on("/D4off", D4_off);
  webServer.on("/Allon", All_on);
  webServer.on("/Alloff", All_off);
  webServer.on("/getIP", get_IP);
  webServer.on("/writeEEPROM", write_EEPROM);
  webServer.on("/restartESP", restart_ESP);
  webServer.on("/clearEEPROM", clear_EEPROM);
  webServer.begin();
}
void loop() {
  webServer.handleClient();
  Blynk.run();
  timer.run();
}


void mainpage() {
  String s = FPSTR(MainPage);
  webServer.send(200, "text/html", s);
}

void get_STATUSD() {
  if (digitalRead(D1) == 0) {
    statusD1 = "0";
  } else {
    statusD1 = "1";
  }
  if (digitalRead(D2) == 0) {
    statusD2 = "0";
  } else {
    statusD2 = "1";
  }
  if (digitalRead(D3) == 0) {
    statusD3 = "0";
  } else {
    statusD3 = "1";
  }
  if (digitalRead(D4) == 0) {
    statusD4 = "0";
  } else {
    statusD4 = "1";
  }
  String s = "{\"D1\": \"" + statusD1 + "\"," +
             "\"D2\": \"" + statusD2 + "\"," +
             "\"D3\": \"" + statusD3 + "\"," +
             "\"D4\": \"" + statusD4 + "\"}";
  webServer.send(200, "application/json", s);
}
void D1_on() {
  Serial.println("on");
  digitalWrite(D1, LOW);
  get_STATUSD();
}
void D1_off() {
  digitalWrite(D1, HIGH);
  get_STATUSD();
}

void D2_on() {
  digitalWrite(D2, LOW);
  get_STATUSD();
}
void D2_off() {
  digitalWrite(D2, HIGH);
  get_STATUSD();
}

void D3_on() {
  digitalWrite(D3, LOW);
  get_STATUSD();
}
void D3_off() {
  digitalWrite(D3, HIGH);
  get_STATUSD();
}

void D4_on() {
  digitalWrite(D4, LOW);
  get_STATUSD();
}
void D4_off() {
  digitalWrite(D4, HIGH);
  get_STATUSD();
}

void All_on() {
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  get_STATUSD();
}
void All_off() {
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  get_STATUSD();
}

void get_IP() {
  String s = WiFi.localIP().toString();
  webServer.send(200, "text/html", s);
}
boolean checkConnection() {
  WiFi.begin(ssid.c_str(), pass.c_str());;
  int count = 0;
  while (count < 50) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      return (true);
    }
    Serial.print(".");
    count++;
    delay(500);
  }
  Serial.println("Timed out.");
  return false;
}
//boolean checkConnection() {
//  int count = 0;
//  while (count < 50) {
//    if (WiFi.status() == WL_CONNECTED) {
//      return true;
//    }
//    delay(200);
//    count++;
//  }
//  return false;
//}
boolean read_EEPROM() {
  Serial.println("Reading EEPROM...");
  if (EEPROM.read(0) != 0) {
    ssid = "";
    pass = "";
    auth = "";
    for (int i = 0; i < 32; ++i) {
      ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(ssid);
    for (int i = 32; i < 96; ++i) {
      pass += char(EEPROM.read(i));
    }
    Serial.print("PASSWORD: ");
    Serial.println(pass);
    for (int i = 96; i < 128; ++i) {
      auth += char(EEPROM.read(i));
    }
    return true;
  } else {
    Serial.println("Data wifi not found!");
    return false;
  }
}

void write_EEPROM() {
  ssid = webServer.arg("ssid");
  pass = webServer.arg("pass");
  auth = webServer.arg("auth");
  Serial.println("Clear EEPROM!");
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(auth);
  for (int i = 0; i < 128; ++i) {
    EEPROM.write(i, 0);
    delay(10);
  }
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(i, ssid[i]);
  }
  for (int i = 0; i < pass.length(); ++i) {
    EEPROM.write(32 + i, pass[i]);
  }
  for (int i = 0; i < auth.length(); ++i) {
    EEPROM.write(96 + i, auth[i]);
  }
  EEPROM.commit();
  Serial.println("Writed to EEPROM!");
  String s = "Wifi configuration saved!";
  webServer.send(200, "text/html", s);
}
void restart_ESP() {
  ESP.restart();
}
void clear_EEPROM() {
  for (int i = 0; i < 512; ++i) {
    EEPROM.write(i, 0);
    delay(10);
  }
  EEPROM.commit();
  String s = "Device has been reset!";
  webServer.send(200, "text/html", s);
}
