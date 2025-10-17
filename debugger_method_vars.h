#include "esphome.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>


#define RX_PIN 16
#define TX_PIN 17

#define CMD_VAL 1
#define SEND_MODE 6
#define SEND_FAN 7
#define SEND_TEMP 8
#define SEND_CRC 14
#define SEND_LEN 16

#define REC_MODE 8
#define REC_FAN 9
#define REC_TEMP 10
#define T1_INDEX 11
#define T2A_INDEX 12
#define T2B_INDEX 13
#define T3_INDEX 14

using namespace esphome;

#define IS_8266 0
const uart_config_t uart_config = {
    .baud_rate = 4800,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};


size_t readBuffSize = 0;
std::vector<uint8_t> recData = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
std::vector<uint8_t> sendFollowMeData = {0xaa, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x00, 0x55};
std::vector<uint8_t> sendData = {0xaa, 0xc3, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x55};
char waitSendData[] = {0xaa, 0xc3, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x55};
std::vector<uint8_t> queryData = {0xaa, 0xc0, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x81, 0x55};
std::vector<uint8_t> checkData = {0xaa, 0xc0, 0x80, 0x00, 0x00, 0x00};


//class xyeVars {
//  public:
uint8_t setTemp = 70;
uint8_t sendTimeCount = 0;
bool newInput = false;
std::string op = "off";
std::string fan = "auto";
uint8_t fanBytes = 0x00;
uint8_t opBytes = 0x00;
bool doneReading = false;
bool firstBoot = true;
uint8_t waitCount = 0;
uint8_t prevResp = 0;
bool waitingToSend = false;
bool waitingForResponse = false;
bool commandSent = false;
bool fanRec = false;
bool tempRec = false;
bool opRec = false;
uint8_t requestedSetTemp = 70;
bool followMeInit = false;

//} xyeVars;
