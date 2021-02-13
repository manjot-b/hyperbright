#include "AudioPlayer.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>

AudioPlayer::AudioPlayer()
{
	ALCdevice* device = alcOpenDevice(nullptr);

	if (!device)
	{
		std::cerr << "Could not open an audio device.\n";
	}
	else
	{
		std::cout << "Audio deivce found!\n";
	}
}