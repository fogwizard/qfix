#include <iostream>
#include <string>
#include <memory>
#include <vector>

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

class Runner {
public:
	int parser_error(){
		return 0;
	}
	virtual void run(void) = 0;
	std::string mFname;
	int mLine;
	virtual ~Runner(){};
};

class errorRunner: public Runner {
public:
	errorRunner(const char *logFile) {
		parser_error();
	}
	void run(void) {
           //auto choose = 0;
           system(fmt::format("vim {0} +{1}", mFname, mLine).c_str());
	}
	virtual ~errorRunner(){};
private:
	std::vector<std::tuple<std::string, int>> errDic;
};

int usage(const char *name)
{
    fmt::print("usage:\n\t{0} <buildLog>", name);
    return 0;
}

int main(int argc, char** argv)
{
    if (2 != argc) {
	return usage(argv[0]);
    }
    auto app = std::make_unique<errorRunner>(argv[0]);
    app->run();
    return 0;
}
