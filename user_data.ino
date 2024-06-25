

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>

const char* ssid = "ssid"; 
const char* password = "password"; 

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char* googleScriptURL = "google script url"; // Replace with your URL

String responseHTML = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Captive Portal</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #000000; color: #ffffff; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh; }
        .container { max-width: 400px; width: 100%; padding: 20px; background-color: #2b2b2b; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.5); }
        h1 { text-align: center; }
        form { display: flex; flex-direction: column; }
        label { margin: 10px 0 5px; }
        input[type='text'] { padding: 10px; margin: 10px 0; border-radius: 5px; border: 1px solid #333; background-color: #1c1c1c; color: #fff; font-size: 16px; }
        input[type='submit'] { padding: 10px; margin: 10px 0; background-color: #007bff; border: none; border-radius: 5px; color: #fff; cursor: pointer; font-size: 16px; }
        input[type='submit']:hover { background-color: #0056b3; }
        .or { text-align: center; margin: 20px 0; font-size: 18px; }
    </style>
    <script>
        function validateForm() {
            var contact = document.getElementById('contact').value;
            if (contact.length != 10 || !/^\d{10}$/.test(contact)) {
                alert('Contact number must be exactly 10 digits.');
                return false;
            }
            return true;
        }
    </script>
</head>
<body>
    <div class='container'>
        <h1>USER DETAILS FORM</h1>
        <p>Please enter your details:</p>
        <form action='/submit' method='POST' onsubmit='return validateForm()'>
            <label for='name'>Name:</label>
            <input type='text' id='name' name='name' required>
            <label for='address'>Email Address:</label>
            <input type='text' id='address' name='address' required>
            <label for='contact'>Contact Number:</label>
            <input type='text' id='contact' name='contact' required>
            
            
            <input type='submit' value='Submit'>
        </form>
    </div>
</body>
</html>

)rawliteral";

String thankYouHTML = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Thank You</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            background-color: #000000; 
            color: #ffffff; 
            margin: 0; 
            padding: 0; 
            display: flex; 
            justify-content: center; 
            align-items: center; 
            height: 100vh; 
            text-align: center; 
        }
        .container { 
            max-width: 400px; 
            width: 100%; 
            padding: 20px; 
            background-color: #2b2b2b; 
            border-radius: 10px; 
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.5); 
        }
        h1 { 
            font-size: 24px; 
        }
    </style>
</head>
<body>
    <div class='container'>
        <h1>Data saved successfully!</h1>
    </div>
</body>
</html>

)rawliteral";

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, LOW); // Turn on the LED
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Turn off the LED initially

  connectToWiFi(); // Connect to WiFi network

  configTime(0, 0, "pool.ntp.org"); // Configure NTP

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("test");

  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.on("/", HTTP_GET, []() {
    webServer.send(200, "text/html", responseHTML);
  });

  webServer.on("/submit", HTTP_POST, []() {
    String name = webServer.arg("name");
    String address = webServer.arg("address");
    String contact = webServer.arg("contact");

    // Print the details to the Serial Monitor
    Serial.print("Name: ");
    Serial.println(name);
    Serial.print("Email Address: ");
    Serial.println(address);
    Serial.print("Contact Number: ");
    Serial.println(contact);

    // Get the current time
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);

    char dateBuffer[11];
    char timeBuffer[9];

    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", p_tm);
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", p_tm);

    // Create a JSON object
    String jsonData = "{";
    jsonData += "\"date\":\"" + String(dateBuffer) + "\",";
    jsonData += "\"time\":\"" + String(timeBuffer) + "\",";
    jsonData += "\"name\":\"" + name + "\",";
    jsonData += "\"address\":\"" + address + "\",";
    jsonData += "\"contact\":\"" + contact + "\"";
    jsonData += "}";

    // Send data to Google Sheets
    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;

    Serial.print("Connecting to ");
    Serial.println(googleScriptURL);

    http.begin(client, googleScriptURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    // Send the thank you response back to the client
    webServer.send(200, "text/html", thankYouHTML);
  });

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
