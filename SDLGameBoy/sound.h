#ifndef sound
#define sound
#include <sdl.h>
#include "defs.h"


class LFSR {
private:
	ushort shiftReg;
	uchar clockShiftReg, clockDivReg, clockWidth;
	void clockRegister();
	ulong cpuTickStamp, clockDiv;
public:
	LFSR();
	void soundPolyReg(uchar input);
	void init();
	uchar note();
	void update();
};
class AudioBaseClass{
public:
	AudioBaseClass();
	void startSoundCallBack();
	void play();
	void pause();
	void soundEnabled(uchar);

protected:
	SDL_AudioSpec spec;
	SDL_AudioDeviceID dev;
	uchar soundEnable, soundDuration, freqLow, freqHi, counterEnable, masterEnable, channelEnable;
	float amp;
	ushort frequency;
	ushort time;
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
	uchar volume;
	void callback(float* target, int num_samples);
	float waveAudio();
	void resetAudio();
};
class SquareChannel1 : public AudioBaseClass {
public:
	SquareChannel1();
	void soundOnReg(uchar input);
	void soundLengthReg(uchar input);
	void soundVolReg(uchar input);
	void soundFreqLowReg(uchar input);
	void soundFreqHiReg(uchar input);
private:
	uchar sweepTime, sweepType, sweepNum;	//first reg variables
	uchar sweepEnable, sweepShifts;
	ushort shadowAudioReg;

	uchar duty;								//second reg variables
	uchar envInitVol, envType, envNum;		//third reg variables
	
	void callback(float* target, int num_samples);
	float waveAudio();
	void resetAudio();
	void timerUpdate();
};
class SquareChannel2 : public AudioBaseClass {
public:
	SquareChannel2();
	void soundOnReg(uchar input);
	void soundLengthReg(uchar input);
	void soundVolReg(uchar input);
	void soundFreqLowReg(uchar input);
	void soundFreqHiReg(uchar input);
private:
	uchar duty;
	uchar envInitVol, envType, envNum;
	void callback(float* target, int num_samples);
	float waveAudio();
	void resetAudio();
};
class NoiseChannel : public AudioBaseClass {
public:
	NoiseChannel();
	void soundOnReg(uchar input);
	void soundLengthReg(uchar input);
	void soundVolReg(uchar input);
	void soundPolyReg(uchar input);
	void soundCounterReg(uchar input);
	void update();
private:
	LFSR shiftReg;
	uchar envInitVol, envType, envNum;
	void callback(float* target, int num_samples);
	float waveAudio();
	void resetAudio();
};


extern SquareChannel1 * square1;
extern SquareChannel2 * square2;
extern NoiseChannel * noise;
extern SineChannel * sine;


int initSound();
void updateSound();
void masterSoundEnable(uchar);
#endif sound