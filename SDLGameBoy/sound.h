#ifndef sound
#define sound
#include <sdl.h>
#include "defs.h"

int initSound();
void updateSound();

class AudioBaseClass{
public://protected:
	SDL_AudioSpec spec;
	SDL_AudioDeviceID dev;
	ushort freq, time;
	ushort playing, durationEnabled, soundDuration, stopped;
	uchar duty, dutyReg;

protected:
	ushort baseReg, pitch;
	ulong lastDecDur;

public:
	uchar mode = 0, mute = 0;
	AudioBaseClass();
	void startSoundCallBack();
	void play();
	void pause();
};

class SineChannel : public AudioBaseClass {
public:
	SineChannel();
	void soundOnReg(uchar input);
	void soundLengthReg(uchar input);
	void soundVolReg(uchar input);
	void soundFreqLowReg(uchar input);
	void soundFreqHiReg(uchar input);

private:
	uchar soundEnable, soundDuration, volume, freqLow, freqHi, counterEnable, masterEnable;
	ushort  frequency;
	void callback(float* target, int num_samples);
	float waveAudio();
	void resetAudio();
};

#endif sound