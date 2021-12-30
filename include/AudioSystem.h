/*
* This file is mostly just a wrapper to make it easier
* to use SDL_mixer in a C++-like fashion.
*/
#include <filesystem>
#include <memory>
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>

//SDL_Mixer expects you to use Mix_Music for longer files and Mix_Chunk
//for shorter ones, so I split this into multiple classses.
struct AudioFile {
	virtual void pause() = 0;
	virtual void play() = 0;
	virtual void stop() = 0;
};

class MusicFile : public AudioFile {
private:
	Mix_Music* _music;
public:
	MusicFile(std::filesystem::path filePath);
	~MusicFile();

	void pause() override;
	void play() override;
	void stop() override;
};

class SoundEffect : public AudioFile {
private:
	Mix_Chunk* _sound;
public:
	SoundEffect(std::filesystem::path filePath);
	~SoundEffect();

	void pause() override;
	void play() override;
	void stop() override;
};

class AudioSystem {
private:
	std::vector<std::unique_ptr<AudioFile>> _soundFiles;
public:
	AudioSystem() = default;

	void addFile(std::filesystem::path filePath);
	void playFile(size_t index);
	void removeFile(size_t index);
};