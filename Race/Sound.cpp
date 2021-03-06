
#include "Sound.h"
#include "SDL.h"
#include "FileUtil.h"
SoundChunk::SoundChunk(Mix_Chunk* c)
{
	mChunk = c;
}

SoundChunk::SoundChunk(std::string path)
{
	mCurrentChanel = -1;
	if((mChunk = Mix_LoadWAV(path.c_str())) == NULL)
	{
		// Should probably throw here ...
		int x = 0;
		x++;
	}
}

SoundChunk::~SoundChunk()
{
	Mix_FreeChunk(mChunk);
}

void SoundChunk::play()
{
	mCurrentChanel = Mix_PlayChannel(-1, mChunk, 0);
}


void SoundChunk::stop()
{
	if (mCurrentChanel >= 0)
	{
		Mix_HaltChannel(mCurrentChanel);
		mCurrentChanel = -1;

	}

}

void SoundChunk::fadeIn(int ms, bool repeat)
{
	// Don't start playing if we are already playing this chanel ...
	if (mCurrentChanel < 0)
	{
	if (repeat)
	{
		 mCurrentChanel = Mix_FadeInChannel(-1,mChunk,-1, ms);
	}
	else
	{
		 mCurrentChanel = Mix_FadeInChannel(-1,mChunk, 0, ms);
	}
	}
}

void SoundChunk::fadeOut(int ms)
{
	if (mCurrentChanel >= 0)
	{
		Mix_FadeOutChannel(mCurrentChanel, ms);
		mCurrentChanel = -1;
	}
}


SoundBank* SoundBank::instance =NULL;


void SoundBank::setVolume(int newVol)
{
	if (mSoundEnabled && mSoundAvailable)
	{
		if (newVol > 128)
		{
			newVol = 128;
		}
		if (newVol < 0)
		{
			newVol = 0;
		}
		Mix_Volume(-1, newVol);
		mVolume = newVol;
	}

}

SoundBank::SoundBank() : mChunks()
{
	mHasBeenSetup= false;
	mSoundEnabled = true;
	mSoundIndex = 0;
	mNumSounds = 1;
	mVolume = 100;
	mSoundAvailable = true;
}

SoundBank::~SoundBank()
{
	this->free();
}


void SoundBank::stopAllSounds()
{
	//for(unsigned int i = 0; i < mChunks.size(); i++)
	//{
	//	for (std::map<std::string, SoundChunk*>::iterator it = mChunks[i].begin(); it != mChunks[i].end(); it++)
	//	{
	//		(*it).second->fadeOut(0);
	//	}
	//}
	Mix_HaltGroup(-1);
}
void SoundBank::setup()
{
	SDL_Init(SDL_INIT_AUDIO);



	// load support for the OGG and MOD sample/music formats
	int flags=MIX_INIT_OGG|MIX_INIT_MP3;
	int initted=Mix_Init(flags);
	if(initted != flags) {
		std::string error = Mix_GetError();
		int x = 0;
		x = x +1;
	}


	mHasBeenSetup = true;
	int error = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	if (error != 0)
	{
		mSoundEnabled = false;
		mSoundAvailable = false;
		return;
	}
	// TODO:  The following should all be read in from a file in Content.
	///       not hardcorded.

	mNumSounds = 2;

	std::map<std::string, SoundChunk*> nextChunk;

	for (int i = 0; i < mNumSounds; i++)
	{
		mChunks.push_back(nextChunk);
	}


	// openFile("fanfare10.wav", "clearAchievement", 0);



	openFile("bell.ogg", "clearGate", 0);
	openFile("Laser.ogg", "laser", 0);
	openFile("SmallExplode.ogg", "explode1", 0);
	openFile("Fire.ogg", "fire", 0);


	Mix_Volume(-1, mVolume);

}


void SoundBank::shutdown()
{
	delete instance;
	Mix_CloseAudio();
	SDL_Quit();
	instance = NULL;
}
SoundBank* SoundBank::getInstance()
{
	if (SoundBank::instance == NULL)
	{
		instance = new SoundBank();
	}
	return SoundBank::instance;
}

void SoundBank::free()
{
	for (unsigned int i = 0; i < mChunks.size(); i++)
	{
		std::map<std::string, SoundChunk*>::iterator iter = mChunks[i].begin();
		while(iter != mChunks[i].end())
		{
			delete(iter->second);
			iter++;
		}
		mChunks[i].clear();
	}
	mChunks.clear();
}

void SoundBank::play(std::string id)
{
	if(mSoundEnabled && mSoundAvailable)
	{
		SoundChunk *c = mChunks[mSoundIndex][id];
		if (c != NULL)
		{
			c->play();
		}
	}
}

void SoundBank::fadeIn(std::string id, int ms, bool repeat)
{
	if (mSoundEnabled && mSoundAvailable)
	{
		mChunks[mSoundIndex][id]->fadeIn(ms, repeat);
	}
}

void SoundBank::fadeOut(std::string id, int ms)
{
	if (mSoundEnabled && mSoundAvailable)
	{
		mChunks[mSoundIndex][id]->fadeOut(ms);
	}
}

void SoundBank::stop(std::string id)
{
	if (mSoundEnabled && mSoundAvailable)
	{
		mChunks[mSoundIndex][id]->stop();
	}
}


void SoundBank::addSound(SoundChunk* s, std::string id, int index)
{
	mChunks[index][id] = s;
}

void SoundBank::removeSound(std::string id){
	//  TODO:: Do removechunks->erase(id);
}

void SoundBank::openFile(std::string path, std::string id, int index)
{
	
	std::string foundPath = FileUtil::getFullPath(path);

	this->addSound(new SoundChunk(foundPath), id, index);
}


int SoundBank::fadeInManual(std::string play, int ms, bool repeat)
{
	int channel = -1;
	if (repeat)
	{

		channel =  Mix_FadeInChannel(-1,mChunks[mSoundIndex][play]->mChunk,-1, ms);
	}
	else
	{
		channel =  Mix_FadeInChannel(-1,mChunks[mSoundIndex][play]->mChunk,0, ms);
	}
	return channel;

}
	void SoundBank::fadeOutManual(int channel, int ms)
	{
				Mix_FadeOutChannel(channel, ms);

	}
	void SoundBank::stopManual(int channel)
	{
		Mix_HaltChannel(channel);

	}
