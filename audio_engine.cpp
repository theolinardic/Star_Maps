#include <audio_engine.h>

void play_sound()
{
	irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
	SoundEngine->play2D("assets/sounds/test.wav", true);
	//	SoundEngine->drop();
}