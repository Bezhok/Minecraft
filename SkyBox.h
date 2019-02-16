#pragma once
#include "Block.h"
class SkyBox :
	public Block
{
public:
	SkyBox();
	~SkyBox();
	void bind_textures(float half_size)
	{
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
};

