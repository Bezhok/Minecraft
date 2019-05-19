#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "SkyBox.h"
#include "block_db.h"


using std::array;


sf::Shader shaderProgram;



GLfloat halfSideLength = BLOCK_SIZE * 0.5f;





sf::Image image;
sf::Texture texture;


Renderer::Renderer()
{
	glewExperimental = GL_TRUE;
	glewInit();

	//glFrontFace(GL_CW); // way around
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	shaderProgram.loadFromFile("D:\\c++\\vs\\Minecraft\\shaders\\shader.vert.glsl", "D:\\c++\\vs\\Minecraft\\shaders\\shader.frag.glsl");
	image.loadFromFile("resources/atlas.png");
	//image.loadFromFile("resources/textures/blocks/grass_side_carried.png");
	//image.loadFromFile("resources/row.png");
	image.flipVertically();

	// Bind the texture
	glEnable(GL_TEXTURE_2D);

	texture.loadFromImage(image);
	texture.generateMipmap();


	
}

Renderer::~Renderer()
{
}

void Renderer::reset_view(sf::Vector2f size)
{
}

void Renderer::draw_chunk_gl_list(std::pair<GLuint, sf::Vector3i>& chunk)
{
	m_chunk_gl_lists.push(&chunk);
}

void Renderer::draw_SFML(const sf::Drawable & drawable)
{
	m_SFML.push(&drawable);
}

float SPHERE_DIAMETER = std::sqrtf(3*BLOCK_SIZE*BLOCK_SIZE);
//sf::Vector3i 
void Renderer::finish_render(sf::RenderWindow &window, const Player &player, World::Map &m_map)
{

	static glm::mat4 model = glm::mat4(1.0f);
	static glm::mat4 view = glm::mat4(1.0f);
	static glm::mat4 projection = glm::mat4(1.0f);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	sf::Shader::bind(&shaderProgram);
	sf::Texture::bind(&texture);
	projection = glm::perspective(45.0f, (GLfloat)window.getSize().x / (GLfloat)window.getSize().y, 0.1f, RENDER_DISTANCE);
	view = glm::lookAt(glm::vec3(player.get_position().x, player.get_position().y, player.get_position().z),
		glm::vec3(
			player.get_position().x - sin(player.m_camera_angle.x / 180 * PI),
			player.get_position().y + tan(player.m_camera_angle.y / 180 * PI),
			player.get_position().z - cos(player.m_camera_angle.x / 180 * PI)
		),
		glm::vec3(0.0f, 1.0f, 0.0f));


	shaderProgram.setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view)));
	shaderProgram.setUniform("projection", sf::Glsl::Mat4(glm::value_ptr(projection)));




	//model = glm::translate(model, glm::vec3(
	//	BLOCK_SIZE*(0 * CHUNK_SIZE + 0.5F),
	//	BLOCK_SIZE*(0 * CHUNK_SIZE + 0.5F),
	//	BLOCK_SIZE*(0 * CHUNK_SIZE + 0.5F)
	//));
	//shaderProgram.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));
	//glEnable(GL_CULL_FACE);
	int i, j, k;
	while (!m_chunk_gl_lists.empty()) {


		i = m_chunk_gl_lists.front()->second.x;
		j = m_chunk_gl_lists.front()->second.y;
		k = m_chunk_gl_lists.front()->second.z;

		model = glm::translate(model, glm::vec3(
			BLOCK_SIZE*(i * CHUNK_SIZE + 0.0F),
			BLOCK_SIZE*(j * CHUNK_SIZE + 0.0F),
			BLOCK_SIZE*(k * CHUNK_SIZE + 0.0F)
		));

		//glm::mat4 mvp = glm::mat4(1.0F);
		//mvp *= projection * view * model;

		glm::vec4 norm_coords = (projection * view * model) * glm::vec4(1.F);

		norm_coords.x /= norm_coords.w;
		norm_coords.y /= norm_coords.w;

		if (norm_coords.z >= -80* BLOCK_SIZE / std::fabsf(norm_coords.w)
			&& std::fabsf(norm_coords.x) <= 1 + 60* BLOCK_SIZE / std::fabsf(norm_coords.w)
			&&  std::fabsf(norm_coords.y) <= 1 + 90* BLOCK_SIZE / std::fabsf(norm_coords.w)
			) {
			shaderProgram.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));

			glBindVertexArray(m_chunk_gl_lists.front()->first);
			glDrawArrays(GL_TRIANGLES, 0, m_map.get_chunk(i, j, k).get_points_count());
			glBindVertexArray(0);


		}
		model = glm::mat4(1.0f);
		m_chunk_gl_lists.pop();
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
