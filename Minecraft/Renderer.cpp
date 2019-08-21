#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "block_db.h"
#include "Chunk.h"
#include "Map.h"
#include "Light.h"

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

	m_wrapper_shader.loadFromFile("shaders\\block_wrapper.vert.glsl", "shaders\\block_wrapper.frag.glsl");
	m_shader_program.loadFromFile("shaders\\shader.vert.glsl", "shaders\\shader.frag.glsl");
	m_shadow_shader.loadFromFile("shaders\\shadow.vert.glsl", "shaders\\shadow.frag.glsl");
	m_water_shader.loadFromFile("shaders\\water.vert.glsl", "shaders\\water.frag.glsl");

	m_image_atlas.loadFromFile(PATH2ATLAS);
	m_image_atlas.flipVertically();


	glEnable(GL_TEXTURE_2D);
	m_texture_atlas.loadFromImage(m_image_atlas);
	GLuint opengl_tex = m_texture_atlas.getNativeHandle();

	glBindTexture(GL_TEXTURE_2D, opengl_tex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
	glBindTexture(GL_TEXTURE_2D, 0);


	m_image_atlas.flipVertically();



	glGenVertexArrays(1, &m_wrapper_VAO);
	glGenBuffers(1, &m_wrapper_VBO);


	//shadows
	glGenFramebuffers(1, &m_depth_map_FBO);
	glGenTextures(1, &m_depth_map);
	glBindTexture(GL_TEXTURE_2D, m_depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_ñolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_ñolor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &m_wrapper_VAO);
	glDeleteBuffers(1, &m_wrapper_VBO);

	glDeleteFramebuffers(1, &m_depth_map_FBO);
	glDeleteTextures(1, &m_depth_map);
}

void Renderer::draw_SFML(const sf::Drawable& drawable)
{
	m_SFML.push(&drawable);
}

void draw_depth_map(GLuint depth_map);

void Renderer::finish_render(sf::RenderWindow& window, Player& player, set_of_chunks& chunks4rendering, sf::Mutex& mutex_chunks4rendering)
{
	static Light light;

	static int counter = 0;
	counter++;
	static const int shadow_update_per_frames = 10;
	if (counter == shadow_update_per_frames) {
		light.update(player.get_position());
	}

	auto color = light.calc_gl_sky_color();
	glClearColor(color.x, color.y, color.z, 1.0f);

	auto window_size = window.getSize();
	glm::mat4 projection_view = player.calc_projection_view(window_size);
	glm::mat4 light_pv = light.get_light_projection_view();


	sf::Vector3i player_pos = {
		Map::coord2chunk_coord(player.get_position().x),
		Map::coord2chunk_coord(player.get_position().y),
		Map::coord2chunk_coord(player.get_position().z)
	};

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);


	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
	//	counter = 1000;
	//}


	mutex_chunks4rendering.lock();
	if (counter == shadow_update_per_frames) {
		counter = 0;

		glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
		generate_depth_map(chunks4rendering, light_pv, player_pos);
	}

	glViewport(0, 0, window.getSize().x, window.getSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_block_mesh(chunks4rendering, light, projection_view, player_pos);
	draw_water_mesh(chunks4rendering, light, projection_view, player_pos, player);


	mutex_chunks4rendering.unlock();


	sf::Vector3i pos = player.determine_look_at_block();
	if (pos.y > 0) {
		draw_wrapper(pos, projection_view);
	}

	glViewport(0, 0, 200, 200);
	draw_depth_map(m_depth_map);

	//sfml render
	window.pushGLStates();
	while (!m_SFML.empty()) {
		window.draw(*(m_SFML.front()));
		m_SFML.pop();
	}
	window.popGLStates();

	window.display();
}

void Renderer::generate_depth_map(set_of_chunks& chunks4rendering, glm::mat4& light_pv, sf::Vector3i& player_pos)
{
	sf::Shader::bind(&m_shadow_shader);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	sf::Texture::bind(&m_texture_atlas);
	GLuint l_pvm_location = glGetUniformLocation(m_shadow_shader.getNativeHandle(), "pvm");

	glm::vec3 global_pos(0.f);
	glm::mat4 pvm(1.f);
	for (auto &chunk : chunks4rendering) {
		if (chunk->m_blocks_mesh.get_final_points_count() > 0) {
			global_pos = calc_global_pos(chunk->get_pos(), player_pos);
			pvm = glm::translate(light_pv, global_pos);
			glUniformMatrix4fv(l_pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
			chunk->m_blocks_mesh.draw();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::draw_block_mesh(set_of_chunks & chunks4rendering, Light& light, glm::mat4 & projection_view, sf::Vector3i & player_pos)
{
	static float fog_density = pow(RENDER_DISTANCE, 1.f / 3.f) / pow(512.f, 1.f / 3.f) * 1e-8f;
	
	m_shader_program.setUniform("light_dir", sf::Glsl::Vec3{ light.get_light_direction().x, abs(light.get_light_direction().y), light.get_light_direction().z });

	m_shader_program.setUniform("light_color", light.calc_gl_light_color());
	m_shader_program.setUniform("fog_color", light.calc_gl_sky_color());
	m_shader_program.setUniform("fog_density", fog_density);

	GLuint pvm_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "pvm");
	GLuint light_pvm_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "light_pvm");

	sf::Shader::bind(&m_shader_program);
	sf::Texture::bind(&m_texture_atlas);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture_atlas.getNativeHandle());
	glUniform1i(glGetUniformLocation(m_shader_program.getNativeHandle(), "atlas"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depth_map);
	glUniform1i(glGetUniformLocation(m_shader_program.getNativeHandle(), "shadow_map"), 1);

	glm::vec3 global_pos(0.f);
	glm::mat4 pvm(1.f);
	glm::mat4 light_pvm(1.f);
	for (auto &chunk : chunks4rendering) {
		if (chunk->m_blocks_mesh.get_final_points_count() > 0) {

			global_pos = calc_global_pos(chunk->get_pos(), player_pos);
			pvm = glm::translate(projection_view, global_pos);

			if (is_chunk_visible(pvm)) {
				light_pvm = glm::translate(light.get_light_projection_view(), global_pos);

				glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
				glUniformMatrix4fv(light_pvm_location, 1, GL_FALSE, glm::value_ptr(light_pvm));
				chunk->m_blocks_mesh.draw();
			}

		}
	}
	
}

void Renderer::draw_water_mesh(set_of_chunks & chunks4rendering, Light & light, glm::mat4 & projection_view, sf::Vector3i & player_pos, Player& player)
{
	static float fog_density = pow(RENDER_DISTANCE, 1.f / 3.f) / pow(512.f, 1.f / 3.f) * 1e-8f;
	static sf::Clock timer;

	sf::Shader::bind(&m_water_shader);
	sf::Texture::bind(&m_texture_atlas);

	m_water_shader.setUniform("light_dir", sf::Glsl::Vec3{ light.get_light_direction().x, abs(light.get_light_direction().y), light.get_light_direction().z });
	m_water_shader.setUniform("light_color", light.calc_gl_light_color());
	m_water_shader.setUniform("fog_color", light.calc_gl_sky_color());
	m_water_shader.setUniform("fog_density", fog_density);

	m_water_shader.setUniform("dtime", timer.getElapsedTime().asSeconds());

	m_water_shader.setUniform("view_pos", sf::Glsl::Vec3{player.get_position().x, player.get_position().y + 0.8f, player.get_position().z });


	GLuint pvm_location = glGetUniformLocation(m_water_shader.getNativeHandle(), "pvm");
	GLuint model_location = glGetUniformLocation(m_water_shader.getNativeHandle(), "model");


	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::vec3 global_pos(0.f);
	glm::mat4 pvm(1.f);
	glm::mat4 model(1.f);
	for (auto &chunk : chunks4rendering) {
		if (chunk->m_water_mesh.get_final_points_count() > 0) {

			global_pos = calc_global_pos(chunk->get_pos(), player_pos);
			pvm = glm::translate(projection_view, global_pos);
			model = glm::translate(glm::mat4(1.f), global_pos);

			glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
			glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

			chunk->m_water_mesh.draw();
		}
	}

	glBlendFunc(GL_ONE, GL_ZERO);
}

glm::fvec3 Renderer::calc_global_pos(const sf::Vector3i& chunk_pos, const sf::Vector3i& player_pos)
{
	//fixes white lines between chunks
	auto delta = chunk_pos - player_pos;

	return 	{ chunk_pos.x * BLOCKS_IN_CHUNK - delta.x / 2000.f,
		chunk_pos.y * BLOCKS_IN_CHUNK - delta.y / 2000.f,
		chunk_pos.z * BLOCKS_IN_CHUNK - delta.z / 2000.f };
}

void Renderer::draw_wrapper(sf::Vector3i& pos, glm::mat4& projection_view)
{

	glLineWidth(6);

	sf::Shader::bind(&m_wrapper_shader);

	auto pvm = glm::translate(projection_view, glm::vec3{
		 Map::coord2chunk_coord(pos.x) * BLOCKS_IN_CHUNK,
		 Map::coord2chunk_coord(pos.y) * BLOCKS_IN_CHUNK,
		 Map::coord2chunk_coord(pos.z) * BLOCKS_IN_CHUNK,
		});

	m_wrapper_shader.setUniform("pvm", sf::Glsl::Mat4(glm::value_ptr(pvm)));


	sf::Vector3i bpos = {
		Map::coord2block_coord_in_chunk(pos.x),
		Map::coord2block_coord_in_chunk(pos.y),
		Map::coord2block_coord_in_chunk(pos.z)
	};

	float m = 0.02f;
	float p = 1 + m;
	GLfloat wrapper_verticies[] = {
		bpos.x - m, bpos.y + p, bpos.z - m,
		bpos.x + p, bpos.y + p, bpos.z - m,
		bpos.x + p, bpos.y + p, bpos.z + p,
		bpos.x - m, bpos.y + p, bpos.z + p,
		bpos.x - m, bpos.y + p, bpos.z - m,

		bpos.x - m, bpos.y - m, bpos.z - m,
		bpos.x + p, bpos.y - m, bpos.z - m,
		bpos.x + p, bpos.y - m, bpos.z + p,
		bpos.x - m, bpos.y - m, bpos.z + p,
		bpos.x - m, bpos.y - m, bpos.z - m,

		bpos.x + p, bpos.y - m, bpos.z - m,
		bpos.x + p, bpos.y + p, bpos.z - m,
		bpos.x + p, bpos.y + p, bpos.z + p,
		bpos.x + p, bpos.y - m, bpos.z + p,
		bpos.x + p, bpos.y - m, bpos.z - m,

		bpos.x - m, bpos.y - m, bpos.z - m,
		bpos.x - m, bpos.y + p, bpos.z - m,
		bpos.x - m, bpos.y + p, bpos.z + p,
		bpos.x - m, bpos.y - m, bpos.z + p,
		bpos.x - m, bpos.y - m, bpos.z - m
	};


	glBindVertexArray(m_wrapper_VAO);
	/**/

	glBindBuffer(GL_ARRAY_BUFFER, m_wrapper_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wrapper_verticies), wrapper_verticies, GL_STATIC_DRAW); //GL_DYNAMIC_DRAW GL_STATIC_DRAW GL_STREAM_DRAW

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	/**/
	glBindVertexArray(0); // Unbind VAO


	glBindVertexArray(m_wrapper_VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 20);
	glBindVertexArray(0);

	glLineWidth(1);
}

bool Renderer::is_chunk_visible(const glm::mat4& pvm)
{
	static const float CHUNK_CENTER = BLOCKS_IN_CHUNK / 2.f - 1.f;
	static const float SPHERE_DIAMETER = sqrtf(3.f*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK);

	glm::vec4 norm_coords = pvm * glm::vec4(CHUNK_CENTER, CHUNK_CENTER, CHUNK_CENTER, 1.F);
	norm_coords.x /= norm_coords.w;
	norm_coords.y /= norm_coords.w;

	return norm_coords.z > -1 * SPHERE_DIAMETER
		&& fabsf(norm_coords.x) < 1 + SPHERE_DIAMETER / fabsf(norm_coords.w)
		&& fabsf(norm_coords.y) < 1 + SPHERE_DIAMETER / fabsf(norm_coords.w);
}

void draw_depth_map(GLuint depth_map)
{
	std::string vert =
		"#version 330 core \n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec2 aTexCoords;\n"

		"out vec2 TexCoords;\n"

		"void main()\n"
		"{"
		"TexCoords = aTexCoords;\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"}";

	std::string frag =
		"#version 330 core \n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoords;\n"
		"uniform sampler2D depthMap;\n"


		"void main()\n"
		"{"
		"float depthValue = texture(depthMap, TexCoords).r;\n"
		"FragColor = vec4(vec3(depthValue), 1.0);\n"
		"}";

	sf::Shader shader;
	shader.loadFromMemory(vert, frag);
	sf::Shader::bind(&shader);

	glBindTexture(GL_TEXTURE_2D, depth_map);

	static GLuint VAO = 0;
	static GLuint VBO = 0;

	if (VAO == 0)
	{
		static float vertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}