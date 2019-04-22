#include <PubSubClient.h>
#include <ESP8266WiFi.h>

class SmartFarmClient
{
private:
  PubSubClient *conn;

  bool justWakeUp;
  const char *id, *user, *password;
  char reportTopic[50], receiveTopic[50];

  void retainConnection();

public:
  SmartFarmClient(
      WiFiClient clnt,
      const char *mqtt_server, int port,
      const char *user, const char *password);
  void setup(const char *id);

  void onReceiveCommand(void (*callback)(String));
  bool sendToServer(String payload);
  void loop();

  ~SmartFarmClient();

  bool connectionState()
  {
    return this->conn->connected();
  }
};
