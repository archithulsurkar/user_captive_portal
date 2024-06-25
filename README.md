# Captive Portal with ESP8266

This project sets up a captive portal using an ESP8266 WiFi module, which collects user details (name, email address, and contact number) and sends the data to a Google Sheets document via a Google Script URL.

## Components Required

- ESP8266 module (e.g., NodeMCU or Wemos D1 Mini)
- USB cable to connect ESP8266 to the computer
- Computer with Arduino IDE installed

## Setup Instructions

### Hardware Setup

1. Connect the ESP8266 module to your computer using a USB cable.

### Software Setup

1. **Install the Arduino IDE:**
   Download and install the Arduino IDE from the [official Arduino website](https://www.arduino.cc/en/software).

2. **Install ESP8266 Board Package:**
   - Open the Arduino IDE.
   - Go to `File` > `Preferences`.
   - In the "Additional Board Manager URLs" field, add the following URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`.
   - Go to `Tools` > `Board` > `Boards Manager`.
   - Search for "esp8266" and install the latest version of the package.

3. **Install Required Libraries:**
   Install the following libraries using the Library Manager (`Sketch` > `Include Library` > `Manage Libraries...`):
   - `ESP8266WiFi`
   - `DNSServer`
   - `ESP8266WebServer`
   - `ESP8266HTTPClient`
   - `WiFiClientSecureBearSSL`

4. **Update WiFi Credentials:**
   Replace the placeholder `ssid` and `password` values with your WiFi network's SSID and password:

   ```cpp
   const char* ssid = "your-ssid";
   const char* password = "your-password";
   ```

5. **Update Google Script URL:**
   Replace the placeholder `googleScriptURL` with your Google Script URL:

   ```cpp
   const char* googleScriptURL = "your-google-script-url";
   ```

6. **Upload the Code:**
   - Select the appropriate board and port from `Tools` > `Board` and `Tools` > `Port`.
   - Click the upload button to upload the code to your ESP8266 module.

## How It Works

1. **WiFi Connection:**
   The ESP8266 connects to the specified WiFi network. The onboard LED will turn on once the connection is successful.

2. **Captive Portal Setup:**
   - The ESP8266 starts an access point with the SSID "test".
   - A DNS server is configured to redirect all DNS queries to the ESP8266's IP address.
   - A web server is set up to serve an HTML form for user details collection.

3. **User Interaction:**
   - When a user connects to the "test" WiFi network and tries to open any web page, they will be redirected to the captive portal.
   - The user fills in their details (name, email address, and contact number) and submits the form.

4. **Data Handling:**
   - The ESP8266 receives the form data and logs it to the serial monitor.
   - The current date and time are obtained using NTP.
   - The data is formatted into a JSON object and sent to the specified Google Script URL.
   - The user is presented with a "Thank You" page upon successful data submission.

## HTML Structure and Styling

The HTML served by the ESP8266 includes basic styling and form validation. The form requires the user's name, email address, and a 10-digit contact number.

## Code Overview

### Libraries

```cpp
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
```

### WiFi Credentials

```cpp
const char* ssid = "your-ssid";
const char* password = "your-password";
```

### DNS and Web Server Setup

```cpp
const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
```

### Google Script URL

```cpp
const char* googleScriptURL = "your-google-script-url";
```

### HTML Content

- Captive portal form: `responseHTML`
- Thank you page: `thankYouHTML`

### Functions

- `connectToWiFi()`: Connects to the specified WiFi network.
- `setup()`: Initializes the serial communication, connects to WiFi, configures NTP, sets up the access point, DNS server, and web server routes.
- `loop()`: Processes DNS requests and handles web server clients.

## Serial Monitor

Use the Serial Monitor to view debug messages and form data submitted by the user.

## Notes

- Ensure that the Google Script URL is correctly configured to accept POST requests and log data to Google Sheets.
- The ESP8266 needs to be connected to the internet to send data to Google Sheets. Ensure it can access your WiFi network and the internet.

---

This README provides an overview of the project and instructions for setting it up. For further customization and troubleshooting, refer to the ESP8266 and Arduino documentation.
