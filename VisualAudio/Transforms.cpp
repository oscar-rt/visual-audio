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
		return false;
	}
	else {
		return false;
	}
}

int process_cla(std::vector<std::string> arguments) {

	std::string head = arguments.at(0);
	std::string filepath = arguments.at(1);
	//image to audio
	if (head == "ita" && file_format_exists(filepath, ".png")) {
		log("began image to wav conversion");
		png_to_wav(filepath);
	}
	//multiple images to audio
	else if (head == "mita") {
		//not supported yet
		return 1;
	}
	//audio to image/images
	else if (head == "ati" && file_format_exists(filepath, ".wav")) {
		log("began wav to image conversion");
		wav_to_png(filepath);
	}
	else {
		return 1;
	}

	return 0;
}

void log(const char* message) {
	
	static int logcounter;
	std::cout  << " >log>" << logcounter << "| " << message << std::endl;
	logcounter++;
 
}
