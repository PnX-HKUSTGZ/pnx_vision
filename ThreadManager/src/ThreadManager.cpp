#include "ThreadManager.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace cv;

using Ms = std::chrono::microseconds;

namespace pnx {
// 初始化函数
// 最佳实践是将参数放在配置文件中,然后在这里读取,而不是写死在代码中
void ThreadManager::InitAll() {
  // 打开串口
  if (!_serial.open_port("/dev/ttyACM0")) {
    std::cout << "open_port failed" << std::endl;
    if (!_serial.open_port("/dev/ttyACM1"))
      exit(250);
  }
  _serial.rm_init();

  if (!_camera.open("0")) {
    std::cout << "open camera failed" << std::endl;
    std::exit(-1);
  }

  // ----------------------------------------------------
  int min_lightness = 80;
  int detect_color = 0;
  rm_auto_aim::Detector::ArmorParams armor_params = {
      .min_light_ratio = 0.6,
      .min_small_center_distance = 0.8,
      .max_small_center_distance = 2.8,
      .min_large_center_distance = 3.2,
      .max_large_center_distance = 4.3,
      .max_angle = 35.0};
  rm_auto_aim::Detector::LightParams light_params = {
      .min_ratio = 0.1, .max_ratio = 0.55, .max_angle = 40.0};
  auto model_path =
      "/home/rm/hnu-yuelu-rm2023-vision/ArmorDetectorT/model/mlp.onnx";
  auto label_path =
      "/home/rm/hnu-yuelu-rm2023-vision/ArmorDetectorT/model/label.txt";
  dt.initModel(min_lightness, detect_color, light_params, armor_params,
               model_path, label_path);

  std::cout << "init all done" << std::endl;
}

void ThreadManager::Exit() {
  _camera.release(); // 关闭相机
}

// 从相机buffer中获取数据,并进行处理
[[noreturn]] void ThreadManager::GenerateThread() {
  cv::Mat temp_frame;
  while (true) {
    // 判断是否成功获取图像
    _camera >> temp_frame;
    // 将图像存入缓冲区
    _img_buffer.Update(temp_frame);

    // 可能还需要加入时间戳信息,用于对齐串口数据

    std::this_thread::sleep_for(Ms(10));
  }
}

//  核心处理线程
[[noreturn]] void ThreadManager::ProcessThread() {
  cv::Mat temp_frame;       /// 临时图像缓冲
  TargetInfo temp_armor;    /// 临时装甲板对象缓冲
  VisionSendData temp_send; /// 串口准备发送的数据

  // 还需要从串口中获取数据
  // ...

  // 获取弹速\云台角度\云台状态\自瞄模式等
  while (true) {
    // 尝试获取一帧图像
    if (_img_buffer.Get(temp_frame)) {

      if (temp_frame.empty()) {
        // 忽略空的帧
        continue;
      }
      int enemy_color = 0; // 写死红色,实际上需要从串口中获取
      // 推理
      dt.detect_for_target(temp_frame, enemy_color, temp_armor);
      _send_buffer.Update(temp_send);
    }
    // 处理完之后还需要进行坐标转换\运动预测\子弹下落补偿等,再放入发送buffer中
    std::this_thread::sleep_for(Ms(10));
  }
}

[[noreturn]] void ThreadManager::SendThread() {
  // 从发送buffer中获取数据,并发送
  VisionSendData temp_send;

  while (true) {
    _send_buffer.Get(temp_send, nullptr);
    _serial.send_data(temp_send.pitch, temp_send.yaw);
    std::this_thread::sleep_for(Ms(10));
  }
}

[[noreturn]] void ThreadManager::ReceiveThread() {
  // 从串口中获取数据,并存入接收buffer中
  // 串口高频率轮询,还需要保存收到数据的时间戳,从而完成与相机的时间戳对齐
  VisionRecvData temp_recv;
  while (true) {
    _serial.try_get_recv_data_for(temp_recv);
    _receive_buffer.Update(temp_recv);

    std::this_thread::sleep_for(Ms(10));
  }
}

} // namespace pnx