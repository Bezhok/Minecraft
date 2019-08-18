#pragma once
#include "pch.h"

class Player;
namespace World {
	class Chunk;
	class Map;
	class ChunkMeshBasic;
}

class Renderer
{
private:
	std::queue<const sf::Drawable*> m_SFML;

	sf::Shader m_shader_program;
	sf::Shader m_wrapper_shader;
	sf::Shader m_shadow_shader;
	sf::Shader m_water_shader;

	GLuint m_wrapper_VAO;
	GLuint m_wrapper_VBO;

	GLuint m_depth_map_FBO;
	GLuint m_depth_map;

	sf::Image m_image_atlas;
	sf::Texture m_texture_atlas;

private:
	void draw_wrapper(sf::Vector3i& pos, glm::mat4& projection_view);

	bool is_chunk_visible(const glm::mat4& pvm);

	glm::fvec3 get_global_pos(const sf::Vector3i& chunk_pos, const sf::Vector3i& player_pos);
public:
	/* init open gl settings */
	Renderer();
	~Renderer();

	/* reset GL_PROJECTION after resizing */
	void reset_view(sf::Vector2f size);

	/* add to queue */
	void draw_SFML(const sf::Drawable& drawable);

	/* real "drawing" */
	void finish_render(sf::RenderWindow& window, Player& player, phmap::parallel_node_hash_set<World::Chunk*> &m_chunks4rendering, sf::Mutex& mutex_chunks4rendering);
};

