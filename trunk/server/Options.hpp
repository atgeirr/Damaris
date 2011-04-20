
#ifndef __DAMARIS_OPTIONS_H
#define __DAMARIS_OPTIONS_H

#include <string>

namespace Damaris {

class Options {
	private:
		std::string* configFile;
		int id;
	public:
		Options(int argc, char** argv);
		std::string* getConfigFile();
		int getID();
};
}

#endif
