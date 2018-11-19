# Visual Audio

![Version](https://img.shields.io/badge/version-0.6.1-green.svg?style=flat-square) 
![Language](https://img.shields.io/badge/language-C++-blue.svg?style=flat-square)

### Credit due
Visual Audio uses the LodePNG decoder/encoder written by Lode Vandevenne ([Repository Link](https://github.com/lvandeve/lodepng)), alongside the AudioFile library written by Adam Stark ([Repository Link](https://github.com/adamstark/AudioFile)). Without either of these projects, this idea would have taken weeks to work out.

## What is Visual Audio?

Visual Audio is an audio-image bi-directional encoder/decoder. Basically, it can turn pixels to samples and samples to pixels. There is a slight loss of quality when an image or an audio file is transformed, and I'm looking into better encoding techniques for future versions, but right now it does the job of generating an image when fed an audio file, and vice versa. 

## How does sound look?

Well, given the particular encoding style I've chosen, a bit noisy. 

<img src="https://github.com/oscar-rt/visual-audio/blob/master/VisualAudio/example.png?raw=true"  width="500" height="500" />

Essentially, the samples in an audio file are laid down left to right, top to bottom in a perfect square image. This is what you see above. A bit of grey space is added at the end so the image can be made into a perfect square since most audio files will usually not contain a squared number of samples. This doesn't mean that the input images have to be square, just that the output images will always be square (this is subject to change in the future).

### Supported format conversions:

* WAV -> PNG
* PNG -> WAV

## How to use VisualAudio

VisualAudio can only be run from the command line currently, so you'll have to use the command prompt and pass the appropriate arguments to get it to do its thing. Here's the structure of a what a call to the executable looks like:

`VisualAudio [head] [path]... [flag]...`

So an example of what it would look like if we wanted to convert an image file by the name of image.png to an audio file:

`VisualAudio ita image.png`

It's that simple! Just wait for VisualAudio to do its thing and you should have your transformed file in a few seconds.

### Head calls:

* `ita`  Image to Audio - followed by a path to the image
* `ati`  Audio to Image - followed by a path to the audio
* `mita`  Multiple Images to Audio - followed by up to 8 paths to 8 images

### Flags

* `-mono=true/false` - Consolidates all images into a single audio channel and consolidates all audio channels into a single image. This flag is set to true by default.

### Download/Get VisualAudio

Check out the releases page!





