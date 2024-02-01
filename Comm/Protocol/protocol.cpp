
#include "protocol.h"
#include "bsp_crc8.h"
#include <cstring>

// header (1) + data float*2 (4*2) + crc8 (1)
constexpr int send_data_len = 10; // default 10 bits,
// header (1) + data float*3 (4*3) + crc8 (1)
constexpr int recv_data_len = 14;

namespace pnx {

std::string Protocol::encode(const VisionSendData &data) {
  static std::string s = ""; // restoration of modes
  static uint8_t tx_buf[255];

  tx_buf[0] = start_of_frame;
  // put data.pitch into tx_buf[1-4]
  memcpy(&tx_buf[1], &data.pitch, 4);
  // put data.yaw into tx_buf[5-8]
  memcpy(&tx_buf[5], &data.yaw, 4);
  // crc8 check
  tx_buf[9] = Get_CRC8_Check(&tx_buf[0], 9);
  // put tx_buf into s
  s.assign((char *)tx_buf, send_data_len);
  return s;
}

bool Protocol::decode(const std::string &s, VisionRecvData &decoded_data) {
  static char ch_arr[255];
  s.copy(ch_arr, s.size());
  if (!CRC8_Check_Sum((uint8_t *)ch_arr, recv_data_len))
    return false;
  else // decode roll pitch yaw
  {
    memcpy(&decoded_data.roll, &ch_arr[1], 4);
    memcpy(&decoded_data.pitch, &ch_arr[5], 4);
    memcpy(&decoded_data.yaw, &ch_arr[9], 4);
  }
  return true;
}

uint8_t Protocol::Get_CRC8_Check(uint8_t *pchMessage, uint16_t dwLength) {
  return crc_8(pchMessage, dwLength);
}

uint8_t Protocol::CRC8_Check_Sum(uint8_t *pchMessage, uint16_t dwLength) {
  uint8_t ucExpected = 0;
  if ((pchMessage == 0) || (dwLength <= 2))
    return 0;
  ucExpected = Get_CRC8_Check(pchMessage, dwLength - 1);
  return (ucExpected == pchMessage[dwLength - 1]);
}

} // namespace pnx
