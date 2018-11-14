#include "Include.h"
#include "Header.h"

void setup();

int main()
{
	std::cout << header;
	setup();


	return 0;
}

void setup() {
	std::cout << "Choose source to transform from [image|audio]: ";
	std::string answer;
	std::cin >> answer;

	while (answer != "image" && answer != "audio"){
		std::cout << "'" << answer <<"' is not a valid option, please try again [image|audio]: ";
		std::cin >> answer;
	}

	if (answer == "image") {
		std::cout << "image chosen";
	}
	else if (answer == "audio"){
		std::cout << "audio chosen";
	}

	std::cin >> answer;
}

void audioFile(){

	AudioFile<double> audioFile;
	audioFile.load("test.wav");

	audioFile.printSummary();

	int numSamples = audioFile.getNumSamplesPerChannel();

	std::cout << numSamples;

	std::string string;
	std::cin >> string;

}