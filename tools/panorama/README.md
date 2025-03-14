# Panorama

## Requirements
* CMake >= 3.12
* C++ 11
* freeimage
* glog
* OpenCV
* Eigen3
* Boost

## Install
```commandline
cd ./tools/panorama
mkdir build && cd build
cmake ..
sudo make install
```

## Usage
```commandline
convert_pano_perspective --input_path /path/to/data/canteen/images/insta_pano_default/VID_20240117_160856  \
                         --output_path /path/to/perspective_images_save_folder
```