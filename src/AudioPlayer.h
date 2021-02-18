#pragma once
class AudioPlayer
{
public:
	AudioPlayer();
	~AudioPlayer();
	void loadSound(const char* filename);
	void playSound(int number);
private:
	int curLoaded;
};
