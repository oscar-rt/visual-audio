#include "Include.h"
#include "Tranforms.h"

std::vector<double> decode(const char* filename) {
	std::vector<unsigned char> png;
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//load and decode
	unsigned error = lodepng::load_file(png, filename);
	if (!error) error = lodepng::decode(image, width, height, png);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
	std::vector<double> samples;

	for (int i = 0; i < image.size(); i+=4) {
		pixel_t pixel(image[i], image[i+1], image[i+2]);
		samples.push_back(pixel.getdouble());
	}

	return samples;
}

void png_to_wav(std::string audio_path) {
	//fix function
	log("decoding png to pixel array");
	std::vector<double> pixels = decode(audio_path.c_str());
	std::vector<std::vector<double>> waves;
	waves.push_back(pixels);
	
	AudioFile<double> audioFile;
	log("created audio file");
	audioFile.makeWave(waves);
	log("populated audio file");
	audioFile.save("atest.wav", AudioFileFormat::Wave);
	log("saved audio file");
}

void mpng_to_wav(std::vector<std::string> audio_paths) {

}