#pragma once

#include "pch.h"

class Player;

class Light;
class BlockWrapper;
namespace World {
    class Chunk;
    class Map;
    class ChunkMeshBasic;
}

class Renderer {
    using set_of_chunks = phmap::parallel_node_hash_set<World::Chunk *>;

public:
    /* init open gl settings */
    Renderer();

    ~Renderer();

    /* add to queue */
    void draw_SFML(const sf::Drawable &drawable);

    /* real "drawing" */
    void finish_render(sf::RenderWindow &window, Player &player, set_of_chunks &chunks4rendering,
                       sf::Mutex &mutex_chunks4rendering);

private:
    void draw_wrapper(const sf::Vector3i &pos);

    bool is_chunk_visible(const glm::mat4 &pvm);

    void generate_depth_map(set_of_chunks &chunks4rendering, sf::Vector3i &player_pos__chunk);

    void draw_block_mesh(set_of_chunks &chunks4rendering,
                         sf::Vector3i &player_pos__chunk);

    void draw_water_mesh(set_of_chunks &chunks4rendering, sf::Vector3i &player_pos__chunk,
                         Player &player);

    void init_openGL();

    void load_shaders();

    void load_atlas();

    void configure_depth_map();

    void finish_renderSFML(sf::RenderWindow &window);

    void set_basic_uniforms(sf::Shader &shader);

    void bind_shadow_map(sf::Shader &shader, int idx);

    static glm::fvec3 calc_global_pos(const sf::Vector3i &chunk_pos, const sf::Vector3i &player_pos__chunk);

    bool should_update_shadows();

    void draw_meshes(const sf::Vector2u &window_size, Player &player, set_of_chunks &chunks4rendering,
                     bool should_update_shadow);


    std::queue<const sf::Drawable *> m_SFML;

    sf::Shader m_shader_program;
    sf::Shader m_wrapper_shader;
    sf::Shader m_shadow_shader;
    sf::Shader m_water_shader;

    GLuint m_depth_map_FBO;
    GLuint m_depth_map;

    sf::Image m_image_atlas;
    sf::Texture m_texture_atlas;

    glm::mat4 m_projection_view;
    glm::mat4 m_light_pv;
    std::unique_ptr<Light> m_light;
    std::unique_ptr<BlockWrapper> m_block_wrapper;
    GLuint m_light_pvm_location, m_pvm_location, m_shadow_pvm_location, m_water_model_location, m_water_pvm_location;

    float m_fog_density = 0.f;
};

