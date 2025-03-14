/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/08/19
 * */

#include "utils/misc.h"


namespace panorama {

std::string EnsureTrailingSlash(const std::string& str) {
    if (str.length() > 0) {
        if (str.back() != '/') {
            return str + "/";
        }
    } else {
        return str + "/";
    }
    return str;
}

bool HasFileExtension(const std::string& file_name, const std::string& ext) {
    CHECK(!ext.empty());
    CHECK_EQ(ext.at(0), '.');
    std::string ext_lower = ext;
    StringToLower(&ext_lower);
    if (file_name.size() >= ext_lower.size() &&
        file_name.substr(file_name.size() - ext_lower.size(), ext_lower.size()) ==
        ext_lower) {
        return true;
    }
    return false;
}

void SplitFileExtension(const std::string& path, std::string* root,
                        std::string* ext) {
    const auto parts = StringSplit(path, ".");
    CHECK_GT(parts.size(), 0);
    if (parts.size() == 1) {
        *root = parts[0];
        *ext = "";
    } else {
        *root = "";
        for (size_t i = 0; i < parts.size() - 1; ++i) {
            *root += parts[i] + ".";
        }
        *root = root->substr(0, root->length() - 1);
        if (parts.back() == "") {
            *ext = "";
        } else {
            *ext = "." + parts.back();
        }
    }
}

bool ExistsFile(const std::string& path) {
    return boost::filesystem::is_regular_file(path);
}

bool ExistsDir(const std::string& path) {
    return boost::filesystem::is_directory(path);
}

bool ExistsPath(const std::string& path) {
    return boost::filesystem::exists(path);
}

void CreateDirIfNotExists(const std::string& path) {
    if (!ExistsDir(path)) {
        CHECK(boost::filesystem::create_directory(path));
    }
}

void ReCreateDirIfNotExists(const std::string& path) {
    if (!ExistsDir(path)) {
       boost::filesystem::create_directories(path);
    }
}

std::string GetPathBaseName(const std::string& path) {
    const std::vector<std::string> names =
            StringSplit(StringReplace(path, "\\", "/"), "/");
    if (names.size() > 1 && names.back() == "") {
        return names[names.size() - 2];
    } else {
        return names.back();
    }
}

std::string GetParentDir(const std::string& path) {
    return boost::filesystem::path(path).parent_path().string();
}

std::string GetRelativePath(const std::string& from, const std::string& to) {
    // This implementation is adapted from:
    // https://stackoverflow.com/questions/10167382
    // A native implementation in boost::filesystem is only available starting
    // from boost version 1.60.
    using namespace boost::filesystem;

    path from_path = canonical(path(from));
    path to_path = canonical(path(to));

    // Start at the root path and while they are the same then do nothing then
    // when they first diverge take the entire from path, swap it with '..'
    // segments, and then append the remainder of the to path.
    path::const_iterator from_iter = from_path.begin();
    path::const_iterator to_iter = to_path.begin();

    // Loop through both while they are the same to find nearest common directory
    while (from_iter != from_path.end() && to_iter != to_path.end() &&
           (*to_iter) == (*from_iter)) {
        ++ to_iter;
        ++ from_iter;
    }

    // Replace from path segments with '..' (from => nearest common directory)
    path rel_path;
    while (from_iter != from_path.end()) {
        rel_path /= "..";
        ++ from_iter;
    }

    // Append the remainder of the to path (nearest common directory => to)
    while (to_iter != to_path.end()) {
        rel_path /= *to_iter;
        ++ to_iter;
    }

    return rel_path.string();
}

std::vector<std::string> GetFileList(const std::string& path) {
    std::vector<std::string> file_list;
    for (auto it = boost::filesystem::directory_iterator(path);
         it != boost::filesystem::directory_iterator(); ++it) {
        if (boost::filesystem::is_regular_file(*it)) {
            const boost::filesystem::path file_path = *it;
            file_list.push_back(file_path.string());
        }
    }
    return file_list;
}

std::vector<std::string> GetRecursiveFileList(const std::string& path) {
    std::vector<std::string> file_list;
    for (auto it = boost::filesystem::recursive_directory_iterator(path);
         it != boost::filesystem::recursive_directory_iterator(); ++it) {
        if (boost::filesystem::is_regular_file(*it)) {
            const boost::filesystem::path file_path = *it;
            file_list.push_back(file_path.string());
        }
    }
    return file_list;
}

std::vector<std::string> GetDirList(const std::string& path) {
    std::vector<std::string> dir_list;
    for (auto it = boost::filesystem::directory_iterator(path);
         it != boost::filesystem::directory_iterator(); ++it) {
        if (boost::filesystem::is_directory(*it)) {
            const boost::filesystem::path dir_path = *it;
            dir_list.push_back(dir_path.string());
        }
    }
    return dir_list;
}

std::vector<std::string> GetRecursiveDirList(const std::string& path) {
    std::vector<std::string> dir_list;
    for (auto it = boost::filesystem::recursive_directory_iterator(path);
         it != boost::filesystem::recursive_directory_iterator(); ++it) {
        if (boost::filesystem::is_directory(*it)) {
            const boost::filesystem::path dir_path = *it;
            dir_list.push_back(dir_path.string());
        }
    }
    return dir_list;
}

size_t GetFileSize(const std::string& path) {
    std::ifstream file(path, std::ifstream::ate | std::ifstream::binary);
    CHECK(file.is_open()) << path;
    return file.tellg();
}

void PrintHeading1(const std::string& heading) {
    std::cout << std::string(78, '=') << std::endl;
    std::cout << heading << std::endl;
    std::cout << std::string(78, '=') << std::endl;
}

void PrintHeading2(const std::string& heading) {
    std::cout << std::string(std::min<int>(heading.size(), 78), '-') << std::endl;
    std::cout << heading << std::endl;
    std::cout << std::string(std::min<int>(heading.size(), 78), '-') << std::endl;
}

template <>
std::vector<std::string> CSVToVector(const std::string& csv) {
    auto elems = StringSplit(csv, ",;");
    std::vector<std::string> values;
    values.reserve(elems.size());
    for (auto& elem : elems) {
        StringTrim(&elem);
        if (elem.empty()) {
            continue;
        }
        values.push_back(elem);
    }
    return values;
}

template <>
std::vector<int> CSVToVector(const std::string& csv) {
    auto elems = StringSplit(csv, ",;");
    std::vector<int> values;
    values.reserve(elems.size());
    for (auto& elem : elems) {
        StringTrim(&elem);
        if (elem.empty()) {
            continue;
        }
        try {
            values.push_back(std::stoi(elem));
        } catch (std::exception) {
            return std::vector<int>(0);
        }
    }
    return values;
}

template <>
std::vector<float> CSVToVector(const std::string& csv) {
    auto elems = StringSplit(csv, ",;");
    std::vector<float> values;
    values.reserve(elems.size());
    for (auto& elem : elems) {
        StringTrim(&elem);
        if (elem.empty()) {
            continue;
        }
        try {
            values.push_back(std::stod(elem));
        } catch (std::exception) {
            return std::vector<float>(0);
        }
    }
    return values;
}

template <>
std::vector<double> CSVToVector(const std::string& csv) {
    auto elems = StringSplit(csv, ",;");
    std::vector<double> values;
    values.reserve(elems.size());
    for (auto& elem : elems) {
        StringTrim(&elem);
        if (elem.empty()) {
            continue;
        }
        try {
            values.push_back(std::stold(elem));
        } catch (std::exception) {
            return std::vector<double>(0);
        }
    }
    return values;
}

std::vector<std::string> ReadTextFileLines(const std::string& path) {
    std::ifstream file(path);
    CHECK(file.is_open()) << path;

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        StringTrim(&line);

        if (line.empty()) {
            continue;
        }

        lines.push_back(line);
    }

    return lines;
}

void RemoveCommandLineArgument(const std::string& arg, int* argc, char** argv) {
    for (int i = 0; i < *argc; ++i) {
        if (argv[i] == arg) {
            for (int j = i + 1; j < *argc; ++j) {
                argv[i] = argv[j];
            }
            *argc -= 1;
            break;
        }
    }
}


}  // namespace panorama
