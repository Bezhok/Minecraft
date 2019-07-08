#pragma once
#include "pch.h"

class Player;
namespace World {
	class Chunk;
	class Map;
}

class Renderer
{
private:
	std::queue<const sf::Drawable*> m_SFML;
	std::queue<World::Chunk*> m_chunks;


	sf::Shader m_shader_program;
	sf::Image m_image_atlas;
	sf::Texture m_texture_atlas;
public:
	/* init open gl settings */
	Renderer();
	~Renderer();

	/* reset GL_PROJECTION after resizing */
	void reset_view(sf::Vector2f size);

	/* add to queue */
	void draw_chunk(World::Chunk* chunk);
	void draw_SFML(const sf::Drawable& drawable);

	/* real "drawing" */
	void finish_render(sf::RenderWindow& window, Player& player, World::Map& m_map, std::unordered_set<World::Chunk*> &m_chunks4rendering, sf::Mutex& mutex_chunks4rendering);
};

