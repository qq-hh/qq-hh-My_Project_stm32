#include <ESP8266WiFi.h>
const char* ssid = "Bean";
const char* password = "Bean@#Tech";
//静态地址、网关、子网掩码
WiFiServer server(80);
IPAddress local_IP(192, 168, 1, 80);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
String ss = "";
String httpResponse = "HTTP/1.0 200 OK\r\n"
                      "Connection: close\r\n"
                      "Content-Type: application/json;\r\n"
                      "\r\n"
                      "{\"code\":1}";
void setup() {
  Serial.begin(115200);
  WiFi.config(local_IP, gateway, subnet);//设置静态IP
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);//路由器的WiFi名称和密码
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
   server.begin();
}

void loop() {
  WiFiClient incomingClient = server.available();
  if (incomingClient) {
    if (incomingClient.find("\r\n\r\n")) {
      while (incomingClient.available()) {
        ss = ss + char(incomingClient.read());
      }
    }
    Serial.println(ss);
    incomingClient.print(httpResponse);
    ss = "";
  }
}
