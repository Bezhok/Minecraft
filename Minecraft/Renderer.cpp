#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "block_db.h"
#include "Chunk.h"

using namespace World;


Renderer::Renderer()
{
	glewExperimental = GL_TRUE;
	glewInit();

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	//glFrontFace(GL_CW); // way around
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	m_shader_program.loadFromFile("D:\\c++\\vs\\Minecraft\\shaders\\shader.vert.glsl", "D:\\c++\\vs\\Minecraft\\shaders\\shader.frag.glsl");
	m_image_atlas.loadFromFile("resources/atlas.png");
	m_image_atlas.flipVertically();

	// Bind the texture
	glEnable(GL_TEXTURE_2D);

	m_texture_atlas.loadFromImage(m_image_atlas);
	GLuint opengl_tex = m_texture_atlas.getNativeHandle();

	glBindTexture(GL_TEXTURE_2D, opengl_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::~Renderer()
{
}

void Renderer::reset_view(sf::Vector2f size)
{
	glViewport(0, 0, static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
}

void Renderer::draw_chunk(Chunk* chunk)
{
	//m_chunks.push(chunk);
}

void Renderer::draw_SFML(const sf::Drawable& drawable)
{
	m_SFML.push(&drawable);
}

void Renderer::finish_render(sf::RenderWindow& window, Player& player, World::Map& m_map, std::unordered_set<World::Chunk*>& m_chunks4rendering, sf::Mutex& mutex_chunks4rendering)
{
	glClearColor(0.5f, 0.7f, 0.9f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	sf::Shader::bind(&m_shader_program);
	sf::Texture::bind(&m_texture_atlas);

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.getSize().x / (GLfloat)window.getSize().y, 0.1f, RENDER_DISTANCE);
	glm::mat4 view = glm::lookAt(
		glm::vec3(
			player.get_position().x,
			player.get_position().y+ COORDS_IN_BLOCK*0.8f,//m_size.y
			player.get_position().z
		),
		glm::vec3(
			player.get_position().x - sin(player.m_camera_angle.x / 180.0f * PI),
			player.get_position().y + COORDS_IN_BLOCK * 0.8f + tan(player.m_camera_angle.y / 180.0f * PI),
			player.get_position().z - cos(player.m_camera_angle.x / 180.0f * PI)
		),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);


	m_shader_program.setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view)));
	m_shader_program.setUniform("projection", sf::Glsl::Mat4(glm::value_ptr(projection)));

	GLint model_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "model");


	float mat[] = {
		1.f, 0, 0, 0,
		0, 1.f, 0, 0,
		0, 0, 1.f, 0,
		0, 0, 0, 1.f,
	};

	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	mutex_chunks4rendering.lock();
	for (auto &chunk : m_chunks4rendering) {
		auto &pos = chunk->get_pos();
		assert(pos.y >= 0);

		mat[12] = static_cast<float>(pos.x * BLOCKS_IN_CHUNK);
		mat[13] = static_cast<float>(pos.y * BLOCKS_IN_CHUNK);
		mat[14] = static_cast<float>(pos.z * BLOCKS_IN_CHUNK);

		glUniformMatrix4fv(model_location, 1, GL_FALSE, mat);

		glBindVertexArray(chunk->get_VAO());
		glDrawArrays(GL_TRIANGLES, 0, chunk->get_final_points_count());
		glBindVertexArray(0);
	}
	mutex_chunks4rendering.unlock();
	glDisable(GL_CULL_FACE);

	//sfml render
	window.pushGLStates();
	while (!m_SFML.empty()) {
		window.draw(*(m_SFML.front()));
		m_SFML.pop();
	}
	window.popGLStates();

	window.display();
}
