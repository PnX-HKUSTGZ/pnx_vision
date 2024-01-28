/* kiko@idiospace.com 2020.01.20 */

#ifndef DataBuffer_HPP
#define DataBuffer_HPP

#include <chrono>
#include <exception>
#include <mutex>

#define DATABUFFER_DEBUG

namespace pnx {

template <typename DataType> class DataBuffer {
protected:
  using Ms = std::chrono::milliseconds;
  using validator = bool (*)(const DataType &); // validate data func

public:
  DataBuffer() = default;

  ~DataBuffer() = default;

  bool Get(DataType &data, validator v = nullptr);

  bool Update(const DataType &data, validator v = nullptr);

private:
  DataType data_buf;
  std::timed_mutex mtx;
}; // class DataBuffer

template <class DataType>
bool DataBuffer<DataType>::Get(DataType &data, validator v) {
  try {
    if (v != nullptr && !v(data))
      return false; // 数据校验

    if (mtx.try_lock_for(Ms(2))) {
      // 加锁成功
      std::lock_guard<std::timed_mutex> guard(mtx, std::adopt_lock);
      // 检查是否为空
      //                if(data_buf.empty()) return false;

      // 弹出数据
      data = data_buf;

      return true;
    } else {

      return false;
    }
  } catch (std::exception &e) {
#ifdef DATABUFFER_DEBUG

    return false;
#endif // DATABUFFER_DEBUG
  }
}

template <class DataType>
bool DataBuffer<DataType>::Update(const DataType &data, validator v) {
  try {
    if (v != nullptr && !v(data))
      return false;

    if (mtx.try_lock_for(Ms(2))) {
      std::lock_guard<std::timed_mutex> guard(mtx, std::adopt_lock);
      data_buf = data;

      return true;
    } else {

      return false;
    }
  } catch (std::exception &e) {
#ifdef DATABUFFER_DEBUG

    return false;
#endif // DATABUFFER_DEBUG
  }
}

} // namespace pnx
#endif // DataBuffer_HPP
