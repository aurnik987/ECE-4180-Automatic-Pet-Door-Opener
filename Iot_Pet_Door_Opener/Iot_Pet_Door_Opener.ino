/*********
  ECE 4180 - FINAL DESIGN PROJECT
  Iot Pet Door Opener
  Device: Sparkfun ESP32 Thing
  Authors:
    Nikhil Auradkar
    Salim Ben Ghorbel
*********/

/*********
  Arduino core for the ESP32
  https://github.com/espressif/arduino-esp32.git
*********/

/*********
  Wifi Server
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

/*********
  Arduino core for ESP32 WiFi chip SMTP Gmail sender
   by jubaid hossain
   4.9.2018
   https://github.com/AIWIndustries/ESP32_Gmail_Sender.git
*********/


// Load necessary librairies
#include <Servo.h>
#include <WiFi.h>
#include <time.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <string>
#include <WiFiClientSecure.h>
#include "Gsender_32.h"

//WIFI Credentials
const char* ssid = ""; //custom
const char* password = ""; //custom

//PIN ASSIGNMENT
static const int led = 5;
static const int servoPin = 13;
static const int motionPin = 16;

// BLE VARIABLES
int scanTime = 5; //In seconds
static const std::string collarIdentifier = "4c000215ef1aef1aef1aef1aef1aef1aef1aef1a1aef1aef00" ;
std::string manufacturerData = "";
static int collarRssi = -9000; //Initialize with an out-of-range value

// Email destination address
String address[] = {""}; //custom

// Used to disable notifications for a certain amount of time
clock_t timer = clock();
clock_t difference;

//Latch Servomotor
Servo servo1;
// Latch states
#define OPEN   1
#define CLOSED 0

// Wifi Server
WiFiServer server(80);


// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output27State = "off";
String doorState = "closed";
String useState = "manual";
String notificationState = "on";

// State variables needed for logic architecture
bool autoState = false;
bool openLatch = false;
bool notificationOn = true;
bool motionDetected = false;
int  pirValue = LOW;

// Email notification function
void sendEmail()
{
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  String subject = "Your Pet awaits.";
  String coreMessage = "";
  //coreMessage = "We are detecting the presence of the pet. Login to webserver if you want to open the door.";
  coreMessage += String("We are detecting the presence of the pet. Go to ") + String(WiFi.localIP()) + String(" if you want to open the door.");
  if (gsender->Subject(subject)->Send(1, address, coreMessage) )
  {
    Serial.println("Message send.");
  }
  else
  {
    Serial.print("Error sending message: ");
    Serial.println(gsender->getError());
  }
}

// callback class that treats advertised devices
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
      manufacturerData = pHex;
      free(pHex);
      if (advertisedDevice.haveRSSI() && (manufacturerData == collarIdentifier) )
      {
        collarRssi = (int)advertisedDevice.getRSSI();
      }
    }
};

//Open/Close latch when isOpen is true/false
void latch (bool isOpen)
{
  if (isOpen)
  {
    servo1.write(0);
    //digitalWrite(led,HIGH);
  }
  else
  {
    servo1.write(60);
    //digitalWrite(led,LOW);
  }
}
//Scan nearby BLE Devices.
void scanDevices()
{
  Serial.println("Scanning...");
  collarRssi = -9000;  //Reinitialize value before new scan
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");

  if (collarRssi != -9000)
    Serial.printf("Device recognized! Rssi: %d \n", collarRssi);
  else
    Serial.printf("Device out of range!\n");
}

void setup() {
  // Initiate Serial Connection
  Serial.begin(115200);

  // Attach pins to correspondant Output/Input function
  servo1.attach(servoPin);
  pinMode(motionPin, INPUT);
  pinMode(led, OUTPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  //Initially close the latch
  latch(false);
}


void loop() {

  pirValue = digitalRead(motionPin);
  if (pirValue == HIGH)
  {
    if (motionDetected == false)
    {
      motionDetected = true;
    }
  }
  else
  {
    motionDetected = false;
  }

  digitalWrite(led, motionDetected);
  if (motionDetected)
  {
    //int rssi = bluetoothScan();
    scanDevices();
    if (collarRssi > -50)
    {
      if (autoState)
      {
        latch(true);
        doorState = "open";
        delay(5000);
        doorState = "closed";
        latch(false);
      }
      else
      {
        // Add 10-minute delay between each email notification to avoid Spam
        difference = clock() - timer + 600000;
        if (notificationOn && (difference >= 600000 || timer < 600000))
        {
          difference = 0;
          timer = clock() + 600000;
          sendEmail();
        }
      }
    }
  }


  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /useState/Auto") >= 0) {
              Serial.println("Auto Mode");
              useState = "auto";
              autoState = true;
            } else if (header.indexOf("GET /useState/Manual") >= 0) {
              Serial.println("Manual Mode");
              useState = "manual";
              autoState = false;
            } else if (header.indexOf("GET /door/Open") >= 0) {
              Serial.println("Door Open");
              latch(true);
              doorState = "open";
              //digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /door/Closed") >= 0) {
              Serial.println("Door Closed");
              latch(false);
              doorState = "closed";
              //digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /notification/On") >= 0) {
              Serial.println("Notifications On");
              notificationOn = true;
              notificationState = "on";
            } else if (header.indexOf("GET /notification/Off") >= 0) {
              Serial.println("Notifications Off");
              notificationOn = false;
              notificationState = "off";
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}");
            client.println(".button3 {background-color: #249FF4;}");
            client.println(".button4 {background-color: #BF4F27;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Pet Door Opener</h1>");

            // Display current states of the system, switch buttons and whether or not it is detecting the presence of the pet
            client.println("<p> The system is on " + useState + " Mode. Press the button to switch modes.</p>");
            // If the useState is manual, it displays the AUTO button
            if (useState == "manual") {
              client.println("<p><a href=\"/useState/Auto\"><button class=\"button button3\">AUTO</button></a></p>");
            } else {
              client.println("<p><a href=\"/useState/Manual\"><button class=\"button button4\">MANUAL</button></a></p>");
            }
            //Displays whether or not it is detecting the pet nearby.
            if (collarRssi > -50)
            {
              client.println("<p>We are detecting the presence of your pet.</p>");
            }
            else
            {
              client.println("<p>We are unable to detect the presence of your pet.</p>");
            }

            client.println("<p>Door is " + doorState + ". Press the button to open/close.</p>");

            if (doorState == "closed") {
              client.println("<p><a href=\"/door/Open\"><button class=\"button\">OPEN</button></a></p>");
            } else {
              client.println("<p><a href=\"/door/Closed\"><button class=\"button button2\">CLOSE</button></a></p>");
            }


            client.println("<p>Notifications are " + notificationState + ". Press the button to enable/disable them.</p>");

            if (notificationState == "on") {
              client.println("<p><a href=\"/notification/Off\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/notification/On\"><button class=\"button\">ON</button></a></p>");
            }
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
