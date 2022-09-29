#ifndef __AUDIO_MANAGER_INTERFACE_H
#define __AUDIO_MANAGER_INTERFACE_H

#include <string>
class AudioManager;

class AudioManagerInterface
{
		AudioManager& _audio_manager;

	public:

		AudioManagerInterface(AudioManager& audio_manager);
		
		void SetSoundEffectVolume(float sfx_volume);
		void SetMusicVolume(float music_volume);
		void SetGlobalVolume(float volume);

		float GetSoundEffectVolume() const;
		const float& GetSoundEffectVolumeRef() const;
		float GetMusicVolume() const;
		const float& GetMusicVolumeRef() const;
		float GetGlobalVolume() const;
		const float& GetGlobalVolumeRef() const;

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
};


#endif