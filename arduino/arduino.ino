#include "WiFiEsp.h"
#include "WiFiEspUdp.h"

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

char ssid[] = "";
char pass[] = "";
int status = WL_IDLE_STATUS;

int sc1 = 5;
int sc2 = 9;

int dc1 = 7;
int dc2 = 8;
int dc3 = 3;

int lt1 = 12;
int lt2 = 13;

unsigned int localPort = 10002;  // local port to listen on

WiFiEspUDP Udp;

char buffer[255];          // buffer to hold incoming packet

void setup()
{
  pinMode(sc1, OUTPUT);
  pinMode(sc2, OUTPUT);

  pinMode(lt1, OUTPUT);
  pinMode(lt2, OUTPUT);

  pinMode(dc1, OUTPUT);
  pinMode(dc2, OUTPUT);
  pinMode(dc3, OUTPUT);

  Serial.begin(115200);   // initialize serial for debugging
  Serial1.begin(9600);    // initialize serial for ESP module
  WiFi.init(&Serial1);    // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  Serial.print("Attempting to start AP ");
  Serial.println(ssid);

  // start access point
  status = WiFi.beginAP(ssid, 10, pass, ENC_TYPE_WPA2_PSK);

  Serial.println("Access point started");
  printWifiStatus();
  
  Udp.begin(localPort);
  Serial.print("Listening on port ");
  Serial.println(localPort);
}


void loop()
{
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // read the packet into packetBufffer
    int len = Udp.read(buffer, 255);
    if (len > 0) {
      buffer[len] = 0;
    }

    processCommand(len);
  }
}

void processCommand(int len) {
  if (buffer[0] == 'H') {
    digitalWrite(lt1, HIGH);
  } else if (buffer[0] == 'L') {
    digitalWrite(lt1, LOW);
  } else if (buffer[0] == 'A') {
    // turn left
    String pwm;
    for(uint32_t i = 1; i < len; i++) {
      pwm += (char)buffer[i];
    }
    analogWrite(sc1, pwm.toInt());
    analogWrite(sc2, 0);
  } else if (buffer[0] == 'D') {
    // turn right
    String pwm;
    for(uint32_t i = 1; i < len; i++) {
      pwm += (char)buffer[i];
    }
    analogWrite(sc1, 0);
    analogWrite(sc2, pwm.toInt());
  } else if (buffer[0] == 'W') {
    // forward
    String pwm;
    for(uint32_t i = 1; i < len; i++) {
      pwm += (char)buffer[i];
    }
    digitalWrite(dc1, LOW);
    digitalWrite(dc2, HIGH);
    analogWrite(dc3, pwm.toInt());
  } else if (buffer[0] == 'S') {
    // backward
    String pwm;
    for(uint32_t i = 1; i < len; i++) {
      pwm += (char)buffer[i];
    }
    digitalWrite(dc1, HIGH);
    digitalWrite(dc2, LOW);
    analogWrite(dc3, pwm.toInt());
  } else if (buffer[0] == 'B') {
    // brake
    digitalWrite(dc1, LOW);
    digitalWrite(dc2, LOW);
    analogWrite(dc3, 0);
  } else if (buffer[0] == 'P') {
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("ACK");
    Udp.endPacket();
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
}

