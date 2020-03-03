#include "game_constants.h"


const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGTH = 600;

const int SHADOW_SIZE = 3 * 1024;

const int RENDER_DISTANCE_IN_CHUNKS = 32;
const float RENDER_DISTANCE = BLOCKS_IN_CHUNK * RENDER_DISTANCE_IN_CHUNKS;
const float DEFAULT_PLAYER_SPEED = 1.3 * 4.317f; //in coordinates

const int BLOCK_RESOLUTION = 16;
const char PATH2ATLAS[] = "resources\\natlas.png";

int vertices_wasnt_free = 0;