#include "../include/AudioSystem.h"
#include <iostream> //Probably won't be needed beyond debugging

/*
*MusicFile
*/
MusicFile::MusicFile(std::string_view path)
{
	_music = Mix_LoadMUS(path.data());
	if (_music == nullptr) {
		std::cout << "Failed to load music. " << Mix_GetError();
	}
}

MusicFile::~MusicFile()
{
	Mix_FreeMusic(_music);
	_music = nullptr;
}

void MusicFile::pause() {
	//NYI
}

void MusicFile::play() {
	Mix_PlayMusic(_music, 0);
}

void MusicFile::stop() {
	//NYI
}

/*
*SoundEffect
*/
SoundEffect::SoundEffect(std::string_view path)
{
	_sound = Mix_LoadWAV(path.data());
	if (_sound == nullptr) {
		std::cout << "Failed to load sound. " << Mix_GetError();
	}
}

SoundEffect::~SoundEffect()
{
	Mix_FreeChunk(_sound);
	_sound = nullptr;
}

void SoundEffect::pause() {
	//NYI
}

void SoundEffect::play() {
	Mix_PlayChannel(-1, _sound, 0);
}

void SoundEffect::stop() {
	//NYI
}

/*
*AudioSystem
*/