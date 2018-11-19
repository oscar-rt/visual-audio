#include <fstream>

#include "Include.h"
#include "Tranforms.h"


bool file_format_exists(std::string path, std::string format) {

	std::ifstream file(path);

	if(file){
		//file exists, check that the format/filetype is correct
		file.close();
		std::string ftype = path.substr(path.size() - format.size(), path.size() - 1);
		if (ftype == format) {
			return true;
		}
		log_e(path + " is not the correct format (" + format + ") for this operation.", "cxEC001");
		return false;
	}
	else {
		log_e(path + " does not exist, please enter a valid path.", "cxEC002");
		return false;
	}
}

int process_cla(std::vector<std::string> arguments) {
	log("processing command line arguments");
	std::string head = arguments.at(0);
	std::string filepath = arguments.at(1);
	//flags
	std::vector<std::string> flags;
	for (int i = 2; i < arguments.size(); i++) {
		if (arguments.at(i)[0] == '-'){
			flags.push_back(arguments.at(i));
		}
	}
	process_flags(flags);
	//image to audio
	if (head == "ita" && file_format_exists(filepath, ".png")) {
		log("image to audio conversion");
		return png_to_wav(filepath);
	}
	//multiple images to audio
	else if (head == "mita" && file_format_exists(filepath, ".png")) {
		log("multiple images to audio conversion");
		std::vector<std::string> paths;
		paths.push_back(filepath);
		for (int i = 2; i < arguments.size(); i++) {
			if (file_format_exists(arguments.at(i), ".png")) {
				paths.push_back(arguments.at(i));
			}
		}
		return mpng_to_wav(paths);
	}
	//audio to image/images
	else if (head == "ati" && file_format_exists(filepath, ".wav")) {
		log("audio to image conversion");
		return wav_to_png(filepath);
	}
	else {
		return 1;
	}
}

void log(const char* message) {
	
	static int logcounter;
	std::cout  << " LOG " << logcounter << "|| " << message << std::endl;
	logcounter++;
 
}

void log_e(std::string error, std::string ecode) {
	std::cout << "ERROR " << "|| " << error << " ....ecode: " << ecode;
}

void process_flag(std::string flag){
	if (flag == "-mono=true") {
		stng_make_mono = true;
	}
	else if (flag == "-mono=false") {
		stng_make_mono = false;
	}
}

void process_flags(std::vector<std::string> flags){
	log("processing flags");
	for (int i = 0; i < flags.size(); i++) {
		process_flag(flags.at(i));
	}
}