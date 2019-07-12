#include "game_constants.h"


const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGTH = 600;

const int RENDER_DISTANCE_CHUNKS = 32;
const float RENDER_DISTANCE = COORDS_IN_BLOCK*BLOCKS_IN_CHUNK*RENDER_DISTANCE_CHUNKS;
const float DEFAULT_PLAYER_SPEED = 1*COORDS_IN_BLOCK/4.F; //in coordinates

const float COORDS_IN_BLOCK = 1.F;


int verticies_wasnt_free = 0;