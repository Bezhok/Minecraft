#pragma once
#include "pch.h"

class Sounds {
public:
	enum class SoundsNames {
		wood1,
		wood4
	};

	std::unordered_map<SoundsNames, sf::SoundBuffer> m_sound_buffers;
	Sounds();
};