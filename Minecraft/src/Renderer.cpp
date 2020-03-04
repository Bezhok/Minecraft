#include "pch.h"
#include "Renderer.h"
#include "Player.h"
#include "block_db.h"
#include "Converter.h"
#include "Map.h"
#include "Light.h"
#include "BlockWrapper.h"
#include "Chunk.h"

using namespace World;

void Renderer::init_openGL() {
    glewExperimental = GL_TRUE;
    glewInit();

    //glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    //glFrontFace(GL_CW); // way around
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);
}

void Renderer::load_shaders() {
    m_wrapper_shader.loadFromFile("shaders\\block_wrapper.vert.glsl", "shaders\\block_wrapper.frag.glsl");
    m_shader_program.loadFromFile("shaders\\shader.vert.glsl", "shaders\\shader.frag.glsl");
    m_shadow_shader.loadFromFile("shaders\\shadow.vert.glsl", "shaders\\shadow.frag.glsl");
    m_water_shader.loadFromFile("shaders\\water.vert.glsl", "shaders\\water.frag.glsl");

    m_pvm_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "pvm");
    m_light_pvm_location = glGetUniformLocation(m_shader_program.getNativeHandle(), "light_pvm");

    m_shadow_pvm_location = glGetUniformLocation(m_shadow_shader.getNativeHandle(), "pvm");

    m_water_pvm_location = glGetUniformLocation(m_water_shader.getNativeHandle(), "pvm");
    m_water_model_location = glGetUniformLocation(m_water_shader.getNativeHandle(), "model");
}

void Renderer::load_atlas() {
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
}

void Renderer::configure_depth_map() {
    glGenFramebuffers(1, &m_depth_map_FBO);
    glGenTextures(1, &m_depth_map);
    glBindTexture(GL_TEXTURE_2D, m_depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::Renderer() {
    init_openGL();

    load_shaders();

    load_atlas();

    configure_depth_map();

    m_fog_density = pow(RENDER_DISTANCE, 1.f / 3.f) / pow(512.f, 1.f / 3.f) * 1e-8f;
    m_block_wrapper = std::make_unique<BlockWrapper>();
    m_light = std::make_unique<Light>();
}

Renderer::~Renderer() {
    glDeleteFramebuffers(1, &m_depth_map_FBO);
    glDeleteTextures(1, &m_depth_map);
}

void Renderer::draw_SFML(const sf::Drawable &drawable) {
    m_SFML.push(&drawable);
}

void Renderer::finish_renderSFML(sf::RenderWindow &window) {
    //sfml render
    window.pushGLStates();
    while (!m_SFML.empty()) {
        window.draw(*(m_SFML.front()));
        m_SFML.pop();
    }
    window.popGLStates();
}

bool Renderer::should_update_shadows() {
    static int counter = 0;
    counter++;
    static const int shadow_update_per_frames = 30;

    bool res = counter == shadow_update_per_frames;
    if (res) counter = 0;
    return res;
}

void Renderer::draw_meshes(const sf::Vector2u &window_size, Player &player, set_of_chunks &chunks4rendering,
                           bool should_update_shadow) {
    sf::Vector3i player_pos__chunk = {
            Converter::coord2chunk_coord(player.get_position().x),
            Converter::coord2chunk_coord(player.get_position().y),
            Converter::coord2chunk_coord(player.get_position().z)
    };
    if (should_update_shadow) {
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        generate_depth_map(chunks4rendering, player_pos__chunk);
    }

    glViewport(0, 0, window_size.x, window_size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_block_mesh(chunks4rendering, player_pos__chunk);
    draw_water_mesh(chunks4rendering, player_pos__chunk, player);
}

void Renderer::finish_render(sf::RenderWindow &window, Player &player, set_of_chunks &chunks4rendering,
                             sf::Mutex &mutex_chunks4rendering) {
    bool should_update = should_update_shadows();
    if (should_update) {
        m_light->update(player.get_position());
    }
    auto color = m_light->calc_gl_sky_color();
    glClearColor(color.x, color.y, color.z, 1.0f);

    auto window_size = window.getSize();
    m_projection_view = player.calc_projection_view(window_size);
    m_light_pv = m_light->get_light_projection_view();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    mutex_chunks4rendering.lock();
    draw_meshes(window_size, player, chunks4rendering, should_update);
    mutex_chunks4rendering.unlock();

    sf::Vector3i pos = player.determine_look_at_block();
    if (pos.y > 0) {
        draw_wrapper(pos);
    }

    finish_renderSFML(window);
    window.display();
}

void Renderer::generate_depth_map(set_of_chunks &chunks4rendering, sf::Vector3i &player_pos__chunk) {
    sf::Shader::bind(&m_shadow_shader);

    glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    sf::Texture::bind(&m_texture_atlas);

    glm::vec3 global_pos(0.f);
    glm::mat4 pvm(1.f);
    for (auto &chunk : chunks4rendering) {
        if (chunk->m_blocks_mesh.get_final_points_count() > 0) {
            global_pos = calc_global_pos(chunk->get_pos(), player_pos__chunk);
            pvm = glm::translate(m_light_pv, global_pos);
            glUniformMatrix4fv(m_shadow_pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
            chunk->m_blocks_mesh.draw();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::bind_shadow_map(sf::Shader &shader, int idx) {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, m_depth_map);
    glUniform1i(glGetUniformLocation(shader.getNativeHandle(), "shadow_map"), idx);
}

void Renderer::draw_block_mesh(set_of_chunks &chunks4rendering,
                               sf::Vector3i &player_pos__chunk) {

    set_basic_uniforms(m_shader_program);

    sf::Shader::bind(&m_shader_program);
    sf::Texture::bind(&m_texture_atlas);
    bind_shadow_map(m_shader_program, 1);

    glm::mat4 pvm(1.f);
    glm::mat4 light_pvm(1.f);
    for (auto &chunk : chunks4rendering) {
        if (chunk->m_blocks_mesh.get_final_points_count() > 0) {

            glm::vec3 global_pos = calc_global_pos(chunk->get_pos(), player_pos__chunk);
            pvm = glm::translate(m_projection_view, global_pos);

            if (is_chunk_visible(pvm)) {
                light_pvm = glm::translate(m_light->get_light_projection_view(), global_pos);

                glUniformMatrix4fv(m_pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
                glUniformMatrix4fv(m_light_pvm_location, 1, GL_FALSE, glm::value_ptr(light_pvm));
                chunk->m_blocks_mesh.draw();
            }

        }
    }
}

void Renderer::set_basic_uniforms(sf::Shader &shader) {
    shader.setUniform("light_dir",
                      sf::Glsl::Vec3{m_light->get_light_direction().x, abs(m_light->get_light_direction().y),
                                     m_light->get_light_direction().z});
    shader.setUniform("light_color", m_light->calc_gl_light_color());
    shader.setUniform("fog_color", m_light->calc_gl_sky_color());
    shader.setUniform("fog_density", m_fog_density);
}

void Renderer::draw_water_mesh(set_of_chunks &chunks4rendering,
                               sf::Vector3i &player_pos__chunk, Player &player) {

    sf::Shader::bind(&m_water_shader);
    sf::Texture::bind(&m_texture_atlas);

    set_basic_uniforms(m_water_shader);
    static sf::Clock timer;
    m_water_shader.setUniform("dtime", timer.getElapsedTime().asSeconds());
    m_water_shader.setUniform("view_pos", sf::Glsl::Vec3{player.get_position().x, player.get_position().y + 0.8f,
                                                         player.get_position().z});
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::vec3 global_pos(0.f);
    glm::mat4 pvm(1.f);
    glm::mat4 model(1.f);
    for (auto &chunk : chunks4rendering) {
        if (chunk->m_water_mesh.get_final_points_count() > 0) {

            global_pos = calc_global_pos(chunk->get_pos(), player_pos__chunk);
            pvm = glm::translate(m_projection_view, global_pos);
            model = glm::translate(glm::mat4(1.f), global_pos);

            glUniformMatrix4fv(m_water_pvm_location, 1, GL_FALSE, glm::value_ptr(pvm));
            glUniformMatrix4fv(m_water_model_location, 1, GL_FALSE, glm::value_ptr(model));

            chunk->m_water_mesh.draw();
        }
    }
    glBlendFunc(GL_ONE, GL_ZERO);
}

glm::fvec3 Renderer::calc_global_pos(const sf::Vector3i &chunk_pos, const sf::Vector3i &player_pos__chunk) {
    //fixes white lines between chunks
    auto delta = chunk_pos - player_pos__chunk /*+ sf::Vector3i{2, 2, 2}*/;

    return {chunk_pos.x * BLOCKS_IN_CHUNK - delta.x / 1000.f,
            chunk_pos.y * BLOCKS_IN_CHUNK - delta.y / 1000.f,
            chunk_pos.z * BLOCKS_IN_CHUNK - delta.z / 1000.f};
}

void Renderer::draw_wrapper(const sf::Vector3i &pos) {
    sf::Shader::bind(&m_wrapper_shader);

    auto pvm = glm::translate(m_projection_view, glm::vec3{
            Converter::coord2chunk_coord(pos.x) * BLOCKS_IN_CHUNK,
            Converter::coord2chunk_coord(pos.y) * BLOCKS_IN_CHUNK,
            Converter::coord2chunk_coord(pos.z) * BLOCKS_IN_CHUNK,
    });

    m_wrapper_shader.setUniform("pvm", sf::Glsl::Mat4(glm::value_ptr(pvm)));

    m_block_wrapper->set_pos(pos);
    m_block_wrapper->draw();
}

bool Renderer::is_chunk_visible(const glm::mat4 &pvm) {
    static const float CHUNK_CENTER = BLOCKS_IN_CHUNK / 2.f - 1.f;
    static const float SPHERE_DIAMETER = sqrtf(3.f * BLOCKS_IN_CHUNK * BLOCKS_IN_CHUNK);

    glm::vec4 norm_coords = pvm * glm::vec4(CHUNK_CENTER, CHUNK_CENTER, CHUNK_CENTER, 1.F);
    norm_coords.x /= norm_coords.w;
    norm_coords.y /= norm_coords.w;

    return norm_coords.z > -1 * SPHERE_DIAMETER
           && fabsf(norm_coords.x) < 1 + SPHERE_DIAMETER / fabsf(norm_coords.w)
           && fabsf(norm_coords.y) < 1 + SPHERE_DIAMETER / fabsf(norm_coords.w);
}

