#pragma once

static bool stng_make_mono = false;

void wav_to_png(std::string audio_path);
void png_to_wav(std::string audio_path);
void mpng_to_wav(std::vector<std::string> audio_paths);
void log(const char* message);

typedef struct pixel_t {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	const unsigned char alpha = 255;

	const double max = 255 * 255 * 255;
	const double hmax = max / 2;

	pixel_t(unsigned char r, unsigned char g, unsigned char b) {
		red = r;
		green = g;
		blue = b;
	}

	pixel_t(double d) {
		setpixel(d);
	}

	void setpixel(double d) {
		double catcher = d + 1;
		long double val = catcher * hmax;

		if (val < 255) {
			red = (int)std::round(val);
			green = 0;
			blue = 0;
		}
		else {
			red = (int)val % 255;
			long double quotient = val / 255;
			if (quotient < 255) {
				green = (int)std::round(quotient);
				blue = 0;
			}
			else {
				green = (int)quotient % 255;
				quotient = quotient / 255;
				if (quotient < 255) {
					blue = (int)std::round(quotient);
				}
				else {
					blue = (int)quotient % 255;
				}
			}
		}
	}

	double getdouble() {
		double r = (double)red;
		double g = (double)green;
		double b = (double)blue;

		double totalrgb = r;
		totalrgb += g * (double)255;
		totalrgb += b * (double)(255 * 255);

		double thedouble = ((totalrgb / hmax) - 1);
		if (thedouble > 1) {
			return 1;
		}
		else {
			return thedouble;
		}
	}

};