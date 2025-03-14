/*
 * Author: Hailin Yu
 * Email: yuhailin@sensetime.com
 * Created Time: 2020/10/19
 * */

#ifndef LOCALIZER_OPTIONS_H
#define LOCALIZER_OPTIONS_H

#include <algorithm>
#include <string>

namespace panorama {

struct ProgramOption {
    enum Type {INT, FLOAT, STRING};

    ProgramOption(const std::string &name, const Type type,
                  const std::string &value, bool parsed, const std::string &desc)
            : name_(name), type_(type), value_(value), parsed(parsed), desc_(desc){}

    bool operator==(const std::string& name) const {
        return name_ == name;
    }

    std::string name_;
    Type type_;
    std::string value_;
    std::string desc_;

    bool parsed;
};

class ArgParser {
public:
    ArgParser();

    void AddOption(const std::string& name, bool required=true,
                   int value = 0, const std::string desc="");

    void AddOption(const std::string& name, bool required=true,
                   float value = 0.0, const std::string desc="");

    void AddOption(const std::string& name, bool required=true,
                   const std::string& value = "", const std::string desc="");

    void Parse(int argc, char** argv);

    int GetIntValue(const std::string& name) const;

    float GetFloatValue(const std::string& name) const;

    std::string GetStringValue(const std::string& name) const;

    inline void SetVersion(const std::string &version);

    inline std::string Version() const;

    void Help() const;

private:
    void ParseOnePara(const std::string& param);

private:
    std::string program_name_;
    std::vector<ProgramOption> options_;
    std::string version_;

    int num_required;
};

void ArgParser::SetVersion(const std::string &version) {
    version_ = version;
}

std::string ArgParser::Version() const {
    return version_;
}

} // namespace panorama

#endif //LOCALIZER_OPTIONS_H
