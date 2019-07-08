#include "game_constants.h"

/* default window size */
const int WINDOW_WIDTH = 800; //800
const int WINDOW_HEIGTH = 600; //600

const int RENDER_DISTANCE_CHUNKS = 16;
const float RENDER_DISTANCE = COORDS_IN_BLOCK*BLOCKS_IN_CHUNK*RENDER_DISTANCE_CHUNKS;

const float DEFAULT_PLAYER_SPEED = 1*COORDS_IN_BLOCK/4.F; // coordinates


int verticies_wasnt_free = 0;