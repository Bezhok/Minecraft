#pragma once
#include "pch.h"
#include "SkyBox.h"
#include "Chunk.h"
#include "Player.h"

class Renderer
{
private:
	std::queue<const sf::Drawable*> m_SFML;
	std::queue<const GLuint*> m_chunk_gl_lists;
	SkyBox m_sky_box;

public:
	/* init open gl settings */
	Renderer();
	~Renderer();

	/* reset GL_PROJECTION after resizing */
	void reset_view(sf::Vector2f size);

	/* add to queue */
	void draw_chunk_gl_list(const GLuint& chunk);
	void draw_SFML(const sf::Drawable& drawable);

	/* real "drawing" */
	void finish_render(sf::RenderWindow &window, const Player &player);
};

