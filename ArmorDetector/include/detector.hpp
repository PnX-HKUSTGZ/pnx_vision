// Copyright 2022 Chen Jun
// Licensed under the MIT License.

#ifndef ARMOR_DETECTOR__DETECTOR_HPP_
#define ARMOR_DETECTOR__DETECTOR_HPP_

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>

// STD
#include <vector>

#include "DataType.hpp"
#include "armor.hpp"
#include "number_classifier.hpp"

namespace rm_auto_aim {
class Detector {
public:
  struct LightParams {
    // width / height
    double min_ratio;
    double max_ratio;
    // vertical angle
    double max_angle;
  };
  struct ArmorParams {
    double min_light_ratio;
    double min_small_center_distance;
    double max_small_center_distance;
    double min_large_center_distance;
    double max_large_center_distance;
    // horizontal angle
    double max_angle;
  };

  Detector(const int &init_min_l, const int &init_color,
           const LightParams &init_l, const ArmorParams &init_a);

  Detector() = default;

  int min_lightness;
  int detect_color;
  LightParams l;
  ArmorParams a;

  std::unique_ptr<NumberClassifier> classifier;

  // Debug msgs
  cv::Mat binary_img;

  std::vector<Armor> detect(const cv::Mat &input, int enemy_color);

  void drawResults(cv::Mat &img);

  cv::Mat getAllNumbersImage();

  cv::Mat preprocessImage(const cv::Mat &input) const;

  std::vector<Light> findLights(const cv::Mat &rbg_img,
                                const cv::Mat &binary_img);

  std::vector<Armor> matchLights(const std::vector<Light> &lights,
                                 int enemy_color);

  void initModel(const int &init_min_l, const int &init_color,
                 const LightParams &init_l, const ArmorParams &init_a,
                 std::string model_path, std::string label_path);

  int detect_for_target(const cv::Mat &frame, int color_label,
                        pnx::TargetInfo &target);

private:
  std::vector<Light> lights_;
  std::vector<Armor> armors_;

  bool isLight(const Light &light);

  bool containLight(const Light &light_1, const Light &light_2,
                    const std::vector<Light> &lights);

  bool isArmor(Armor &armor);
};

} // namespace rm_auto_aim

#endif // ARMOR_DETECTOR__DETECTOR_HPP_
