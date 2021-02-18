#pragma once
#include <AL/al.h>
#include <AL/alc.h>
class AudioPlayer
{
public:
	AudioPlayer();
	~AudioPlayer();
	void playSound(int number);
	void playMusic(int number);
	void playGameMusic();
	void stopGameMusic();
	void playPickupCollision();
	void playCarIdle();
	void stopCarIdle();
private:
	void init();
	int curLoaded;
	void loadSound(const char* filename);
	void AudioPlayer::CheckError(int op , int _err );
	bool AudioPlayer::_strcmp(const char* base, const char* cp);
	bool AudioPlayer::loadWavFile(const char* filename, ALuint* buffer,
		ALsizei* size, ALsizei* frequency,
		ALenum* format);
};
