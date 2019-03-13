#include <SFML/Graphics.hpp>
#include "DebugData.h"


void DebugData::start()
{
	start_time = end_time;

	end_time = timer.getElapsedTime().asMilliseconds();
	update_frequency_time = update_frequency_timer.getElapsedTime().asMilliseconds();
}

void DebugData::count()
{
	if (update_frequency_time >= 200) {
		update_frequency_timer.restart();
		timer.restart();

		frame_time = (end_time - start_time);
		fps = 1000.f / frame_time;
	}
}
