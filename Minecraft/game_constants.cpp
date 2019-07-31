#include "game_constants.h"


const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGTH = 600;

const int RENDER_DISTANCE_CHUNKS = 32;
const float RENDER_DISTANCE = COORDS_IN_BLOCK*BLOCKS_IN_CHUNK*RENDER_DISTANCE_CHUNKS;
const float DEFAULT_PLAYER_SPEED = COORDS_IN_BLOCK; //in coordinates

const float COORDS_IN_BLOCK = 1.F;

const int BLOCK_RESOLUTION = 16;

const char PATH2ATLAS[] = "resources\\atlas.png";

int verticies_wasnt_free = 0;