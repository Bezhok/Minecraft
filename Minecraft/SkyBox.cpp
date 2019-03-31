#include "game_constants.h"
#include "SkyBox.h"

using namespace World;

SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}

void SkyBox::bind_textures()
{
	float half_size = RENDER_DISTANCE / 2;
	glBindTexture(GL_TEXTURE_2D, m_box[0]);
	glBegin(GL_QUADS);
	//front
	glTexCoord2f(0, 0);   glVertex3f(-half_size, -half_size, -half_size);
	glTexCoord2f(1, 0);   glVertex3f(half_size, -half_size, -half_size);
	glTexCoord2f(1, 1);   glVertex3f(half_size, half_size, -half_size);
	glTexCoord2f(0, 1);   glVertex3f(-half_size, half_size, -half_size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_box[1]);
	glBegin(GL_QUADS);
	//back
	glTexCoord2f(0, 0); glVertex3f(half_size, -half_size, half_size);
	glTexCoord2f(1, 0); glVertex3f(-half_size, -half_size, half_size);
	glTexCoord2f(1, 1); glVertex3f(-half_size, half_size, half_size);
	glTexCoord2f(0, 1); glVertex3f(half_size, half_size, half_size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_box[2]);
	glBegin(GL_QUADS);
	//left
	glTexCoord2f(0, 0); glVertex3f(-half_size, -half_size, half_size);
	glTexCoord2f(1, 0); glVertex3f(-half_size, -half_size, -half_size);
	glTexCoord2f(1, 1); glVertex3f(-half_size, half_size, -half_size);
	glTexCoord2f(0, 1); glVertex3f(-half_size, half_size, half_size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_box[3]);
	glBegin(GL_QUADS);
	//right
	glTexCoord2f(0, 0); glVertex3f(half_size, -half_size, -half_size);
	glTexCoord2f(1, 0); glVertex3f(half_size, -half_size, half_size);
	glTexCoord2f(1, 1); glVertex3f(half_size, half_size, half_size);
	glTexCoord2f(0, 1); glVertex3f(half_size, half_size, -half_size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_box[4]);
	glBegin(GL_QUADS);
	//bottom
	glTexCoord2f(0, 0); glVertex3f(-half_size, -half_size, half_size);
	glTexCoord2f(1, 0); glVertex3f(half_size, -half_size, half_size);
	glTexCoord2f(1, 1); glVertex3f(half_size, -half_size, -half_size);
	glTexCoord2f(0, 1); glVertex3f(-half_size, -half_size, -half_size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_box[5]);
	glBegin(GL_QUADS);
	//top  		
	glTexCoord2f(0, 0); glVertex3f(-half_size, half_size, -half_size);
	glTexCoord2f(1, 0); glVertex3f(half_size, half_size, -half_size);
	glTexCoord2f(1, 1); glVertex3f(half_size, half_size, half_size);
	glTexCoord2f(0, 1); glVertex3f(-half_size, half_size, half_size);
	glEnd();
}

void SkyBox::load_textures()
{
	m_box = {
		load_texture("resources/textures/sky/cloud1/cloud1_05.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_03.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_04.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_06.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_01.gif"),
		load_texture("resources/textures/sky/cloud1/cloud1_02.gif")
	};
}

