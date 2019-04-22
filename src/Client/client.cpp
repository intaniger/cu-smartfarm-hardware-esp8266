#include "client.h"

void (*onCommandReceived)(String);

void mqttCallback(char *topic, byte *payload, unsigned int msglen)
{
  char buffer[msglen];
  for (int i = 0; i < msglen; i++)
  {
    buffer[i] = (char)payload[i];
  }
  onCommandReceived(String(buffer).substring(0, msglen));
}

SmartFarmClient::SmartFarmClient(WiFiClient clnt, const char *server, int port, const char *user, const char *password)
{
  this->user = user;
  this->password = password;
  this->conn = new PubSubClient(clnt);
  this->conn->setServer(server, port);
  this->justWakeUp = true;
}

void SmartFarmClient::setup(const char *id)
{
  this->id = id;
  strcpy(this->receiveTopic, "CUSmartFarm/");
  strcpy(this->reportTopic, "CUSmartFarm/");
  strcat(this->receiveTopic, this->id);
  strcat(this->receiveTopic, "/svr_out");
  strcat(this->reportTopic, this->id);
  strcat(this->reportTopic, "/svr_recv");
}

void SmartFarmClient::onReceiveCommand(void (*callback)(String))
{
  onCommandReceived = callback;
  this->conn->setCallback(mqttCallback);
}

void SmartFarmClient::retainConnection()
{
  while (!this->conn->connected())
  {
    if (this->conn->connect(this->id, this->user, this->password))
    {
      this->conn->subscribe(this->receiveTopic);
    }
    else
    {
      delay(500);
    }
  }
}

bool SmartFarmClient::sendToServer(String payload)
{
  this->retainConnection();
  char cpayload[200];
  payload.toCharArray(cpayload, 200);
  return this->conn->publish(this->reportTopic, cpayload);
}

void SmartFarmClient::loop()
{
  this->retainConnection();
  this->conn->loop();
  if (this->justWakeUp)
  {
    this->justWakeUp = false;
    this->sendToServer("{\"t\": \"greeting\"}");
  }
}

SmartFarmClient::~SmartFarmClient()
{
}