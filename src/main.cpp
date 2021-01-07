#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <tuple>

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

class Runner {
public:
	virtual void run(void) = 0;
	std::string mFname;
	int mLine;
	virtual ~Runner(){};
};

class errorRunner: public Runner {
public:
	errorRunner(const char *logFile) {
            if(fs::is_regular_file(logFile)) {
                std::shared_ptr<std::ifstream> inf(new std::ifstream(), [](std::ifstream *fp) {
			if(fp->is_open()) {
			    fp->close();
			}
		});

		fmt::print("input file= {0}\n", logFile);
                inf->open(logFile, std::ios::in);
		std::string line;
		auto count = 0;
		while(std::getline(*inf, line)) {
			if (std::string::npos == line.find("error:")) {
				continue;
			}
			errDic.push_back(err_msg_t(line, 100));
			count++;
		}
		fmt::print("total error={0}\n", count);
            } else {
                fmt::print("file {0} not exist\n", logFile);
            }
	}
	void run(void) {
           //auto choose = 0;
           //system(fmt::format("vim {0} +{1}", mFname, mLine).c_str());
	}
	void dump(void) {
		for_each(errDic.begin(), errDic.end(), [](err_msg_t fp){
                    fmt::print("{0} +{1}\n",std::get<0>(fp), std::get<1>(fp));
		});
	}
	virtual ~errorRunner(){};
private:
	std::vector<err_msg_t> errDic;
};

int usage(const char *name)
{
    fmt::print("usage:\n\t{0} <buildLog>\n", name);
    return 0;
}

int main(int argc, char** argv)
{
    if (2 != argc) {
	return usage(argv[0]);
    }
    auto app = std::make_unique<errorRunner>(argv[1]);
    app->run();
    app->dump();

    return 0;
}

