// hardware controller <Mega 2560>
#include <Arduino.h>

class Controller
{
private:
public:
  Controller();
  ~Controller();

  void SubscribeStatus(void (*callback)(String));
  void SendCommand(String cmd);
  bool loop();

  void (*subCallback)(String);
};