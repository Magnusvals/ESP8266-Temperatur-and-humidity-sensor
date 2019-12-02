// Code For ESP8266 + DHT22 Temperatur and Humidity sensor colection using a simple HTML page.
// Used Exampel code from Adafruit in this sketch.



/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Replace with your network credentials
const char* ssid     = "YOUR_SSID_HERE";
const char* password = "YOUR_PASSWORD_HERE";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

#define DHTPIN 14  // GPIO 14 is D5 on ESP8266
// Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE); // Creates DHT sensor object

uint32_t delayMS;

String data[2];



void setup() {
  Serial.begin(9600);

  // Initialize device.
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 2000;





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

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  delay(delayMS);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    data[0] = String(event.temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    data[1] = String(event.relative_humidity);
    Serial.println(F("%"));
  }




  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
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


            // Display the HTML web page



            client.println("<!doctype html><html><head>");
            client.println("<meta charset = \"UHF-8\">");
            client.println("<title>DHT22 sensor</title>");
            client.println("<meta http-equiv=\"refresh\" content=\"5; url=\">");
            // client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");

            client.println("<style>");
            client.println("body {background-color: #7F7F7F;}");
            client.println("h1 {color: White; size: 30px; text-align: center;}");
            client.println("p {color: white; font-size: 15px}");
            client.println("table, th, td {border: 2px solid black;padding: 2px;}");
            client.println("img {width: 70px; height: 100px;}");
            client.println("</style>");

            client.println("</head>");
            client.println("<body>");
            client.println("<h1> DHT Temperature and Humidity sensor </h1>");

            client.println("<table style=\"width: 1000\">");

            client.println("<tr>");
            client.println("<th>Temperature</th>");
            client.println("<th>Humidity</th>");
            client.println("</tr>");


            client.println("<tr>");
            client.print("<td>");
            client.print(data[0]);
            client.println("</td>");
            client.print("<td>");
            client.print(data[1]);
            client.println("</td>");
            client.println("</tr>");


            client.println("</table>");
            client.println("</body>");
            client.println("</html>");

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
