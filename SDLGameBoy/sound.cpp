#include "sound.h"
#include "Memory.h"
#include "CPU.h"
#include "Graphics.h"
#include "Timer.h"
#include <math.h>

AudioBaseClass::AudioBaseClass(){
	masterEnable = 0;
	spec.freq = 44100;//not actual sound frequency by audio rate
	#define delta_t spec.freq //looks like if i have this modify the freq to match the frame rate i might be able to have it sound proper
	spec.format = AUDIO_F32SYS;//range of -1 to 1, .1 works best
	spec.channels = 1;//audio channels every other depends on which channel to use might change later to 2, will require more need more work
	spec.samples = 16;//buffer size, larger buffer less time in callback
	spec.userdata = this;
}
void AudioBaseClass::startSoundCallBack() {
	dev = SDL_OpenAudioDevice(nullptr, 0, &spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
}
void AudioBaseClass::play() {
	SDL_PauseAudioDevice(dev, 0);
}
void AudioBaseClass::pause() {
	SDL_PauseAudioDevice(dev, 1);
}
void AudioBaseClass::soundEnabled(uchar input) {
	masterEnable = input;
}

LFSR::LFSR() {
	init();
	cpuTickStamp = 0;
	clockDiv = 0;
}
void LFSR::clockRegister() {
	uchar result;
	result = (shiftReg & 0x1) ^ ((shiftReg & 0x2) >> 1);
	shiftReg |= (result << 15);
	if (clockWidth) {
		if (result)
			shiftReg |= 0x80;
		else
			shiftReg &= ~0x80;
	}
	shiftReg >>= 1;
}
void LFSR::init() {
	shiftReg = 0x7FFF;
}
uchar LFSR::note() {
	return shiftReg & 0x1;
}
void LFSR::soundPolyReg(uchar input) {
	clockShiftReg = (input & 0xF0) >> 4;
	clockWidth = (input & 0x8) >> 3;
	clockDivReg = input & 0x7;
}
void LFSR::update() {
	if (getCPUTicks() - cpuTickStamp >= clockDiv) {
		clockDiv = (clockDivReg == 0) ? 4 : clockDivReg * 4;
		clockDiv *= (2 << ((clockShiftReg > 14) ? 14 : clockShiftReg));
		cpuTickStamp = getCPUTicks();
		clockRegister();
	}
}

SquareChannel1::SquareChannel1() : AudioBaseClass() {
	spec.callback = [](void* param, Uint8* stream, int len)
	{
		((SquareChannel1*)param)->callback((float *)stream, len / sizeof(int));
	};
	startSoundCallBack();
	play();
}
void SquareChannel1::soundOnReg(uchar input) {
	sweepTime= (input & 0x70) >> 4;
	sweepType= (input & 0x8) >> 3;
	sweetNum= input & 0x7;
	soundMemWrite(0x10, input);
};
void SquareChannel1::soundLengthReg(uchar input) {
	duty = (input & 0xC0) >> 6;
	soundDuration = input & 0x3F;
	soundMemWrite(0x11, input);
};
void SquareChannel1::soundVolReg(uchar input) {
	envInitVol = (input & 0xF0) >> 4;
	envType = (input & 0x8) >> 3;
	envNum = input & 0x7;
	soundMemWrite(0x12, input);
};
void SquareChannel1::soundFreqLowReg(uchar input) {
	freqLow = input;
	frequency = 131072 / (2048 - ((freqHi << 8) | freqLow));
	soundMemWrite(0x13, input);
};
void SquareChannel1::soundFreqHiReg(uchar input) {
	freqHi = input & 0x7;
	counterEnable = (input & 0x40) >> 6;
	frequency = 131072 / (2048 - ((freqHi << 8) | freqLow));
	if ((input & 0x80) >> 7)
		resetAudio();
	soundMemWrite(0x14, input);
};
void SquareChannel1::callback(float* target, int num_samples){
	float sample = 0;
	for (int position = 0; position < num_samples; position++){
		if (masterEnable == 0)
			sample = 0;
		else if (channelEnable == 0)
			sample = 0;
		else if (counterEnable){
			if (duty == 0){
				sample = 0;
				lastDecDuty = time;
				channelEnable = 0;
			}
			else {
				sample = waveAudio();
				if (time - lastDecDuty >= 172){
					soundMemWrite(0x11,(readMem(0xFF11) & 0xC0) | --soundDuration);
					lastDecDuty = time;
				}
			}
		}
		else
			sample = waveAudio();
		time = (++time) % delta_t;
		target[position] = sample;
	}
}
float SquareChannel1::waveAudio() {
	float amp;
	if (envNum) {
		if (time - lastVolStep >= 689 * envNum) {
			envType ? envInitVol-- : envInitVol++;
			envNum--;
			soundMemWrite(0x12, (envInitVol << 4) | (envType << 3) | envNum);
			lastVolStep = time;
		}
		amp = envInitVol / 15.0f;
	}
	else
		amp = 1;
	return sin(M_PI * 2 * frequency * time / (float)delta_t) < 0 ? -0.2f * amp: 0.2f * amp;
}
void SquareChannel1::resetAudio() {
	if (soundDuration == 0) {
		soundDuration = 63;
	}
	soundMemWrite(0x11, (duty << 6) | soundDuration);
	
	envNum = 0x7;
	envInitVol = 0xF;
	soundMemWrite(0x12, (envInitVol << 4) | (envType << 3) | envNum);

	channelEnable = 1;
};

SquareChannel2::SquareChannel2() : AudioBaseClass() {
	spec.callback = [](void* param, Uint8* stream, int len)
	{
		((SquareChannel2*)param)->callback((float *)stream, len / sizeof(int));
	};
	startSoundCallBack();
	play();
}
void SquareChannel2::soundOnReg(uchar input) {
	soundMemWrite(0x15, input);
};
void SquareChannel2::soundLengthReg(uchar input) {
	duty = (input & 0xC0) >> 6;
	soundDuration = input & 0x3F;
	soundMemWrite(0x16, input);
};
void SquareChannel2::soundVolReg(uchar input) {
	envInitVol = (input & 0xF0) >> 4;
	envType = (input & 0x8) >> 3;
	envNum = input & 0x7;
	soundMemWrite(0x17, input);
};
void SquareChannel2::soundFreqLowReg(uchar input) {
	freqLow = input;
	frequency = 131072 / (2048 - ((freqHi << 8) | freqLow));
	soundMemWrite(0x18, input);
};
void SquareChannel2::soundFreqHiReg(uchar input) {
	freqHi = input & 0x7;
	counterEnable = (input & 0x40) >> 6;
	frequency = 131072 / (2048 - ((freqHi << 8) | freqLow));
	if ((input & 0x80) >> 7)
		resetAudio();
	soundMemWrite(0x19, input);
};
void SquareChannel2::callback(float* target, int num_samples) {
	float sample = 0;
	for (int position = 0; position < num_samples; position++) {
		if (masterEnable == 0)
			sample = 0;
		else if (channelEnable == 0)
			sample = 0;
		else if (counterEnable) {
			if (duty == 0) {
				sample = 0;
				lastDecDuty = time;
				channelEnable = 0;
			}
			else {
				sample = waveAudio();
				if (time - lastDecDuty >= 172) {
					soundMemWrite(0x16, (readMem(0xFF16) & 0xC0) | --soundDuration);
					lastDecDuty = time;
				}
			}
		}
		else
			sample = waveAudio();
		time = (++time) % delta_t;
		target[position] = sample;
	}
}
float SquareChannel2::waveAudio() {
	return sin(M_PI * 2 * frequency * time / (float)delta_t) < 0 ? -0.2f : 0.2f;
}
void SquareChannel2::resetAudio() {
	channelEnable = 1;
};

SineChannel::SineChannel() : AudioBaseClass() {
	spec.callback = [](void* param, Uint8* stream, int len)
	{
		((SineChannel*)param)->callback((float *)stream, len / sizeof(int));
	};
	startSoundCallBack();
	play();
}
void SineChannel::soundOnReg(uchar input) {
	soundEnable = (input & 0x80) >> 7;
	soundMemWrite(0x1A, input);
};
void SineChannel::soundLengthReg(uchar input) {
	soundDuration = input;
	soundMemWrite(0x1B, input);
};
void SineChannel::soundVolReg(uchar input) {
	volume = (0x60 & input) >> 5;
	soundMemWrite(0x1C, input);
};
void SineChannel::soundFreqLowReg(uchar input) {
	freqLow = input;
	frequency = 65536 / (2048 - ((freqHi << 8) | freqLow));
	soundMemWrite(0x1D, input);
};
void SineChannel::soundFreqHiReg(uchar input) {
	freqHi = input & 0x7;
	counterEnable = (input & 0x40) >> 6;
	frequency = 65536 / (2048 - ((freqHi << 8) | freqLow));
	if ((input & 0x80) >> 7)
		resetAudio();
	soundMemWrite(0x1E, input);
};
void SineChannel::callback(float* target, int num_samples)
{
	float sample = 0;
	for (int position = 0; position < num_samples; position++)
	{
		if (masterEnable == 0)
			sample = 0;
		else if (channelEnable == 0)
			sample = 0;
		else if (counterEnable)
		{
			if (soundDuration == 0)
			{
				sample = 0;
				lastDecDuty = time;
				channelEnable = 0;
			}
			else {
				sample = waveAudio();
				if (time - lastDecDuty >= 172)
				{
					soundMemWrite(0x1B, --soundDuration);
					lastDecDuty = time;
				}
			}
		}
		else
			sample = waveAudio();

		time = (++time) % delta_t;
		target[position] = sample;
	}
}
float SineChannel::waveAudio() {
	switch (volume)
	{
	case 0:
		return 0;
	case 1:
		return 0.2f * sin(soundEnable * M_PI * 2 * frequency * time / (float)delta_t);
	case 2:
		return 0.1f * sin(soundEnable * M_PI * 2 * frequency * time / (float)delta_t);
	case 3:
		return 0.05f * sin(soundEnable * M_PI * 2 * frequency * time / (float)delta_t);
	}
}
void SineChannel::resetAudio() {
	channelEnable = 1;
};

NoiseChannel::NoiseChannel() : AudioBaseClass() {
	spec.callback = [](void* param, Uint8* stream, int len)
	{
		((NoiseChannel*)param)->callback((float *)stream, len / sizeof(int));
	};
	startSoundCallBack();
	play();
}
void NoiseChannel::soundOnReg(uchar input) {
	soundMemWrite(0x1F, input);
};
void NoiseChannel::soundLengthReg(uchar input) {
	soundDuration = input & 0x3F;
	soundMemWrite(0x20, input);
};
void NoiseChannel::soundVolReg(uchar input) {
	envInitVol = (input & 0xF0) >> 4;
	envType = (input & 0x8) >> 3;
	envNum = input & 0x7;
	soundMemWrite(0x21, input);
};
void NoiseChannel::soundPolyReg(uchar input) {
	freqLow = input;
	shiftReg.soundPolyReg(input);
	soundMemWrite(0x22, input);
};
void NoiseChannel::soundCounterReg(uchar input) {
	counterEnable = (input & 0x40) >> 6;
	if ((input & 0x80) >> 7)
		resetAudio();
	soundMemWrite(0x23, input);
};
void NoiseChannel::update() {
	shiftReg.update();
}
void NoiseChannel::callback(float* target, int num_samples)
{
	float sample = 0;
	for (int position = 0; position < num_samples; position++)
	{

		if (masterEnable == 0)
			sample = 0;
		else if (channelEnable == 0)
			sample = 0;
		else if (counterEnable)
		{
			if (soundDuration == 0)
			{
				sample = 0;
				lastDecDuty = time;
				channelEnable = 0;
			}
			else {
				sample = waveAudio();
				if (time - lastDecDuty >= 172)
				{
					soundMemWrite(0x20, (readMem(0xFF20) & 0xC0) | --soundDuration);
					lastDecDuty = time;
				}
			}
		}
		else
			sample = waveAudio();
		target[position] = sample;
	}
}
float NoiseChannel::waveAudio() {
	return (shiftReg.note()) ? .2f : -.2;
}
void NoiseChannel::resetAudio() {
	shiftReg.init();
	channelEnable = 1;
};

SquareChannel1 * square1;
SquareChannel2 * square2;
SineChannel * sine;
NoiseChannel * noise;

int initSound() {
	square1 = new SquareChannel1;
	square2 = new SquareChannel2;
	sine = new SineChannel;
	noise = new NoiseChannel;
	return 0; //change this to an error if it fails
}

void soundMaster() {
	uchar ChannelControl = readMem(0xFF24);
	uchar SO2Volume = (ChannelControl & 0x70) >> 4;
	uchar SO1Volume = ChannelControl & 0x07;

	uchar SoundOutSelect = readMem(0xFF25);
}

void updateSound() {
	noise->update();//Might be the only function needed since everything else should be either triggered by mem writes or in a seperate thread
	
	
	//Might be able to remove these functions
	static ushort counter = 0;
	if (++counter < FRAMESPERSEC / 1000)//update audio every .001 of a second of cpu speed time helps with fps time might need to be reduced if higher update rate is needed
		return;
	counter = 0;
	soundMaster();
}

void masterSoundEnable(uchar input) {
	uchar enable = input >> 7;
	/*comment lines to disable channels
		This is the master enable, disable audio because circuit would be off	
	*/
	//square1->soundEnabled(enable);
	//square2->soundEnabled(enable);
	//sine->soundEnabled(enable);
	noise->soundEnabled(enable);
	soundMemWrite(0x26, (input & 0x80) | (readMem(0xFF26) & 0x0F)); //only bit 7 gets written from direct writes
}