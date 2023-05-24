//Software gestione Giorgi Salvavita 23/05/2023 5Au

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "SPIFFS.h"
#include <Update.h>
#include <WebServer.h>
#include "ESPAsyncWebServer.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SCD30.h>
#include <math.h>             // Includo la libreria della matematica

//********************************************
// Variabili Globali
int CO=0;     // Misura del valore di co
int COOld=0;  // Valore vecchio

int CO2=0;    // Misura CO2
int CO2Old=0; // Valore vecchio

int umidita1=0;  // Misura umidità
int umiditaOld=0; // Valore vecchio

float temperatura=0;   // temperatura
float temperaturaOld=0;

int RC=10;    // costante di tempo filtro
int cicalinoPin0=16;
int LED1=15;     // led rosso di segnalazione
int All=0; // variabile di allarme 0/1;
int scr_pos = 0;         // scroll per display
int pulsante=0;
int TastieraA=39;
int TastieraAR=0; 


//******* Soglie di protezione *******

//int COMax=1200;
//int CO2Max=1300;
//float temperaturaMax=40;
//int umiditaMax=90;
//***********************************
// Definizione intervalli convertitore ADC per riconoscimento tasto premuto caso 10bit
int tasto1[]  =    {0,80};
int tasto2[]    =  {80,200};
int tasto3[]   =   {200,450};
int tasto4[]    =  {450,600};
//**********************************
//variabili server


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


#define TASK_A 1
#define TASK_B 1
#define TASK_C 1
#define TASK_D 0
#define DEBUG 0

// Definizione variabili del display OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define Num_Dati  5      // numero di pagine da visualizzare su display

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SCD30  scd30;         // sensore CO2


//ADC AVG
const int numReadings = 7;
int readings[numReadings]; // the readings from the analog input
int readIndex = 0; // the index of the current reading
uint16_t adcAVG;
uint16_t adcRead;
float total = 0; // the running total

//variabili sensori da sovrascirvere con i dati provenienti dai sensori reali
//float temperature=25.0,sogliaTemperatura=40.0, co2=1123.0,sogliaCo2=1200.0, co=12.0,sogliaCo=200.0, umidita=70.0,sogliaUmidita=99.0 ;

float temperature=25;
float sogliaTemperatura=40.0;

int umidita=70;
int sogliaUmidita=99;

int co2=1000;
int sogliaCo2=1200;

int co=1200;
int sogliaCo=1100;


//gpio
const int gpio04 = 4;
const int gpio12 = 12;//gpio12 rele2 espertino
const int gpio16 = 16;
const int gpio14 = 14;//gpio14 rele1 espertino
const int gpio05 = 5;//gpio05 led builtin1 espertino o SS
const int gpio13 = 13;//gpio13 led builtin2 espertino

const int gpio23 = 23;//MOSI
const int gpio22 = 22;
const int gpio21 = 21;
const int gpio18 = 18;//SCK
const int gpio19 = 19;//MISO
const int gpio25 = 25;

// !!!!  IMPORTANTE LEGGERE RIGUARDO I PIN ADC !!!!!   https://github.com/khoih-prog/ESP_WiFiManager/issues/39#issuecomment-713195975
// si consiglia di usare per gli ADC i pin GPIO32-GPIO39 perchè gli altri facenti parti del registro adc2 che sono i GPIO0, 2, 4, 12, 13, 14, 15, 25, 26 e 27 sono utilizzati dal WiFi

const int adc32 = 32;
const int adc33 = 33;
const int adc34 = 34;
const int adc35 = 35;
const int adc36 = 36;
const int adc37 = 37;


//WiFi_AP
char ssidAP[] = "Giorgi-EB_AP";        
char passAP[] = "12345678"; 

IPAddress ipAP(192, 168, 10, 30);
IPAddress gwAP(192, 168, 10, 3);
IPAddress maskAP(255, 255, 255, 0);

uint16_t ota_port=8000, rest_port=8080, asinc_webserver_port=80;
WiFiServer serverREST(rest_port);
WebServer serverOTA(ota_port);
AsyncWebServer serverWeb (asinc_webserver_port);
String payload;

// prototipi funzioni

void setup();
void loop();
void ScriviOLED(int co2, float temp, int ur, int co, int allarme);
void ScriviOLED1(float valore, char *stringa);
int ctrlButton( int TastieraAR );

bool listen_service(WiFiClient wifi_client, String service);
void listnerREST();
int readAdcAVG(int pinAdc);
void digitalCommand(WiFiClient wifi_client);
void analogCommand(WiFiClient wifi_client);
void modeCommand(WiFiClient wifi_client);
void process(WiFiClient wifi_client);
void getTemp(WiFiClient wifi_client);
void getCo2(WiFiClient wifi_client);
void getUmidita(WiFiClient wifi_client);
void getCo(WiFiClient wifi_client);
void getSogliaTemperatura(WiFiClient wifi_client);
void getSogliaCo2(WiFiClient wifi_client);
void getSogliaCo(WiFiClient wifi_client);
void getSogliaUmidita(WiFiClient wifi_client);


void getSogliaTemperatura(WiFiClient wifi_client){

  wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(sogliaTemperatura)));

}

void getSogliaCo2(WiFiClient wifi_client){
 
 wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(sogliaCo2)));


}

void getSogliaCo(WiFiClient wifi_client){

 wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(sogliaCo)));

}

void getSogliaUmidita(WiFiClient wifi_client){

 wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(sogliaUmidita)));

}

void startService();
void startService2();
String IpAddressToString(const IPAddress& ipAddress);
//String processor(const String& var);


// define tasks
void TaskA( void *pvParameters );
void TaskB( void *pvParameters );
void TaskC( void *pvParameters );
void TaskD( void *pvParameters );
void TaskE( void *pvParameters );


String IpAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

int readAdcAVG(int pinAdc){
  
 for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    adcRead=analogRead(pinAdc);
    readings[thisReading] = adcRead;
    total = total + adcRead;
  }
  adcAVG = total / numReadings;
  total=0;


return adcAVG;


}

bool listen_service(WiFiClient wifi_client, String service){
  //check service
  String currentLine = "";
  while (wifi_client.connected()) {
    if (wifi_client.available() > 0) {
      char c = wifi_client.read();
      currentLine += c;
      if (c == '\n') {
        wifi_client.println("HTTP/1.1 200 OK");
		wifi_client.println("Connection: close");
        wifi_client.println();
        return 0;
      }
      else if (currentLine.endsWith(service + "/")) {
        return 1;
      }
    }
  }
}

//---------------------------------------------------------------------------------------

 
  

void digitalCommand(WiFiClient wifi_client) {
  int pin, value;

  // Read pin number
  String pinNumber;
  char c = wifi_client.read();
  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/9/1"
  // If the next character is a ' ' it means we have an URL
  // with a value like: "/digital/9"

  while (c != ' ' && c != '/') {
    pinNumber += c;
    c = wifi_client.read();
  }
  pin = pinNumber.toInt();

  if (c == '/') {
    value = wifi_client.parseInt();
    //value = 1;
    digitalWrite(pin, value);

    // Send feedback to wifi_client
    wifi_client.print("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\nPin D" + String(pin) + " set to: " + String(value));
  }
  else {
    value = digitalRead(pin);

    // Send feedback to wifi_client
    wifi_client.print("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\nPin D" + String(pin) + " reads: " + String(value));
  }
}


void analogCommand(WiFiClient wifi_client) {
  int pin, value, truePin;
  bool analog = false;
  char pinType = 'D';
  // Read pin number
  String pinNumber;

  char c = wifi_client.read(); 
  if(c == 'A'){
    analog = true;
    pinType = 'A';
    c = wifi_client.read();
  }

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  // If the next character is a ' ' it means we have an URL
  // with a value like: "/analog/13"
  while (c != ' ' && c != '/') {
    pinNumber += c;
    c = wifi_client.read();
  }

  pin = pinNumber.toInt();
  
  truePin = pin;
    
  if(analog){
    switch(pin){
      case 0:
        truePin = adc32;
      break;
  
      case 1:
        truePin = adc33;
      break;
  
      case 2:
        truePin = adc34;
      break;
      
      case 3:
        truePin = adc35;
      break;
  
      case 4:
        truePin = adc36;
      break;
  
      case 5:
        truePin = adc37;
      break;
    }
  } 
   
  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (c == '/') {
    // Read value and execute command
    String analogValue;
    char c = wifi_client.read();
    while (c != ' ' && c != '/') {
      analogValue += c;
      c = wifi_client.read();
    }
    value = analogValue.toInt();
    //analogWrite(truePin, value);

    // Send feedback to wifi_client
    wifi_client.print("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\nPin " + String(pinType) + String(pin) + " set to: " + String(value));
  }
  else {
    // Read analog pin
    value = readAdcAVG(truePin);

    // Send feedback to wifi_client
    wifi_client.print("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\nPin " + String(pinType) + String(pin) + " reads: " + String(value));
  }
}


void modeCommand(WiFiClient wifi_client) {
  int pin, truePin;
  char feedback[100];

  // Read pin number
  String pinNumber;
  bool analog = false;
  char pinType = 'D';
  char c = wifi_client.read();
  if(c == 'A'){
    analog = true;
    pinType = 'A';
    c = wifi_client.read();
  }
  while (c != ' ' && c != '/') {
    pinNumber += c;
    c = wifi_client.read();
  }
  pin = pinNumber.toInt();

  truePin = pin;

  if(analog){
    switch(pin){
      case 0:
        truePin = adc32;
      break;
  
      case 1:
        truePin = adc33;
      break;
  
      case 2:
        truePin = adc34;
      break;
      
      case 3:
        truePin = adc35;
      break;
  
      case 4:
        truePin = adc36;
      break;
  
      case 5:
        truePin = adc37;
      break;
    }
  }
  // If the next character is not a '/' we have a malformed URL
  if (c != '/') {
    wifi_client.print("HTTP/1.1 200 OK\r\n\r\nError!\r\n\r\n");

    return;
  }

  String mode = wifi_client.readStringUntil(' ');

  if (mode == "input") {
    pinMode(truePin, INPUT);

    // Send feedback to wifi_client
    wifi_client.print("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\nPin " + String(pinType) + String(pin) + " configured as INPUT!");

    return;
  }

  if (mode == "output") {
    pinMode(truePin, OUTPUT);

    // Send feedback to wifi_client
    wifi_client.print("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\nPin " + String(pinType) + String(pin) + " configured as OUTPUT!\r\n");
    return;
  }

  wifi_client.print(String("HTTP/1.1 200 OK\r\n\r\nError!\r\nInvalid mode" + mode + "\r\n"));
}


void getTemp(WiFiClient wifi_client){

  wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(temperature)));

}

void getUmidita(WiFiClient wifi_client){

  wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(umidita)));

}

void getCo2(WiFiClient wifi_client){

  wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(co2)));

}

void getCo(WiFiClient wifi_client){

  wifi_client.print(String("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + String(co)));

}


void process(WiFiClient wifi_client){
  // read the command//
  if (listen_service(wifi_client, "dev")) {
    String command = wifi_client.readStringUntil('/');

    if (command == "mode") {
      modeCommand(wifi_client);
    }
    else if (command == "digital") {
      digitalCommand(wifi_client);
    }
    else if (command == "analog") {
      analogCommand(wifi_client);
    }

    else if (command == "getTemp") {
      getTemp(wifi_client);
    }

    else if (command == "getUmidita") {
      getUmidita(wifi_client);
    }
    
    else if (command == "getCo2") {
      getCo2(wifi_client);
    }
    
    else if (command == "getCo") {
      getCo(wifi_client);
    }

    else if (command == "getSogliaTemperatura") {
      getSogliaTemperatura(wifi_client);
    }

    else if (command == "getSogliaCo2") {
      getSogliaCo2(wifi_client);
    }

    else if (command == "getSogliaCo") {
      getSogliaCo(wifi_client);
    }
    
    else if (command == "getSogliaUmidita") {
      getSogliaUmidita(wifi_client);
    }
    
    else {
      wifi_client.print(String("HTTP/1.1 200 OK\r\n\r\nError!\r\nUnknown command : " + command + "\r\n\r\n"));
    }
  }
}


void listnerREST(){
    
  //listen for incoming clients for REST API
  WiFiClient wifi_client = serverREST.available();
  if (wifi_client) {
      // Process request
      process(wifi_client);
      vTaskDelay(1 / portTICK_PERIOD_MS);  // 1ms delay
      //Close connection and free resources.
      wifi_client.stop();
  } 
}

//Style 
String style =
"<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
"form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

//Login page
String loginIndex = 
"<form name=loginForm>"
"<h1>ESP32 Login</h1>"
"<input name=userid placeholder='User ID'> "
"<input name=pwd placeholder=Password type=Password> "
"<input type=submit onclick=check(this.form) class=btn value=Login></form>"
"<script>"
"function check(form) {"
"if(form.userid.value=='admin' && form.pwd.value=='admin')"
"{window.open('/serverIndex')}"
"else"
"{alert('Error Password or Username')}"
"}"
"</script>" + style;
 
//Server Index Page
String serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
"<label id='file-input' for='file'>   Choose file...</label>"
"<input type='submit' class=btn value='Update'>"
"<br><br>"
"<div id='prg'></div>"
"<br><div id='prgbar'><div id='bar'></div></div><br></form>"
"<script>"
"function sub(obj){"
"var fileName = obj.value.split('\\\\');"
"document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
"};"
"$('form').submit(function(e){"
"e.preventDefault();"
"var form = $('#upload_form')[0];"
"var data = new FormData(form);"
"$.ajax({"
"url: '/update',"
"type: 'POST',"
"data: data,"
"contentType: false,"
"processData:false,"
"xhr: function() {"
"var xhr = new window.XMLHttpRequest();"
"xhr.upload.addEventListener('progress', function(evt) {"
"if (evt.lengthComputable) {"
"var per = evt.loaded / evt.total;"
"$('#prg').html('progress: ' + Math.round(per*100) + '%');"
"$('#bar').css('width',Math.round(per*100) + '%');"
"}"
"}, false);"
"return xhr;"
"},"
"success:function(d, s) {"
"console.log('success!') "
"},"
"error: function (a, b, c) {"
"}"
"});"
"});"
"</script>" + style;

String readFile(String name){

  File file;
  uint8_t i=0;
  #define SIZE 1024
  char c[SIZE];
  String readout="";

  if(!SPIFFS.begin(true)){
     Serial.println("An Error has occurred while mounting SPIFFS");
    
  }
  
  file = SPIFFS.open(name);

  if(!file){
      Serial.println("Failed to open file for reading");
      
  }
  
  
  while(file.available()){
      i++;
      c[i]= file.read();

      //Serial.print(c[i]);
       
      readout+=c[i];
      
  }
  
  file.close();

  return readout;
}

/*
// Replaces web page placeholder %variable%  with value
String processor(const String& var){
  //Serial.print(var);
  
  if(var == "co2"){
    
    return co2;
  }
  if(var == "co"){
    
    return co;
  }
  else if(var == "temp"){
    
    return temperature;
  }
  else if(var == "pressr"){
    
    return pressr;
  }
  else if(var == "alt"){
    
    return alt;
  }
  
  return String();
  
}
*/


void startService(){
 
    //return index page which is stored in serverIndex
  serverOTA.on("/", HTTP_GET, []() {
    serverOTA.sendHeader("Connection", "close");
    serverOTA.send(200, "text/html", loginIndex);
  });
  
  serverOTA.on("/serverIndex", HTTP_GET, []() {
    serverOTA.sendHeader("Connection", "close");
    serverOTA.send(200, "text/html", serverIndex);
  });
  //handling uploading firmware file 
  serverOTA.on("/update", HTTP_POST, []() {
    serverOTA.sendHeader("Connection", "close");
    serverOTA.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = serverOTA.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      //flashing firmware to ESP
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  //start the server
  serverREST.begin();
  serverOTA.begin();

  // you're connected now, so print out the status
  //printWifiStatus();

  Serial.print("WiFi REST Server on port ");
  Serial.println(rest_port);

  Serial.print("WiFi OTA Server on port ");
  Serial.println(ota_port);

}

void startService2(){

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  // Route for root / web page
  serverWeb.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(SPIFFS, "/index.html", String(), false, processor);
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to load js file
  serverWeb.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.js", "text/javascript");
  });

  // Route to mp3 file
  serverWeb.on("/alarm.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/alarm.mp3", "audio/mpeg");
  });
  
  // Route to load style.css file
  serverWeb.on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.css", "text/css");
  });

  serverWeb.on("/bgimage.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bgimage.jpg", "image/jpg");
  });

  // Start server
  serverWeb.begin();
  Serial.print("WiFi Web Server on port ");
  Serial.println(asinc_webserver_port);
  
}

void setup() {
  
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  pinMode(adc32, INPUT);
  pinMode(adc33, INPUT);
  pinMode(adc34, INPUT);
  pinMode(adc35, INPUT);
  pinMode(adc36, INPUT);
  pinMode(adc37, INPUT);
   
  pinMode(gpio23, INPUT_PULLUP);
  pinMode(gpio22, INPUT_PULLUP);
  pinMode(gpio21, INPUT_PULLUP);
  pinMode(gpio18, INPUT_PULLUP);
  pinMode(gpio19, INPUT_PULLUP); 
  pinMode(gpio25, INPUT_PULLUP);
  
  //pinMode(gpio04, INPUT);
  pinMode(gpio05, OUTPUT);
  pinMode(gpio12, OUTPUT);
  pinMode(gpio13, OUTPUT);
  pinMode(gpio14, OUTPUT);
  pinMode(gpio16, OUTPUT);
  
   
  for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(500);
  }
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAP, passAP);
  Serial.println("AP_START...");
  delay(100);
  WiFi.softAPConfig(ipAP, gwAP, maskAP);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)… ");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssidAP, passAP);
  
  Serial.print("AP IP address: ");
  Serial.println(ipAP);
   
  analogReadResolution(10);

  
#if (TASK_A == 1)
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskA
    ,  "TaskA"   // A name just for humans
    ,  4096  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
#endif

#if (TASK_B == 1)
  xTaskCreatePinnedToCore(
    TaskB
    ,  "TaskB"
    ,  4096  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
#endif

#if (TASK_C == 1)  
    xTaskCreatePinnedToCore(
    TaskC
    ,  "TaskC"
    ,  4096  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
#endif 

#if (TASK_D == 1)  
    xTaskCreatePinnedToCore(
    TaskD
    ,  "TaskD"
    ,  4096  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
#endif 

#if (DEBUG == 1)  
    xTaskCreatePinnedToCore(
    TaskE
    ,  "TaskE"
    ,  4096  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
#endif 

void flush();
//*****************************
//void setup menù
if (!scd30.begin()) {
       while (1) { delay(10); }
  }


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();  // cancello display
  pinMode(cicalinoPin0,OUTPUT);
  pinMode(LED1,OUTPUT);
  delay(100);
}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

#if (TASK_A == 1)
  void TaskA(void *pvParameters)  // This is a task.
  {
    (void) pvParameters;

    startService();

    for (;;) // A Task shall never return or exit.
    {
      serverOTA.handleClient();

      vTaskDelay(1000 / portTICK_PERIOD_MS );//100ms delay

      listnerREST();
        
    }
  }
#endif

#if (TASK_B == 1)
  void TaskB(void *pvParameters)  // This is a task.
  {

    (void) pvParameters;

    startService2();
    
    for (;;)
    {
    
     
     vTaskDelay(1000 / portTICK_PERIOD_MS);  // ms delay


    }

  }
#endif

#if (TASK_C == 1) 

void TaskC(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  

  for (;;) // A Task shall never return or exit.
   {  
    //digitalWrite(2,HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS );//100ms delay
    // Lettura SCD30
 if (scd30.dataReady())
 {
  scd30.read();                           // leggo SCD30
  temperature=float(scd30.temperature);   // prelevo temperatura
  umidita=scd30.relative_humidity;        // prelevo umidità
  co2=scd30.CO2;                          // prelevo CO2
 }  
 temperatura=temperature;  // assegnazione per non cambiare tutte le varibili nella parte display

 CO2=co2;
 
 TastieraAR=analogRead( TastieraA );                     // Lettura tastiera
 pulsante=ctrlButton(TastieraAR);                        // Leggo la tastiera a pulsanti
  // lettura sensore di fumo
 co=((analogRead(36)*2)+RC*COOld)/(1+RC);  // filtraggio passa basso
 COOld=co;
 
 // scrivo sul display a seconda della posizione dello scroll
 
 if(scr_pos ==0)   // se sono in prima posizione del menu visualizzo tutti i parametri
 {
  ScriviOLED(CO2, temperatura, umidita, co, All);  // parametri C02, temp, umidità, CO, stato allarme 0/1
 }
 
 if(scr_pos ==1)   // se sono in seconda posizione del menu visualizzo tutti i parametri
 {
  ScriviOLED1(sogliaCo2,"CO2max");
  if (pulsante == 2)   // controllo tasti x variazione soglia
  {
     sogliaCo2=sogliaCo2-20;
     if(sogliaCo2 < 0)
      sogliaCo2=0;
  }
  if (pulsante == 3)
  {
    sogliaCo2=sogliaCo2+20;
    if(sogliaCo2 > 2200)
       sogliaCo2=2200;
  } // fine controllo variazione soglia
 }  

 if(scr_pos ==2)   // se sono in seconda posizione del menu visualizzo tutti i parametri
 {
  ScriviOLED1(sogliaTemperatura,"TempMax");
    if (pulsante == 2)   // controllo tasti x variazione soglia
  {
  sogliaTemperatura=sogliaTemperatura-1;
      if(sogliaTemperatura < 0)
       sogliaTemperatura=0;
  }
  if (pulsante == 3)
  {
    sogliaTemperatura=sogliaTemperatura+1;
    if(sogliaTemperatura > 80)
       sogliaTemperatura=80;
  }  // fine controllo variazione soglia
 }  
 if(scr_pos ==3)   // se sono in seconda posizione del menu visualizzo tutti i parametri
 {
  ScriviOLED1(sogliaUmidita,"urMax");
      if (pulsante == 2)   // controllo tasti x variazione soglia
  {
  sogliaUmidita=sogliaUmidita-5;
      if(sogliaUmidita < 0)
       sogliaUmidita=0;
  }
  if (pulsante == 3)
  {
    sogliaUmidita=sogliaUmidita+5;
    if(sogliaUmidita > 100)
       sogliaUmidita=100;
  }  // fine controllo variazione soglia
 }
  if(scr_pos ==4)   // se sono in seconda posizione del menu visualizzo tutti i parametri
 {
  ScriviOLED1(sogliaCo,"COMax");
    if (pulsante == 2)   // controllo tasti x variazione soglia
  {
     sogliaCo=sogliaCo-20;
     if(sogliaCo < 0)
      sogliaCo=0;
  }
  if (pulsante == 3)
  {
    sogliaCo=sogliaCo+20;
    if(sogliaCo > 2200)
       sogliaCo=2200;
  }
 }
 
 //delay(100);

 // Controllo superamento soglie
 if((co>sogliaCo)||(CO2>sogliaCo2)||(temperatura>sogliaTemperatura)||(umidita>sogliaUmidita))
 {
   digitalWrite(cicalinoPin0,HIGH);   // attivo cicalino
   All=1;                             // attivo flag allarme
   //digitalWrite(LED1,HIGH);         // accendo LED Rosso su scheda
   for(long int k=0;k<=400000;k++)
   {   
    if(k==200000)
       digitalWrite(LED1,HIGH);       // spengo LED Rosso su scheda
    if(k==400000)
       digitalWrite(LED1,LOW);        // spengo LED Rosso su scheda
   }
      
 }
 else
 {
   digitalWrite(cicalinoPin0,LOW);   // disattivo cicalino
   All=0;                             // disattivo flag allarme
   digitalWrite(LED1,LOW);           // spengo LED Rosso su scheda
 }

// **** Gestione Tasti
 if (pulsante == 1)        // è stato premuto tasto Left
{
  if (--scr_pos<=0)       // decremento l'indice di 1 e verifico se minore o uguale di 0
   scr_pos=0;             // se minore o uguale di zero lo pongo a 0
}
if (pulsante == 4)         // è stato premuto tasto Rigth
{
  if (++scr_pos>=Num_Dati)   // incremento l'indice di 1 e verifico se è maggiore o uguale di Numero di Dati
   scr_pos=Num_Dati-1;       // se maggiore 0 uguale del numero di dati lo setto al massimo valore Numero di Dati
}  
       
  }// fine Task
}
#endif

#if(TASK_D == 1)
  void TaskD(void *pvParameters)  // This is a task.
  {
    (void) pvParameters;
    
              
    for (;;) // loop
    {
                  

    }      
  }
#endif


#if(DEBUG == 1)
  void TaskE(void *pvParameters)  // This is a task.
  {
    (void) pvParameters;
    
              
    for (;;) // loop
    {
                        

            vTaskDelay(5000/portTICK_PERIOD_MS);//msec
     
    }
  }
#endif


void loop() {}
 
 
// *** Funzioni ausiliarie
void ScriviOLED(int co2, float temp, int ur, int co, int allarme)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,1);
  display.println(("GiorgiE.B."));
  display.setTextSize(1);
  // plot CO2
  display.setCursor(1,20);
  display.println("CO2 =");
  display.setCursor(38,20);
  display.println(co2);
  display.setCursor(70,20);
  display.println("ppm");
  // plot temperatura
  display.setCursor(1,32);
  display.println("Temp=");
  display.setCursor(38,32);
  display.println(temp);
  display.setCursor(70,32);
  display.println("C");
  // plot umidità relativa
  display.setCursor(1,44);
  display.println("Hr =");
  display.setCursor(38,44);
  display.println(ur);
  display.setCursor(70,44);
  display.println("%");
  // CO
  display.setCursor(1,56);
  display.println("CO =");
  display.setCursor(38,56);
  display.println(co);
  display.setCursor(70,56);
  display.println("ppm");
    // allarme
  display.setTextSize(2);  
  display.setCursor(92,32);
  if (allarme == 0)
     display.println("OFF");
  if (allarme == 1) 
  display.println("ON");
    display.display();
}  // fine funzione scrittura su display

// funzione di stampa di un solo valore su display
void ScriviOLED1(float valore, char *stringa)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,1);
  display.println(("GiorgiE.B."));
 
  display.setTextSize(1);
  display.setCursor(1,32);
  display.println(stringa);
  display.setCursor(60,32);
  display.println(valore);
  display.display();
}  // fine funzione scrittura su display1

 int ctrlButton( int TastieraAR )         // Funzione per la determinazione dei tasti premuti
{
  if ( tasto1[0] <= TastieraAR && TastieraAR <= tasto1[1] ) { return 1; }       // Verifico se è nel range stabilito
  if ( tasto2[0] <= TastieraAR && TastieraAR <= tasto2[1] ) { return 2; }           // Verifico se è nel range stabilito
  if ( tasto3[0] <= TastieraAR && TastieraAR <= tasto3[1] ) { return 3; }         // Verifico se è nel range stabilito
  if ( tasto4[0] <= TastieraAR && TastieraAR <= tasto4[1] ) { return 4; }               // Verifico se è nel range stabilito
  return 0;
}   // ********************************************************************************************
  
