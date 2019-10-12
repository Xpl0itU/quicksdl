//---------------------------------------------------------------------//
// AssetManager.cpp                                                    //
// Singleton                                                           //
// Used to load all assets used in the game,                           //
// Caches all loaded assets so that nothing is loaded more than once   //
//---------------------------------------------------------------------//
#include "AssetManager.h"
//-----------------------------------------------------------------
// QuickSDL
//-----------------------------------------------------------------

#ifdef __WIIU__
#include <coreinit/memdefaultheap.h>
#ifdef __cplusplus
extern "C" {
#endif
int OSGetSharedData(int what, int flags, uint32_t **addr, uint32_t *size);
#ifdef __cplusplus
}
#endif
#endif

namespace QuickSDL {

	//Initializing to NULL
	AssetManager* AssetManager::sInstance = NULL;

	AssetManager* AssetManager::Instance() {

		//Create a new instance if no instance was created before
		if (sInstance == NULL)
			sInstance = new AssetManager();

		return sInstance;
	}

	void AssetManager::Release() {

		delete sInstance;
		sInstance = NULL;
	}

	AssetManager::AssetManager() {

	}

	AssetManager::~AssetManager() {

		//Freeing all loaded Textures
		for (auto tex : mTextures) {

			if (tex.second != NULL) {

				SDL_DestroyTexture(tex.second);
			}
		}

		mTextures.clear();

		//Freeing all rendered text
		for (auto text : mText) {

			if (text.second != NULL) {

				SDL_DestroyTexture(text.second);
			}
		}

		mText.clear();

		//Freeing all loaded fonts
		for (auto font : mFonts) {

			if (font.second != NULL) {

				TTF_CloseFont(font.second);
			}
		}

		mFonts.clear();

		//Freeing all loaded music
		for (auto music : mMusic) {

			if (music.second != NULL) {

				Mix_FreeMusic(music.second);
			}
		}

		mMusic.clear();

		//Freeing all loaded sound effects
		for (auto sfx : mSFX) {

			if (sfx.second != NULL) {

				Mix_FreeChunk(sfx.second);
			}
		}

		mSFX.clear();
	}

	SDL_Texture* AssetManager::GetTexture(std::string filename) {
		//Get the full path of the file
#ifdef __WIIU__
		std::string fullPath = ROMFS;
#else
		std::string fullPath = SDL_GetBasePath();
#endif
		fullPath.append("Assets/" + filename);

		//If the file has not been already loaded, load it and add it to the mTextures map
		if (mTextures[fullPath] == nullptr)
			mTextures[fullPath] = Graphics::Instance()->LoadTexture(fullPath);

		//returning the cached file from the map
		return mTextures[fullPath];
	}

	TTF_Font* AssetManager::GetFont(std::string filename, int size) {

		//Get the full path of the font
#ifdef __WIIU__
		std::string fullPath = ROMFS;
#else
		std::string fullPath = SDL_GetBasePath();
#endif
		fullPath.append("Assets/" + filename);

		//The key takes into account the size of the font aswell since the same font can be opened with different sizes
		std::string key = fullPath + (char)size;

		//If the font has not been already loaded, load it and add it to the mFonts map
		if (mFonts[key] == nullptr) {

			mFonts[key] = TTF_OpenFont(fullPath.c_str(), size);
			//Error handling for opening the font
			if (mFonts[key] == nullptr)
				printf("Font Loading Error: Font-%s Error-%s", filename.c_str(), TTF_GetError());
		}

		//returning the cached font from the map
		return mFonts[key];
	}

	SDL_Texture* AssetManager::GetText(std::string text, std::string filename, int size, SDL_Color color) {

		//Get the font from the font cache
		TTF_Font* font = GetFont(filename, size);

		//The key takes into account the font, text, size, and color to differentiate text textures
		std::string key = text + filename + (char)size + (char)color.r + (char)color.b + (char)color.g;

		//If the same text has not been rendered before, render it and add it to the mText map
		if (mText[key] == nullptr)
			mText[key] = Graphics::Instance()->CreateTextTexture(font, text, color);

		//returning the cached texture containing the text
		return mText[key];
	}

#ifdef __WIIU__
	SDL_Texture* AssetManager::GetWiiUText(std::string text, int size, SDL_Color color) {

		/* Get the address to the shared font */
		uint32_t *font_addr;
		uint32_t  font_size;
		OSGetSharedData(2, 0, &font_addr, &font_size);
		
		/* Copy the font to RW memory (because shared data is RO) */
		void *font_rw = MEMAllocFromDefaultHeap(font_size);
		memcpy(font_rw, font_addr, font_size);

		TTF_Font* font = TTF_OpenFontRW(SDL_RWFromMem(font_rw, font_size), 1, size);

		//The key takes into account the font, text, size, and color to differentiate text textures
		std::string key = text + "wiiu_system" + (char)size + (char)color.r + (char)color.b + (char)color.g;

		//If the same text has not been rendered before, render it and add it to the mText map
		if (mText[key] == nullptr)
			mText[key] = Graphics::Instance()->CreateTextTexture(font, text, color);

		//returning the cached texture containing the text
		return mText[key];
	}
#endif

	Mix_Music* AssetManager::GetMusic(std::string filename) {

		//Get the full path of the WAV file
#ifdef __WIIU__
		std::string fullPath = ROMFS;
#else
		std::string fullPath = SDL_GetBasePath();
#endif
		fullPath.append("Assets/" + filename);

		//If the file has not been already loaded, load it and add it to the mMusic map
		if (mMusic[fullPath] == nullptr) {

			mMusic[fullPath] = Mix_LoadMUS(fullPath.c_str());
			//Error handling for file loading
			if (mMusic[fullPath] == NULL)
				printf("Music Loading Error: File-%s Error-%s", filename.c_str(), Mix_GetError());
		}

		//returning the cached file from the map
		return mMusic[fullPath];
	}

	Mix_Chunk* AssetManager::GetSFX(std::string filename) {

		//Get the full path of the WAV file
#ifdef __WIIU__
		std::string fullPath = ROMFS;
#else
		std::string fullPath = SDL_GetBasePath();
#endif
		fullPath.append("Assets/" + filename);

		//If the file has not been already loaded, load it and add it to the mSFX map
		if (mSFX[fullPath] == nullptr) {

			mSFX[fullPath] = Mix_LoadWAV(fullPath.c_str());
			//Error handling for file loading
			if (mSFX[fullPath] == NULL)
				printf("SFX Loading Error: File-%s Error-%s", filename.c_str(), Mix_GetError());
		}

		//returning the cached file from the map
		return mSFX[fullPath];
	}
}