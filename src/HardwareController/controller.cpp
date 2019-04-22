#include "controller.h"

Controller::Controller()
{
  Serial.begin(115200);
}

Controller::~Controller()
{
}

bool Controller::loop()
{
  bool isNewDataApproach = false;
  String newData = "";
  while (Serial.available())
  {
    isNewDataApproach = true;
    newData = Serial.readStringUntil('\r');
  }
  if (isNewDataApproach)
  {
    this->subCallback(newData);
  }

  return isNewDataApproach;
}

void Controller::SubscribeStatus(void (*callback)(String))
{
  this->subCallback = callback;
}

void Controller::SendCommand(String cmd)
{
  if (cmd.lastIndexOf('\r') == -1)
    Serial.print(cmd + "\r");
  else
    Serial.print(cmd);
}