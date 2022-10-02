#include <stdint.h>
#include <stdlib.h>
#include "ncerr.h"



err enterSPP();

err enterHID();

void mouseCommand(uint8_t buttons, uint8_t x, uint8_t y);

// void keyboardCommand(uint8_t key);

// void keyboardEnterCommand();

err enterCmdModeValidate();

// bool validateMode(String word);

// bool validateHID(String word);

// bool saveRemoteAddress(String address);

// String getRemoteAddress();

// bool connectToRemote(String address);

// String getInquiry(uint8_t timeout);

err connectToSaved(int idx);

err disconnectRemote();

void randomMouseCmd();