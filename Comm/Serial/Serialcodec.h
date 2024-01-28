

#include "../Protocol/protocol.h"
#include "DataType.hpp"
#include "serial.h"

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
  bool send_data(float pitch, float yaw);
};

} // namespace pnx
