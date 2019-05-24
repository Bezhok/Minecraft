#include "pch.h"
#include "Maths.h"


size_t calculate_3D_hash(int x, int y, int z)
{
	////using std::string;
	std::string coord_str = std::to_string(x) + std::string(" ") + std::to_string(y) + std::string(" ") + std::to_string(z);
	std::hash<std::string> hash_fn;

	return hash_fn(coord_str);
	//return (x + y * 16 + z * 80 * y);
}

size_t calculate_2D_hash(int x, int y)
{
	std::string coord_str = std::to_string(x) + std::string(" ") + std::to_string(y);
	std::hash<std::string> hash_fn;

	return hash_fn(coord_str);
}