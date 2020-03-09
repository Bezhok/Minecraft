#pragma once

#include "pch.h"

class DebugData {
public:
    float get_fps() { return fps; };

    float get_frame_time() { return frame_time; };

    /* counting delta time */
    void start();

    /* update data if need */
    void count();

private:
    float fps = 0;
    float frame_time = 0;
    int start_time = 0;
    int end_time = 0;
    int update_frequency_time = 0;
    sf::Clock update_frequency_timer;
    sf::Clock timer;
};
