/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/10/19
 * */
#include <vector>
#include <string>

#include "pano/panorama.h"
#include "utils/options.h"
#include "utils/misc.h"

std::vector<std::string> LoadImageList(const std::string &path) {
    std::vector<std::string> image_path_list;
    auto file_path_list = panorama::GetRecursiveFileList(path);
    auto is_image = [](const std::string &path) {
        return panorama::HasFileExtension(path, ".png") ||
               panorama::HasFileExtension(path, ".jpg") ||
               panorama::HasFileExtension(path, ".bmp");
    };
    image_path_list.reserve(file_path_list.size());
    for (const auto & path : file_path_list) {
        if (is_image(path)) {
            image_path_list.emplace_back(path);
        }
    }
    std::sort(image_path_list.begin(), image_path_list.end());
    return image_path_list;
}

std::string SubDirectory(const std::string &path, const std::string &part) {
    auto cpath = path;
    if (!boost::filesystem::is_directory(cpath)) {
        cpath = panorama::GetParentDir(cpath);
        cpath += "/";
    }

    auto cpart = part;
    auto cpart_pos = cpath.find_last_of(cpart);
    CHECK(cpart_pos != std::string::npos);

    cpart_pos = cpath.length()-cpart_pos-1;
    cpart_pos += cpart.size()+1;
    auto sub_dir = cpath.substr(cpart_pos);
    return sub_dir;
}


int main(int argc, char** argv) {
    panorama::ArgParser parser;
    parser.SetVersion("1.0.0");
    parser.AddOption("input_path", true, "",
                     "Panorama image path");
    parser.AddOption("output_path", true, "",
                     "Perspective image path, if the folder not exist, "
                          "the program may create it");
    parser.AddOption("param", false, "6,60,600,600",
                     "Image number,Vertical fov,Image width, Image height, "
                          "default is 6,60,600,600");
    parser.AddOption("pitch", false, 0.0f,
                     "Pitch value, default is 0");
    parser.Parse(argc, argv);

    auto input_path = parser.GetStringValue("input_path");
    auto output_path = parser.GetStringValue("output_path");
    auto param = parser.GetStringValue("param");
    auto pitch = parser.GetFloatValue("pitch");


    //Print Info
    panorama::PrintHeading1("Convert panorama to perspective image");
    std::cout << "Param: " << param << std::endl;
    std::cout << "Pitch: " << pitch << std::endl;

    // Param
    std::string item;
    std::stringstream ss(param);
    std::vector<double> values;
    while (getline(ss, item, ',')) {
        values.emplace_back(std::strtod(item.c_str(), nullptr));
    }

    auto image_path_list = LoadImageList(input_path);

    if (image_path_list.empty()) {
        std::cout << "The input path does not contain any image" << std::endl;
        return 0;
    }

    // Load panorama image
    panorama::Bitmap image;
    if (!image.Read(image_path_list[0], true)) {
        std::cout << "Panorama image read failed. " << std::endl;
        return 1;
    }
    panorama::Panorama pano;
    pano.PerspectiveParamsProcess(values[2], values[3], values[0], values[1],
                                  image.Width(), image.Height(), pitch);
    std::cout << "Panorama width: " << image.Width() << std::endl;
    std::cout << "Panorama height: " << image.Height() << std::endl;

    // Convert
    for (size_t i = 0; i != image_path_list.size(); ++i) {
        const auto & image_path = image_path_list[i];
        std::cout << "\r" << "Processd: " << i+1 << " / " << image_path_list.size() << std::flush;

        // Load panorama images
        panorama::Bitmap panorama_image;
        if (!panorama_image.Read(image_path, true)) {
            std::cout << "Panorama image read failed: " << image_path << std::endl;
            continue;
        }

        // Create Directory
        auto sub_dir = SubDirectory(image_path, input_path);
        auto target_dir = panorama::JoinPaths(output_path, sub_dir);
        panorama::ReCreateDirIfNotExists(target_dir);

        // Convert
        std::vector<panorama::Bitmap> perspective_images;
        pano.PanoramaToPerspectives(&panorama_image, perspective_images);

        // Save
        auto image_name = panorama::GetPathBaseName(image_path);
        std::string main_name, ext_name;
        panorama::SplitFileExtension(image_name, &main_name, &ext_name);
        for (size_t j = 0; j != perspective_images.size(); ++j) {
            auto new_name = main_name+"_"+std::to_string(j)+".jpg";
            auto output_image_path = panorama::JoinPaths(target_dir, new_name);
            perspective_images[j].Write(output_image_path, FIF_JPEG);
        }
    }
    std::cout << std::endl;
    panorama::PrintHeading1("Success");

    return 0;
}