#include "AudioPlayer.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>

#define GAMEMUSIC 0
#define PICKUPCOLLISION 1
#define CARIDLE 2
#define STARTMENUMUSIC 3
#define POWERSTATIONCOLLISION 4

#define NUM_BUFFERS 5 //NUMBER OF SOUND FILES
#define NUM_SOURCES 5
#define NUM_ENVIRONMENTS 5

namespace hyperbright {
namespace audio {

ALfloat listenerPos[] = { 0.0,0.0,4.0 };
ALfloat listenerVel[] = { 0.0,0.0,0.0 };
ALfloat listenerOri[] = { 0.0,0.0,1.0, 0.0,1.0,0.0 };

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

ALuint  buffer[NUM_BUFFERS];
ALuint  source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];

ALsizei size, freq;
ALenum  format;
ALvoid* data;
ALCdevice* device;
ALCcontext* ctx;


/*
    * Struct that holds the RIFF data of the Wave file.
    * The RIFF data is the meta data information that holds,
    * the ID, size and format of the wave file
    */
struct RIFF_Header {
    char chunkID[4];
    int chunkSize;//size not including chunkSize or chunkID
    char format[4];
};

//Struct to hold fmt subchunk data for WAVE files.
struct WAVE_Format {
    char subChunkID[4];
    int subChunkSize;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
};

//Struct to hold the data of the wave file
struct WAVE_Data {
    char subChunkID[4]; //should contain the word data
    int subChunk2Size; //Stores the size of the data block
};

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::loadSound(const char* filename) {
    loadWavFile(filename, buffer + curLoaded, &size, &freq, &format);
    curLoaded++;
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::CheckError(int op = -1, int _err = 0) {
    int err;
    if (op == -1)
        err = alGetError(); // clear any error messages
    else
        err = _err;
    if (err != AL_NO_ERROR) {
        if (err == AL_INVALID_NAME)
            fprintf(stderr, "Error : Invalid Name\n");
        else if (err == AL_INVALID_ENUM)
            fprintf(stderr, "Error : Invalid Enum\n");
        else if (err == AL_INVALID_VALUE)
            fprintf(stderr, "Error : Invalid Value\n");
        else if (err == AL_INVALID_OPERATION)
            fprintf(stderr, "Error : Invalid Operation\n");
        else if (err == AL_OUT_OF_MEMORY)
            fprintf(stderr, "Error : Out of Memory\n");
        exit(1);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////

bool AudioPlayer::_strcmp(const char* base, const char* cp) {
    for (int lx = 0; base[lx] != 0; lx++) {
        if (cp[lx] != base[lx])
            return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////

bool AudioPlayer::loadWavFile(const char* filename, ALuint* buffer,
    ALsizei* size, ALsizei* frequency,
    ALenum* format) {
    //Local Declarations
    fprintf(stderr, "[1] filename = %s\n", filename);
    FILE* soundFile = NULL;
    WAVE_Format wave_format;
    RIFF_Header riff_header;
    WAVE_Data wave_data;
    unsigned char* data = 0;

    try {
        soundFile = fopen(filename, "rb");
        if (!soundFile)
            throw (filename);

        // Read in the first chunk into the struct
        fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

        //check for RIFF and WAVE tag in memeory
        if (_strcmp("RIFF", riff_header.chunkID) == false ||
            _strcmp("WAVE", riff_header.format) == false)
            throw ("Invalid RIFF or WAVE Header");

        //Read in the 2nd chunk for the wave info
        fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
        //check for fmt tag in memory
        if (_strcmp("fmt ", wave_format.subChunkID) == false)
            throw ("Invalid Wave Format");

        //check for extra parameters;
        if (wave_format.subChunkSize > 16)
            fseek(soundFile, sizeof(short), SEEK_CUR);

        //Read in the the last byte of data before the sound file
        fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
        //check for data tag in memory
        if (_strcmp("data", wave_data.subChunkID) == false)
            throw ("Invalid data header");

        //Allocate memory for data
        data = new unsigned char[wave_data.subChunk2Size];

        // Read in the sound data into the soundData variable
        if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
            throw ("error loading WAVE data into struct!");

        //Now we set the variables that we passed in with the
        //data from the structs
        *size = wave_data.subChunk2Size;
        *frequency = wave_format.sampleRate;
        //The format is worked out by looking at the number of
        //channels and the bits per sample.
        if (wave_format.numChannels == 1) {
            if (wave_format.bitsPerSample == 8)
                *format = AL_FORMAT_MONO8;
            else if (wave_format.bitsPerSample == 16)
                *format = AL_FORMAT_MONO16;
        }
        else if (wave_format.numChannels == 2) {
            if (wave_format.bitsPerSample == 8)
                *format = AL_FORMAT_STEREO8;
            else if (wave_format.bitsPerSample == 16)
                *format = AL_FORMAT_STEREO16;
        }
        //create our openAL buffer and check for success
        CheckError();
        alGenBuffers(1, buffer);
        CheckError();
        //now we put our data into the openAL buffer and
        //check for success
        alBufferData(*buffer, *format, (void*)data, *size, *frequency);
        CheckError();
        //clean up and return true if successful
        fclose(soundFile);
        fprintf(stderr, "load ok\n");
        delete[] data;
        return true;
    }
    catch (std::string error) {
        //our catch statement for if we throw a string
        std::cout << error << std::endl;
        //clean up memory if wave loading fails
        if (soundFile != NULL)
            fclose(soundFile);
        if (data)
            delete[] data;
        //return false to indicate the failure to load wave
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::init() {
    alListenerfv(AL_POSITION, listenerPos);
    alListenerfv(AL_VELOCITY, listenerVel);
    alListenerfv(AL_ORIENTATION, listenerOri);

    // Generate buffers, or else no sound will happen!
    alGenSources(NUM_SOURCES, source);
    CheckError();

    loadSound("rsc/sounds/game_music.wav");
    alSourcef(source[GAMEMUSIC], AL_PITCH, 1.0f);
    alSourcef(source[GAMEMUSIC], AL_GAIN, 2.0f);
    alSourcefv(source[GAMEMUSIC], AL_POSITION, source0Pos);
    alSourcefv(source[GAMEMUSIC], AL_VELOCITY, source0Vel);
    alSourcei(source[GAMEMUSIC], AL_BUFFER, buffer[GAMEMUSIC]);
    alSourcei(source[GAMEMUSIC], AL_LOOPING, AL_TRUE);

    loadSound("rsc/sounds/pickup_collision.wav");
    alSourcef(source[PICKUPCOLLISION], AL_PITCH, 1.0f);
    alSourcef(source[PICKUPCOLLISION], AL_GAIN, 1.0f);
    alSourcefv(source[PICKUPCOLLISION], AL_POSITION, source0Pos);
    alSourcefv(source[PICKUPCOLLISION], AL_VELOCITY, source0Vel);
    alSourcei(source[PICKUPCOLLISION], AL_BUFFER, buffer[PICKUPCOLLISION]);

    loadSound("rsc/sounds/car_idle_loop.wav");
    alSourcef(source[CARIDLE], AL_PITCH, 1.0f);
    alSourcef(source[CARIDLE], AL_GAIN, 0.1f);
    alSourcefv(source[CARIDLE], AL_POSITION, source0Pos);
    alSourcefv(source[CARIDLE], AL_VELOCITY, source0Vel);
    alSourcei(source[CARIDLE], AL_BUFFER, buffer[CARIDLE]);
    alSourcei(source[CARIDLE], AL_LOOPING, AL_TRUE);

    loadSound("rsc/sounds/start_menu_music.wav");
    alSourcef(source[STARTMENUMUSIC], AL_PITCH, 1.0f);
    alSourcef(source[STARTMENUMUSIC], AL_GAIN, 1.0f);
    alSourcefv(source[STARTMENUMUSIC], AL_POSITION, source0Pos);
    alSourcefv(source[STARTMENUMUSIC], AL_VELOCITY, source0Vel);
    alSourcei(source[STARTMENUMUSIC], AL_BUFFER, buffer[STARTMENUMUSIC]);
    alSourcei(source[STARTMENUMUSIC], AL_LOOPING, AL_TRUE);

    loadSound("rsc/sounds/powerstation_collision.wav");
    alSourcef(source[POWERSTATIONCOLLISION], AL_PITCH, 1.0f);
    alSourcef(source[POWERSTATIONCOLLISION], AL_GAIN, 1.0f);
    alSourcefv(source[POWERSTATIONCOLLISION], AL_POSITION, source0Pos);
    alSourcefv(source[POWERSTATIONCOLLISION], AL_VELOCITY, source0Vel);
    alSourcei(source[POWERSTATIONCOLLISION], AL_BUFFER, buffer[POWERSTATIONCOLLISION]);

    return;
}

//////////////////////////////////////////////////////////////////////////////

AudioPlayer::AudioPlayer()
{
    device = alcOpenDevice(nullptr);
    ctx = alcCreateContext(device, NULL);
    alcMakeContextCurrent(ctx);
    curLoaded = 0;

    if (!device)
    {
        std::cerr << "Could not open an audio device.\n";
    }
    else
    {
        std::cout << "Audio deivce found!\n";
    }
    init();
}

//////////////////////////////////////////////////////////////////////////////

AudioPlayer::~AudioPlayer() {
    //alDeleteBuffers(NUM_BUFFERS,source);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(device);
}

//////////////////////////////////////////////////////////////////////////////
//USED FOR TESTING AND INITIAL DESIGN DELETE LATER
void AudioPlayer::playMusic(int number) {
    alSourcef(source[number], AL_PITCH, 1.0f);
    alSourcef(source[number], AL_GAIN, 1.0f);
    alSourcefv(source[number], AL_POSITION, source0Pos);
    alSourcefv(source[number], AL_VELOCITY, source0Vel);
    alSourcei(source[number], AL_BUFFER, buffer[number]);
    alSourcei(source[number], AL_LOOPING, AL_TRUE);
    alSourcePlay(source[number]);
}

//////////////////////////////////////////////////////////////////////////////
//USED FOR TESTING AND INITIAL DESIGN DELETE LATER
void AudioPlayer::playSound(int number) {
    alSourcef(source[number], AL_PITCH, 1.0f);
    alSourcef(source[number], AL_GAIN, 1.0f);
    alSourcefv(source[number], AL_POSITION, source0Pos);
    alSourcefv(source[number], AL_VELOCITY, source0Vel);
    alSourcei(source[number], AL_BUFFER, buffer[number]);
    alSourcePlay(source[number]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playGameMusic() {
    alSourcePlay(source[GAMEMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::stopGameMusic() {
    alSourceStop(source[GAMEMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playPickupCollision() {

    alSourcePlay(source[PICKUPCOLLISION]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playPowerstationCollision() {

    alSourcePlay(source[POWERSTATIONCOLLISION]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playCarIdle() {

    alSourcePlay(source[CARIDLE]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::stopCarIdle() {
    alSourceStop(source[CARIDLE]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playStartMenuMusic() {

    alSourcePlay(source[STARTMENUMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::stopStartMenuMusic() {
    alSourceStop(source[STARTMENUMUSIC]);
}
}	// namespace audio
}   // namespace hyperbright