#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "SkyBox.h"

Renderer::Renderer()
{
	m_sky_box.load_textures();

	glFrontFace(GL_CW); // way around
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	/* reset matrix */
	reset_view({ static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGTH)});

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	/* fog */
	glEnable(GL_FOG);

	float fog_color[4] = { 0.5, 0.69, 1.0, 1 };
	glFogfv(GL_FOG_COLOR, fog_color); // color
	glFogi(GL_FOG_MODE, GL_LINEAR); // formula
	glFogf(GL_FOG_DENSITY, 2.f); // fog deep
	glHint(GL_FOG_HINT, GL_DONT_CARE); // way to calculate
	glFogf(GL_FOG_START, 2*RENDER_DISTANCE/5); // start
	glFogf(GL_FOG_END, RENDER_DISTANCE / 2); // end

	
	glDisable(GL_FOG); // during debugging
}

Renderer::~Renderer()
{
}

void Renderer::reset_view(sf::Vector2f size)
{
	float relation = size.x / float(size.y);

	glViewport(0, 0, size.x, size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.f, relation, 1.f, RENDER_DISTANCE);
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
	window.clear({ 128, 176, 255, 255 });
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
