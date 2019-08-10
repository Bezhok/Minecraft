#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "block_db.h"
#include "Chunk.h"
#include "Map.h"

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
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &m_wrapper_VAO);
	glDeleteBuffers(1, &m_wrapper_VBO);
}

void Renderer::reset_view(sf::Vector2f size)
{
	glViewport(0, 0, static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
}

void Renderer::draw_SFML(const sf::Drawable& drawable)
{
	m_SFML.push(&drawable);
}

void Renderer::finish_render(sf::RenderWindow& window, Player& player, phmap::parallel_node_hash_set<World::Chunk*>& m_chunks4rendering, sf::Mutex& mutex_chunks4rendering)
{
	bool is_day = true;

	static float sun_angle_degrees = 89.9f;
	if (sun_angle_degrees > 180) {
		is_day = false;
	}

	if (is_day) {
		glClearColor(0.57f, 0.73f, 0.99f, 0.0f);
	}
	else {
		glClearColor(0.109f, 0.156f, 0.313f, 0.0f);
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	sf::Shader::bind(&m_shader_program);
	sf::Texture::bind(&m_texture_atlas);

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.getSize().x / (GLfloat)window.getSize().y, 0.1f, RENDER_DISTANCE);
	glm::mat4 view = glm::lookAt(
		glm::vec3(
			player.get_position().x,
			player.get_position().y + 0.8f,//m_size.y
			player.get_position().z
		),
		glm::vec3(
			player.get_position().x - sin(player.m_camera_angle.x / 180.0f * PI),
			player.get_position().y + 0.8f + tan(player.m_camera_angle.y / 180.0f * PI),
			player.get_position().z - cos(player.m_camera_angle.x / 180.0f * PI)
		),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	glm::mat4 projection_view = projection*view;



	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		sun_angle_degrees += 0.2f;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		sun_angle_degrees -= 0.2f;
	}


	if (sun_angle_degrees > 360) {
		sun_angle_degrees = 0;
	}

	if (sun_angle_degrees < 0) {
		sun_angle_degrees = 360;
	}

	//visual//
	float radians = sun_angle_degrees /180*PI;
	float sun_y = RENDER_DISTANCE / sqrtf(1 + powf(1.f/tanf(radians), 2));
	float sun_x = sun_y / tanf(radians);
	sun_x += player.get_position().x;
	//todo
	sun_y += 30; //water level
	float sun_z = player.get_position().z;
	//visual//

	m_shader_program.setUniform("sun_dir", sf::Glsl::Vec3{ cosf(radians), abs(sinf(radians)), 0 });
	m_shader_program.setUniform("is_day", is_day);

	GLint pvm_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "pvm");
	GLint model_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "model");



	sf::Vector3i player_pos = { 
		Map::coord2chunk_coord(player.get_position().x)+2,
		Map::coord2chunk_coord(player.get_position().y)+2,
		Map::coord2chunk_coord(player.get_position().z)+2
	};

	static const float SPHERE_DIAMETER = sqrtf(3.f*BLOCKS_IN_CHUNK*BLOCKS_IN_CHUNK);

	float mat[] = {
		1., 0, 0, 0,
		0, 1., 0, 0,
		0, 0, 1., 0,
		0, 0, 0, 1.,
	};

	float chunk_center = BLOCKS_IN_CHUNK / 2.f - 1.f;

	glEnable(GL_CULL_FACE);
	mutex_chunks4rendering.lock();
	for (auto &chunk : m_chunks4rendering) {
		if (chunk->m_blocks_mesh.get_final_points_count() > 0) {
			auto &pos = chunk->get_pos();

			//fixs white lines between chunks
			auto delta = pos - player_pos;
			glm::mat4 pvm = glm::translate(projection_view,
				glm::fvec3(
				pos.x * BLOCKS_IN_CHUNK-delta.x/2000.f,
				pos.y * BLOCKS_IN_CHUNK-delta.y/2000.f,
				pos.z * BLOCKS_IN_CHUNK-delta.z/2000.f)
			);

			glm::vec4 norm_coords = pvm * glm::vec4(chunk_center, chunk_center, chunk_center, 1.F);
			norm_coords.x /= norm_coords.w;
			norm_coords.y /= norm_coords.w;

			bool is_chunk_visible = norm_coords.z > -1 * SPHERE_DIAMETER
				&& fabsf(norm_coords.x) < 1 + SPHERE_DIAMETER / fabsf(norm_coords.w)
				&& fabsf(norm_coords.y) < 1 + SPHERE_DIAMETER / fabsf(norm_coords.w);


			if (is_chunk_visible) {
				assert(pos.y >= 0);

				mat[12] = static_cast<float>((pos.x) * BLOCKS_IN_CHUNK);
				mat[13] = static_cast<float>((pos.y) * BLOCKS_IN_CHUNK);
				mat[14] = static_cast<float>((pos.z) * BLOCKS_IN_CHUNK);

				glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
				glUniformMatrix4fv(model_location, 1, GL_FALSE, mat);

				glBindVertexArray(chunk->m_blocks_mesh.get_VAO());
				glDrawArrays(GL_TRIANGLES, 0, chunk->m_blocks_mesh.get_final_points_count());
				glBindVertexArray(0);
			}
		}
	}
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//only water
	for (auto &chunk : m_chunks4rendering) {
		if (chunk->m_water_mesh.get_final_points_count() > 0) {
			auto &pos = chunk->get_pos();
			assert(pos.y >= 0);


			auto delta = pos - player_pos;
			glm::mat4 pvm = glm::translate(projection_view,
				glm::fvec3(
					pos.x * BLOCKS_IN_CHUNK - delta.x / 2000.f,
					pos.y * BLOCKS_IN_CHUNK - delta.y / 2000.f,
					pos.z * BLOCKS_IN_CHUNK - delta.z / 2000.f)
			);


			mat[12] = static_cast<float>((pos.x) * BLOCKS_IN_CHUNK);
			mat[13] = static_cast<float>((pos.y) * BLOCKS_IN_CHUNK);
			mat[14] = static_cast<float>((pos.z) * BLOCKS_IN_CHUNK);

			glUniformMatrix4fv(pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
			glUniformMatrix4fv(model_location, 1, GL_FALSE, mat);

			glBindVertexArray(chunk->m_water_mesh.get_VAO());
			glDrawArrays(GL_TRIANGLES, 0, chunk->m_water_mesh.get_final_points_count());
			glBindVertexArray(0);
		}
	}
	
	glBlendFunc(GL_ONE, GL_ZERO);
	mutex_chunks4rendering.unlock();

	sf::Vector3i pos = player.get_block_look_at_pos();
	if (pos.y > 0) {
		draw_wrapper(pos, projection, view);
	}

	//sfml render
	window.pushGLStates();

	while (!m_SFML.empty()) {
		window.draw(*(m_SFML.front()));
		m_SFML.pop();
	}
	window.popGLStates();

	window.display();
}

void Renderer::draw_wrapper(sf::Vector3i& pos, glm::mat4& projection, glm::mat4& view)
{
	float mat[] = {
		1.f, 0, 0, 0,
		0, 1.f, 0, 0,
		0, 0, 1.f, 0,
		0, 0, 0, 1.f,
	};

	glLineWidth(4);

	sf::Shader::bind(&m_wrapper_shader);
	m_wrapper_shader.setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view)));
	m_wrapper_shader.setUniform("projection", sf::Glsl::Mat4(glm::value_ptr(projection)));

	GLint model_location = glGetUniformLocation(m_wrapper_shader.getNativeHandle(), "model");

	sf::Vector3i bpos = {
		Map::coord2block_coord_in_chunk(pos.x),
		Map::coord2block_coord_in_chunk(pos.y),
		Map::coord2block_coord_in_chunk(pos.z)
	};

	float m = 0.005f;
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


	mat[12] = static_cast<float>(Map::coord2chunk_coord(pos.x) * BLOCKS_IN_CHUNK);
	mat[13] = static_cast<float>(Map::coord2chunk_coord(pos.y) * BLOCKS_IN_CHUNK);
	mat[14] = static_cast<float>(Map::coord2chunk_coord(pos.z) * BLOCKS_IN_CHUNK);

	glUniformMatrix4fv(model_location, 1, GL_FALSE, mat);

	glBindVertexArray(m_wrapper_VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 20);
	glBindVertexArray(0);


	glLineWidth(1);
}
