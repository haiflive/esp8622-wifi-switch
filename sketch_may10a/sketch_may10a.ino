/*********
  Description: simple wi-fi switch(on/off) for esp-8266-01
  Author: haiflive
  Board version: esp8266-01
  Source project details at https://randomnerdtutorials.com/esp8266-web-server/
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "your_wifi_connection_name";
const char* password = "your_wifi_password";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
bool is_changing_state_header;

// Auxiliar variables to store the current output state
String output0State = "off";
String output1State = "off";
String output2State = "off";
String output3State = "off";

// Assign output variables to GPIO pins
const int output0 = 0;
const int output1 = 1;
const int output2 = 2;
const int output3 = 3;

// register relay
const char* host = "192.168.1.105";
const uint16_t port = 80;
const char* url = "/relay_registration.php";
unsigned long timeout_loop = 0;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output0, OUTPUT);
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  // Set outputs to HIGH
  digitalWrite(output0, HIGH);
  digitalWrite(output1, HIGH);
  digitalWrite(output2, HIGH);
  digitalWrite(output3, HIGH);

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
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  registerRelay();

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
            is_changing_state_header = false;

            // turns the GPIOs on and off
            if (header.indexOf("GET /0/on") >= 0) {
              Serial.println("GPIO 0 on");
              output0State = "on";
              is_changing_state_header = true;
              digitalWrite(output0, LOW);
            } else if (header.indexOf("GET /0/off") >= 0) {
              Serial.println("GPIO 0 off");
              output0State = "off";
              is_changing_state_header = true;
              digitalWrite(output0, HIGH);
            } else if (header.indexOf("GET /1/on") >= 0) {
              Serial.println("GPIO 1 on");
              output1State = "on";
              is_changing_state_header = true;
              digitalWrite(output1, LOW);
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("GPIO 1 off");
              output1State = "off";
              is_changing_state_header = true;
              digitalWrite(output1, HIGH);
            } else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("GPIO 2 on");
              output2State = "on";
              is_changing_state_header = true;
              digitalWrite(output2, LOW);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("GPIO 2 off");
              output2State = "off";
              is_changing_state_header = true;
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /3/on") >= 0) {
              Serial.println("GPIO 3 on");
              output3State = "on";
              is_changing_state_header = true;
              digitalWrite(output3, LOW);
            } else if (header.indexOf("GET /3/off") >= 0) {
              Serial.println("GPIO 3 off");
              output3State = "off";
              is_changing_state_header = true;
              digitalWrite(output3, HIGH);
            }

            if(is_changing_state_header) {
              // redirrect to main page after state changed
              client.println("HTTP/1.1 302 Found");
              client.println("Location: /");
              client.println("Connection: close");
              client.println();
            } else {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons 
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #77878A;}</style></head>");
              
              // Web Page Heading
              client.println("<body><h1>ESP8266 Web Server</h1>");
              
              // Display current state, and ON/OFF buttons for GPIO 0  
              client.println("<p>GPIO 0 - State</p>");
              // If the output0State is off, it displays the ON button       
              if (output0State=="on") {
                client.println("<p><a href=\"/0/off\"><button class=\"button\">ON</button></a></p>");
              } else {
                client.println("<p><a href=\"/0/on\"><button class=\"button button2\">OFF</button></a></p>");
              } 
                
              // Display current state, and ON/OFF buttons for GPIO 1  
              client.println("<p>GPIO 1 - State </p>");
              // If the output1State is off, it displays the ON button       
              if (output1State=="on") {
                client.println("<p><a href=\"/1/off\"><button class=\"button\">ON</button></a></p>");
              } else {
                client.println("<p><a href=\"/1/on\"><button class=\"button button2\">OFF</button></a></p>");
              }

              // Display current state, and ON/OFF buttons for GPIO 2  
              client.println("<p>GPIO 2 - State </p>");
              // If the output2State is off, it displays the ON button       
              if (output2State=="on") {
                client.println("<p><a href=\"/2/off\"><button class=\"button\">ON</button></a></p>");
              } else {
                client.println("<p><a href=\"/2/on\"><button class=\"button button2\">OFF</button></a></p>");
              }

              // Display current state, and ON/OFF buttons for GPIO 3  
              client.println("<p>GPIO 3 - State </p>");
              // If the output3State is off, it displays the ON button       
              if (output3State=="on") {
                client.println("<p><a href=\"/3/off\"><button class=\"button\">ON</button></a></p>");
              } else {
                client.println("<p><a href=\"/3/on\"><button class=\"button button2\">OFF</button></a></p>");
              }
              client.println("</body></html>");
              
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
            }

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

/**
 * send ping to registration relay every 60 seconds
 */
void registerRelay() {
  if(timeout_loop == 0) {
    timeout_loop = millis();
  }

  if (millis() - timeout_loop < 60000) {
    return; // continue
  }

  timeout_loop = 0; // reset timer
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  Serial.println("sending data to server");
  if (client.connected()) {
//    client.println("GET /relay_registration.php HTTP/1.1\r\nHost: "+"site.ru"+"\r\nConnection: close\r\n\r\n");
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(6000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
