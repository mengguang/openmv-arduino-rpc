/* Arduino RPC (Remote Procedure Call) library
   Copyright (c) 2020 OpenMV
   written by Kwabena Agyeman and Larry Bank
*/

#ifndef __ARDUINO_RPC__
#define __ARDUINO_RPC__

#include <Arduino.h>
#include <SoftwareSerial.h>

const uint8_t __COMMAND_HEADER_PACKET_MAGIC[] = {0x09, 0x12}; // 0x1209
const uint8_t __COMMAND_DATA_PACKET_MAGIC[] = {0xd1, 0xab}; // 0xABD1
const uint8_t __RESULT_HEADER_PACKET_MAGIC[] = {0x21, 0x90}; // 0x9021
const uint8_t __RESULT_DATA_PACKET_MAGIC[] = {0xba, 0x1d}; // 0x1DBA

//
// The list of registered callback functions for remote procedure calls
// is limited in length by the amount of available RAM. On AVR targets,
// using a std::map or dictionary library would use too much FLASH and RAM
// so we'll use a fixed size list instead. For non-AVR targets, we can allow
// more simultaneous callbacks
//
#ifdef __AVR__
#define MAX_CALLBACKS 16
#else
#define MAX_CALLBACKS 32
#endif

// Callback function prototype
typedef void (RPC_CALLBACK)(int iEvent, uint8_t *data, int data_len);

typedef struct tagrpclist
{
  RPC_CALLBACK *pfnCallback;
  int id;
} RPCLIST;
// Communication types
enum
{
  RPC_UART=0,
  RPC_SOFTUART,
  RPC_I2C,
  RPC_CAN,
  RPC_SPI,
  RPC_USB
};
 
class RPC
{
  public:
    bool begin(int comm_type, unsigned long speed, int pin1, int pin2);
    bool begin(int comm_type, unsigned long speed);
    bool register_callback(int rpc_id, RPC_CALLBACK *);
    bool call(int rpc_id, uint8_t *out_data, int out_data_len, uint8_t *in_data, int *in_data_len, int send_timeout, int recv_timeout);

  private:
    uint16_t _crc16(uint8_t *data, int len);
    bool _get_packet(uint16_t magic_value, uint8_t *data, int *data_len, int timeout);
    bool _put_packet(uint16_t magic_value, uint8_t *data, int data_len, int timeout);
    bool _put_command(int cmd, uint8_t *data, int data_len, int timeout);
    bool _get_result(uint8_t *data, int *data_len, int timeout);

    int _comm_type; // communication method (e.g. UART/I2C/SPI/USB/CAN)
    unsigned long _speed; // baud/bit rate
    int _pin1, _pin2; // optional pins to specify the comm bus
    SoftwareSerial *_sserial;
    RPCLIST _rpcList[MAX_CALLBACKS]; // list of registered callbacks
    int _rpc_count; // number of registered callback functions
};

#endif // __ARDUINO_RPC__
