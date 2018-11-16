#include "Include.h"

int main(int argc, char *args[])
{	
	if (argc > 2) {
		std::vector<std::string> arguments;

		for (int i = 1; i < argc; i++) {
			std::string argument = args[i];
			arguments.push_back(argument);
		}

		int code = process_cla(arguments);

		if (code == 1) {
			std::cin.get();
			return 1;
		}
		return 0;
	}
}












