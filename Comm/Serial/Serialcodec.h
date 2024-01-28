/**
 * @file Serialcodec.h
 * @author modify by NeoZng (neozng1@hnu.edu.cn)
 * @brief
 * @version 0.2
 * @date 2021-08-19
 *
 * @copyright Copyright HNUYueLuRM-Vision (c) 2021
 * @todo 1.test recv API,data_len
 *       2.optimize the logic of whole recv function
 */

/* kiko@idispace.com 2021.01 */

// #define SERIAL_CODEC_DEBUG

/* SerialCodec API
 * 1. SerialCodec(int usb_id);
 * @Brief: open serial_port "/dev/ttyUSB(usb_id)"
 *
 * 2. SerialCode(std::string device_name);
 * @Brief: open serial_port "device_name"
 *
 * 3. bool try_get_recv_data_for(Protocol::VisionRecvData& recv_data, int
 * milli_secs = 10);
 * @Brief: try to retrieve data from serial port within mill_secs milliseconds
 * @return: if data has been fetched and decoded properly, then return true,
 * otherwise return false
 * @NOTICE: this method will block the process for at most mill_secs
 * milliseconds
 *
 * 4. bool send_data(int cam_id, float pitch, float yaw, float distance);
 * @Brief: encode and send data to the serial port
 * @return: true if data sent succeed
 */

#include "../Protocol/protocol.h"
#include "DataType.hpp"
#include "serial.h"

#include <chrono>
#include <exception>

namespace pnx {

// Read & Write from or to the serial port
// Ensure integrity of receieved data pack thourgh this
// wrappered class combined with Serial and Protocol
/**
 * @brief class for read & write from or to the serial port
 */
class SerialCodec : public Serial, public Protocol {
public:
  /**
   * @breif try to get a frame of data for vision
   * @param recv_data received data
   * @param milli_secs timeout, default 10ms (3ms is enough in test)
   * @return bool flag to indicate whether receiving worked fine
   */
  bool try_get_recv_data_for(VisionRecvData &recv_data, int milli_secs = 10);

  /**
   * @brief encode and send data to the serial port
   * @param state target state,
   * @param type target type
   * @param pitch
   * @param yaw
   * @return true if send successfully, false otherwise
   */
  bool send_data(TargetState state, TargetType type, float pitch, float yaw);
};

} // namespace pnx
