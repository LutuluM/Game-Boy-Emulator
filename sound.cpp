#include <sdl.h>
#include "Memory.h"
#include "CPU.h"
#include "defs.h"
#include <math.h>



struct MyAudioEngine
{
	SDL_AudioSpec spec;
	Uint16 freq = 0, time = 0, mode = 0, mute = 0;
	//#define delta_t 1/(45128.0f)
	//#define delta_t 1/(44100.0f)
	#define delta_t 44100

	MyAudioEngine()
	{
		spec.freq = 44100;//not actual sound frequency by audio rate
		spec.format = AUDIO_F32SYS;//range of -1 to 1, .1 works best
		spec.channels = 1;//audio channels every other depends on which channel to use might change later to 2, will require more need more work
		spec.samples = 16;//buffer size, larger buffer less time in callback
		spec.userdata = this;//pointer passed to callback function
		spec.callback = [](void* param, Uint8* stream, int len)
		{
			((MyAudioEngine*)param)->callback((float *)stream, len / sizeof(int));
		};
		SDL_AudioDeviceID dev = SDL_OpenAudioDevice(nullptr, 0, &spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
		SDL_PauseAudioDevice(dev, 0);
	}

	void callback(float* target, int num_samples)
	{
		float sample = 0;
		Uint16 pitch = this->freq;
		for (int position = 0; position < num_samples; position++)
		{
			if (mute == 0)
				sample = 0;
			else
			{
				if (mode == 0)
					sample = sin(M_PI * 2 * pitch * time / (float)delta_t) < 0 ? -0.1f / sqrt(2) : 0.1f / sqrt(2);
				else
					sample = 0.1f * sin(M_PI * 2 * pitch * time / (float)delta_t);
				time = (++time) % delta_t;
			}			
			target[position] = sample;
		}
	}

};

MyAudioEngine * soundMixer1;
MyAudioEngine * soundMixer2;
MyAudioEngine * soundMixer3;

ushort binToFreq(ushort bin) {
	return 131072 / (2048 - bin);
}

int initSound() {
	soundMixer1 = new MyAudioEngine;
	soundMixer2 = new MyAudioEngine;
	soundMixer3 = new MyAudioEngine;
	soundMixer3->mode = 1;
	return 0; //change this to an error if it fails
}

void soundChannel1(){
	uchar Chan1Sweep = readMem(0xFF10);
	uchar sweepTime = (Chan1Sweep & 0x70) >> 4;
	uchar sweepSub = (Chan1Sweep & 0x08) >> 3;
	uchar sweepShift = Chan1Sweep & 0x07;

	uchar Chan1Sound = readMem(0xFF11);
	uchar dutyCycle = (Chan1Sound & 0xB0) >> 6;
	uchar soundLength = Chan1Sound & 0x3F;

	uchar Chan1Envelope = readMem(0xFF12);
	uchar initalVolume = (Chan1Envelope & 0xF0) >> 4;
	uchar volumeInc = (Chan1Envelope & 0x08) >> 3;
	uchar totalSweeps = Chan1Envelope & 0x07;

	uchar Chan1FreqLow = readMem(0xFF13);
	uchar Chan1FreqHigh = readMem(0xFF14);
	uchar reset = (Chan1FreqHigh & 0x80) >> 7;
	uchar counterConsecutive = (Chan1FreqHigh & 0x40) >> 6;
	ushort freq = ((Chan1FreqHigh & 0x07) << 8) | Chan1FreqLow;

	soundMixer1->freq = binToFreq(freq);
}

void soundChannel2() {
	uchar Chan2Sound = readMem(0xFF16);
	uchar dutyCycle = (Chan2Sound & 0xB0) >> 6;
	uchar soundLength = Chan2Sound & 0x3F;

	uchar Chan2Envelope = readMem(0xFF17);
	uchar initalVolume = (Chan2Envelope & 0xF0) >> 4;
	uchar volumeInc = (Chan2Envelope & 0x08) >> 3;
	uchar totalSweeps = Chan2Envelope & 0x07;

	uchar Chan2FreqLow = readMem(0xFF18);
	uchar Chan2FreqHigh = readMem(0xFF19);
	uchar reset = (Chan2FreqHigh & 0x80) >> 7;
	uchar counterConsecutive = (Chan2FreqHigh & 0x40) >> 6;
	ushort freq = ((Chan2FreqHigh & 0x07) << 8) | Chan2FreqLow;

	soundMixer2->freq = binToFreq(freq);
}

void soundChannel3() {
	uchar Chan3Sound = readMem(0xFF1A);
	uchar soundOn = (Chan3Sound & 0x80) >> 7;

	uchar Chan3Length = readMem(0xFF1B);

	uchar Chan3Output = readMem(0xFF1C);
	uchar volume = (Chan3Output & 0x60) >> 5;

	uchar Chan3FreqLow = readMem(0xFF1D);
	uchar Chan3FreqHigh = readMem(0xFF1E);
	uchar reset = (Chan3FreqHigh & 0x80) >> 7;
	uchar counterConsecutive = (Chan3FreqHigh & 0x40) >> 6;
	ushort freq = ((Chan3FreqHigh & 0x07) << 8) | Chan3FreqLow;

	uchar Chan3Pattern0 = readMem(0xFF30);
	uchar Chan3Pattern1 = readMem(0xFF31);
	uchar Chan3Pattern2 = readMem(0xFF32);
	uchar Chan3Pattern3 = readMem(0xFF33);
	uchar Chan3Pattern4 = readMem(0xFF34);
	uchar Chan3Pattern5 = readMem(0xFF35);
	uchar Chan3Pattern6 = readMem(0xFF36);
	uchar Chan3Pattern7 = readMem(0xFF37);
	uchar Chan3Pattern8 = readMem(0xFF38);
	uchar Chan3Pattern9 = readMem(0xFF39);
	uchar Chan3Pattern10 = readMem(0xFF3A);
	uchar Chan3Pattern11 = readMem(0xFF3B);
	uchar Chan3Pattern12 = readMem(0xFF3C);
	uchar Chan3Pattern13 = readMem(0xFF3D);
	uchar Chan3Pattern14 = readMem(0xFF3E);
	uchar Chan3Pattern15 = readMem(0xFF3F);

	soundMixer3->freq = binToFreq(freq);
}

void soundChannel4() {
	uchar Chan4Sound = readMem(0xFF20);
	uchar soundLength = Chan4Sound & 0x3F;

	uchar Chan4Envelope = readMem(0xFF21);
	uchar initalVolume = (Chan4Envelope & 0xF0) >> 4;
	uchar volumeInc = (Chan4Envelope & 0x08) >> 3;
	uchar totalSweeps = Chan4Envelope & 0x07;

	uchar Chan4Polynomial = readMem(0xFF22);
	uchar shiftFreq = (Chan4Polynomial & 0xF0) >> 4;
	uchar counterWidth = (Chan4Polynomial & 0x08) >> 3;
	uchar dividingRatio = Chan4Polynomial & 0x03;

	uchar Chan4Inital = readMem(0xFF23);
	uchar reset = (Chan4Inital & 0x80) >> 7;
	uchar counterConsecutive = (Chan4Inital & 0x40) >> 6;
}

void soundMaster() {
	uchar ChannelControl = readMem(0xFF24);
	uchar SO2Volume = (ChannelControl & 0x70) >> 4;
	uchar SO1Volume = ChannelControl & 0x07;

	uchar SoundOutSelect = readMem(0xFF25);

	uchar SoundMaster = readMem(0xFF26);
	uchar SoundEnabled = (SoundMaster & 0x80) >> 7;

	soundMixer1->mute = SoundEnabled;
	soundMixer2->mute = SoundEnabled;
	soundMixer3->mute = SoundEnabled;
}


void updateSound() {
	//Might want to limit the call rate of function for less cpu utilization and faster speed or make it threaded.

	soundChannel1();
	soundChannel2();
	soundChannel3();
	soundChannel4();
	soundMaster();
}



//might change later as parameters for each mixer of as function calls
double sineWave(int freq, double amp,double delta_t1) {
	return amp * sin(M_PI * 2 * freq * delta_t1);
}

double squareWave(int freq, double amp, double delta_t1) {
	return sin(M_PI * 2 * freq * delta_t1) > 0 ? amp : -amp;
}

double whiteNoise() {

}