#include "Sounds.h"
#include "pch.h"

Sounds::Sounds()
{
	m_sound_buffers[SoundsNames::wood1].loadFromFile("resources//audio//wood1.ogg");
	m_sound_buffers[SoundsNames::wood4].loadFromFile("resources//audio//wood4.ogg");
}