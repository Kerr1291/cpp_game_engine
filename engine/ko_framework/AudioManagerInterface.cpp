#include <AudioManagerInterface.h>
#include <audio.h>

AudioManagerInterface::AudioManagerInterface(AudioManager& audio_manager)
:_audio_manager(audio_manager)
{}

void AudioManagerInterface::SetSoundEffectVolume(float sfx_volume)
{
	_audio_manager.SetSoundEffectVolume(sfx_volume);
}
void AudioManagerInterface::SetMusicVolume(float music_volume)
{
	_audio_manager.SetMusicVolume(music_volume);
}
void AudioManagerInterface::SetGlobalVolume(float volume)
{
	_audio_manager.SetGlobalVolume(volume);
}

float AudioManagerInterface::GetSoundEffectVolume() const
{
	return _audio_manager.GetSoundEffectVolume();
}

const float& AudioManagerInterface::GetSoundEffectVolumeRef() const
{
	return _audio_manager.GetSoundEffectVolumeRef();
}

float AudioManagerInterface::GetMusicVolume() const
{
	return _audio_manager.GetMusicVolume();
}
const float& AudioManagerInterface::GetMusicVolumeRef() const
{
	return _audio_manager.GetMusicVolumeRef();
}
float AudioManagerInterface::GetGlobalVolume() const
{
	return _audio_manager.GetGlobalVolume();
}
const float& AudioManagerInterface::GetGlobalVolumeRef() const
{
	return _audio_manager.GetGlobalVolumeRef();
}

void AudioManagerInterface::StopAllAudio()
{
	_audio_manager.StopAllAudio();
}

void AudioManagerInterface::PauseSoundEffects()
{
	_audio_manager.PauseSoundEffects();
}
void AudioManagerInterface::StopSoundEffects()
{
	_audio_manager.StopSoundEffects();
}
void AudioManagerInterface::PauseMusic(const std::string& filename)
{
	_audio_manager.PauseMusic(filename);
}
void AudioManagerInterface::StopMusic(const std::string& filename)
{
	_audio_manager.StopMusic(filename);
}

void AudioManagerInterface::ResumeSoundEffects()
{
	_audio_manager.ResumeSoundEffects();
}
void AudioManagerInterface::ResumeMusic(const std::string& filename)
{
	_audio_manager.ResumeMusic(filename);
}

void AudioManagerInterface::PlaySound(const std::string& filename, float volume)
{
	_audio_manager.PlaySound(filename,volume);
}
void AudioManagerInterface::PlayMusic(const std::string& filename, float volume, bool looping)
{
	_audio_manager.PlayMusic(filename,volume,looping);
}

bool AudioManagerInterface::LoadSound(const std::string& filename)
{
	return _audio_manager.LoadSound(filename);
}
bool AudioManagerInterface::LoadMusic(const std::string& filename)
{
	return _audio_manager.LoadMusic(filename);
}