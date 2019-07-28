#include "TerrainGenerator.h"

using namespace World;


TerrainGenerator::TerrainGenerator()
{
	m_noise.SetNoiseType(FastNoise::Perlin);
	m_noise.SetFrequency(0.006f);
	m_noise.SetFractalOctaves(5);


	m_biome_noise.SetNoiseType(FastNoise::ValueFractal);
	m_biome_noise.SetFrequency(0.002f); //0.0009
	m_biome_noise.SetFractalOctaves(5);
	//m_biome_noise.SetFractalType(FastNoise::Billow);
	//m_noise.SetGradientPerturbAmp(5);
	//m_noise.GradientPerturbFractal();
}


TerrainGenerator::~TerrainGenerator()
{
}
