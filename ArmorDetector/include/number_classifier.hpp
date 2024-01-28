// Copyright 2022 Chen Jun

#ifndef ARMOR_DETECTOR__NUMBER_CLASSIFIER_HPP_
#define ARMOR_DETECTOR__NUMBER_CLASSIFIER_HPP_

// OpenCV
#include <opencv2/opencv.hpp>

// STL
#include <string>
#include <vector>

#include "armor.hpp"

namespace rm_auto_aim
{
    class NumberClassifier
    {
    public:
        NumberClassifier(
            const std::string &model_path, const std::string &label_path, const double threshold);

        void extractNumbers(const cv::Mat &src, std::vector<Armor> &armors);

        void classify(std::vector<Armor> &armors);

        double threshold;

    private:
        cv::dnn::Net net_;
        std::vector<std::string> class_names_;
    };
}  // namespace rm_auto_aim

#endif  // ARMOR_DETECTOR__NUMBER_CLASSIFIER_HPP_