#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

/*
  Arduino Yún Bridge example

  This example for the Arduino Yún shows how to use the
  Bridge library to access the digital and analog pins
  on the board through REST calls. It demonstrates how
  you can create your own API when using REST style
  calls through the browser.

  Possible commands created in this shetch:

  "/arduino/digital/13"     -> digitalRead(13)
  "/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
  "/arduino/analog/2/123"   -> analogWrite(2, 123)
  "/arduino/analog/2"       -> analogRead(2)
  "/arduino/mode/13/input"  -> pinMode(13, INPUT)
  "/arduino/mode/13/output" -> pinMode(13, OUTPUT)

  This example code is part of the public domain

  http://www.arduino.cc/en/Tutorial/Bridge

*/

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <HttpClient.h>
#include "DHT.h"
#define DHTPIN A0 
#define DHTTYPE DHT11

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;
EthernetClient client1;
HttpClient httpClient;

int i = 0;

void setup() {
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  //counter
  Serial.println(i);
  i++;
  // Initialize the client library
  if (i == 1200){
    DHT dht(DHTPIN, DHTTYPE);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int airquality = analogRead(A1);
  
    String data = "?temp="+(String)t+"&humidity="+(String)h+"&AQ="+(String)airquality;

    //data = "?temp=8&humidity=9&AQ=10";
    
    //Serial.println("timer "+data);

    /*if (client1.connect("http://172.26.105.126",8888)) {
      client1.println("POST /rest/rest.php/reading HTTP/1.1");
      client1.println("Host: 172.26.105.126");
      client1.println("Contect-Type: application/x-www-form-urlencoded");
      client1.print("Content-Length: ");
      client1.println(data.length());
      client1.println();
      client1.print(data);
      Serial.println("loaded");
    }*/
    
    String url = "http://172.26.105.126:8888/rest/rest.php/reading"+data;
    Serial.println(url);
    httpClient.get(url);
  
  
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (httpClient.available()) {
      char c = httpClient.read();
      Serial.print(c);
    }
    Serial.flush();
    i = 0;
  }
  

  //update every one minute

  //update database ends here

  
  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(BridgeClient client) {
  // read the command
  String command = client.readStringUntil('/');


  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
}


void analogCommand(BridgeClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  } else {
    if (pin == 0){
     DHT dht(DHTPIN, DHTTYPE);
     float h = dht.readHumidity();
     float t = dht.readTemperature();
     client.println("Status: 200");
     client.println("Access-Control-Allow-Origin: *"); 
     client.println("Access-Control-Allow-Methods: GET");
     client.println("Content-type: application/json");
     client.println();
     client.print("{\"humidity\":");
     client.print(h);
     client.print(",\"temperature\":"); 
     client.print(t);
     client.println("}");
   }
   if (pin == 1){
     int airquality = analogRead(A1);
     client.println("Status: 200");
     client.println("Access-Control-Allow-Origin: *"); 
     client.println("Access-Control-Allow-Methods: GET");
     client.println("Content-type: application/json");
     client.println();
     client.print("{\"airquality\":");
     client.print(airquality);
     client.println("}");
   }
    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(BridgeClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

    if (mode == "output") {
      pinMode(pin, OUTPUT);
      // Send feedback to client
      client.print(F("Pin D"));
      client.print(pin);
      client.print(F(" configured as OUTPUT!"));
      return;
    }

  client.print(F("error: invalid mode "));
  client.print(mode);
}


