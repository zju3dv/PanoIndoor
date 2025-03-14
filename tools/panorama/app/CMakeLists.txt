# Copyright (c) 2020, SenseTime.
# Author: Hailin Yu
# Email: yuhailin@sensetime.com
# Created Time: 2020/12/31

# convert panorama to perspective
set(ConvertPanoramaToPerspective convert_pano_perspective)
add_executable(${ConvertPanoramaToPerspective} convert_pano_perspective.cc)
target_link_libraries(${ConvertPanoramaToPerspective} panorama)