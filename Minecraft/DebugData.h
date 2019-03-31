#pragma once
#include "pch.h"

class DebugData
{
private:
	int start_time = 0;
	int end_time = 0;
	int update_frequency_time;
	sf::Clock update_frequency_timer;
	sf::Clock timer;

public:
	float fps = 0;
	float frame_time = 0;

	void start();

	void count();
};