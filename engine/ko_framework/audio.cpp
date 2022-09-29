#include <audio.h>
#include <SFML/Audio.hpp>

#include <Parser.h>

//Functions for tweakbar's interface

/*
void TW_CALL tbSetSoundVolume(const void* data, void* null)
{
	float v = (*(const float*)data);
	External::audio->SetSoundEffectVolume(v);
}
void TW_CALL tbGetSoundVolume(void* data, void* null)
{
	*(float*)data = External::audio->GetSoundEffectVolume();
}

void TW_CALL tbSetMusicVolume(const void* data, void* null)
{
	float v = (*(const float*)data);
	External::audio->SetMusicVolume(v);
}
void TW_CALL tbGetMusicVolume(void* data, void* null)
{
	*(float*)data = External::audio->GetMusicVolume();
}

void TW_CALL tbSetGlobalVolume(const void* data, void* null)
{
	float v = (*(const float*)data);
	External::audio->SetGlobalVolume(v);
}
void TW_CALL tbGetGlobalVolume(void* data, void* null)
{
	*(float*)data = External::audio->GetGlobalVolume();
}


void TW_CALL tbPlaySoundButton(void* null)
{
	External::audio->PlaySound("./audio/"+tbSoundLoadField);
}
void TW_CALL tbPlayMusicButton(void* null)
{
	External::audio->PlayMusic("./audio/"+tbMusicLoadField);
}
void TW_CALL tbStopSoundButton(void* null)
{
	External::audio->StopSoundEffects();
}
void TW_CALL tbStopAllAudioButton(void* null)
{
	External::audio->StopAllAudio();
}
*/
////////////////////////////////////////////////////////////////////////////


AudioManager::AudioManager()
{
	global_volume = 1.0f;
	sfx_volume = 1.0f;
	music_volume = 1.0f;
}

AudioManager::~AudioManager()
{
	Cleanup();
}

	//TwAddVarCB(_t, "Global", TW_TYPE_FLOAT, tbSetGlobalVolume, tbGetGlobalVolume, 0, " group='Volume' label='Global'");
	//TwAddVarCB(_t, "Sound", TW_TYPE_FLOAT, tbSetSoundVolume, tbGetSoundVolume, 0, " group='Volume' label='Sound'");
	//TwAddVarCB(_t, "Music", TW_TYPE_FLOAT, tbSetMusicVolume, tbGetMusicVolume, 0, " group='Volume' label='Music'");

	//TwAddVarRW(_t, "File-Sound", TW_TYPE_STDSTRING, &tbSoundLoadField, " label='Sound File:'");
	//TwAddButton(_t, "LoadSound", tbPlaySoundButton, 0, " label='Play Sound' ");
	//TwAddVarRW(_t, "File-Music", TW_TYPE_STDSTRING, &tbMusicLoadField, " label='Music File:'");
	//TwAddButton(_t, "LoadMusic", tbPlayMusicButton, 0, " label='Play Music' ");

	//TwAddButton(_t, "StopSound", tbStopSoundButton, 0, " label='Stop Sound Effects' ");
	//TwAddButton(_t, "StopAllSound", tbStopAllAudioButton, 0, " label='Stop All Audio' ");


void AudioManager::Init()
{}

void AudioManager::Update(double fps)
{
	//remove sound effects that are finished
	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); )
	{
		if( (*s_iter)->getStatus() == 0 )// && s_iter->getPlayingOffset() == loaded_sounds[  ]->getDuration() )
		{
			delete (*s_iter);
			s_iter = playing_sounds.erase( s_iter );
		}
		else
			++s_iter;
	}
}

void AudioManager::SetSoundEffectVolume(float _sfx_volume)
{
	sfx_volume = _sfx_volume;
	glm::clamp(sfx_volume, 0.0f, 1.0f);
	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); ++s_iter)
	{
		(*s_iter)->setVolume( sfx_volume * global_volume * 100.0f );
	}
}

void AudioManager::SetMusicVolume(float _music_volume)
{
	music_volume = _music_volume;
	glm::clamp(music_volume, 0.0f, 1.0f);
	for(unsigned i = 0; i < loaded_music.size(); ++i)
	{
		loaded_music[i]->setVolume( music_volume * global_volume * 100.0f );
	}
}

void AudioManager::SetGlobalVolume(float volume)
{
	global_volume = volume;
	glm::clamp(global_volume,0.0f,1.0f);

	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); ++s_iter)
	{
		(*s_iter)->setVolume( sfx_volume * global_volume * 100.0f );
	}

	for(unsigned i = 0; i < loaded_music.size(); ++i)
	{
		loaded_music[i]->setVolume( music_volume * global_volume * 100.0f );
	}
}

void AudioManager::StopAllAudio()
{
	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); ++s_iter)
	{
		(*s_iter)->stop();
	}

	for(unsigned i = 0; i < loaded_music.size(); ++i)
	{
		loaded_music[i]->stop();
	}
}

void AudioManager::Cleanup()
{
	//unload sounds
	for(unsigned i = 0; i < loaded_sounds.size(); ++i)
	{
		delete loaded_sounds[i];
	}

	//unload music
	for(unsigned i = 0; i < loaded_music.size(); ++i)
	{
		delete loaded_music[i];
	}

	loaded_sounds.clear();
	sound_map.clear();
	loaded_music.clear();
	music_map.clear();
}

void AudioManager::PauseSoundEffects()
{
	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); ++s_iter)
	{
		(*s_iter)->pause();
	}
}
void AudioManager::StopSoundEffects()
{
	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); ++s_iter)
	{
		(*s_iter)->stop();
	}
}
void AudioManager::PauseMusic(const std::string& filename)
{
	std::map< std::string, unsigned >::iterator finder = music_map.find( filename );
	if( finder == music_map.end() )
		return;

	loaded_music[ finder->second ]->pause();
}
void AudioManager::StopMusic(const std::string& filename)
{
	std::map< std::string, unsigned >::iterator finder = music_map.find( filename );
	if( finder == music_map.end() )
		return;

	loaded_music[ finder->second ]->stop();
}

void AudioManager::ResumeSoundEffects()
{
	std::list< sf::Sound* >::iterator s_iter = playing_sounds.begin();
	for(; s_iter != playing_sounds.end(); ++s_iter)
	{
		(*s_iter)->play();
	}
}

void AudioManager::ResumeMusic(const std::string& filename)
{
	PlayMusic(filename);
}

void AudioManager::PlaySound(const std::string& filename, float volume)
{
#if CODEBLOCKS == 1
    return;
#endif
	glm::clamp(volume,0.0f,1.0f);
	std::map< std::string, unsigned >::iterator finder = sound_map.find( filename );
	if( finder == sound_map.end() )
	{
		if( LoadSound( filename ) )
			finder = sound_map.find( filename );
		else
			return;
	}

	sf::Sound* new_sound = new sf::Sound();
	new_sound->setBuffer( *loaded_sounds[ finder->second ] );
	new_sound->play();
	new_sound->setVolume( sfx_volume * volume * global_volume * 100.0f );
	playing_sounds.push_back( new_sound );
}

void AudioManager::PlayMusic(const std::string& filename, float volume, bool looping)
{
#if CODEBLOCKS == 1
    return;
#endif
	glm::clamp(volume,0.0f,1.0f);
	std::map< std::string, unsigned >::iterator finder = music_map.find( filename );
	if( finder == music_map.end() )
	{
		if( LoadMusic( filename ) )
			finder = music_map.find( filename );
		else
			return;
	}

	if( loaded_music[ finder->second ]->getStatus() == 2 ) //playing
		loaded_music[ finder->second ]->stop();

	loaded_music[ finder->second ]->play();
	loaded_music[ finder->second ]->setVolume( music_volume * volume * global_volume * 100.0f );
	if( looping )
		loaded_music[ finder->second ]->setLoop( true );
}

bool AudioManager::LoadSound(const std::string& filename)
{
	std::map< std::string, unsigned >::iterator finder = sound_map.find( filename );
	if( finder != sound_map.end() )
		return true;

	sf::SoundBuffer* sound = new sf::SoundBuffer();
	if( !sound->loadFromFile( filename ) )
	{
		std::cerr << "Failed to open music file. Make sure the file exists and that the extension is supported. \nSupported extensions are: ogg, wav, flac, aiff, au, raw, paf, svx, nist, voc, ircam, w64, mat4, mat5 pvf, htk, sds, avr, sd2, caf, wve, mpc2k, rf64.\n";

		delete sound;
		return false;
	}

	sound_map[ filename ] = loaded_sounds.size();
	loaded_sounds.push_back( sound );
	return true;
}

bool AudioManager::LoadMusic(const std::string& filename)
{
	std::map< std::string, unsigned >::iterator finder = music_map.find( filename );
	if( finder != music_map.end() )
		return true;

	sf::Music* music = new sf::Music();
	if( !music->openFromFile( filename ) )
	{
		std::cerr << "Failed to open music file. Make sure the file exists and that the extension is supported. \nSupported extensions are: ogg, wav, flac, aiff, au, raw, paf, svx, nist, voc, ircam, w64, mat4, mat5 pvf, htk, sds, avr, sd2, caf, wve, mpc2k, rf64.\n";

		delete music;
		return false;
	}

	music_map[ filename ] = loaded_music.size();
	loaded_music.push_back( music );
	return true;
}

sf::Music* AudioManager::GetMusicObject(const std::string& filename)
{
	std::map< std::string, unsigned >::iterator finder = music_map.find( filename );
	if( finder == music_map.end() )
		return 0;

	return loaded_music[ finder->second ];
}

sf::SoundBuffer* AudioManager::GetSoundBufferObject(const std::string& filename)
{
	std::map< std::string, unsigned >::iterator finder = sound_map.find( filename );
	if( finder == sound_map.end() )
		return 0;

	return loaded_sounds[ finder->second ];
}
