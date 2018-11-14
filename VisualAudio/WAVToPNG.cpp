#include "Include.h"

typedef struct {

	int factor1;
	int factor2;
	int e_margin;

} factorpair;

const char* filenames[] = { "image1.png", "image2.png", "image3.png" };

void setRGBFromDouble(double d, png_byte &r, png_byte &g, png_byte &b) {

	d += 1;

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

bool is_perfect_square(int n) {
	if (n < 0)
		return false;
	int root(round(sqrt(n)));
	return n == root * root;
}

int nextPerfectSquare(int n) {
	for (int i = n + 1; i < INT_MAX; i++) {
		if (is_perfect_square(i)) {
			return i;
		}
	}
}

typedef struct {

	const float TARGET_FACTOR = .25;
	int size_target;
	int lowest_emargin;
	std::vector<factorpair> factorpairs;

	bool unsuitable() {

		if (factorpairs.empty()) {
			return true;
		}
		else {
			int limit = size_target * TARGET_FACTOR;
			factorpair fpair = factorpairs.at(0);
			if (fpair.e_margin > limit) {
				return true;
			}
			return false;
		}
	}

	void allocate(factorpair fpair) {
		if (fpair.e_margin < lowest_emargin) {
			factorpairs.insert(factorpairs.begin(), fpair);
		}
		else {
			factorpairs.push_back(fpair);
		}
	}

	bool isFactor(int x) {
		if (factorpairs.empty()) {
			return false;
		}
		else {
			for (int i = 0; i < factorpairs.size(); i++) {
				factorpair pair = factorpairs.at(i);
				return pair.factor1 != x && pair.factor2 != x;
			}
		}
	}

} factorpairs;

void bestImageSizeSquare(int &width, int &height, int size) {

	int idealdim = sqrt(size);
	if (idealdim * idealdim == size) {
		width = idealdim;
		height = idealdim;
	}
	else {
		factorpairs fpairs;
		fpairs.lowest_emargin = size;
		fpairs.size_target = size;
		for (int i = 2; i < size; i++) {
			factorpair fpair;
			if (size % i == 0) {
				fpair.factor1 = i;
				fpair.factor2 = size / i;
				int f1_emargin = (idealdim > fpair.factor1) ? idealdim - fpair.factor1 : fpair.factor1 - idealdim;
				int f2_emargin = (idealdim > fpair.factor2) ? idealdim - fpair.factor2 : fpair.factor2 - idealdim;
				fpair.e_margin = f1_emargin + f2_emargin;
				fpairs.allocate(fpair);
			}
		}
		if (fpairs.unsuitable()) {
			int nps = nextPerfectSquare(idealdim);
			factorpair fpair = fpairs.factorpairs.at(0);
			std::cout << "Faulty factors = " << fpair.factor1 << " * " << fpair.factor2 << std::endl;
			width = nps;
			height = nps;
			std::cout << "Better factor = " << nps << std::endl;
		}
		else {
			factorpair fpair = fpairs.factorpairs.at(0);
			std::cout << "Best factors = " << fpair.factor1 << " * " << fpair.factor2 << std::endl;
		}
	}
}



int generate_img(int width, int height, std::vector<double> samples, const char* filename) {

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
			setRGBFromDouble(samples.at(counter), row[x], row[x + 1], row[x + 2]);
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

void audioToImg(std::string audioPath) {

	AudioFile<double> audioFile;
	audioFile.load(audioPath);

	int numChannels = audioFile.getNumChannels();
	int numSamples = audioFile.getNumSamplesPerChannel();

	std::vector<std::vector<double>> channels;

	for (int c = 0; c < numChannels; c++) {
		std::vector<double> samples;

		for (int i = 0; i < numSamples; i++) {
			samples.push_back(audioFile.samples[c][i]);
		}

		channels.push_back(samples);
	}

	int width = 0;
	int height = 0;

	bestImageSizeSquare(width, height, numSamples);

	int sizeA = width * height;

	if (sizeA > numSamples) {
		int discrepancy = sizeA - numSamples;
		for (int i = 0; i < channels.size(); i++) {
			for (int j = 0; j < discrepancy; j++) {
				channels.at(i).push_back(0);
			}
		}
	}

	for (int i = 0; i < channels.size(); i++) {
		std::vector<double> samples = channels.at(i);
		generate_img(width, height, samples, filenames[i]);
	}

}