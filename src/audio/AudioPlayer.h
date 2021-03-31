#pragma once
#include <AL/al.h>
#include <AL/alc.h>

namespace hyperbright {
namespace audio {
class AudioPlayer
{
public:
	AudioPlayer();
	~AudioPlayer();
	void playSound(int number);//USED FOR TESTING AND INITIAL DESIGN DELETE LATER
	void playMusic(int number);//USED FOR TESTING AND INITIAL DESIGN DELETE LATER
	void playGameMusic();
	void stopGameMusic();
	void playStartMenuMusic();
	void playMenuSwitchSound();
	void playMenuEnterSound();
	void stopStartMenuMusic();
	void playPickupCollision();
	void playCarIdle();
	void adjustCarIdlePitch(float speed);
	void stopCarIdle();
	void playPowerstationCollision();
	void playUsePowerupSound();
	void playEmpSound();
	void playSpeedSound();
	void playTrapHitSound();
	void playZapSound();
	void setMusicVolume(float volume);
private:
	void init();
	int curLoaded;
	void loadSound(const char* filename);
	void AudioPlayer::CheckError(int op, int _err);
	bool AudioPlayer::_strcmp(const char* base, const char* cp);
	bool AudioPlayer::loadWavFile(const char* filename, ALuint* buffer,
		ALsizei* size, ALsizei* frequency,
		ALenum* format);
};
}	// namespace audio
}	// namespace hyperbright