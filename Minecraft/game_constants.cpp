#include "game_constants.h"

/* default window size */
const int WINDOW_WIDTH = 800; //800
const int WINDOW_HEIGTH = 600; //600

const int RENDER_DISTANCE_CHUNKS = 15;
const float RENDER_DISTANCE = BLOCK_SIZE*CHUNK_SIZE*RENDER_DISTANCE_CHUNKS;

const float STANDART_PLAYER_SPEED = 4*BLOCK_SIZE/4.F; // coordinates


int verticies_wasnt_free = 0;