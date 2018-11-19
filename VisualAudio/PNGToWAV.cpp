#include "Include.h"
#include "Tranforms.h"

std::vector<double> decode(const char* filename) {
	std::vector<unsigned char> png;
	std::vector<unsigned char> image; //the raw pixels
	std::vector<double> samples;
	unsigned width, height;

	//load and decode
	unsigned error = lodepng::load_file(png, filename);
	if (!error) error = lodepng::decode(image, width, height, png);

	//if there's an error, display it
	if (error) {
		log_e(lodepng_error_text(error), "lodePNG_DECODE-ERROR: " + error);
		return samples;
	}
	
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...


	for (int i = 0; i < image.size(); i+=4) {
		pixel_t pixel(image[i], image[i+1], image[i+2]);
		samples.push_back(pixel.getdouble());
	}

	return samples;
}

int png_to_wav(std::string audio_path) {
	log("decoding png to pixel array...");
	std::vector<double> pixels = decode(audio_path.c_str());
	std::vector<std::vector<double>> waves;
	waves.push_back(pixels);
	
	AudioFile<double> audioFile;
	log("created audio file");
	log("populating audio file...");
	audioFile.makeWave(waves);
	log("populated audio file!");
	log("saving audio file...");
	audioFile.save("audio.wav", AudioFileFormat::Wave);
	log("saved audio file");
	return 0;
}

int mpng_to_wav(std::vector<std::string> audio_paths) {
	log("decoding pngs to pixel arrays...");
	std::vector<std::vector<double>> waves;
	for (int i = 0; i < audio_paths.size(); i++) {
		std::vector<double> pixels = decode(audio_paths.at(i).c_str());
		waves.push_back(pixels);
	}
	log("done decoding pngs!");

	if (stng_make_mono) {
		log("making mono wave vector...");
		for (int c = 1; c < waves.size(); c++) {
			for (int s = 0; s < waves.at(c).size(); s++) {
				waves.at(0).at(s) = (waves.at(0).at(s) + waves.at(c).at(s)) / 2;
			}
		}
		for (int c = waves.size() - 1; c > 0; c--){
			waves.pop_back();
		}
		log("made mono wave vector!");
	}

	AudioFile<double> audioFile;
	log("created audio file");
	log("populating audio file...");
	audioFile.makeWave(waves);
	log("populated audio file!");
	log("saving audio file...");
	audioFile.save("audio.wav", AudioFileFormat::Wave);
	log("saved audio file");
	return 0;
}