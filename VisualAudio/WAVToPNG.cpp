#include "Include.h"
#include "Tranforms.h"

const char* filenames[] = { "image1.png", "image2.png", "image3.png", "image4.png",
							"image5.png", "image6.png", "image7.png", "image8.png", };

void set_best_size(long int &width, long int &height, int size) {

	long int idealdim = (int)sqrt(size);

	//this will rarely be the case
	if (idealdim * idealdim == size){
		width = idealdim;
		height = idealdim;
	}
	//add one since integers round down, no need to worry about overflow since (a + 1) * a > c  where a = sqrt(c)
	else if (idealdim * (idealdim + 1) > size) {
		width = idealdim;
		height = idealdim + 1;
	}
}


void encode(const char* filename, std::vector<pixel_t>& pixels, unsigned width, unsigned height)
{
	std::vector<unsigned char> png;
	std::vector<unsigned char> image;

	for (int i = 0; i < pixels.size(); i++) {
		image.push_back(pixels.at(i).red);
		image.push_back(pixels.at(i).green);
		image.push_back(pixels.at(i).blue);
		image.push_back(pixels.at(i).alpha);
	}

	unsigned error = lodepng::encode(png, image, width, height);
	if (!error) lodepng::save_file(png, filename);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}


void wav_to_png(std::string audio_path) {

	log("attempting to load audiofile...");

	AudioFile<double> audioFile;
	audioFile.load(audio_path);

	log("loaded audiofile!");

	long int numchannels = audioFile.getNumChannels();
	long int numsamples = audioFile.getNumSamplesPerChannel();

	std::vector<std::vector<pixel_t>> channels_ap;

	for (int c = 0; c < numchannels; c++) {
		std::vector<pixel_t> pixels;

		for (int i = 0; i < numsamples; i++) {
			double sample = audioFile.samples[c][i];
			pixel_t pixele(sample);
			pixels.push_back(pixele);
		}

		channels_ap.push_back(pixels);
	}

	long int width = 0;
	long int height = 0;

	log("made pixels vector");

	set_best_size(width, height, numsamples);

	long int sizeA = width * height;
	
	//fill extra space with grey pixels
	if (sizeA > numsamples) {
		long int discrepancy = sizeA - numsamples;
		for (int i = 0; i < channels_ap.size(); i++) {
			for (long int j = 0; j < discrepancy; j++) {
				pixel_t pixel(127, 127, 127);
				channels_ap.at(i).push_back(pixel);
			}
		}
	}

	log("found best size and adjused accordingly");

	//if make mono, consolidate all channels as pixels to one channel then generate an image
	if (stng_make_mono) {
		for (int c = 1; c < channels_ap.size(); c++) {
			for (int i = 0; i < numsamples; i++) {
				//maybe clean this up later?
				channels_ap.at(0).at(i).red = (channels_ap.at(0).at(i).red + channels_ap.at(c).at(i).red) / 2;
				channels_ap.at(0).at(i).green = (channels_ap.at(0).at(i).green + channels_ap.at(c).at(i).green) / 2;
				channels_ap.at(0).at(i).blue = (channels_ap.at(0).at(i).blue + channels_ap.at(c).at(i).blue) / 2;
			}
		}

		encode("image.png", channels_ap.at(0), width, height);
	}
	//generate an image for each channel
	else {
		for (int i = 0; i < channels_ap.size(); i++) {
			std::vector<pixel_t> pixels = channels_ap.at(i);
			encode(filenames[i], channels_ap.at(i),width, height);
		}
	}

	log("generated images.");
}