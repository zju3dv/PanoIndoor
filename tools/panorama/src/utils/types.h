/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/08/19
 * */

#ifndef LOCALIZER_TYPES_H
#define LOCALIZER_TYPES_H
#include <cstdint>

#include <Eigen/Core>

#include "utils/alignment.h"

// Define non-copyable or non-movable classes.
#define NON_COPYABLE(class_name)          \
  class_name(class_name const&) = delete; \
  void operator=(class_name const& obj) = delete;
#define NON_MOVABLE(class_name) class_name(class_name&&) = delete;

namespace Eigen {

typedef Eigen::Matrix<float, 3, 4> Matrix3x4f;
typedef Eigen::Matrix<double, 3, 4> Matrix3x4d;
typedef Eigen::Matrix<uint8_t, 3, 1> Vector3ub;
typedef Eigen::Matrix<uint8_t, 4, 1> Vector4ub;
typedef Eigen::Matrix<double, 6, 1> Vector6d;

typedef Eigen::Matrix<float, 3, 3, Eigen::RowMajor> RowMatrix3f;
typedef Eigen::Matrix<double, 3, 3, Eigen::RowMajor> RowMatrix3d;
typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> RowMatrix4f;
typedef Eigen::Matrix<double, 4, 4, Eigen::RowMajor> RowMatrix4d;
typedef Eigen::Matrix<float, 3, 4, Eigen::RowMajor> RowMatrix3x4f;
typedef Eigen::Matrix<double, 3, 4, Eigen::RowMajor> RowMatrix3x4d;

typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXi;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXf;
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXd;
typedef Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXuc;

}  // namespace Eigen

namespace panorama {

////////////////////////////////////////////////////////////////////////////////
// Index types, determines the maximum number of objects.
////////////////////////////////////////////////////////////////////////////////

// Unique identifier for cameras.
typedef uint32_t camera_t;

// Unique identifier for images.
typedef uint32_t image_t;

// Each image pair gets a unique ID, see `Database::ImagePairToPairId`.
typedef uint64_t image_pair_t;

// Index per image, i.e. determines maximum number of 2D points per image.
typedef uint32_t point2D_t;

// Unique identifier per added 3D point. Since we add many 3D points,
// delete them, and possibly re-add them again, the maximum number of allowed
// unique indices should be large.
typedef uint64_t point3D_t;

// Values for invalid identifiers or indices.
const camera_t kInvalidCameraId = std::numeric_limits<camera_t>::max();
const image_t kInvalidImageId = std::numeric_limits<image_t>::max();
const image_pair_t kInvalidImagePairId =
        std::numeric_limits<image_pair_t>::max();
const point2D_t kInvalidPoint2DIdx = std::numeric_limits<point2D_t>::max();
const point3D_t kInvalidPoint3DId = std::numeric_limits<point3D_t>::max();

}  // namespace SenseLoc

// This file provides specializations of the templated hash function for
// custom types. These are used for comparison in unordered sets/maps.
namespace std {

// Hash function specialization for uint32_t pairs, e.g., image_t or camera_t.
template <>
struct hash<std::pair<uint32_t, uint32_t>> {
    std::size_t operator()(const std::pair<uint32_t, uint32_t>& p) const {
        const uint64_t s = (static_cast<uint64_t>(p.first) << 32) +
                           static_cast<uint64_t>(p.second);
        return std::hash<uint64_t>()(s);
    }
};

}  // namespace std
#endif //LOCALIZER_TYPES_H
