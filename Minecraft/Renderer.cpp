#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "SkyBox.h"
#include "block_db.h"


using std::array;

GLfloat halfSideLength = BLOCK_SIZE * 0.5f;


Renderer::Renderer()
{
	glewExperimental = GL_TRUE;
	glewInit();

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

	//m_texture_atlas.generateMipmap();

	glEnable(GL_CULL_FACE);
}

Renderer::~Renderer()
{
}

void Renderer::reset_view(sf::Vector2f size)
{
	glViewport(0, 0, size.x, size.y);
}

void Renderer::draw_chunk(Chunk* chunk)
{
	//m_chunks.push(chunk);
}

void Renderer::draw_SFML(const sf::Drawable& drawable)
{
	m_SFML.push(&drawable);
}

float SPHERE_DIAMETER = std::sqrtf(3 * BLOCK_SIZE*BLOCK_SIZE);


void Renderer::finish_render(sf::RenderWindow& window, const Player& player, World::Map& m_map, std::unordered_map<size_t, Chunk*> &m_chunks4rendering)
{

	static glm::mat4 model = glm::mat4(1.0f);
	static glm::mat4 view = glm::mat4(1.0f);
	static glm::mat4 projection = glm::mat4(1.0f);

	glClearColor(0.6, 0.8, 1.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	sf::Shader::bind(&m_shader_program);
	sf::Texture::bind(&m_texture_atlas);

	projection = glm::perspective(45.0f, (GLfloat)window.getSize().x / (GLfloat)window.getSize().y, 0.1f, RENDER_DISTANCE);
	view = glm::lookAt(
		glm::vec3(
			player.get_position().x,
			player.get_position().y+0.0F,
			player.get_position().z
		),
		glm::vec3(
			player.get_position().x - sin(player.m_camera_angle.x / 180 * PI),
			player.get_position().y + tan(player.m_camera_angle.y / 180 * PI),
			player.get_position().z - cos(player.m_camera_angle.x / 180 * PI)
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



	glm::vec4 norm_coords;
	glEnable(GL_CULL_FACE);
	for (auto &e : m_chunks4rendering) {
		Chunk* chunk = e.second;
		auto &pos = chunk->get_pos();


		mat[12] = pos.x * CHUNK_SIZE;
		mat[13] = pos.y * CHUNK_SIZE;
		mat[14] = pos.z * CHUNK_SIZE;


		//glm::mat4 mvp = glm::mat4(1.0F);
		//mvp *= projection * view * model;

		//norm_coords = (projection * view * model) * glm::vec4(1.F);

		//norm_coords.x /= norm_coords.w;
		//norm_coords.y /= norm_coords.w;

		////TODO invalid "rendering sphere"
		//if (norm_coords.z >= -80 * BLOCK_SIZE / std::fabsf(norm_coords.w)
		//	&& std::fabsf(norm_coords.x) <= 1 + 60 * BLOCK_SIZE / std::fabsf(norm_coords.w)
		//	&& std::fabsf(norm_coords.y) <= 1 + 90 * BLOCK_SIZE / std::fabsf(norm_coords.w)
		//	) {
			//m_shader_program.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));

			glUniformMatrix4fv(model_location, 1, GL_FALSE, mat);

			glBindVertexArray(chunk->get_VAO());
			glDrawArrays(GL_TRIANGLES, 0, chunk->get_points_count());
			glBindVertexArray(0);
		//}

	}
	glDisable(GL_CULL_FACE);

	//sfml render
	window.pushGLStates();
	while (!m_SFML.empty()) {
		window.draw(*(m_SFML.front()));
		m_SFML.pop();
	}
	window.popGLStates();

	window.display(); // swap buffers
}


/*
	glm::vec4 norm_coords;
	//glEnable(GL_CULL_FACE);
	while (!m_chunks.empty()) {
		Chunk* chunk = m_chunks.front();
		auto &pos = chunk->get_pos();


		model = glm::translate(model, glm::vec3(
			BLOCK_SIZE*(pos.x * CHUNK_SIZE + 0.0F),
			BLOCK_SIZE*(pos.y * CHUNK_SIZE + 0.0F),
			BLOCK_SIZE*(pos.z * CHUNK_SIZE + 0.0F)
		));

		//glm::mat4 mvp = glm::mat4(1.0F);
		//mvp *= projection * view * model;

		norm_coords = (projection * view * model) * glm::vec4(1.F);

		norm_coords.x /= norm_coords.w;
		norm_coords.y /= norm_coords.w;

		//TODO invalid "rendering sphere"
		if (norm_coords.z >= -80 * BLOCK_SIZE / std::fabsf(norm_coords.w)
			&& std::fabsf(norm_coords.x) <= 1 + 60 * BLOCK_SIZE / std::fabsf(norm_coords.w)
			&& std::fabsf(norm_coords.y) <= 1 + 90 * BLOCK_SIZE / std::fabsf(norm_coords.w)
			) {
			m_shader_program.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));

			glBindVertexArray(chunk->get_VAO());
			glDrawArrays(GL_TRIANGLES, 0, chunk->get_points_count());
			glBindVertexArray(0);
		}

		model = glm::mat4(1.0f);
		m_chunks.pop();
	}
*/