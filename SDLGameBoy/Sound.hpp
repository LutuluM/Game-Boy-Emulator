#ifndef _Sound_hpp
#define _Sound_hpp
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
    SDL_AudioDeviceID dev = 0;
    uchar soundEnable = 0, soundDuration = 0, freqLow = 0, freqHi = 0, counterEnable = 0, masterEnable = 0, channelEnable = 0;
    float amp = 0;
    ushort frequency = 0;
    ushort time = 0;
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
    uchar volume = 0;
    void callback(float* target, int num_samples);
    void resetAudio();
    float waveAudio();
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
    uchar sweepTime = 0, sweepType = 0, sweepNum = 0;    //first reg variables
    uchar sweepEnable = 0, sweepShifts = 0;
    ushort shadowAudioReg = 0;

    uchar duty = 0;                                        //second reg variables
    uchar envInitVol = 0, envType = 0, envNum = 0;        //third reg variables
    
    void callback(float* target, int num_samples);
    void resetAudio();
    void timerUpdate();
    float waveAudio();
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
    uchar duty = 0;
    uchar envInitVol = 0, envType = 0, envNum = 0;
    void callback(float* target, int num_samples);
    void resetAudio();
    float waveAudio();
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
    uchar envInitVol = 0, envType = 0, envNum = 0;
    void callback(float* target, int num_samples);
    void resetAudio();
    float waveAudio();
};


extern SquareChannel1 * square1;
extern SquareChannel2 * square2;
extern NoiseChannel * noise;
extern SineChannel * sine;


int initSound();
void updateSound();
void masterSoundEnable(uchar);
#endif