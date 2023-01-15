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
	inline static std::unordered_map<std::string, std::shared_ptr<Sound>> _sounds;

public:
	/**
	 * @brief Returns a pointer to a sound.
	 * @param filename The sound file's name (relative to the working directory)
	 */
	static Sound* get(const std::string& filename) {
		if (_sounds.find(filename) == _sounds.end()) {
			_sounds.emplace(filename, std::make_shared<Sound>(filename.c_str()));
		}
		return _sounds[filename].get();
	}
};

#endif
