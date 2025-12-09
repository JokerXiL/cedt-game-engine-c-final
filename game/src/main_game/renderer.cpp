#include <game/main_game/renderer.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <game/main_game/enemy.hpp>
#include <game/main_game/map.hpp>
#include <game/main_game/system/particle_system.hpp>
#include <engine/render/render_graph.hpp>
#include <engine/pbr/standard_material.hpp>
#include <game/main_game/sweep_material.hpp>
#include <engine/pbr/mesh.hpp>
#include <engine/pbr/mesh_factory.hpp>
#include <engine/pbr/model.hpp>
#include <engine/pbr/scene.hpp>
#include <engine/pbr/light.hpp>
#include <engine/pbr/pass/shadow_pass.hpp>
#include <engine/pbr/pass/sky_pass.hpp>
#include <engine/pbr/pass/pbr_pass.hpp>
#include <engine/pbr/ground_material.hpp>
#include <engine/ui/pass/ui_pass.hpp>
#include <engine/resource/caches.hpp>

#include <cmath>
#include <iostream>

namespace main_game {

// Helper function to create an attack arc mesh (90-degree arc on XZ plane)
static std::unique_ptr<engine::pbr::Mesh> create_attack_arc_mesh(float radius, int segments = 16) {
    engine::pbr::MeshData data;

    const float arc_angle = glm::half_pi<float>();  // 90 degrees
    const float start_angle = -arc_angle / 2.0f;    // Center the arc
    const float height = 0.1f;                       // Thin arc above ground

    // Center vertex
    data.positions.push_back(glm::vec3(0.0f, height, 0.0f));
    data.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    data.uvs.push_back(glm::vec2(0.5f, 0.5f));

    // Arc vertices
    for (int i = 0; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;
        float angle = start_angle + t * arc_angle;

        float x = std::sin(angle) * radius;
        float z = std::cos(angle) * radius;

        data.positions.push_back(glm::vec3(x, height, z));
        data.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        data.uvs.push_back(glm::vec2(t, 1.0f));
    }

    // Create triangles (fan from center)
    for (int i = 0; i < segments; ++i) {
        data.indices.push_back(0);           // Center
        data.indices.push_back(i + 1);       // Current arc vertex
        data.indices.push_back(i + 2);       // Next arc vertex
    }

    return std::make_unique<engine::pbr::Mesh>(data);
}

Renderer::Renderer()
    : _scene(std::make_unique<engine::pbr::Scene>())
    , _shader_cache(std::make_unique<engine::resource::ShaderCache>(
          engine::resource::ShaderLoader{}))
    , _texture_cache(std::make_unique<engine::resource::TextureCache>(
          engine::resource::TextureLoader{})) {

    // Create shader loader function for materials
    engine::pbr::StandardMaterial::ShaderLoadFunc shader_loader =
        [this](const std::string& vert, const std::string& frag) {
            return _shader_cache->load(vert, frag);
        };

    // Create model cache with shader loader
    _model_cache = std::make_unique<engine::resource::ModelCache>(
        engine::resource::ModelLoader{shader_loader});

    // Build render graph with passes
    _shadow_pass = _graph.add_pass(std::make_unique<engine::pbr::ShadowPass>());
    _sky_pass = _graph.add_pass(std::make_unique<engine::pbr::SkyPass>(shader_loader));
    _pbr_pass = _graph.add_pass(std::make_unique<engine::pbr::PBRPass>());
    _ui_pass = _graph.add_pass(std::make_unique<engine::ui::UIPass>());
    _graph.compile();

    // Set shared PBR context on PBR passes
    _shadow_pass->set_context(&_pbr_context);
    _sky_pass->set_context(&_pbr_context);
    _pbr_pass->set_context(&_pbr_context);
    _pbr_pass->set_shadow_pass(_shadow_pass);

    // Configure sky colors
    _sky_pass->set_sky_colors(
        glm::vec3(0.3f, 0.5f, 0.85f),   // Sky top: deep blue
        glm::vec3(0.7f, 0.8f, 0.9f)     // Horizon: light blue
    );
    _sky_pass->set_sun(
        glm::vec3(0.5f, 1.0f, 0.3f),    // Sun direction (high in sky)
        glm::vec3(1.0f, 0.95f, 0.8f),   // Warm sun color
        0.9997f                          // Small sun disk
    );

    // Create ground mesh
    _ground_mesh = engine::pbr::mesh_factory::create_plane(100.0f, 100.0f);

    // Load player model (materials are created inside ModelLoader)
    _player_model = _model_cache->load("player.glb");
    
    for (size_t i = 0; i < _player_model.get()->animation_count(); ++i) {
        auto anim = *_player_model->get_animation(i);
        std::cout << "Animation " << i << ": " << anim.name() << std::endl;
        std::cout << "  Duration: " << (anim.duration() / anim.ticks_per_second()) << " seconds" << std::endl;
    }
    // Create enemy cube mesh and materials (different colors per type)
    _enemy_mesh = engine::pbr::mesh_factory::create_cube(1.0f);

    // Melee enemies: red
    _melee_enemy_material = std::make_unique<engine::pbr::StandardMaterial>(
        shader_loader,
        glm::vec3(0.8f, 0.2f, 0.2f),   // albedo (red)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.6f                             // roughness
    );

    // Ranged enemies: purple
    _ranged_enemy_material = std::make_unique<engine::pbr::StandardMaterial>(
        shader_loader,
        glm::vec3(0.6f, 0.2f, 0.8f),   // albedo (purple)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.6f                             // roughness
    );

    // Ground: pixelated green material
    _ground_material = std::make_unique<engine::pbr::GroundMaterial>(
        shader_loader,
        glm::vec3(0.3f, 0.6f, 0.3f),  // base green color
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.8f                             // roughness
    );
    _ground_material->pixel_size = 0.5f;       // World-space pixel size
    _ground_material->color_variation = 0.8f;  // Strong color variation

    // Sweep effect material for melee attacks
    _sweep_material = std::make_unique<game::main_game::SweepMaterial>(shader_loader);
    _sweep_material->color = glm::vec3(1.0f, 0.6f, 0.2f);  // Warm orange
    _sweep_material->alpha = 0.9f;
    _sweep_material->trail_length = 0.5f;

    // Create attack arc mesh (radius will be scaled at render time)
    _attack_arc_mesh = create_attack_arc_mesh(1.0f);

    // Create projectile mesh and material (small cube, scaled to cuboid in render)
    _projectile_mesh = engine::pbr::mesh_factory::create_cube(1.0f);
    _projectile_material = std::make_unique<engine::pbr::StandardMaterial>(
        shader_loader,
        glm::vec3(1.0f, 1.0f, 0.2f),   // albedo (yellow)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.3f                             // roughness
    );

    // Create particle mesh (small cube, shared material - color set per-particle)
    _particle_mesh = engine::pbr::mesh_factory::create_cube(0.08f);
    _particle_material = std::make_unique<engine::pbr::StandardMaterial>(
        shader_loader,
        glm::vec3(1.0f, 0.3f, 0.2f),   // default red (will be overridden)
        glm::vec3(1.0f),                // specular
        0.0f,                            // metallic
        0.4f                             // roughness
    );

    // Setup default lighting
    _scene->set_ambient(glm::vec3(0.15f, 0.15f, 0.2f));

    // Main directional light (sun) with shadows
    auto sun = engine::pbr::Light::create_directional(
        glm::vec3(-0.5f, -1.0f, -0.3f),  // direction
        glm::vec3(1.0f, 0.98f, 0.9f),    // warm white color
        1.0f                              // intensity
    );
    sun.casts_shadow = true;
    sun.shadow_map_index = 0;
    _scene->add_light(sun);

    // Fill light (point light with shadow)
    auto fill = engine::pbr::Light::create_point(
        glm::vec3(-5.0f, 8.0f, 5.0f),   // position
        glm::vec3(0.4f, 0.5f, 0.6f),    // cool blue tint
        15.0f,                            // intensity
        30.0f                            // range
    );
    fill.casts_shadow = true;
    fill.shadow_map_index = 0;  // Use first cubemap slot (index 0 for point lights)
    _scene->add_light(fill);
}

Renderer::~Renderer() = default;

void Renderer::update(float delta) {
    if (_attack_visual_timer > 0.0f) {
        _attack_visual_timer -= delta;

        // Animate sweep progress (0 to 1 over the duration)
        float elapsed = ATTACK_VISUAL_DURATION - _attack_visual_timer;
        float progress = elapsed / ATTACK_VISUAL_DURATION;

        // Use easing for more dynamic feel (ease-out)
        _sweep_material->sweep_progress = 1.0f - (1.0f - progress) * (1.0f - progress);

        // Fade out alpha near the end
        if (progress > 0.7f) {
            float fade = 1.0f - (progress - 0.7f) / 0.3f;
            _sweep_material->alpha = 0.9f * fade;
        } else {
            _sweep_material->alpha = 0.9f;
        }
    }
}

void Renderer::show_melee_attack(glm::vec3 position, float rotation_y, float range) {
    _attack_visual_timer = ATTACK_VISUAL_DURATION;
    _attack_position = position;
    _attack_rotation_y = rotation_y;
    _attack_range = range;
}

void Renderer::render(const GameState& game_state, std::function<void()> ui_callback) {
    // Update scene camera
    _scene->set_camera(&game_state.camera.orbit_camera());

    // Build PBR context
    _pbr_context.clear();
    _pbr_context.scene = _scene.get();

    // Initialize player skeleton bind pose once from model
    static bool skeleton_initialized = false;
    if (!skeleton_initialized && _player_model && _player_model->get_skeleton()) {
        auto model_skeleton = _player_model->get_skeleton();
        if (model_skeleton->bindpose) {
            auto& player_skeleton = const_cast<GameState&>(game_state).player.get_skeleton();
            // Resize player skeleton to match model's bone count
            size_t model_bone_count = model_skeleton->get_bone_count();
            player_skeleton.transforms.resize(model_bone_count, glm::mat4(1.0f));
            player_skeleton.bindpose = model_skeleton->bindpose;
            player_skeleton.bone_index_map = model_skeleton->bone_index_map;
            player_skeleton.root_node = model_skeleton->root_node;
            
            // Initialize transforms for T-pose (bind pose)
            // For the mesh to render in its original pose, bone transforms should be identity
            // (skinning becomes a no-op, vertices stay where they are in mesh space)
            for (size_t i = 0; i < model_bone_count; ++i) {
                player_skeleton.transforms[i] = glm::mat4(1.0f);
            }
            
            // Initialize animation clips if available
            if (_player_model->animation_count() > 0) {
                auto idle_anim = _player_model->find_animation("Idle.anm_Skeleton");
                auto run_anim = _player_model->find_animation("Run_Gun.anm_Skeleton");
                auto melee_anim = _player_model->find_animation("Spell1_Sword_Run.anm_Skeleton");
                auto ranged_anim = _player_model->find_animation("Spell1_Gun.anm_Skeleton");
                const_cast<GameState&>(game_state).player.set_animation_clips(
                    idle_anim, run_anim, melee_anim, ranged_anim);
            }

            skeleton_initialized = true;
        }
    } else if (!skeleton_initialized) {
        skeleton_initialized = true;
    }

    // Submit ground (follows player position for infinite ground effect)
    glm::vec3 player_pos = game_state.player.position();
    glm::mat4 ground_transform = glm::translate(glm::mat4(1.0f), glm::vec3(player_pos.x, 0.0f, player_pos.z));
    _pbr_context.submit(*_ground_mesh, *_ground_material, ground_transform);
    
    // Submit player - use skinned rendering only if model has a skeleton
    if (_player_model->get_skeleton() && _player_model->get_skeleton()->get_bone_count() > 0) {
        _pbr_context.submit_skinned(*_player_model, get_player_transform(game_state), game_state.player.get_skeleton());
    } else {
        _pbr_context.submit(*_player_model, get_player_transform(game_state));
    }

    // Submit enemies (colored by type)
    for (const auto& enemy : game_state.enemy_manager.enemies()) {
        if (enemy.is_alive()) {
            auto& material = (enemy.type() == EnemyType::Melee)
                ? *_melee_enemy_material
                : *_ranged_enemy_material;
            _pbr_context.submit(*_enemy_mesh, material, get_enemy_transform(enemy));
        }
    }

    // Submit projectiles (cuboid oriented along velocity)
    for (const auto& proj : game_state.projectile_manager.projectiles()) {
        glm::mat4 proj_transform = glm::translate(glm::mat4(1.0f), proj.position);

        // Orient cuboid along velocity direction
        glm::vec3 vel_dir = glm::normalize(proj.velocity);
        float yaw = std::atan2(vel_dir.x, vel_dir.z);
        float pitch = -std::asin(vel_dir.y);
        proj_transform = glm::rotate(proj_transform, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        proj_transform = glm::rotate(proj_transform, pitch, glm::vec3(1.0f, 0.0f, 0.0f));

        // Scale to cuboid shape (elongated along Z which is now the velocity direction)
        proj_transform = glm::scale(proj_transform, glm::vec3(0.05f, 0.05f, 0.2f));

        _pbr_context.submit(*_projectile_mesh, *_projectile_material, proj_transform, false);
    }

    // Submit particles (each with its own color)
    for (const auto& particle : game_state.particle_system.particles()) {
        // Scale based on particle size
        glm::mat4 particle_transform = glm::mat4(1.0f);
        particle_transform = glm::translate(particle_transform, particle.position);
        float scale = particle.size / 0.08f;  // Normalize to mesh size
        particle_transform = glm::scale(particle_transform, glm::vec3(scale));

        // Submit with per-particle color override
        _pbr_context.submit(*_particle_mesh, *_particle_material, particle_transform, particle.color, false);
    }

    // Submit sweep effect if active (must be submitted before graph executes)
    if (_attack_visual_timer > 0.0f) {
        glm::mat4 attack_transform = glm::mat4(1.0f);
        attack_transform = glm::translate(attack_transform, _attack_position);
        attack_transform = glm::rotate(attack_transform, _attack_rotation_y, glm::vec3(0.0f, 1.0f, 0.0f));
        attack_transform = glm::scale(attack_transform, glm::vec3(_attack_range, 1.0f, _attack_range));

        _pbr_context.submit(*_attack_arc_mesh, *_sweep_material, attack_transform, false);
    }

    // Set UI callback on the UI pass
    if (ui_callback) {
        _ui_pass->set_callback(std::move(ui_callback));
    }

    // Execute render graph
    _graph.execute();
}

glm::mat4 Renderer::get_player_transform(const GameState& game_state) const {
    const Player& player = game_state.player;
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, player.position());
    transform = glm::rotate(transform, player.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));
    return transform;
}

glm::mat4 Renderer::get_enemy_transform(const Enemy& enemy) const {
    glm::mat4 transform = glm::mat4(1.0f);
    // Offset Y by 0.5 so cube sits on ground (cube is 1.0 unit, centered at origin)
    glm::vec3 pos = enemy.position() + glm::vec3(0.0f, 0.5f, 0.0f);
    transform = glm::translate(transform, pos);
    transform = glm::rotate(transform, enemy.rotation_y(), glm::vec3(0.0f, 1.0f, 0.0f));
    return transform;
}

} // namespace main_game
