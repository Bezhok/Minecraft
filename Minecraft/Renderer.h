#pragma once
#include "pch.h"
#include "SkyBox.h"
#include "Chunk.h"
#include "Player.h"

class Renderer
{
private:
	std::queue<const sf::Drawable*> m_SFML;
	std::queue<Chunk*> m_chunks;
	SkyBox m_sky_box;

	sf::Shader m_shader_program;
	sf::Image m_image_atlas;
	sf::Texture m_texture_atlas;
public:
	/* init open gl settings */
	Renderer();
	~Renderer();
	void create_chunk_vbo(World::Map& m_map);
	/* reset GL_PROJECTION after resizing */
	void reset_view(sf::Vector2f size);

	/* add to queue */
	void draw_chunk(Chunk* chunk);
	void draw_SFML(const sf::Drawable& drawable);

	/* real "drawing" */
	void finish_render(sf::RenderWindow& window, const Player& player, World::Map& m_map, std::unordered_map<size_t, Chunk*> &m_chunks4rendering);
};

