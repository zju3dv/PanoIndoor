/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/10/19
 * */
#include "utils/options.h"

#include <iostream>

namespace panorama {

ArgParser::ArgParser() : num_required(0) {}

void ArgParser::AddOption(const std::string &name, bool required,
                          int value, const std::string desc) {
    options_.emplace_back(name, ProgramOption::INT, std::to_string(value), !required, desc);
    if (required) ++num_required;
}

void ArgParser::AddOption(const std::string &name, bool required,
                          float value, const std::string desc) {
    options_.emplace_back(name, ProgramOption::INT, std::to_string(value), !required, desc);
    if (required) ++num_required;
}

void ArgParser::AddOption(const std::string &name, bool required,
                          const std::string &value, const std::string desc) {
    options_.emplace_back(name, ProgramOption::INT, value, !required, desc);
    if (required) ++num_required;
}

void ArgParser::Parse(int argc, char **argv) {
    program_name_ = argv[0];
    int beg = 0;
    while(beg < program_name_.size() && (program_name_[beg] == ' ' || program_name_[beg] == '.')) ++beg;
    if(beg == program_name_.size()) {
        std::cout << "Parameter error: " << program_name_ << std::endl;
        exit(1);
    }
    auto line_pos = program_name_.find_last_of('/');
    if (line_pos != std::string::npos) {
        program_name_ = program_name_.substr(line_pos+1);
    }

    if (argc == 1) {
        Help();
        exit(0);
    }

    for (int i = 1; i != argc; ++i) {
        ParseOnePara(std::string(argv[i]));
    }
    for (int i = 0; i != options_.size(); ++i) {
        if(!options_[i].parsed){
            std::cout << "Parameter " << options_[i].name_
                      << " uninitialized!" << std::endl;
            exit(1);
        }
    }
}

void ArgParser::ParseOnePara(const std::string &parameter) {
    std::string param = parameter;

    int i = 0;
    while (i < param.size() && param[i] == ' ') ++i;
    if(i == param.size()){
        std::cout << "Parameter error: " << param << std::endl;
        exit(1);
    }
    param = param.substr(i);

    int j = param.size()-1;
    while (j >= 0 && param[j] == ' ') --j;
    if(j < 0) {
        std::cout << "Parameter error: " << param << std::endl;
        exit(1);
    }
    param = param.substr(0, j+1);

    if(!(param.size() > 2 && param[0] == '-'  && param[1] == '-')) {
        std::cout << "Parameter error: " << param << std::endl;
        exit(1);
    }
    param = param.substr(2);

    int equal_pos = param.find_first_of('=');
    if (equal_pos == std::string::npos && param == "help") {
        Help();
        exit(0);
    }
    if(equal_pos == std::string::npos) {
        std::cout << "Parameter error: " << param << std::endl;
        exit(1);
    }

    std::string name = param.substr(0, equal_pos);
    std::string value = param.substr(equal_pos+1, param.size()-equal_pos);

    int option_index = 0;
    for (option_index = 0; option_index != options_.size(); ++option_index) {
        if (options_[option_index] == name) {
            options_[option_index].value_ = value;
            options_[option_index].parsed = true;
            break;
        }
    }

    if (option_index == options_.size()) {
        std::cout << "Unrecognized parameter " << name << std::endl;
        exit(1);
    }
}

int ArgParser::GetIntValue(const std::string &name) const {
    for (int i = 0; i != options_.size(); ++i) {
        if (options_[i] == name) {
            if (options_[i] == name) {
                return atoi(options_[i].value_.c_str());
            }
        }
    }
    std::cout << "Option " << name << " is not exist!" << std::endl;
    exit(1);
}

float ArgParser::GetFloatValue(const std::string &name) const {
    for (int i = 0; i != options_.size(); ++i) {
        if (options_[i] == name) {
            if (options_[i] == name) {
                return atof(options_[i].value_.c_str());
            }
        }
    }
    std::cout << "Option " << name << " is not exist!" << std::endl;
    exit(1);
}

std::string ArgParser::GetStringValue(const std::string &name) const {
    for (int i = 0; i != options_.size(); ++i) {
        if (options_[i] == name) {
            if (options_[i] == name) {
                return options_[i].value_;
            }
        }
    }
    std::cout << "Option " << name << " is not exist!" << std::endl;
    exit(1);
}

void ArgParser::Help() const {
    std::cout << program_name_ << " " << version_ << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << program_name_ << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    for (int i = 0; i != options_.size(); ++i) {
        std::cout << "\t" << options_[i].name_ << " ";
        if (options_[i].parsed) {
            std::cout << "[optional] ";
        } else {
            std::cout << "[required] ";
        }
        std::cout << options_[i].desc_ << std::endl;
    }
    std::cout << std::endl;
}

} // namespace panorama