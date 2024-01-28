
#ifndef DATATYPE_H
#define DATATYPE_H

#include <opencv2/opencv.hpp>

namespace pnx {
// compulsory class enumerate
enum class WorkMode : int {
  none = 0,
  auto_shoot = 1,    // hero,infantry
  auto_windmill = 2, // infantry
  hook_shot = 3      // hero
};

enum class SelfColor : int { none = 0, red = 1, blue = 2 };

enum class BulletSpeed : int {
  none = 0,
  hero10 = 10,
  hero16 = 16,
  infantry15 = 15,
  infantry18 = 18,
  infantry30 = 30 // sentry have the same bullet speed
};

enum class TargetState : int { none = 0, converging = 1, fire = 2 };

enum class TargetType : int {
  none = 0,
  hero = 1,
  engineer = 2,
  infantry3 = 3,
  infantry4 = 4,
  infantry5 = 5,
  outpost = 6,
  sentry = 7,
  base = 8
};

struct TargetInfo {
  TargetInfo() {}

  // TargetInfo(){}

  /**
   * @brief draw target for visualization
   *
   * @param canvas where to draw
   */
  void DrawTarget(cv::Mat &canvas) {
    // 画出边框
    for (int i = 0, j = vertexes.size(); i < j; i++) {
      cv::line(canvas, vertexes[i % j], vertexes[(i + 1) % j],
               cv::Scalar(0, 255, 0), 2);
    }
    // 写出类型
    cv::putText(canvas, std::to_string(static_cast<int>(target_type)),
                vertexes[0], 0, 1, cv::Scalar(0, 0, 255), 1);
  }

  std::vector<cv::Point2f> vertexes;
  TargetType target_type;
  bool _big_flag = true;
  bool shoot = false;
  float pitch = 0;
  float yaw = 0;
};

struct VisionSendData {
  // strut initiate function
  VisionSendData() {}
  VisionSendData(TargetState state_, TargetType type_, float pitch_, float yaw_)
      : state(state_), type(type_), pitch(pitch_), yaw(yaw_) {}

  TargetState state;
  TargetType type;

  float pitch;
  float yaw; // formatted in float data
};

struct VisionRecvData {
  VisionRecvData() {}
  VisionRecvData(SelfColor self_color_, WorkMode mode_, BulletSpeed speed_,
                 float pitch_, float yaw_, float roll_)
      : self_color(self_color_), mode(mode_), speed(speed_), pitch(pitch_),
        yaw(yaw_), roll(roll_) {}

  SelfColor self_color;
  WorkMode mode;
  BulletSpeed speed;

  float pitch;
  float yaw;
  float roll;
};
} // namespace pnx

#endif // !DATATYPE_H