#ifndef YUELURMVISION2022_THREADMANAGER_H
#define YUELURMVISION2022_THREADMANAGER_H
#pragma once

#include "DataBuffer.hpp"
#include "DataType.hpp"
#include "Serialcodec.h"
#include <detector.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <thread>
#include "Camera.h"

using namespace std;
using namespace cv;

namespace pnx {

/**
 * @brief multi-thread task manager
 */
class ThreadManager {
public:
  /**
   * @brief Construct a new Thread Manager object
   */
  ThreadManager(){};

  ~ThreadManager() = default;

  /**
   * @brief init all thread and peripherals
   */
  void InitAll();

  /// 所有线程的入口函数都不会返回，故使用[[noreturn]]标记 (C++11),
  /// 编译器此时不会告警，可能进行特殊优化
  /// @ref https://en.cppreference.com/w/cpp/language/attributes/noreturn

  /**
   * @brief generate thread, read image from camera and save to buffer
   */
  [[noreturn]] void GenerateThread();

  /**
   * @brief process thread, process image read from buffer
   */
  [[noreturn]] void ProcessThread();

  /**
   * @brief send thread, read data from buffer and send to serial port
   */
  [[noreturn]] void SendThread();

  /**
   * @brief receive thread, receive data from serial port, decode into specific
   * struct and save to buffer
   */
  [[noreturn]] void ReceiveThread();

  void Exit();

  std::thread::native_handle_type GenerateId{};
  std::thread::native_handle_type ProcessId{};
  std::thread::native_handle_type SendId{};
  std::thread::native_handle_type ReceiveId{};

private:
  SerialCodec _serial;      /// 串口解析
  rm_auto_aim::Detector dt; /// 装甲板检测器
  VideoCapture _camera;     /// 相机
  

  DataBuffer<VisionRecvData> _receive_buffer; /// 接收缓冲区
  DataBuffer<VisionSendData> _send_buffer;    /// 发送缓冲区
  DataBuffer<cv::Mat> _img_buffer;            /// 图像缓冲区

}; // class ThreadManager

} // namespace pnx
#endif // YUELURMVISION2022_THREADMANAGER_H