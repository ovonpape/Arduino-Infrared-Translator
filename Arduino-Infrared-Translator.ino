#include <IRremote.h>
#include <avr/sleep.h>

const int RECV_PIN = 2; // Data Pin of the IR-Receiver
const int SEND_PIN = 3; // Base Pin of the Transistor used to drive the IR-LED. You can try to drive the LED directly but it might affect signal strenght

// Protocol and Address that are Sent by the remote control you want to use
const decode_type_t INCOMING_PROTOCOL = NEC;
const uint16_t INCOMING_ADDRESS = 0;

// Protocol and Address that are expected by the device you want to control
const decode_type_t SENDING_PROTOCOL = NEC;
const uint16_t SENDING_ADDRESS = 2;

// DEBUGGING_MODE controls the output of the received signals. By activating this, you can obtain the commands that are sent/expected by your device and remote control (via Serial Monitor)
const bool DEBUGGING_MODE = true;

// Commands that are expected from the device to be controlled.
// These specific values were obtained from the remote of a SilverCrest Soundbar.
// If you want to control a different device, you will need to obtained the commands by enabling DEBUGGING_MODE.
enum SendingCommands
{
  POWER = 72,
  MUTE = 73,
  SRC_AUX = 13,
  SRC_HDMI = 70,
  SRC_BLUETOOTH = 25,
  SRC_OPTICAL = 9,
  SRC_LINE_IN = 69,
  BT_PAIR = 79,
  MC_PREV = 68,
  MC_NEXT = 64,
  MC_PAUSE = 67,
  UP = 5,
  DOWN = 15,
  VOL_UP = 22,
  VOL_DOWN = 12,
  TREBLE_UP = 94,
  TREBLE_DOWN = 24,
  BASS_UP = 28,
  BASS_DOWN = 8,
  EQ_MUSIC = 90,
  EQ_MOVIE = 66,
  EQ_DIALOG = 82,
  EQ_3D = 74
};

// Commands sent by the universal remote control (i.e. the one you want to use to control the device). 
// These specific values were obtained from a FireTV remote configured to work with an Amazon Basics Soundbar.
// If you are using a different remote, you will need to obtained the commands by enabling DEBUGGING_MODE.
enum ReceivingCommands
{
  EC_POWER = 64,
  EC_MUTE = 72,
  EC_VOL_UP = 65,
  EC_VOL_DOWN = 69,
};

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

bool dataReceived = false;
uint16_t receivedCommand;

void setup()
{
  if (DEBUGGING_MODE)
  {
    Serial.begin(9600);
  }
  irrecv.enableIRIn();                                             // Start IR reception
  attachInterrupt(digitalPinToInterrupt(RECV_PIN), irIsr, CHANGE); // Add interrupt

  startupRoutine(); //comment this out if you dont want to perform any specfic upon startup
}

void loop()
{
  if (dataReceived)
  {
    SendingCommands sendingCommand = decodeCommand(receivedCommand);
    sendCommand(sendingCommand);
    dataReceived = false;
  }

  // TODO: implement sleep-mode for reduced Power consumption
}

//Performs actions upon startup (in my case: Power on and Change Source, EQ Profile and Volume).
void startupRoutine()
{

  delay(1000);
  sendCommand(POWER);
  delay(500);
  sendCommand(SRC_OPTICAL);
  delay(500);
  sendCommand(EQ_DIALOG);

  for (int i = 0; i < 10; i++)
  {
    delay(500);
    sendCommand(VOL_DOWN);
  }
}

void irIsr()
{

  if (irrecv.decode()) // When an IR signal is received
  {

    if (dataReceived == false)
    {
      decode_type_t receivedProtocol = irrecv.decodedIRData.protocol;
      uint16_t receivedAddress = irrecv.decodedIRData.address;
      receivedCommand = irrecv.decodedIRData.command;

      if (DEBUGGING_MODE)
      {
        IRRawDataType receivedData = irrecv.decodedIRData.decodedRawData;
        Serial.println("Protocol: " + String(receivedProtocol) +
                       ", Address: " + String(receivedAddress) +
                       ", Command: " + String(receivedCommand) +
                       ", Data: " + String(receivedData));
      }

      if (receivedProtocol == INCOMING_PROTOCOL && receivedAddress == INCOMING_ADDRESS)
      {
        dataReceived = true;
      }
      irrecv.resume(); // Continue reception for the next signal
    }
  }
}

void sendCommand(SendingCommands sendingCommand)
{
  uint8_t sRepeats = 0;
  switch (SENDING_PROTOCOL)
  {

  case NEC:
    irsend.sendNEC(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats);
    break;
  case NEC2:
    irsend.sendNEC2(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case ONKYO:
    irsend.sendOnkyo(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case RC5:
    irsend.sendRC5(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats);
    break;
  case RC6:
    irsend.sendRC6(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case SAMSUNG:
    irsend.sendSamsung(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case SAMSUNG48:
    irsend.sendSamsung48(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case SAMSUNG_LG:
    irsend.sendSamsungLG(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case SHARP:
    irsend.sendSharp(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  case SONY:
    irsend.sendSony(SENDING_ADDRESS & 0xFF, sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;

  case FAST:
    irsend.sendFAST(sendingCommand, sRepeats); // Not Tested yet, might require changes
    break;
  }
}

SendingCommands decodeCommand(ReceivingCommands receivedCommand)
{
  switch (receivedCommand)
  {
  case EC_POWER:
    return POWER;
  case EC_MUTE:
    return MUTE;
  case EC_VOL_UP:
    return VOL_UP;
  case EC_VOL_DOWN:
    return VOL_DOWN;

  default:
    return -1;
  }
}