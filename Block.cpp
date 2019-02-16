#include "Block.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>
#include "game_constants.h"
bool get_block(int mx, int my, int mz);

using std::vector;

Block::Block()
{
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

	//m_box = {
	//	load_texture("resources/textures/blocks/grass_side_carried.png"),
	//	load_texture("resources/textures/blocks/grass_side_carried.png"),
	//	load_texture("resources/textures/blocks/grass_side_carried.png"),
	//	load_texture("resources/textures/blocks/grass_side_carried.png"),
	//	load_texture("resources/textures/blocks/dirt.png"),
	//	load_texture("resources/textures/blocks/grass_carried.png")
	//};
}


Block::~Block()
{
	//for (GLuint e : m_box) {
	//	glDeleteTextures(1, &e);
	//}
}

void Block::bind_textures(float half_size, const sf::Vector3f &pos, const sf::Vector3f &shift)
{
	if (!get_block(pos.x, pos.y, pos.z - 1)) {
		glBindTexture(GL_TEXTURE_2D, m_box[0]);
		glBegin(GL_QUADS);
		//front
		glTexCoord2f(0, 0);   glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 0);   glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 1);   glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(0, 1);   glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glEnd();
	}

	if (!get_block(pos.x, pos.y, pos.z + 1)) {
		glBindTexture(GL_TEXTURE_2D, m_box[1]);
		glBegin(GL_QUADS);
		//back
		glTexCoord2f(0, 0); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z+ half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z+ half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+ half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glEnd();
	}

	if (!get_block(pos.x - 1, pos.y, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[2]);
		glBegin(GL_QUADS);
		//left
		glTexCoord2f(0, 0); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z + half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z+ half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glEnd();
	}

	if (!get_block(pos.x + 1, pos.y, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[3]);
		glBegin(GL_QUADS);
		//right
		glTexCoord2f(0, 0); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glEnd();
	}

	if (!get_block(pos.x, pos.y - 1, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[4]);
		glBegin(GL_QUADS);
		//bottom
		glTexCoord2f(0, 0); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y -half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glEnd();
	}

	if (!get_block(pos.x, pos.y + 1, pos.z)) {
		glBindTexture(GL_TEXTURE_2D, m_box[5]);
		glBegin(GL_QUADS);
		//top  		
		glTexCoord2f(0, 0); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 0); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z -half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(1, 1); glVertex3f(2*half_size*pos.x+half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z+half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glTexCoord2f(0, 1); glVertex3f(2*half_size*pos.x -half_size+shift.x*2*half_size*CHUNK_WIDTH, 2*half_size*pos.y+half_size+shift.y*2*half_size, 2*half_size*pos.z+ half_size+shift.z*2*half_size*CHUNK_WIDTH);
		glEnd();
	}
}
