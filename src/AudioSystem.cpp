#include "../include/AudioSystem.h"
#include <iostream> //Probably won't be needed beyond debugging

/*
*MusicFile
*/
MusicFile::MusicFile(std::filesystem::path filePath)
{
	_music = Mix_LoadMUS(filePath.generic_string().c_str());
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
SoundEffect::SoundEffect(std::filesystem::path filePath)
{
	_sound = Mix_LoadWAV(filePath.generic_string().c_str());
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
void AudioSystem::addFile(std::filesystem::path filePath)
{
	if (filePath.extension() == ".wav") {
		_soundFiles.emplace_back(new SoundEffect{ filePath.generic_string().c_str() });
	}
	else {
		_soundFiles.emplace_back(new MusicFile{ filePath.generic_string().c_str() });
	}
}

void AudioSystem::playFile(size_t index)
{
	_soundFiles[index]->play();
}

void AudioSystem::removeFile(size_t index)
{
	_soundFiles.erase(_soundFiles.begin() + index);
}
