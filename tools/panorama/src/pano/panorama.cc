/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/10/19
 * */

#include "pano/panorama.h"

#include "utils/logging.h"


namespace panorama {

Panorama::Panorama() {
    panorama_width_ = 1920;
    panorama_height_ = 960;
    perspective_image_count_ = 8;
}

void Panorama::SetPerspectiveParams(int perspective_width, int perspective_height, int image_count, double fov_w) {
    for (int i = 0; i < image_count; i++) {
        perspective_widths_.emplace_back(perspective_width);
        perspective_heights_.emplace_back(perspective_height);
        // std::cout << "perpective width and height: " << perspective_width << " " << perspective_height << std::endl;
        focal_lengths_.emplace_back(perspective_width * 0.5 / tan(fov_w / 360.0 * M_PI));
    }
    perspective_image_count_ = image_count;
}

float Panorama::GetPerspectiveFocalLength(int perspective_id) const { return focal_lengths_[perspective_id]; }

int Panorama::GetPerspectiveWidth(int perspective_id) const { return perspective_widths_[perspective_id]; }

int Panorama::GetPerspectiveHeight(int perspective_id) const { return perspective_heights_[perspective_id]; }

int Panorama::GetPanoramaWidth() const { return panorama_width_; }

int Panorama::GetPanoramaHeight() const { return panorama_height_; }

std::vector<Eigen::RowMatrixXd> Panorama::GetPanoramaMapX() const {
    return map_xs_;
}

std::vector<Eigen::RowMatrixXd> Panorama::GetPanoramaMapY() const {
    return map_ys_;
}

Eigen::RowMatrixXi Panorama::GetPanoramaRMapX() const { return rmap_x_; }

Eigen::RowMatrixXi Panorama::GetPanoramaRMapY() const { return rmap_y_; }

Eigen::RowMatrixXi Panorama::GetPanoramaRmapId() const { return rmap_idx_; }

bool Panorama::UpdateMap(Eigen::RowMatrixXd &map_x, Eigen::RowMatrixXd &map_y, const Eigen::Matrix3d &transform,
                         const int perspective_id, const int perspective_width, const int perspective_height,
                         const double focal_length) {
    // For each pixel in the output image
    for (int u = 0; u < perspective_width; u++) {
        for (int v = 0; v < perspective_height; v++) {
            float u_original, v_original;
            PersepectivePointToPanoramaPoint(u, v, u_original, v_original, transform, perspective_width,
                                             perspective_height, focal_length);
            map_x(v, u) = u_original;
            map_y(v, u) = v_original;

            // Get int original pixel value
            int u_ori = std::round(u_original);
            int v_ori = std::round(v_original);
            rmap_x_(v_ori, u_ori) = u;
            rmap_y_(v_ori, u_ori) = v;

            rmap_idx_(v_ori, u_ori) = perspective_id;
        }
    }

    return true;
}

// Set the perspective params and generate correspondence map_x and map_y
bool Panorama::PerspectiveParamsProcess(int image_width, int image_height, size_t image_count, double fov_w,
                                        int panorama_width, int panorama_height, double pitch_inc) {
    map_xs_.clear();
    map_ys_.clear();
    transforms_.clear();
    perspective_widths_.clear();
    perspective_heights_.clear();
    focal_lengths_.clear();

    // Set perspective params
    SetPerspectiveParams(image_width, image_height, image_count, fov_w);

    // Set Panorama image size
    panorama_width_ = panorama_width;
    panorama_height_ = panorama_height;

    // Resize rmap
    rmap_x_.resize(panorama_height, panorama_width);
    rmap_y_.resize(panorama_height, panorama_width);
    rmap_idx_.resize(panorama_height, panorama_width);
    rmap_idx_.fill(-1);

    // Generate map_x and map_y
    double yaw_interval = 360.0 / static_cast<double>(image_count);

    const double disturbation_pitch = 0;
    const double disturbation_yaw = 0;
    const double disturbation_roll = 0;

    for (size_t i = 0; i < image_count; ++i) {
        // Calculate current roll pitch and yaw
        double roll, pitch, yaw;
        pitch = disturbation_pitch + pitch_inc;
        yaw = disturbation_yaw + i * yaw_interval;
        roll = disturbation_roll;

        Eigen::Matrix3d transform;
        transform = Eigen::AngleAxisd(yaw / 180 * M_PI, Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(pitch / 180 * M_PI, Eigen::Vector3d::UnitX()) *
                    Eigen::AngleAxisd(roll / 180 * M_PI, Eigen::Vector3d::UnitZ());

        transforms_.emplace_back(transform);
        // std::cout << "Transform matrix = " << transform << std::endl;

        // Create map_x and map_y with the same size and output image
        Eigen::RowMatrixXd cur_map_x, cur_map_y;
        cur_map_x.resize(image_height, image_width);
        cur_map_y.resize(image_height, image_width);

        // Update Map
        UpdateMap(cur_map_x, cur_map_y, transform, i, image_width, image_height, focal_lengths_[i]);

        // Update map_y and map_x
        map_xs_.emplace_back(cur_map_x);
        map_ys_.emplace_back(cur_map_y);
    }

    perspective_image_count_ = image_count;

    return true;
}

// Set the perspective params and generate correspondence map_x and map_y
bool Panorama::PerspectiveParamsProcess(int perspective_width, double fov_w, double fov_h, size_t image_count,
                                        int panorama_width, int panorama_height) {
    map_xs_.clear();
    map_ys_.clear();
    transforms_.clear();
    perspective_widths_.clear();
    perspective_heights_.clear();
    focal_lengths_.clear();

    // Set perspective params
    for (int i = 0; i < image_count; i++) {
        perspective_widths_.emplace_back(perspective_width);
        double focal_length = perspective_width * 0.5 / tan(fov_w / 360.0 * M_PI);
        focal_lengths_.emplace_back(focal_length);
        int perspective_height = (int)(focal_length * tan(fov_h / 360.0 * M_PI) * 2);
        perspective_heights_.emplace_back(perspective_height);
        std::cout << "perpective width and height: " << perspective_width << " " << perspective_height << std::endl;
    }
    perspective_image_count_ = image_count;

    // Set Panorama image size
    panorama_width_ = panorama_width;
    panorama_height_ = panorama_height;

    // Resize rmap
    rmap_x_.resize(panorama_height, panorama_width);
    rmap_y_.resize(panorama_height, panorama_width);
    rmap_idx_.resize(panorama_height, panorama_width);
    rmap_idx_.fill(-1);

    // Generate map_x and map_y
    double yaw_interval = 360.0 / static_cast<double>(image_count);

    const double disturbation_pitch = 0;
    const double disturbation_yaw = 0;
    const double disturbation_roll = 0;

    // Bitmap bitmap;
    // bitmap.Allocate(panorama_width, panorama_height, false);
    // BitmapColor<uint8_t> color(0);
    // bitmap.Fill(color);

    for (size_t i = 0; i < image_count; ++i) {
        // Calculate current roll pitch and yaw
        double roll, pitch, yaw;
        pitch = disturbation_pitch;
        yaw = disturbation_yaw + i * yaw_interval;
        roll = disturbation_roll;

        Eigen::Matrix3d transform;
        transform = Eigen::AngleAxisd(yaw / 180 * M_PI, Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(pitch / 180 * M_PI, Eigen::Vector3d::UnitX()) *
                    Eigen::AngleAxisd(roll / 180 * M_PI, Eigen::Vector3d::UnitZ());

        transforms_.emplace_back(transform);
        // std::cout << "Transform matrix = " << transform << std::endl;

        // Create map_x and map_y with the same size and output image
        Eigen::RowMatrixXd cur_map_x, cur_map_y;
        cur_map_x.resize(perspective_heights_[i], perspective_widths_[i]);
        cur_map_y.resize(perspective_heights_[i], perspective_widths_[i]);

        // Update Map
        UpdateMap(cur_map_x, cur_map_y, transform, i, perspective_widths_[i], perspective_heights_[i],
                  focal_lengths_[i]);

        // Update map_y and map_x
        map_xs_.emplace_back(cur_map_x);
        map_ys_.emplace_back(cur_map_y);

        // Update rmap_x and rmap_y
        Eigen::Matrix3d transform_inv = transform.inverse();
        double u, v, w;
        for (int y = 0; y < panorama_height; ++y) {
            for (int x = 0; x < panorama_width; ++x) {
                Eigen::Vector3d bearing;
                ConvertPanoramaToBearing(x, y, bearing);
                bearing = transform_inv * bearing;
                ConvertBearingToPerspective(bearing,
                                            perspective_widths_[i], perspective_heights_[i],
                                            focal_lengths_[i], u, v, w);
                if (w < 0 || u < 0 || v < 0 || u >= perspective_widths_[i] ||
                    v >= perspective_heights_[i]) {
                    continue;
                }
                rmap_x_(y, x) = u;
                rmap_y_(y, x) = v;
                rmap_idx_(y, x) = i;
                // bitmap.SetPixel(x, y, BitmapColor<uint8_t>((i + 1) * 30));
            }
        }
    }

    // bitmap.Write("panorama-2-perspective.jpg");

    perspective_image_count_ = image_count;

    return true;
}

bool Panorama::PerspectiveParamsProcess(int panorama_width, int panorama_height,
                                        std::vector<PanoramaParam> panorama_params) {

    map_xs_.clear();
    map_ys_.clear();
    transforms_.clear();
    perspective_widths_.clear();
    perspective_heights_.clear();
    focal_lengths_.clear();

    // Set Panorama image size
    panorama_width_ = panorama_width;
    panorama_height_ = panorama_height;

    // Resize rmap
    rmap_x_.resize(panorama_height, panorama_width);
    rmap_y_.resize(panorama_height, panorama_width);
    rmap_idx_.resize(panorama_height, panorama_width);
    rmap_idx_.fill(-1);

    for (size_t i = 0; i < panorama_params.size(); ++i) {
        double focal_length = panorama_params[i].pers_w * 0.5 / tan(panorama_params[i].fov_w / 360.0 * M_PI);

        // Calculate current roll pitch and yaw
        Eigen::Matrix3d transform;
        transform = Eigen::AngleAxisd(panorama_params[i].yaw / 180 * M_PI, Eigen::Vector3d::UnitY()) *
                    Eigen::AngleAxisd(panorama_params[i].pitch / 180 * M_PI, Eigen::Vector3d::UnitX()) *
                    Eigen::AngleAxisd(panorama_params[i].roll / 180 * M_PI, Eigen::Vector3d::UnitZ());

        transforms_.emplace_back(transform);

        // std::cout << "Transform matrix = " << transform << std::endl;

        // Create map_x and map_y with the same size and output image
        Eigen::RowMatrixXd cur_map_x, cur_map_y;
        cur_map_x.resize(panorama_params[i].pers_h, panorama_params[i].pers_w);
        cur_map_y.resize(panorama_params[i].pers_h, panorama_params[i].pers_w);
        //
        UpdateMap(cur_map_x, cur_map_y, transform, i, panorama_params[i].pers_w, panorama_params[i].pers_h,
                  focal_length);

        // Update map_y and map_x
        map_xs_.emplace_back(cur_map_x);
        map_ys_.emplace_back(cur_map_y);

        focal_lengths_.emplace_back(focal_length);
        perspective_widths_.emplace_back(panorama_params[i].pers_w);
        perspective_heights_.emplace_back(panorama_params[i].pers_h);

    }
    perspective_image_count_ = panorama_params.size();

    return true;
}

bool Panorama::PanoramaToPerspectives(const Bitmap *img_in, std::vector<Bitmap> &img_outs) {
    img_outs.clear();
    // Check the input image
    if (img_in->Width() != panorama_width_ || img_in->Height() != panorama_height_) {
        std::cout << "Error! Input image size error ... " << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < (size_t)perspective_image_count_; ++i) {
        Bitmap img_out;
        if (img_in->Channels() == 3) {
            img_out.Allocate(perspective_widths_[i], perspective_heights_[i], true);
        } else {
            img_out.Allocate(perspective_widths_[i], perspective_heights_[i], false);
        }
        for (int y = 0; y < perspective_heights_[i]; ++y) {
            for (int x = 0; x < perspective_widths_[i]; ++x) {
                BitmapColor<float> color;
                img_in->InterpolateBilinear(map_xs_[i](y, x), map_ys_[i](y, x), &color);
                BitmapColor<uint8_t> color_uint;
                color_uint.r = color.r > 255.0 ? 255 : static_cast<uint8_t>(color.r);
                color_uint.g = color.g > 255.0 ? 255 : static_cast<uint8_t>(color.g);
                color_uint.b = color.b > 255.0 ? 255 : static_cast<uint8_t>(color.b);
                img_out.SetPixel(x, y, color_uint);
            }
        }
        img_outs.emplace_back(img_out);
    }

    return true;
}

void Panorama::ConvertPerspectiveCoordToBearings(int perspective_id, double u, double v, Eigen::Vector3d &bearing) {
    CHECK_LT(perspective_id, perspective_image_count_);
    CHECK_GE(perspective_id, 0);

    Eigen::Matrix3d transform = transforms_[perspective_id];

    double x = u - perspective_widths_[perspective_id] * 0.5;
    double y = v - perspective_heights_[perspective_id] * 0.5;
    double z = focal_lengths_[perspective_id];

    Eigen::Vector3d point(x, y, z);
    bearing = transform * point;

    bearing.normalize();
}

void Panorama::ConvertBearingsToPanorama(const Eigen::Vector3d &bearing, double &u, double &v) {
    double lat = -std::asin(bearing[1]);
    double lon = std::atan2(bearing[0], bearing[2]);

    u = panorama_width_ * (0.5 + lon / (2 * M_PI));
    u = std::max(0.0f, std::min(static_cast<float>(u), static_cast<float>(panorama_width_ - 1)));

    v = panorama_height_ * (0.5 - lat / M_PI);
    v = std::max(0.0f, std::min(static_cast<float>(v), static_cast<float>(panorama_height_ - 1)));
}

void Panorama::ConvertPanoramaToBearing(const double u, const double v, Eigen::Vector3d &bearing) {
    const double lon = (u / panorama_width_ - 0.5) * (2 * M_PI);
    const double lat = -(v / panorama_height_ - 0.5) * M_PI;

    // convert to equirectangular coordinates
    bearing(0) = std::cos(lat) * std::sin(lon);
    bearing(1) = -std::sin(lat);
    bearing(2) = std::cos(lat) * std::cos(lon);
}

void Panorama::ConvertBearingToPerspective(const Eigen::Vector3d &bearing,
                                           const int perspective_width, const int perspective_height,
                                           const double focal_length, double& u, double& v, double& w) {
    Eigen::Vector3d point = bearing / std::fabs(bearing.z());
    point *= focal_length;
    u = point.x() + perspective_width * 0.5;
    v = point.y() + perspective_height * 0.5;
    w = point.z();
}

void Panorama::ConvertPerspectiveCoordToPanorama(int perspective_id, double u_in, double v_in, double &u_out,
                                                 double &v_out) {
    CHECK_LT(perspective_id, perspective_image_count_);
    CHECK_GE(perspective_id, 0);

    Eigen::Vector3d bearing;
    ConvertPerspectiveCoordToBearings(perspective_id, u_in, v_in, bearing);
    ConvertBearingsToPanorama(bearing, u_out, v_out);
    // std::cout << perspective_id << "old u_out = " << u_out << " , old v_out = " << v_out << std::endl;

    // FIXME:
    // u_in = u_in > map_xs_[perspective_id].cols() ? map_xs_[perspective_id].cols() - 0.01 : u_in;
    // u_in = u_in < 0 ? 0.01 : u_in;
    // v_in = v_in > map_xs_[perspective_id].rows() ? map_xs_[perspective_id].rows() - 0.01 : v_in;
    // v_in = v_in < 0 ? 0.01 : v_in;

    // u_out = map_xs_[perspective_id](v_in, u_in);
    // v_out = map_ys_[perspective_id](v_in, u_in);
    // std::cout << perspective_id << "new u_out = " << u_out << " , new v_out = " << v_out << std::endl;
}

bool Panorama::PersepectivePointToPanoramaPoint(const int u_in, const int v_in, float &u_out, float &v_out,
                                                const Eigen::Matrix3d &transform, const int perspective_width,
                                                const int perspective_height, const double focal_length) {
    // Convert the u,v input to 3d coordinates
    double x = u_in - perspective_width * 0.5;
    double y = v_in - perspective_height * 0.5;
    double z = focal_length;

    Eigen::Vector3d point(x, y, z);

    Eigen::Vector3d bearing = transform * point;

    bearing.normalize();

    double lat = -std::asin(bearing[1]);
    double lon = std::atan2(bearing[0], bearing[2]);

    u_out = panorama_width_ * (0.5 + lon / (2 * M_PI));
    u_out = std::max(0.0f, std::min(u_out, static_cast<float>(panorama_width_ - 1.5)));

    v_out = panorama_height_ * (0.5 - lat / M_PI);
    v_out = std::max(0.0f, std::min(v_out, static_cast<float>(panorama_height_ - 1.5)));

    return true;
}

}  // namespace panorama
