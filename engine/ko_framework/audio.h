#ifndef AUDIO_H
#define AUDIO_H

#include <globals.h>

namespace sf
{
	//sfml's audio storage types
	class SoundBuffer;
	class Music;
	class Sound;
}

class AudioManager
{
	friend class AudioManagerInterface;
	friend class GameSystemInterface;
	friend class SystemInterface;
	friend class Editor;

	public:

	AudioManager();
	~AudioManager();

	private:

	void Init();

	void Update(double fps);

	void Cleanup();

	void SetSoundEffectVolume(float sfx_volume);
	void SetMusicVolume(float music_volume);
	void SetGlobalVolume(float volume);

	float GetSoundEffectVolume() const { return sfx_volume; }
	const float& GetSoundEffectVolumeRef() const { return sfx_volume; }
	float GetMusicVolume() const { return music_volume; }
	const float& GetMusicVolumeRef() const { return music_volume; }
	float GetGlobalVolume() const { return global_volume; }
	const float& GetGlobalVolumeRef() const { return global_volume; }

	void StopAllAudio();

	void PauseSoundEffects();
	void StopSoundEffects();
	void PauseMusic(const std::string& filename);
	void StopMusic(const std::string& filename);

	void ResumeSoundEffects();
	void ResumeMusic(const std::string& filename);

	void PlaySound(const std::string& filename, float volume = 1.0f);
	void PlayMusic(const std::string& filename, float volume = 1.0f, bool looping = 1);
	
	bool LoadSound(const std::string& filename);
	bool LoadMusic(const std::string& filename);

	sf::Music* GetMusicObject(const std::string& filename);
	sf::SoundBuffer* GetSoundBufferObject(const std::string& filename);

	private:

		float global_volume;
		float sfx_volume;
		float music_volume;

		std::list< sf::Sound* > playing_sounds;

		std::map< std::string, unsigned >   sound_map;
		std::map< std::string, unsigned >   music_map;

		std::vector< sf::SoundBuffer* > loaded_sounds;
		std::vector< sf::Music* >		loaded_music;
};


#endif