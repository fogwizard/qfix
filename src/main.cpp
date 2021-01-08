#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <regex>

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif
#include "fmt/format.h"

using err_msg_t = std::tuple<std::string, int>;

class Runner
{
public:
    virtual void run(void) = 0;
    virtual ~Runner() {};
};

class errorRunner : public Runner
{
public:
    errorRunner(const char *logFile)
    {
        if (fs::is_regular_file(logFile)) {
            std::shared_ptr<std::ifstream> inf(new std::ifstream(),[](std::ifstream *fp) {
                if (fp->is_open()) {
                    fp->close();
                }
            });

            fmt::print("input file= {0}\n", logFile);
            inf->open(logFile, std::ios::in);
            std::string line;
            std::regex reg("^(\\s)[^\x21-\x7E](\\[)(\\d)m");

            while (std::getline(*inf, line)) {
                if (std::string::npos == line.find("error:")) {
                    continue;
                }
                std::vector<std::string> s = split(line, "error:");
                std::vector<std::string> v = split(s[1], ":");
                errDic.push_back(err_msg_t(std::regex_replace(v[0], reg, ""),
                                           std::atoi(v[1].c_str())));
            }
            fmt::print("total error={0}\n", total_error = errDic.size());
        } else {
            fmt::print("file {0} not exist\n", logFile);
        }
    }
    std::vector<std::string> split(std::string str, std::string pattern)
    {
        std::string::size_type pos;
        std::vector<std::string> result;
        str += pattern; //??????????
        int size = str.size();
        for (int i = 0; i < size; i++) {
            pos = str.find(pattern, i);
            if (pos < size) {
                std::string s = str.substr(i, pos - i);
                result.push_back(s);
                i = pos + pattern.size() - 1;
            }
        }
        return result;
    }
    void run(void)
    {
        if (0 == total_error) {
            fmt::print("qfix: build successful\n");
            return;
        }
        auto idx = 0;
        for_each(errDic.begin(), errDic.end(), [&idx](err_msg_t fp) {
            fmt::print("[{0}]{1} +{2}\n", idx++, std::get<0>(fp), std::get<1>(fp));
        });
        fmt::print("please choose:\n");
        auto choose = 0;
        std::cin >> choose;
        if (choose > errDic.size()) {
            fmt::print("input out of range\n");
            return;
        }
        std::string cmd = fmt::format("vim {0} +{1}",
                                      std::get<0>(errDic[choose]), std::get<1>(errDic[choose]));
        fmt::print("{0}\n", cmd.c_str());
        system(cmd.c_str());
    }
    virtual ~errorRunner() {};

private:
    std::vector<err_msg_t> errDic;
    int total_error;
};

int usage(const char *name)
{
    fmt::print("usage:\n\t{0} <buildLog>\n", name);
    return 0;
}

int main(int argc, char **argv)
{
    if (2 != argc) {
        return usage(argv[0]);
    }
    auto app = std::make_unique<errorRunner>(argv[1]);
    app->run();
    return 0;
}
