#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "SkyBox.h"

Renderer::Renderer()
{
	m_sky_box.load_textures();

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();//reset matrix
	gluPerspective(90.f, 1.f, 1.f, RENDER_DISTANCE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
}


Renderer::~Renderer()
{
}

void Renderer::draw_chunk_gl_list(const GLuint &chunk)
{
	m_chunk_gl_lists.push(&chunk);
}

void Renderer::draw_SFML(const sf::Drawable & drawable)
{
	m_SFML.push(&drawable);
}

void Renderer::finish_render(sf::RenderWindow &window, const Player &player)
{
	window.clear();
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		player.get_position().x,
		player.get_position().y,
		player.get_position().z,
		player.get_position().x - sin(player.m_camera_angle.x / 180 * PI),
		player.get_position().y + tan(player.m_camera_angle.y / 180 * PI),
		player.get_position().z - cos(player.m_camera_angle.x / 180 * PI),
		0,
		1,
		0
	);

	glEnable(GL_CULL_FACE);
	//draw chunks
	while (!m_chunk_gl_lists.empty()) {
		glCallList(*(m_chunk_gl_lists.front()));
		m_chunk_gl_lists.pop();
	}
	glDisable(GL_CULL_FACE);

	//draw skybox
	glTranslatef(player.get_position().x, player.get_position().y, player.get_position().z);
	m_sky_box.bind_textures();
	glTranslatef(-player.get_position().x, -player.get_position().y, -player.get_position().z);

	//sfml render
	window.pushGLStates();
	while ( !m_SFML.empty() ) {
		window.draw(*(m_SFML.front()));
		m_SFML.pop();
	}
	window.popGLStates();

	window.display(); // swap buffers
}
