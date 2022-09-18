#include <WiFiNINA.h>
#include "main.h"

char ssid[] = "dummy SSID";   //  your network SSID (name) between the " "
char pass[] = "dummy Password";    // your network password between the " "
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

//WiFiClient client = server.available();
WiFiClient client;

void (* resetFunc) (void) = 0;  // reset function

void setup_wifi() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  //while (!Serial);
  
  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();
  digitalWrite(LED3, HIGH);
}

void check_wifi() {
  status = WiFi.status();
  // WiFiが切断された場合に再接続しようとするが、結局できていない
  if (status != WL_CONNECTED) {
    Serial.println(status);
    WiFi.disconnect();
    digitalWrite(LED3, LOW);    
    delay(100);
    
    setup_wifi();
    digitalWrite(LED3, HIGH);
  }
}

void loop_wifi() {
  // put your main code here, to run repeatedly:
  
  // check wifi and restart if needed
  check_wifi();
  
  client = server.available();
  if (client) {
    printWEB();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  while (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    delay(1000);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB() {

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //html header
            client.print(
              "<head>"
                "<title>Badge</title>"
                "<style type='text/css'>"
                  "body {"
                    "background-color: darkgrey"
                  "}"
                  "p {"
                    "background-color: lightgrey"
                  "}"
                  "button {"
                    "width: 150;"
                    "padding: 20;"
                  "}"
                "</style>"
                
                "<form method='get' action='m0' id='bOpen'></form>"
                "<form method='get' action='m1' id='bBlink'></form>"
                "<form method='get' action='m5' id='bClose'></form>"

                "<form method='get' action='e1' id='bCenter'></form>"
                "<form method='get' action='e2' id='bUp'></form>"
                "<form method='get' action='e3' id='bDown'></form>"
                "<form method='get' action='e4' id='bRight'></form>"
                "<form method='get' action='e5' id='bLeft'></form>"
              "</head>"                         
            );

            // buttons
            client.print(
              "<body>"
                "<h1>Eye</h1>"
                "<p>"
                  "<button type=\"submit\" form='bOpen' style='position: absolute; left: 10%; top: 10%'> Open </button>"
                  "<button type=\"submit\" form='bBlink' style='position: absolute; left: 10%; top: 30%'> Blink </button>"
                  "<button type=\"submit\" form='bClose' style='position: absolute; left: 10%; top: 50%'> Close </button>"

                  "<button type=\"submit\" form='bCenter' style='position: absolute; left: 50%; top: 30%'> Center </button>"
                  "<button type=\"submit\" form='bUp' style='position: absolute; left: 50%; top: 10%'> Up </button>"
                  "<button type=\"submit\" form='bDown' style='position: absolute; left: 50%; top: 50%'> Down </button>"
                  "<button type=\"submit\" form='bRight' style='position: absolute; left: 70%; top: 30%'> Right </button>"
                  "<button type=\"submit\" form='bLeft' style='position: absolute; left: 30%; top: 30%'> Left </button>"
                "</p>"
              "</body>"           
            );

            // Eye manipulation
            /*client.print(
              "<h1>Eye</h1>"
              "<p>"
                "<body onload=\"init();\">"
                  "<canvas id=\"canvas\" width=800 height=800 style=\"background-color:white\"></canvas>"
                  "<script type=\"application/javascript\">"
                    "var canvas = document.getElementById(\"canvas\");"
                    "canvas.addEventListener(\"mousedown\", onDown, false);"
                    "canvas.addEventListener(\"mouseup\", onUp, false);"
                    "canvas.addEventListener(\"mousemove\", onMove, false);"
                    "canvas.addEventListener(\"touchstart\", touchStart, false);"
                    "canvas.addEventListener(\"touchend\", touchEnd, false);"
                    "canvas.addEventListener(\"touchmove\", touchMove, false);"
                    "function init() {"
                      "drag = false;"
                      "drawFrame();"
                      "const canvas = document.getElementById(\"canvas\");"
                      "drawEye(canvas.width / 2, canvas.height / 2);"
                    "}"
                    "function onDown(e) {"
                      "drag = true;"
                    "}"
                    "function onUp(e) {"
                      "drag = false;"
                    "}"
                    "function drawFrame() {"
                      "const canvas = document.getElementById(\"canvas\");"
                      "const ctx = canvas.getContext(\"2d\");"
                      "ctx.linestyle = \"black\";"
                      "ctx.beginPath();"
                      "ctx.arc(canvas.width / 2, canvas.height / 2, canvas.width / 2, 0, 2.0 * Math.PI);"
                      "ctx.stroke();"
                    "}"
            );

            client.print("function drawEye(x, y) {"
                          "const canvas = document.getElementById(\"canvas\");"
                          "ctx = canvas.getContext(\"2d\");"
                          "ctx.beginPath();"
                          "ctx.arc(x, y, canvas.width / 10.0, 0, 2.0 * Math.PI);"
                          "ctx.stroke();"
                        "}");

            client.print("function onMove(e) {"
                          "if (!drag) {"
                            "return;"
                          "}"
                          "var message = \"clicked \" + e.offsetX + \" \" + e.offsetY;"
                          "const canvas = document.getElementById(\"canvas\");"
                          "ctx = canvas.getContext(\"2d\");"
                          "ctx.clearRect(0, 0, canvas.width, canvas.height);"

                          // badge body
                          "drawFrame();"
                          // eye
                          "drawEye(e.offsetX, e.offsetY);"
                        "}");

            client.print("function touchStart(e) {"
                          "const pos = getPosT(e);"
                          "onDown(pos);"
                        "}");

            client.print("function touchEnd(e) {"
                          "const pos = getPosT(e);"
                          "onUp(pos);"
                        "}");

            client.print("function touchMove(e) {"
                          "const pos = getPosT(e);"
                          "onMove(pos);"
                        "}");

            // スマホのタップ位置取得
            client.print("function scrollX() {"
                          "return document.documentElement.scrollLeft || document.body.scrollLeft;"
                        "}"
                        "function scrollY() {"
                          "return document.documentElement.scrollTop || document.body.scrollTop;"
                        "}"
                        "function getPosT (e) {"
                          "const canvas = document.getElementById(\"canvas\");"
                          "var cRect = e.target.getBoundingClientRect();"
                          "var offsetX = e.changedTouches[0].clientX - cRect.left + scrollX();"
                          "var offsetY = e.changedTouches[0].clientY - cRect.top + scrollY();"
                          "return {offsetX:offsetX, offsetY:offsetY};"
                        "}");
            client.print("</script>");
            client.print("</body>");
            client.print("</p>");
            */

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /m0")) {
          eyelidCommand(0, 300);
        }
        if (currentLine.endsWith("GET /m1")) {
          eyelidCommand(1, 300);
        }
        if (currentLine.endsWith("GET /m5")) {
          eyelidCommand(5, 300);
        }
        if (currentLine.endsWith("GET /e1")) {
          eyeCommand(1);
        }
        if (currentLine.endsWith("GET /e2")) {
          eyeCommand(2);
        }
        if (currentLine.endsWith("GET /e3")) {
          eyeCommand(3);
        }
        if (currentLine.endsWith("GET /e4")) {
          eyeCommand(4);
        }
        if (currentLine.endsWith("GET /e5")) {
          eyeCommand(5);
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}