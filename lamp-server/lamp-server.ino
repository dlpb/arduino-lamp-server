/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

int RED_MAIN_PIN = 4;
int RED_SUBSIDIARY_PIN = 5;
int GREEN_MAIN_PIN = 6;
int GREEN_SUBSIDIARY_PIN = 7;

void setup() {

  pinMode(RED_MAIN_PIN, OUTPUT);
  pinMode(RED_SUBSIDIARY_PIN, OUTPUT);
  pinMode(GREEN_MAIN_PIN, OUTPUT);
  pinMode(GREEN_SUBSIDIARY_PIN, OUTPUT);

  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {

    String req_str = "";
    
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if(req_str.length() < 25) {
          req_str += c;
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("GET")) {
          printHttpHeadersToClient(client);
          
          client.println();
          client.println("[");

          client.println("  {");
          client.println("    \"name\":\"red-main\",");
          client.println("    \"colour\":\"red\",");
          client.println("    \"subsidiary\":false,");
          client.print("    \"state\":\"");
          client.print(digitalRead(RED_MAIN_PIN)? "on": "off");
          client.println("\"");
          client.println("  },");
          
          client.println("  {");
          client.println("    \"name\":\"red-subsidiary\",");
          client.println("    \"colour\":\"red\",");
          client.println("    \"subsidiary\":true,");
          client.print("    \"state\":\"");
          client.print(digitalRead(RED_SUBSIDIARY_PIN)? "on": "off");
          client.println("\"");
          client.println("  },");
          
          client.println("  {");
          client.println("    \"name\":\"green-main\",");
          client.println("    \"colour\":\"green\",");
          client.println("    \"subsidiary\":false,");
          client.print("    \"state\":\"");
          client.print(digitalRead(GREEN_MAIN_PIN)? "on": "off");
          client.println("\"");
          client.println("  },");
           
          client.println("  {");
          client.println("    \"name\":\"green-subsidiary\",");
          client.println("    \"colour\":\"green\",");
          client.println("    \"subsidiary\":true,");
          client.print("    \"state\":\"");
          client.print(digitalRead(GREEN_SUBSIDIARY_PIN)? "on": "off");
          client.println("\"");
          client.println("  }");          

          client.println("]");
          break;
        }
        else if(c == '\n' && currentLineIsBlank && req_str.startsWith("POST")) {


          boolean subsidiary = req_str.indexOf("subsidiary") > 0;
          boolean red = req_str.indexOf("/red") > 0;
          boolean green = req_str.indexOf("/green") > 0;
          boolean on = req_str.indexOf("/on") > 0;

          String lampName = getLampName(red, green, subsidiary);
          String lampColourStr = red ? "red": green? "green" : "unknown";
          String subsidiaryStr = subsidiary ? "true": "false";
          String stateStr = on ? "on": "off";

          turnOnOrOffLamp(red, green, subsidiary, on);
       
          printHttpHeadersToClient(client);          
          printLampStateToClient(lampName, lampColourStr, subsidiaryStr, stateStr, client);

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

String getLampName(boolean red, boolean green, boolean subsidiary){
  if(red){
    if(subsidiary){
      return "red-subsidiary";
    }
    else {
      return "red-main";
    }
  }
  
  if(green){
    if(subsidiary){
      return "green-subsidiary";
    }
    else {
      return "green-main";
    }
  }
  return "unknown";  
}

void turnOnOrOffLamp(boolean red, boolean green, boolean subsidiary, boolean on){
  if(red){
    if(subsidiary){
      if(on){
        digitalWrite(RED_SUBSIDIARY_PIN, HIGH);  
      }
      else {
        digitalWrite(RED_SUBSIDIARY_PIN, LOW);
      }
    }
    else {
      if(on){
        digitalWrite(RED_MAIN_PIN, HIGH);
      }
      else {
        digitalWrite(RED_MAIN_PIN, LOW);  
      }
    }
  }
  
  if(green){
    if(subsidiary){
      if(on){
        digitalWrite(GREEN_SUBSIDIARY_PIN, HIGH);  
      }
      else {
        digitalWrite(GREEN_SUBSIDIARY_PIN, LOW);
      }
    }
    else {
      if(on){
        digitalWrite(GREEN_MAIN_PIN, HIGH);
      }
      else {
        digitalWrite(GREEN_MAIN_PIN, LOW);  
      }
    }
  }  
}

void printLampStateToClient(String lampName, String colour, String subsidiary, String state, EthernetClient client){
  client.println();
  client.println("{");
  client.print("  \"name\":\"");
  client.print(lampName);
  client.println("\",");
  client.print("  \"colour\":\"");
  client.print(colour);
  client.println("\",");
  client.print("  \"subsidiary\":");
  client.print(subsidiary);
  client.println(",");
  client.print("  \"state\":\"");
  client.print(state);
  client.println("\"");
  client.println("}");
  client.println();
}

void printHttpHeadersToClient(EthernetClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println("Cache-Control: no-cache, no-store, max-age=0, must-revalidate"); 
}
