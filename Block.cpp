#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "Block.h"
#include "World.h"
#include "game_constants.h"

Block::Block(World *world) 
{
	m_world = world;

	GLuint top = load_texture("resources/textures/blocks/grass_carried.png");
	GLuint bottom = load_texture("resources/textures/blocks/dirt.png");
	GLuint side = load_texture("resources/textures/blocks/grass_side_carried.png");

	m_box = {
		side,
		side,
		side,
		side,
		bottom,
		top
	};
}

Block::~Block()
{
}

void Block::bind_textures(const sf::Vector3f &pos)
{
	float half_size = BLOCK_SIZE / 2;
	const float d_x = BLOCK_SIZE*(pos.x),
				d_y = BLOCK_SIZE*(pos.y),
				d_z = BLOCK_SIZE*(pos.z);

	if (!m_world->is_block(pos.x, pos.y, pos.z - 1)) {
		glBindTexture(GL_TEXTURE_2D, m_box[0]);
		glBegin(GL_QUADS);
		//front
		glTexCoord2f(0, 0);   glVertex3f( - half_size + d_x,  - half_size + d_y,  - half_size + d_z);
		glTexCoord2f(1, 0);   glVertex3f( + half_size + d_x,  - half_size + d_y,  - half_size + d_z);
		glTexCoord2f(1, 1);   glVertex3f( + half_size + d_x,  + half_size + d_y,  - half_size + d_z);
		glTexCoord2f(0, 1);   glVertex3f( - half_size + d_x,  + half_size + d_y,  - half_size + d_z);
		glEnd();
	}

	if (!m_world->is_block(pos.x, pos.y, pos.z + 1)) {
		glBindTexture(GL_TEXTURE_2D, m_box[1]);
		glBegin(GL_QUADS);
		//back
		glTexCoord2f(0, 0); glVertex3f( + half_size + d_x,  - half_size + d_y,  + half_size + d_z);
		glTexCoord2f(1, 0); glVertex3f( - half_size + d_x,  - half_size + d_y,  + half_size + d_z);
		glTexCoord2f(1, 1); glVertex3f( - half_size + d_x,  + half_size + d_y,  + half_size + d_z);
		glTexCoord2f(0, 1); glVertex3f( + half_size + d_x,  + half_size + d_y,  + half_size + d_z);
		glEnd();
	}

	if (!m_world->is_block(pos.x - 1, pos.y, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[2]);
		glBegin(GL_QUADS);
		//left
		glTexCoord2f(0, 0); glVertex3f( - half_size + d_x,  - half_size + d_y,  + half_size + d_z);
		glTexCoord2f(1, 0); glVertex3f( - half_size + d_x,  - half_size + d_y,  - half_size + d_z);
		glTexCoord2f(1, 1); glVertex3f( - half_size + d_x,  + half_size + d_y,  - half_size + d_z);
		glTexCoord2f(0, 1); glVertex3f( - half_size + d_x,  + half_size + d_y,  + half_size + d_z);
		glEnd();
	}

	if (!m_world->is_block(pos.x + 1, pos.y, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[3]);
		glBegin(GL_QUADS);
		//right
		glTexCoord2f(0, 0); glVertex3f( + half_size + d_x,  - half_size + d_y,  - half_size + d_z);
		glTexCoord2f(1, 0); glVertex3f( + half_size + d_x,  - half_size + d_y,  + half_size + d_z);
		glTexCoord2f(1, 1); glVertex3f( + half_size + d_x,  + half_size + d_y,  + half_size + d_z);
		glTexCoord2f(0, 1); glVertex3f( + half_size + d_x,  + half_size + d_y,  - half_size + d_z);
		glEnd();
	}

	if (!m_world->is_block(pos.x, pos.y - 1, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[4]);
		glBegin(GL_QUADS);
		//bottom
		glTexCoord2f(0, 0); glVertex3f( - half_size + d_x,  - half_size + d_y,  + half_size + d_z);
		glTexCoord2f(1, 0); glVertex3f( + half_size + d_x,  - half_size + d_y,  + half_size + d_z);
		glTexCoord2f(1, 1); glVertex3f( + half_size + d_x,  - half_size + d_y,  - half_size + d_z);
		glTexCoord2f(0, 1); glVertex3f( - half_size + d_x,  - half_size + d_y,  - half_size + d_z);
		glEnd();
	}

	if (!m_world->is_block(pos.x, pos.y + 1, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[5]);
		glBegin(GL_QUADS);
		//top  		
		glTexCoord2f(0, 0); glVertex3f( - half_size + d_x,  + half_size + d_y,  - half_size + d_z);
		glTexCoord2f(1, 0); glVertex3f( + half_size + d_x,  + half_size + d_y,  - half_size + d_z);
		glTexCoord2f(1, 1); glVertex3f( + half_size + d_x,  + half_size + d_y,  + half_size + d_z);
		glTexCoord2f(0, 1); glVertex3f( - half_size + d_x,  + half_size + d_y,  + half_size + d_z);
		glEnd();
	}
}
