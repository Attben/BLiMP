/*
* This file is mostly just a wrapper to make it easier
* to use SDL_mixer in a C++-like fashion.
*/
#include <SDL.h>
#include <SDL_mixer.h>
#include <string_view>
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
	MusicFile(std::string_view path);
	~MusicFile();

	void pause() override;
	void play() override;
	void stop() override;
};

class SoundEffect : public AudioFile {
private:
	Mix_Chunk* _sound;
public:
	SoundEffect(std::string_view path);
	~SoundEffect();

	void pause() override;
	void play() override;
	void stop() override;
};

class AudioSystem {
private:
	std::vector<AudioFile> _soundFiles;
public:
	AudioSystem();
};