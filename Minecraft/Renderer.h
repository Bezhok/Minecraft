#pragma once
#include "pch.h"

class Player;
class Light;
namespace World {
	class Chunk;
	class Map;
	class ChunkMeshBasic;
}

class Renderer
{
	using set_of_chunks = phmap::parallel_node_hash_set<World::Chunk*>;
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
	void generate_depth_map(set_of_chunks& chunks4rendering, glm::mat4& light_pv, sf::Vector3i& player_pos);
	void draw_block_mesh(set_of_chunks& chunks4rendering, Light& light, glm::mat4& projection_view, sf::Vector3i& player_pos);
	void draw_water_mesh(set_of_chunks& chunks4rendering, Light& light, glm::mat4& projection_view, sf::Vector3i& player_pos, Player& player);

	glm::fvec3 calc_global_pos(const sf::Vector3i& chunk_pos, const sf::Vector3i& player_pos);
public:
	/* init open gl settings */
	Renderer();
	~Renderer();

	/* add to queue */
	void draw_SFML(const sf::Drawable& drawable);

	/* real "drawing" */
	void finish_render(sf::RenderWindow& window, Player& player, set_of_chunks &chunks4rendering, sf::Mutex& mutex_chunks4rendering);
};

