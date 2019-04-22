#include <Arduino.h>
#include <WiFiManager.h>

#include <HardwareController/controller.h>
#include <Client/client.h>

IPAddress apIP(21, 21, 21, 21);
WiFiManager wifiManager;
WiFiClient espClient;
SmartFarmClient client(espClient, "164.115.27.177", 1883, "", "");

Controller hardware;

int lastConnectionCheckMillis = 0;
char DeviceID[15],
    hotspotSSID[50] = "CU_SMARTFARM_AP_";

String flatMACAddress(String macAddr)
{
  return macAddr.substring(0, 2) +
         macAddr.substring(3, 5) +
         macAddr.substring(6, 8) +
         macAddr.substring(9, 11) +
         macAddr.substring(12, 14) +
         macAddr.substring(15);
}

void passCommand(String cmd)
{
  hardware.SendCommand(cmd);
}

void reportStatusToServer(String status)
{
  client.sendToServer(status);
}

void setup()
{
  Serial.begin(115200);
  delay(1000); // Wait for Mega 2560 ready
  flatMACAddress(WiFi.macAddress()).toCharArray(DeviceID, 15);
  strcat(hotspotSSID, DeviceID);

  hardware.SendCommand("{\"cmd\":\"display_connection_info\", \"DevID\":\"" + String(DeviceID) + "\"}");
  WiFi.mode(WIFI_AP);
  wifiManager.setConnectTimeout(5);
  wifiManager.setDebugOutput(false);
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  wifiManager.autoConnect(hotspotSSID);

  client.setup(DeviceID);
  client.onReceiveCommand(passCommand);
  hardware.SendCommand("{\"cmd\":\"indicate_connection_status\", \"status\":\"internet_connected\"}");

  hardware.SubscribeStatus(reportStatusToServer);
}

void loop()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    ESP.reset();
  }
  if (millis() - lastConnectionCheckMillis > 5000)
  {
    lastConnectionCheckMillis = millis();
    hardware.SendCommand(
        client.connectionState() ? "{\"cmd\":\"indicate_connection_status\", \"status\":\"connected\"}" : "{\"cmd\":\"indicate_connection_status\", \"status\":\"failure\"}");
  }

  client.loop();
  hardware.loop();
  delay(100);
}