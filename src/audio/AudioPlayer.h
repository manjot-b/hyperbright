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
	void startWinMusic();
	void stopWinMusic();
	void playCarCollisionSound();
	void playWallCollisionSound();
	void startLossMusic();
	void stopLossMusic();

	void muteCarIdle();
	void unmuteCarIdle(); 

private:
	float carIdleGain;
	void init();
	int curLoaded;
	void loadSound(const char* filename);
	void CheckError(int op, int _err);
	bool _strcmp(const char* base, const char* cp);
	bool loadWavFile(const char* filename, ALuint* buffer,
		ALsizei* size, ALsizei* frequency,
		ALenum* format);
};
}	// namespace audio
}	// namespace hyperbright
