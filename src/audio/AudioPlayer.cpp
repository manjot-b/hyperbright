#include "AudioPlayer.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>

#define GAMEMUSIC 0
#define PICKUPCOLLISION 1
#define CARIDLE 2
#define STARTMENUMUSIC 3
#define POWERSTATIONCOLLISION 4
#define MENUSWITCH 5
#define MENUENTER 6
#define USEPOWERUP 7
#define EMP 8
#define SPEED 9
#define TRAPHIT 10
#define ZAP 11
#define LOSSMUSIC 12
#define WINMUSIC 13
#define WALLCOLLISION 14
#define CARCOLLISION1 15
#define CARCOLLISION2 16
#define CARCOLLISION3 17

#define NUM_BUFFERS 18 //NUMBER OF SOUND FILES
#define NUM_SOURCES 18
#define NUM_ENVIRONMENTS 1

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
    alSourcef(source[GAMEMUSIC], AL_GAIN, 1.0f);
    alSourcefv(source[GAMEMUSIC], AL_POSITION, source0Pos);
    alSourcefv(source[GAMEMUSIC], AL_VELOCITY, source0Vel);
    alSourcei(source[GAMEMUSIC], AL_BUFFER, buffer[GAMEMUSIC]);
    alSourcei(source[GAMEMUSIC], AL_LOOPING, AL_TRUE);

    loadSound("rsc/sounds/pickup_collision.wav");
    alSourcef(source[PICKUPCOLLISION], AL_PITCH, 1.0f);
    alSourcef(source[PICKUPCOLLISION], AL_GAIN, 0.20f);
    alSourcefv(source[PICKUPCOLLISION], AL_POSITION, source0Pos);
    alSourcefv(source[PICKUPCOLLISION], AL_VELOCITY, source0Vel);
    alSourcei(source[PICKUPCOLLISION], AL_BUFFER, buffer[PICKUPCOLLISION]);

    loadSound("rsc/sounds/car_idle_loop.wav");
    alSourcef(source[CARIDLE], AL_PITCH, 1.0f);
    alSourcef(source[CARIDLE], AL_GAIN, 0.32f);
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

    loadSound("rsc/sounds/menu_switch.wav");
    alSourcef(source[MENUSWITCH], AL_PITCH, 1.0f);
    alSourcef(source[MENUSWITCH], AL_GAIN, 1.0f);
    alSourcefv(source[MENUSWITCH], AL_POSITION, source0Pos);
    alSourcefv(source[MENUSWITCH], AL_VELOCITY, source0Vel);
    alSourcei(source[MENUSWITCH], AL_BUFFER, buffer[MENUSWITCH]);

    loadSound("rsc/sounds/menu_enter.wav");
    alSourcef(source[MENUENTER], AL_PITCH, 1.0f);
    alSourcef(source[MENUENTER], AL_GAIN, 1.0f);
    alSourcefv(source[MENUENTER], AL_POSITION, source0Pos);
    alSourcefv(source[MENUENTER], AL_VELOCITY, source0Vel);
    alSourcei(source[MENUENTER], AL_BUFFER, buffer[MENUENTER]);

    loadSound("rsc/sounds/use_powerup.wav");
    alSourcef(source[USEPOWERUP], AL_PITCH, 1.0f);
    alSourcef(source[USEPOWERUP], AL_GAIN, 1.0f);
    alSourcefv(source[USEPOWERUP], AL_POSITION, source0Pos);
    alSourcefv(source[USEPOWERUP], AL_VELOCITY, source0Vel);
    alSourcei(source[USEPOWERUP], AL_BUFFER, buffer[USEPOWERUP]);

    loadSound("rsc/sounds/emp.wav");
    alSourcef(source[EMP], AL_PITCH, 1.0f);
    alSourcef(source[EMP], AL_GAIN, 1.0f);
    alSourcefv(source[EMP], AL_POSITION, source0Pos);
    alSourcefv(source[EMP], AL_VELOCITY, source0Vel);
    alSourcei(source[EMP], AL_BUFFER, buffer[EMP]);

    loadSound("rsc/sounds/speed.wav");
    alSourcef(source[SPEED], AL_PITCH, 1.0f);
    alSourcef(source[SPEED], AL_GAIN, 1.0f);
    alSourcefv(source[SPEED], AL_POSITION, source0Pos);
    alSourcefv(source[SPEED], AL_VELOCITY, source0Vel);
    alSourcei(source[SPEED], AL_BUFFER, buffer[SPEED]);

    loadSound("rsc/sounds/trap_hit.wav");
    alSourcef(source[TRAPHIT], AL_PITCH, 1.0f);
    alSourcef(source[TRAPHIT], AL_GAIN, 1.0f);
    alSourcefv(source[TRAPHIT], AL_POSITION, source0Pos);
    alSourcefv(source[TRAPHIT], AL_VELOCITY, source0Vel);
    alSourcei(source[TRAPHIT], AL_BUFFER, buffer[TRAPHIT]);

    loadSound("rsc/sounds/zap.wav");
    alSourcef(source[ZAP], AL_PITCH, 1.0f);
    alSourcef(source[ZAP], AL_GAIN, 1.0f);
    alSourcefv(source[ZAP], AL_POSITION, source0Pos);
    alSourcefv(source[ZAP], AL_VELOCITY, source0Vel);
    alSourcei(source[ZAP], AL_BUFFER, buffer[ZAP]);

    loadSound("rsc/sounds/loss.wav");
    alSourcef(source[LOSSMUSIC], AL_PITCH, 1.0f);
    alSourcef(source[LOSSMUSIC], AL_GAIN, 1.0f);
    alSourcefv(source[LOSSMUSIC], AL_POSITION, source0Pos);
    alSourcefv(source[LOSSMUSIC], AL_VELOCITY, source0Vel);
    alSourcei(source[LOSSMUSIC], AL_BUFFER, buffer[LOSSMUSIC]);
    alSourcei(source[LOSSMUSIC], AL_LOOPING, AL_TRUE);

    loadSound("rsc/sounds/win.wav");
    alSourcef(source[WINMUSIC], AL_PITCH, 1.0f);
    alSourcef(source[WINMUSIC], AL_GAIN, 1.0f);
    alSourcefv(source[WINMUSIC], AL_POSITION, source0Pos);
    alSourcefv(source[WINMUSIC], AL_VELOCITY, source0Vel);
    alSourcei(source[WINMUSIC], AL_BUFFER, buffer[WINMUSIC]);
    alSourcei(source[WINMUSIC], AL_LOOPING, AL_TRUE);

    loadSound("rsc/sounds/wall_collision.wav");
    alSourcef(source[WALLCOLLISION], AL_PITCH, 1.0f);
    alSourcef(source[WALLCOLLISION], AL_GAIN, 1.0f);
    alSourcefv(source[WALLCOLLISION], AL_POSITION, source0Pos);
    alSourcefv(source[WALLCOLLISION], AL_VELOCITY, source0Vel);
    alSourcei(source[WALLCOLLISION], AL_BUFFER, buffer[WALLCOLLISION]);

    loadSound("rsc/sounds/car_collision1.wav");
    alSourcef(source[CARCOLLISION1], AL_PITCH, 1.0f);
    alSourcef(source[CARCOLLISION1], AL_GAIN, 0.6f);
    alSourcefv(source[CARCOLLISION1], AL_POSITION, source0Pos);
    alSourcefv(source[CARCOLLISION1], AL_VELOCITY, source0Vel);
    alSourcei(source[CARCOLLISION1], AL_BUFFER, buffer[CARCOLLISION1]);

    loadSound("rsc/sounds/car_collision2.wav");
    alSourcef(source[CARCOLLISION2], AL_PITCH, 1.0f);
    alSourcef(source[CARCOLLISION2], AL_GAIN, 0.6f);
    alSourcefv(source[CARCOLLISION2], AL_POSITION, source0Pos);
    alSourcefv(source[CARCOLLISION2], AL_VELOCITY, source0Vel);
    alSourcei(source[CARCOLLISION2], AL_BUFFER, buffer[CARCOLLISION2]);

    loadSound("rsc/sounds/car_collision3.wav");
    alSourcef(source[CARCOLLISION3], AL_PITCH, 1.0f);
    alSourcef(source[CARCOLLISION3], AL_GAIN, 0.6f);
    alSourcefv(source[CARCOLLISION3], AL_POSITION, source0Pos);
    alSourcefv(source[CARCOLLISION3], AL_VELOCITY, source0Vel);
    alSourcei(source[CARCOLLISION3], AL_BUFFER, buffer[CARCOLLISION3]);


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

void AudioPlayer::playMenuSwitchSound() {

    alSourcePlay(source[MENUSWITCH]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playMenuEnterSound() {

    alSourcePlay(source[MENUENTER]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playUsePowerupSound() {

    alSourcePlay(source[USEPOWERUP]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playZapSound() {

    alSourcePlay(source[ZAP]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playEmpSound() {

    alSourcePlay(source[EMP]);
}

//////////////////////////////////////////////////////////////////////////////


void AudioPlayer::playTrapHitSound() {
    alSourcePlay(source[TRAPHIT]);

}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::startWinMusic() {

    alSourcePlay(source[WINMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::stopWinMusic() {

    alSourceStop(source[WINMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::startLossMusic() {

    alSourcePlay(source[LOSSMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::stopLossMusic() {

    alSourceStop(source[LOSSMUSIC]);
}

//////////////////////////////////////////////////////////////////////////////


void AudioPlayer::playWallCollisionSound() {

    alSourcePlay(source[WALLCOLLISION]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playCarCollisionSound() {
    int selector = rand() % 3;
    if (selector == 0) {
        alSourcePlay(source[CARCOLLISION1]);
    }
    else if (selector == 1) {
        alSourcePlay(source[CARCOLLISION2]);
    }
    else {
        alSourcePlay(source[CARCOLLISION3]);
    }
}


void AudioPlayer::playSpeedSound() {

    alSourcePlay(source[SPEED]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::adjustCarIdlePitch(float speed) {

    if (speed < 0) {
        speed = 0.f;
    }
    alSourcef(source[CARIDLE], AL_PITCH, 1.0f + (speed/30.f));
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::playCarIdle() {

    alSourcePlay(source[CARIDLE]);
}

//////////////////////////////////////////////////////////////////////////////

void AudioPlayer::stopCarIdle() {
    alSourceStop(source[CARIDLE]);
}

void AudioPlayer::setMusicVolume(float volume) {
    if (volume < 0.f)volume = 0.f;

    alSourcef(source[GAMEMUSIC], AL_GAIN, volume);
    alSourcef(source[STARTMENUMUSIC], AL_GAIN, volume);
    alSourcef(source[WINMUSIC], AL_GAIN, volume);
    alSourcef(source[LOSSMUSIC], AL_GAIN, volume);
    
    /*
    // added this to turn down all volume
    alSourcef(source[PICKUPCOLLISION], AL_GAIN, volume);
    alSourcef(source[CARIDLE], AL_GAIN, volume);
    alSourcef(source[POWERSTATIONCOLLISION], AL_GAIN, volume);
    alSourcef(source[USEPOWERUP], AL_GAIN, volume);
    alSourcef(source[EMP], AL_GAIN, volume);
    alSourcef(source[MENUSWITCH], AL_GAIN, volume);
    alSourcef(source[MENUENTER], AL_GAIN, volume);
    alSourcef(source[SPEED], AL_GAIN, volume);
    alSourcef(source[TRAPHIT], AL_GAIN, volume);*/
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