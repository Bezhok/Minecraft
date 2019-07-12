#include "TerrainGenerator.h"

using namespace World;


TerrainGenerator::TerrainGenerator()
{
	m_noise.SetNoiseType(FastNoise::PerlinFractal);
	m_noise.SetFrequency(0.006f);
	m_noise.SetFractalOctaves(5);
}


TerrainGenerator::~TerrainGenerator()
{
}
