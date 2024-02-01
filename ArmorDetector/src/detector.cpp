// Copyright (c) 2022 ChenJun
// Licensed under the MIT License.

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

// STD
#include <algorithm>
#include <cmath>
#include <vector>

#include "armor.hpp"
#include "detector.hpp"

#include "DataType.hpp"

namespace rm_auto_aim {
Detector::Detector(const int &init_min_l, const int &init_color,
                   const LightParams &init_l, const ArmorParams &init_a)
    : min_lightness(init_min_l), detect_color(init_color), l(init_l),
      a(init_a) {}

std::vector<Armor> Detector::detect(const cv::Mat &input, int enemy_color) {
  binary_img = preprocessImage(input);
  lights_ = findLights(input, binary_img);
  armors_ = matchLights(lights_, enemy_color);

  if (!armors_.empty()) {
    classifier->extractNumbers(input, armors_);
    classifier->classify(armors_);
  }

  return armors_;
}

cv::Mat Detector::getAllNumbersImage() {
  if (armors_.empty()) {
    return cv::Mat(cv::Size(20, 28), CV_8UC1);
  } else {
    std::vector<cv::Mat> number_imgs;
    number_imgs.reserve(armors_.size());
    for (auto &armor : armors_) {
      number_imgs.emplace_back(armor.number_img);
    }
    cv::Mat all_num_img;
    cv::vconcat(number_imgs, all_num_img);
    return all_num_img;
  }
}

void Detector::drawResults(cv::Mat &img) {
  // Draw Lights
  for (const auto &light : lights_) {
    cv::circle(img, light.top, 3, cv::Scalar(255, 255, 255), 1);
    cv::circle(img, light.bottom, 3, cv::Scalar(255, 255, 255), 1);
    auto line_color =
        light.color == RED ? cv::Scalar(255, 255, 0) : cv::Scalar(255, 0, 255);
    cv::line(img, light.top, light.bottom, line_color, 1);
  }

  // Draw armors
  for (const auto &armor : armors_) {
    cv::line(img, armor.left_light.top, armor.right_light.bottom,
             cv::Scalar(0, 255, 0), 2);
    cv::line(img, armor.left_light.bottom, armor.right_light.top,
             cv::Scalar(0, 255, 0), 2);
  }

  // Show numbers and confidence
  for (const auto &armor : armors_) {
    cv::putText(img, armor.classfication_result, armor.left_light.top,
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 255), 2);
  }
}

cv::Mat Detector::preprocessImage(const cv::Mat &rgb_img) const {
  cv::Mat gray_img;
  cv::cvtColor(rgb_img, gray_img, cv::COLOR_RGB2GRAY);

  cv::Mat binary_img;
  cv::threshold(gray_img, binary_img, min_lightness, 255, cv::THRESH_BINARY);

  return binary_img;
}

std::vector<Light> Detector::findLights(const cv::Mat &rbg_img,
                                        const cv::Mat &binary_img) {
  using std::vector;
  vector<vector<cv::Point>> contours;
  vector<cv::Vec4i> hierarchy;
  cv::findContours(binary_img, contours, hierarchy, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);

  vector<Light> lights;

  for (const auto &contour : contours) {
    if (contour.size() < 5)
      continue;

    auto r_rect = cv::minAreaRect(contour);
    auto light = Light(r_rect);

    if (isLight(light)) {
      auto rect = light.boundingRect();
      if ( // Avoid assertion failed
          0 <= rect.x && 0 <= rect.width &&
          rect.x + rect.width <= rbg_img.cols && 0 <= rect.y &&
          0 <= rect.height && rect.y + rect.height <= rbg_img.rows) {
        int sum_r = 0, sum_b = 0;
        auto roi = rbg_img(rect);
        // Iterate through the ROI
        for (int i = 0; i < roi.rows; i++) {
          for (int j = 0; j < roi.cols; j++) {
            if (cv::pointPolygonTest(
                    contour, cv::Point2f(j + rect.x, i + rect.y), false) >= 0) {
              // if point is inside contour
              sum_r += roi.at<cv::Vec3b>(i, j)[0];
              sum_b += roi.at<cv::Vec3b>(i, j)[2];
            }
          }
        }
        // Sum of red pixels > sum of blue pixels ?
        light.color = sum_r > sum_b ? RED : BLUE;
        lights.emplace_back(light);
      }
    }
  }

  return lights;
}

bool Detector::isLight(const Light &light) {
  // The ratio of light (short side / long side)
  float ratio = light.width / light.length;
  bool ratio_ok = l.min_ratio < ratio && ratio < l.max_ratio;

  bool angle_ok = light.tilt_angle < l.max_angle;

  bool is_light = ratio_ok && angle_ok;

  return is_light;
}

std::vector<Armor> Detector::matchLights(const std::vector<Light> &lights,
                                         int enemy_color) {
  std::vector<Armor> armors;

  // Loop all the pairing of lights
  for (auto light_1 = lights.begin(); light_1 != lights.end(); light_1++) {
    for (auto light_2 = light_1 + 1; light_2 != lights.end(); light_2++) {
      if (light_1->color != enemy_color || light_2->color != enemy_color)
        continue;

      if (containLight(*light_1, *light_2, lights)) {
        continue;
      }
      auto armor = Armor(*light_1, *light_2);
      if (isArmor(armor)) {
        armors.emplace_back(armor);
      }
    }
  }

  return armors;
}

// Check if there is another light in the boundingRect formed by the 2 lights
bool Detector::containLight(const Light &light_1, const Light &light_2,
                            const std::vector<Light> &lights) {
  auto points = std::vector<cv::Point2f>{light_1.top, light_1.bottom,
                                         light_2.top, light_2.bottom};
  auto bounding_rect = cv::boundingRect(points);

  for (const auto &test_light : lights) {
    if (test_light.center == light_1.center ||
        test_light.center == light_2.center)
      continue;

    if (bounding_rect.contains(test_light.top) ||
        bounding_rect.contains(test_light.bottom) ||
        bounding_rect.contains(test_light.center)) {
      return true;
    }
  }

  return false;
}

bool Detector::isArmor(Armor &armor) {
  Light light_1 = armor.left_light;
  Light light_2 = armor.right_light;
  // Ratio of the length of 2 lights (short side / long side)
  float light_length_ratio = light_1.length < light_2.length
                                 ? light_1.length / light_2.length
                                 : light_2.length / light_1.length;
  bool light_ratio_ok = light_length_ratio > a.min_light_ratio;

  // Distance between the center of 2 lights (unit : light length)
  float avg_light_length = (light_1.length + light_2.length) / 2;
  float center_distance =
      cv::norm(light_1.center - light_2.center) / avg_light_length;
  bool center_distance_ok = (a.min_small_center_distance < center_distance &&
                             center_distance < a.max_small_center_distance) ||
                            (a.min_large_center_distance < center_distance &&
                             center_distance < a.max_large_center_distance);

  // Angle of light center connection
  cv::Point2f diff = light_1.center - light_2.center;
  float angle = std::abs(std::atan(diff.y / diff.x)) / CV_PI * 180;
  bool angle_ok = angle < a.max_angle;

  bool is_armor = light_ratio_ok && center_distance_ok && angle_ok;
  armor.armor_type =
      center_distance > a.min_large_center_distance ? LARGE : SMALL;

  return is_armor;
}

void Detector::initModel(const int &init_min_l, const int &init_color,
                         const LightParams &init_l, const ArmorParams &init_a,
                         std::string model_path, std::string label_path) {
  min_lightness = init_min_l;
  detect_color = init_color;
  l = init_l;
  a = init_a;
  double threshold = 0.7;
  this->classifier =
      std::make_unique<NumberClassifier>(model_path, label_path, threshold);
}

int Detector::detect_for_target(const cv::Mat &frame, int color_label,
                                pnx::TargetInfo &target) {
  auto armors = detect(frame, color_label);

  cv::Mat raw= frame.clone();
  drawResults(raw);
  cv::imshow("raw", raw);
  cv::waitKey(1);

  if (armors.empty())
    return 0;

  std::sort(armors.begin(), armors.end(),
            [&frame](const Armor &armor1, const Armor &armor2) {
              cv::Point2f _center((frame.cols - 1) / 2.0,
                                  (frame.rows - 1) / 2.0);
              return cv::norm(armor1.center - _center) >
                     cv::norm(armor2.center - _center);
            });

  auto armor = armors[0];
  // 整理输出
  target.shoot = true;
  std::vector<cv::Point2f> vertexes(
      {armor.left_light.top, armor.left_light.bottom, armor.right_light.bottom,
       armor.right_light.top});
  target.vertexes = vertexes;
  target.pitch = armor.center.y - (float)frame.rows / 2;
  target.yaw = armor.center.x - (float)frame.cols / 2;
  target._big_flag = (armor.armor_type == LARGE);

  target.target_type = pnx::TargetType(armor.idx);

  return 1;
}

} // namespace rm_auto_aim
