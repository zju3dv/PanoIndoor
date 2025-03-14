# Copyright (c) 2020, SenseTime.
# Author: Hailin Yu
# Email: yuhailin@sensetime.com
# Created Time: 2020/08/19

include_directories(.)

set(LIBRARY_NAME panorama)

aux_source_directory(pano pano_src)
aux_source_directory(utils utils_src)

add_library(${LIBRARY_NAME} SHARED ${pano_src} ${utils_src})

target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES}
        ${FREEIMAGE_LIBRARIES} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} ${GLOG_LIBRARIES})

## install
install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)