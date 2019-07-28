#pragma once
#include "pch.h"
#include "game_constants.h"

namespace World {
	class TerrainGenerator
	{
	private:
		/*std::array<float, BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK> m_height_map;*/
		FastNoise m_noise;
		
	public:
		FastNoise m_biome_noise;

		float get_noise(int x, int y) { return m_noise.GetNoise(static_cast<float>(x), static_cast<float>(y)); };

		float get_noise(float x, float y) { return m_noise.GetNoise(x, y); };

		TerrainGenerator();
		~TerrainGenerator();
	};
}
   