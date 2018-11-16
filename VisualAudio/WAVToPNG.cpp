#include "Include.h"
#include "Tranforms.h"

const char* filenames[] = { "image1.png", "image2.png", "image3.png", "image4.png",
							"image5.png", "image6.png", "image7.png", "image8.png", };

typedef struct {

	png_byte r;
	png_byte g;
	png_byte b;

} pixel_rgb;

//essentially converting a base 10 double d to base 255
void double_to_rgb(double d, png_byte &r, png_byte &g, png_byte &b) {

	d += 1; //make sample positive

	double realmax = 255 * 255 * 255;
	double realmin = 0;

	double multiplier = realmax / 2;
	long int asrealmax = d * multiplier;

	if (asrealmax > 255) {
		double quotient = asrealmax / 255;
		r = asrealmax % 255;

		if (quotient > 255) {
			long int lquotient = quotient;
			g = lquotient % 255;
			quotient = asrealmax / 255;
			if (quotient > 255) {
				lquotient = quotient;
				b = lquotient % 255;
			}
			else {
				b = quotient;
			}
		}
		else {
			g = quotient;
			b = 0;
		}
	}
	else {
		r = asrealmax;
		g = 0;
		b = 0;
	}
}

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


int generate_img(long int width, long int height, std::vector<pixel_rgb> pixel_a, const char* filename) {

	const char* title = "image";
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	double counter = 0;

	fp = fopen(filename, "wb");

	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = (png_charp)"Title";
		title_text.text = (png_charp)title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep)malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width * 3; x += 3) {
			pixel_rgb pixel = pixel_a[counter];
			row[x] = pixel.r;
			row[x + 1] = pixel.g;
			row[x + 2] = pixel.b;
			counter++;
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

	//FINAL
finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return code;
}

void wav_to_png(std::string audio_path) {

	log("attempting to load audiofile...");

	AudioFile<double> audioFile;
	audioFile.load(audio_path);

	log("loaded audiofile!");

	long int numchannels = audioFile.getNumChannels();
	long int numsamples = audioFile.getNumSamplesPerChannel();

	std::vector<std::vector<pixel_rgb>> channels_ap;

	for (int c = 0; c < numchannels; c++) {
		std::vector<pixel_rgb> pixels;

		for (int i = 0; i < numsamples; i++) {
			pixel_rgb pixel;
			double sample = audioFile.samples[c][i];
			double_to_rgb(sample, pixel.r, pixel.g, pixel.b);
			pixels.push_back(pixel);
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
				pixel_rgb pixel;
				pixel.r = 127;
				pixel.g = 127;
				pixel.b = 127;
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
				channels_ap.at(0).at(i).r = (channels_ap.at(0).at(i).r + channels_ap.at(c).at(i).r) / 2;
				channels_ap.at(0).at(i).g = (channels_ap.at(0).at(i).g + channels_ap.at(c).at(i).g) / 2;
				channels_ap.at(0).at(i).b = (channels_ap.at(0).at(i).b + channels_ap.at(c).at(i).b) / 2;
			}
		}
		generate_img(width, height, channels_ap.at(0), "image.png");
	}
	//generate an image for each channel
	else {
		for (int i = 0; i < channels_ap.size(); i++) {
			std::vector<pixel_rgb> pixels = channels_ap.at(i);
			generate_img(width, height, pixels, filenames[i]);
		}
	}

	log("generated image.");
}