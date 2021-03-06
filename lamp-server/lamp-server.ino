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

int RED_MAIN_PIN = 6;
int GREEN_MAIN_PIN = 7;

String ON = "on";
String OFF = "off";

String GET = "GET";
String POST = "POST";

String RED = "red";
String GREEN = "green";
String UNKNOWN = "unknown";

String fieldEnd = "\",";
String fieldNameStart = "    \"";
String fieldNameEnd = "\":\"";

String statusStr = "/status";


void setup() {

  pinMode(RED_MAIN_PIN, OUTPUT);
  pinMode(GREEN_MAIN_PIN, OUTPUT);

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
          Serial.println("Status Page");
          if(req_str.indexOf(statusStr) > 0) {
            printHttpHeadersToClient(client);
            client.println(); //extra line break needed for http standard
            printLampStatusToClient(client);
          }
          else {
            printHttpHtmlHeadersToClient(client);
            client.println(); //extra line break needed for http standard
            printStatusPage(client);
          }
          break;
        }
        else if(c == '\n' && currentLineIsBlank && req_str.startsWith("POST")) {


          boolean red = req_str.indexOf(RED) > 0;
          boolean green = req_str.indexOf(GREEN) > 0;
          boolean on = req_str.indexOf(ON) > 0;

          String lampName = getLampName(red, green);
          String lampColourStr = red ? RED: green? GREEN : UNKNOWN;
          String stateStr = on ? ON : OFF;

          turnOnOrOffLamp(red, green, on);
       
          printHttpHeadersToClient(client); 
          client.println(); //extra line break needed for http standard         
          printIndividualLampStateToClient(lampName, lampColourStr, stateStr, client);

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

String getLampName(boolean red, boolean green){
  if(red){
      return RED;
  }
  
  if(green){
      return GREEN;
  }
  return UNKNOWN;  
}

void turnOnOrOffLamp(boolean red, boolean green, boolean on){
  if(red){
    if(on){
      digitalWrite(RED_MAIN_PIN, HIGH);
    }
    else {
      digitalWrite(RED_MAIN_PIN, LOW);  
    }
  }
  
  if(green){
    if(on){
      digitalWrite(GREEN_MAIN_PIN, HIGH);
    }
    else {
      digitalWrite(GREEN_MAIN_PIN, LOW);  
    }
    
  }  
}

void printLampStatusToClient(EthernetClient client){
  client.println();
  client.println("[");
  printIndividualLampStateToClient(RED, RED, digitalRead(RED_MAIN_PIN)? ON: OFF, client);
  client.println(",");
  printIndividualLampStateToClient(GREEN, GREEN, digitalRead(GREEN_MAIN_PIN)? ON: OFF, client);
  client.print('\n');
  client.println("]");
}

void printIndividualLampStateToClient(String lampName, String colour, String state, EthernetClient client){
  client.println();
  client.println("{");
  printJsonFieldToClient("name", lampName, client);
  printJsonFieldToClient("colour", colour, client);
  printLastJsonFieldToClient("state", state, client);
  client.println("}");
  client.println();
}

void printJsonFieldToClient(String fieldName, String fieldValue, EthernetClient client){
  client.print(fieldNameStart);
  client.print(fieldName);
  client.print(fieldNameEnd);
  client.print(fieldValue);
  client.println(fieldEnd);  
}

void printLastJsonFieldToClient(String name, String value, EthernetClient client) {
  client.print(fieldNameStart);
  client.print(name);
  client.print(fieldNameEnd);
  client.print(value);
  client.println("\"");
}

void printStatusPage(EthernetClient client) {
  String th = "<th>";
  String thc = "</th>";
  
  client.println("<html>");
  client.println("<head>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
  client.println("<title>Lamp</title>");
  client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css\" integrity=\"sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh\" crossorigin=\"anonymous\">");
  client.println("</head>");
  client.println("<body>");
  client.println("<table class=\"table\">");

  printTableRowToClient(RED, digitalRead(RED_MAIN_PIN)? ON: OFF, client);
  printTableRowToClient(GREEN, digitalRead(GREEN_MAIN_PIN)? ON: OFF, client);
  
  client.println("</table>");

  String astart = "<a href=\"";
  String amiddle = "\">";
  String aend = "</a> ";

  client.print(astart);
  client.print(statusStr);
  client.print(amiddle);
  client.print(statusStr);
  client.print(aend);
  client.print(astart);
  client.print("https://git.io/JL7eL");
  client.print(amiddle);
  client.print("Git");
  client.print(aend);

  client.println("</body>");
  client.println("</html>"); 
}

void printTableRowToClient(String name, String state, EthernetClient client){
  String td = "<td>";
  String tdc = "</td>";

  String form = "<form method=\"POST\" action=\"/";
  String formc = "</form>";

  String button = "<button class=\"btn btn-primary\"> Turn ";
  String buttonc = "</button>";

  String slash = "/";
  String actionClose = "\">";
  
  client.println("<tr>");
  client.print(td);
  client.print(name);
  client.println(tdc);
  client.println(td);
  client.print(state);
  client.println(tdc);
  client.println(td);
  if(state == OFF) {
    client.print(form);
    client.print(name);
    client.print(slash);
    client.print(ON);
    client.println(actionClose);
    client.print(button);
    client.print(ON);
    client.println(buttonc);
    client.println(formc);
  }
  else if(state == ON) {
    client.print(form);
    client.print(name);
    client.print(slash);
    client.print(OFF);
    client.println(actionClose);
    client.print(button);
    client.print(OFF);
    client.println(buttonc);
    client.println(formc);
  }
  client.println(tdc);
  client.println("</tr>");
}


void printSharedHttpHeaders(EthernetClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Connection: close");
  client.println("Cache-Control: no-cache, no-store, max-age=0, must-revalidate"); 
}

void printHttpHeadersToClient(EthernetClient client){
  printSharedHttpHeaders(client);
  client.println("Content-Type: application/json");
}

void printHttpHtmlHeadersToClient(EthernetClient client){
  printSharedHttpHeaders(client);
  client.println("Content-Type: text/html");
}
