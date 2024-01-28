/* kiko@idiospace.com */

// #define __packed __attribute__((packed))
#pragma once
#include "DataType.hpp"
#include "stdint.h"
#include <cstdint>
#include <string>

namespace pnx {

class Protocol {

protected:
  static short constexpr start_of_frame = 0XA5;

public:
  static std::string
  encode(const VisionSendData
             &data); // Encode data from VisionSendData to std::string
  static bool
  decode(const std::string &s,
         VisionRecvData
             &decoded_data); // Decode data from std::string to VisionRecvData

private:
  static uint8_t Get_CRC8_Check(uint8_t *pchMessage, uint16_t dwLength);

  static uint8_t CRC8_Check_Sum(uint8_t *pchMessage, uint16_t dwLength);
};

} // namespace pnx
