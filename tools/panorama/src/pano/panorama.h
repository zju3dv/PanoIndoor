/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/10/19
 * */

#ifndef PANORAMA_PANORAMA_H
#define PANORAMA_PANORAMA_H

#include <stdio.h>
#include <iostream>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

// Opencv
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "utils/bitmap.h"
#include "utils/types.h"

namespace panorama {

struct PanoramaParam {
    double pitch, yaw, roll, fov_w, fov_h;
    int pers_w, pers_h;
    PanoramaParam() {}
    PanoramaParam(double pitch, double yaw, double roll, double fov_w, int pers_w, int pers_h)
            : pitch(pitch), yaw(yaw), roll(roll), fov_w(fov_w), pers_w(pers_w), pers_h(pers_h) {}
};

class Panorama {
public:
    /**
     * Convert panorama to perspective projection
     *
     */
    void SetPerspectiveParams(int perspective_width, int perspective_height, int image_count, double fov_w);

    float GetPerspectiveFocalLength(int perspective_id) const;

    int GetPerspectiveWidth(int perspective_id) const;

    int GetPerspectiveHeight(int perspective_id) const;

    int GetPanoramaWidth() const;

    int GetPanoramaHeight() const;

    // Get pixel map from perspective image to panorama image
    std::vector<Eigen::RowMatrixXd> GetPanoramaMapX() const;
    std::vector<Eigen::RowMatrixXd> GetPanoramaMapY() const;

    // Get pixel map from panorama image to perspective image
    Eigen::RowMatrixXi GetPanoramaRMapX() const;

    Eigen::RowMatrixXi GetPanoramaRMapY() const;

    Eigen::RowMatrixXi GetPanoramaRmapId() const;

    // convert the coordnate in one of the four perspected images onto the
    // bearing of the panorama,i.e. the surface of the unit sphere
    void ConvertPerspectiveCoordToBearings(int perspective_id, double u, double v, Eigen::Vector3d& bearing);

    // convert the coord in one of the four perspective images onto the panorama image
    void ConvertPerspectiveCoordToPanorama(int perspective_id, double u_in, double v_in, double& u_out, double& v_out);

    // convert the coord on the panorama image onto the bearing
    void ConvertPanoramaToBearing(const double u, const double v, Eigen::Vector3d& bearing);

    void ConvertBearingToPerspective(const Eigen::Vector3d &bearing,
                                     const int perspective_width, const int perspective_height,
                                     const double focal_length, double& u, double& v, double& w);

    // convert the coord on the bearing onto the orignal panorama image
    void ConvertBearingsToPanorama(const Eigen::Vector3d& bearing, double& u, double& v);

    // Process the panorama image in batch
    bool PanoramaToPerspectives(const Bitmap* img_in, std::vector<Bitmap>& img_outs);

    bool PerspectiveParamsProcess(int image_width, int image_height, size_t image_count, double fov_w,
                                  int panorama_width, int panorama_height, double pitch_inc=0.0);

    bool PerspectiveParamsProcess(int perspective_width, double fov_w, double fov_h, size_t image_count,
                                  int panorama_width, int panorama_height);

    bool PerspectiveParamsProcess(int panorama_width, int panorama_height, std::vector<PanoramaParam> panorama_params);

    bool PersepectivePointToPanoramaPoint(const int u_in, const int v_in, float& u_out, float& v_out,
                                          const Eigen::Matrix3d& transform, const int perspective_width,
                                          const int perspective_height, const double focal_length);

    Panorama();

    ~Panorama() {}

private:
    bool UpdateMap(Eigen::RowMatrixXd& map_x, Eigen::RowMatrixXd& map_y, const Eigen::Matrix3d& transform,
                   const int perspective_id, const int perspective_width, const int perspective_height,
                   const double focal_length);

private:
    // Input panorama image size
    int panorama_width_, panorama_height_;

    int perspective_image_count_;

    // Output perspective image sizes
    std::vector<int> perspective_widths_;
    std::vector<int> perspective_heights_;

    // Output image focal length
    std::vector<double> focal_lengths_;

    // Create the camera transform matrix
    std::vector<Eigen::Matrix3d> transforms_;

    // Image transform matrix mat(perspective pixel -> panorama pixel)
    std::vector<Eigen::RowMatrixXd> map_xs_;
    std::vector<Eigen::RowMatrixXd> map_ys_;

    // Image transform matrix mat(panorama pixel -> perspective pixel)
    Eigen::RowMatrixXi rmap_x_;
    Eigen::RowMatrixXi rmap_y_;
    Eigen::RowMatrixXi rmap_idx_;

    std::vector<double> rolls_;
    std::vector<double> pitches_;
    std::vector<double> yaws_;
};

}  // namespace panorama


#endif //PANORAMA_PANORAMA_H
