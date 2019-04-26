#pragma once
#include "pch.h"
#include "SkyBox.h"
#include "Chunk.h"
#include "Player.h"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void draw_chunk_gl_list(const GLuint& chunk);
	void draw_SFML(const sf::Drawable& drawable);

	void finish_render(sf::RenderWindow &window, const Player &player);

private:
	std::queue<const sf::Drawable*> m_SFML;
	std::queue<const GLuint*> m_chunk_gl_lists;
	SkyBox m_sky_box;
};

