#include <sdl.h>
#include "Memory.h"
#include "CPU.h"
#include "defs.h"
#include "Graphics.h"
#include "Timer.h"
#include <math.h>
#include <iostream>

class AudioBaseClass
{
public://protected:
		SDL_AudioSpec spec;
		SDL_AudioDeviceID dev;
		Uint16 freq = 0, time = 0; 
		Uint16 playing = 0, durationEnabled = 0, soundDuration = 0, stopped = 0;
	public:
		uchar mode = 0, mute = 0;
		AudioBaseClass()
		{
			spec.freq = 44100;//not actual sound frequency by audio rate
			#define delta_t spec.freq //looks like if i have this modify the freq to match the frame rate i might be able to have it sound proper
			spec.format = AUDIO_F32SYS;//range of -1 to 1, .1 works best
			spec.channels = 1;//audio channels every other depends on which channel to use might change later to 2, will require more need more work
			spec.samples = 16;//buffer size, larger buffer less time in callback
			spec.userdata = this;
		}

		void initSound() {
			dev = SDL_OpenAudioDevice(nullptr, 0, &spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
		}

		void play() {

			std::cout << "play\n";
			SDL_PauseAudioDevice(dev, 0);
		}

		void pause() {
			SDL_PauseAudioDevice(dev, 1);
		}
};

class LFSR {
	private:
		ushort shiftReg;

		void clockRegister() {
			uchar result;
			result = (shiftReg & 0x1) ^ (shiftReg & 0x2);
			shiftReg |= (result << 15);
			if (readMem(0xFF22) & 0x8) {
				if (result)
					shiftReg |= 0x80;
				else
					shiftReg &= ~0x80;
			}
			shiftReg >>= 1;
		}

	public:

		void init() {
			shiftReg = 0x7FFF;
		}

		uchar note() {
			return shiftReg & 0x1;
		}

		void update() {
			static ulong clockDiv = 0;
			static ulong cpuTickStamp = 0;
			if (getCPUTicks() - cpuTickStamp >= clockDiv) {

				uchar Chan4Polynomial = readMem(0xFF22);
				uchar clockShiftReg = (Chan4Polynomial & 0xF0) >> 4;
				uchar clockDivReg = Chan4Polynomial & 0x07;

				clockDiv = (clockDivReg == 0) ? 4 : clockDivReg * 4;
				clockDiv = clockDiv / (clockDiv * (2 << ((clockDivReg > 14) ? 14 : clockDivReg)));
				cpuTickStamp = getCPUTicks();
				clockRegister();
			}
		}
};

class NoiseChannel : public AudioBaseClass {
	public:
		LFSR shiftReg;
	
		NoiseChannel() : AudioBaseClass() {
			spec.callback = [](void* param, Uint8* stream, int len)
			{
				((NoiseChannel*)param)->callback((float *)stream, len / sizeof(int));
			};
			initSound();
			play();
		}

	void callback(float* target, int num_samples)
	{
		float sample = 0;
		for (int position = 0; position < num_samples; position++)
		{
			
			if (mute == 0)
				sample = 0;
			else
				sample = shiftReg.note();
			time = (++time) % delta_t;
			target[position] = sample;
		}
	}

	
};

class SineChannel : public AudioBaseClass {
	public:
		SineChannel() : AudioBaseClass() {
			spec.callback = [](void* param, Uint8* stream, int len)
			{
				((SineChannel*)param)->callback((float *)stream, len / sizeof(int));
			};
			initSound();
			play();
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
				sample = 0.2f * sin(M_PI * 2 * pitch * time / (float)delta_t);
			time = (++time) % delta_t;
			target[position] = sample;
		}
	}


};

class SquareChannel : public AudioBaseClass {
public:
	SquareChannel() : AudioBaseClass() {
		spec.callback = [](void* param, Uint8* stream, int len)
		{
			((SquareChannel*)param)->callback((float *)stream, len / sizeof(int));
		};
		initSound();
		play();
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
				sample = sin(M_PI * 2 * pitch * time / (float)delta_t) < 0 ? -0.2f : 0.2f;
			time = (++time) % delta_t;
			target[position] = sample;
		}
	}
};

SquareChannel * square1;
SquareChannel *square2;
SineChannel * sine;
NoiseChannel * noise;


int initSound() {
	square1 = new SquareChannel;
	square2 = new SquareChannel;
	sine = new SineChannel;
	noise = new NoiseChannel;
	return 0; //change this to an error if it fails
}

void soundChannel1(){
	uchar Chan1Sweep = readMem(0xFF10);
	uchar sweepTime = (Chan1Sweep & 0x70) >> 4;
	uchar sweepSub = (Chan1Sweep & 0x08) >> 3;
	uchar sweepShift = Chan1Sweep & 0x07;

	uchar Chan1Sound = readMem(0xFF11);
	uchar dutyCycle = (Chan1Sound & 0xC0) >> 6;	//singal volume control 12.5,25,50,75 50 normal
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

	square1->freq = 131072 / (2048 - freq);
	square1->durationEnabled = counterConsecutive;
	square1->soundDuration = (64 - soundLength) * (1 / 256.0f);

}

void soundChannel2() {
	uchar Chan2Sound = readMem(0xFF16);
	uchar dutyCycle = (Chan2Sound & 0xC0) >> 6;
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

	square2->freq = 131072 / (2048 - freq);
	square2->durationEnabled = counterConsecutive;
	square2->soundDuration = (64 - soundLength) * (1 / 256.0f);
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

	sine->freq = 65536 / (2048 - freq);
	sine->durationEnabled = counterConsecutive;
	sine->soundDuration = (64 - Chan3Length) * (1 / 256.0f);
}

void soundChannel4() {
	uchar Chan4Sound = readMem(0xFF20);
	uchar soundLength = Chan4Sound & 0x3F;

	uchar Chan4Envelope = readMem(0xFF21);
	uchar initalVolume = (Chan4Envelope & 0xF0) >> 4;
	uchar volumeInc = (Chan4Envelope & 0x08) >> 3;
	uchar totalSweeps = Chan4Envelope & 0x07;

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

	square1->mute = SoundEnabled;
	square2->mute = SoundEnabled;
	sine->mute = SoundEnabled;
	noise->mute = SoundEnabled;
}

void updateSound() {
	
	static ushort counter = 0;
	noise->shiftReg.update();
	//Might want to limit the call rate of function for less cpu utilization and faster speed or make it threaded.
	if (++counter < FRAMESPERSEC / 100)//update audio every .01 of a second of cpu speed time helps with fps time might need to be reduced if higher update rate is needed
		return;

	counter = 0;
	soundChannel1();
	soundChannel2();
	soundChannel3();
	soundChannel4();
	soundMaster();
}