#pragma once

#include "pch.h"

static const float PI = std::atanf(1) * 4.f;

namespace glm {
	//using vec3i = sf::Vector3i;
	//using vec2i = sf::Vector2i;

	using vec3i = glm::vec<3, int>;
	using vec2i = glm::vec<2, int>;
}

namespace std
{
	template<>
	struct hash<sf::Vector3i>
	{
		size_t operator()(const sf::Vector3i& vect) const noexcept
		{
			std::hash<decltype(vect.x)> hasher;

			auto hash1 = hasher(vect.x);
			auto hash2 = hasher(vect.y);
			auto hash3 = hasher(vect.z);

			return std::hash<decltype(vect.x)>{}((hash1 ^ (hash2 << hash3) ^ hash3));
		}
	};
}

namespace std
{
	template<>
	struct hash<sf::Vector2i>
	{
		size_t operator()(const sf::Vector2i& vect) const noexcept
		{
			std::hash<decltype(vect.x)> hasher;

			auto hash1 = hasher(vect.x);
			auto hash2 = hasher(vect.y);

			return std::hash<decltype(vect.x)>{}((hash1 ^ hash2) >> 2);
		}
	};
}

