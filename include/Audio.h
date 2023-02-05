/**
 * @file Audio.h
 * Defines a function which gets a sound when given a filename.
 * This allows to load only one Sound object per file.
 */

#ifndef LAB_AUDIO_H
#define LAB_AUDIO_H

#include "Sound.h"
#include <memory>
#include <string>
#include <unordered_map>

/**
 * @brief Class used to manage audio ressources.
 */
class Audio
{
private:
	// mapping between filenames and pointers to corresponding sound objects
	inline static std::unordered_map<std::string, std::unique_ptr<Sound>> _sounds;

public:
	/**
	 * @brief Returns a pointer to a sound.
	 * @param filename The sound file's name (relative to the working directory)
	 */
	static Sound* get(const std::string& filename) {
		if (_sounds.find(filename) == _sounds.end()) {
			_sounds.emplace(filename, std::make_unique<Sound>(filename.c_str()));
		}
		return _sounds[filename].get();
	}
};

#endif
