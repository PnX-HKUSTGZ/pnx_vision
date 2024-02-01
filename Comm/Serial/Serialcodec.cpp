#include "Serialcodec.h"
#include <chrono>
#include <exception>

constexpr int recv_data_len = 14; // default 10 bits,

namespace pnx {

bool SerialCodec::send_data(float pitch, float yaw) {
  // visionSendData should be modified
  auto data = VisionSendData(pitch, yaw);
  auto data_str = Protocol::encode(data);
  return Serial::send(data_str) > 0;
}

// try receive a valid package within milli_secs
// and decode it to a Protocol::Vision_recv_data
bool SerialCodec::try_get_recv_data_for(VisionRecvData &recv_data,
                                        int milli_secs) {
  try {
    auto start = clk::now();

    static bool pack_start = false;
    static std::string recv_buf;         // receive buffer
    static std::string tmp_str;          // receive some data from each recv
    size_t find_pos = std::string::npos; // not find by default

    tmp_str.clear();

    while (true) {
      // step1: try to receive str from serial port for up to milli_secs
      // milliseconds
      if (Serial::try_recv_for(tmp_str, milli_secs)) {
        find_pos = tmp_str.find_first_of(static_cast<char>(start_of_frame));

        // step1: handle newly received data
        // package has not started && find frame header : 0XA5
        if (!pack_start && find_pos != std::string::npos) {
          pack_start = true;
          recv_buf = tmp_str.substr(find_pos);
          // grab from the header identifier 0XA5 to the end
        } else if (!pack_start && find_pos == std::string::npos) {
          auto end = clk::now();
          auto gap = std::chrono::duration_cast<Ms>(end - start);
          if (gap.count() > milli_secs) {
            break;
          }
          // package not started and didn't find identifier
          // just pass and try to get some new data
          continue;
        } else if (pack_start && find_pos == std::string::npos) {
          // pack started and received data of this pack, just append them
          recv_buf.append(tmp_str);
        } else if (pack_start && find_pos != std::string::npos) {
          // pack started and find another frame
          // just grab the front data before next frame
          recv_buf.append(tmp_str.substr(0, find_pos));
        }

        // step2: check data integrity and unpack data
        if (recv_buf.size() >= recv_data_len) {
          // 将接收到的数据转成解码成结构体
          if (decode(recv_buf, recv_data)) {
            pack_start = false;
            recv_buf.clear();
            return true;
          } else {
            pack_start = false;
            recv_buf.clear();
            auto end = clk::now();
            auto gap = std::chrono::duration_cast<Ms>(end - start);
            if (gap.count() > milli_secs) {
              break;
            }
          }
        }
      }
      auto end = clk::now();
      auto gap = std::chrono::duration_cast<Ms>(end - start);
      if (gap.count() > milli_secs) {
        break;
      }
    }
    return false;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}

} // namespace pnx
