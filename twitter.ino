/*
  Twitter Client with Strings
 
 This sketch connects to Twitter using an Ethernet shield. It parses the XML
 returned, and looks for <text>this is a tweet</text>
 
 You can use the Arduino Ethernet shield, or the Adafruit Ethernet shield, 
 either one will work, as long as it's got a Wiznet Ethernet module on board.
 
 This example uses the DHCP routines in the Ethernet library which is part of the 
 Arduino core from version 1.0 beta 1
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
  * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 21 May 2011
 by Tom Igoe
 
 This code is in the public domain.
 
 */
#if defined(ARDUINO) && ARDUINO > 18   // If using Arduino 0019 or later, include SPI.h
#include <SPI.h>
#endif
#include <Ethernet.h>
#include <EthernetDNS.h>
#include <Twitter.h>
#include <Servo.h> 
 

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
IPAddress ip(xxx,xxx,xxx,xx); //<<< ENTER YOUR IP ADDRESS HERE!!!

// initialize the library instance:
EthernetClient client;

const int requestInterval = 20000;  // delay between requests

char serverName[] = "api.twitter.com";  // twitter URL

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
String tweet2 = "";                  // string to hold the tweet
boolean readingTweet = false;       // if you're currently reading the tweet
Twitter twitter("aaaaaaaaa-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); // replace aaaaaaa with your token
Servo myservo;
Servo drp;


void setup() {
  
  pinMode(2, OUTPUT);
  myservo.attach(9);
  myservo.write(0);
  drp.attach(6);
  
  
  // reserve space for the strings:
  currentLine.reserve(256);
  //tweet.reserve(150);

// initialize serial:
  Serial.begin(9600);
  // attempt a DHCP connection:
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Ethernet.begin(mac, ip);
  }
  // connect to Twitter:
  connectToServer();
}



void loop() {
  if (client.connected()) {
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();

      // add incoming byte to end of line:
      currentLine += inChar; 

      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      } 
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<text>")) {
        // tweet is beginning. Clear the tweet string:
        readingTweet = true; 
        tweet2 = "";
      }
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet) {
        if (inChar != '<') {
          tweet2 += inChar;
        } 
        else {
          // if you got a "<" character,
          // you've reached the end of the tweet:
          readingTweet = false;
          Serial.println(tweet2);
          
          if(tweet2 == ">Dr Pepper" || tweet2 == ">dr pepper" || tweet2 == ">Dr pepper"){
           digitalWrite(2, HIGH);
           drp.write(90);
           delay(6000);
           drp.write(0);
           digitalWrite(2, LOW);
           tweet("Dr Pepper dispensed");
          }
          
          if(tweet2 == ">Mountain Dew" || tweet2 == ">Mountain dew" || tweet2 == ">mountain dew"){
           digitalWrite(2, HIGH);
           myservo.write(90);
           delay(6000);
           myservo.write(0);
           digitalWrite(2, LOW);
           tweet("Mountain Dew dispensed");
          }

          if(tweet2 == ">what do you have to drink?" || tweet2 == ">What do you have to drink?" || tweet2 == ">what do you have to drink" || tweet2 == ">What do you have to drink"){
           digitalWrite(2, HIGH);
           digitalWrite(2, LOW);
           tweet("I have Dr Pepper and Mountain Dew");
          }
          
          // close the connection to the server:
          client.stop(); 
        }
      }
    }   
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
  
}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(serverName, 80)) {
    Serial.println("making HTTP request...");
  // make HTTP GET request to twitter:
    client.println("GET /1/statuses/user_timeline.xml?screen_name=FridgeAlex&count=1 HTTP/1.1");
    client.println("HOST: api.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}   

void tweet(char msg[]) {
  Serial.println("connecting ...");
  if (twitter.post(msg)) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}

