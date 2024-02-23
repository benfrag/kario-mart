#include "game/core/game.h"
#include <unordered_map>
#include <random>
#include <functional>

struct LocalPlayerComponent
{

};

Game::Game(EngineCore* engine) : engine(engine)
{

}

// Hash function for pairs of floats
struct PairHash {
    template <class T1, class T2>
    size_t operator () (const std::pair<T1, T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

std::vector<Vector3> create_plane(float size, int grid, float maxHeight) {
    std::vector<Vector3> vertices;
    std::unordered_map<std::pair<float, float>, float, PairHash> heightMap;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0, maxHeight);

    float halfSize = size / 2.0f;
    float step = size / static_cast<float>(grid);

    for (int i = 0; i < grid; ++i) {
        for (int j = 0; j < grid; ++j) {
            float x0 = -halfSize + i * step;
            float x1 = -halfSize + (i + 1) * step;
            float z0 = -halfSize + j * step;
            float z1 = -halfSize + (j + 1) * step;

            // Ensure unique vertices get a random height, shared vertices use the same height
            auto getOrCreateHeight = [&heightMap, &dis, &gen](float x, float z) {
                auto key = std::make_pair(x, z);
                if (heightMap.find(key) == heightMap.end()) {
                    heightMap[key] = dis(gen); // Assign random height
                }
                return heightMap[key];
            };

            // Triangle 1 - Clockwise
            vertices.push_back({x0, getOrCreateHeight(x0, z0), z0});
            vertices.push_back({x0, getOrCreateHeight(x0, z1), z1});
            vertices.push_back({x1, getOrCreateHeight(x1, z0), z0});

            // Triangle 2 - Clockwise
            vertices.push_back({x1, getOrCreateHeight(x1, z0), z0});
            vertices.push_back({x0, getOrCreateHeight(x0, z1), z1});
            vertices.push_back({x1, getOrCreateHeight(x1, z1), z1});
        }
    }

    return vertices;
}

void setup_untextured_plane(EngineCore* engine, float size, float dimensions)
{
    float indiv_size = size / dimensions;
    bool black = false;
    for (int x = 0; x < dimensions; ++x)
    {
        float zstart = -(size / 2.f) + (indiv_size * x);
        for (int z = 0; z < dimensions; ++z)
        {
            float currentx = -(size / 2.f) + z * indiv_size;
            Entity new_plane = engine->ecs.create_entity();
            GeometryComponent plane_geo;
            plane_geo.vertices = {
                {0, 0, 0}, {indiv_size, 0, indiv_size}, {indiv_size, 0, 0},
                {0, 0, 0}, {0, 0, indiv_size}, {indiv_size, 0, indiv_size},
            };
            plane_geo.clr = black ? PACK(20, 20, 20, 255) : PACK(255, 0, 220, 255);
            black = !black;
            engine->ecs.add_component(new_plane, TransformComponent({currentx, 0, zstart}));
            engine->ecs.add_component(new_plane, plane_geo);
        }

    }
}

void setup_local_player_systems(EngineCore* engine)
{
    auto local_player_input_system = engine->ecs.register_system<LocalPlayerInputSystem>();

    Signature local_player_input_signature;
    local_player_input_signature.set(engine->ecs.get_component_type_id<LocalPlayerComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<TransformComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<PositionComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<CameraComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<PhysicsComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<GeometryComponent>());

    engine->ecs.set_system_signature<LocalPlayerInputSystem>(local_player_input_signature);
}

void setup_local_player_entity(EngineCore* engine)
{
    GeometryComponent car_geo;
    
    car_geo.vertices = {
        { 0.0f, 0.0f, 0.0f},    {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 2.0f },
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 2.0f},    {1.0f, 0.0f, 2.0f },
        { 1.0f, 0.0f, 2.0f},    {1.0f, 1.0f, 2.0f},    {0.0f, 1.0f, 2.0f },
        { 1.0f, 0.0f, 2.0f},    {0.0f, 1.0f, 2.0f},    {0.0f, 0.0f, 2.0f },
        { 0.0f, 0.0f, 2.0f},    {0.0f, 1.0f, 2.0f},    {0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 2.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 2.0f},    {1.0f, 1.0f, 2.0f },
        { 0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 2.0f},    {1.0f, 1.0f, 0.0f },
        { 1.0f, 0.0f, 2.0f},    {0.0f, 0.0f, 2.0f},    {0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 2.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f },
    };

    car_geo.clr = PACK(240, 240, 248, 255);



    Entity local_player = engine->ecs.create_entity();
    engine->ecs.add_component(local_player, TransformComponent{{-5, 0, 5}}); //transform for geo

    engine->ecs.add_component(local_player, PositionComponent{{-5, 2, -5}}); //position for camera
    engine->ecs.add_component(local_player, car_geo); //position for camera
    engine->ecs.add_component(local_player, CameraComponent(90.0f, engine->get_window_config().width, engine->get_window_config().height, 0.1f, 100.0f));
    engine->ecs.add_component(local_player, LocalPlayerComponent{});
    engine->ecs.add_component(local_player, PhysicsComponent{});

    CameraComponent* main_camera = engine->ecs.get_component<CameraComponent>(local_player);
    std::cout << "comp yaw : " << main_camera->yaw << std::endl;

    //setup player

    engine->render_manager.set_active_view_matrix(&(main_camera->view_projection_matrix));

}

void Game::setup()
{
/*    auto physics_system = engine->ecs.register_system<PhysicsSystem>();

    Signature physics_signature;
    physics_signature.set(engine->ecs.get_component_type_id<TransformComponent>());
    physics_signature.set(engine->ecs.get_component_type_id<PhysicsComponent>());

    engine->ecs.set_system_signature<PhysicsSystem>(physics_signature);
*/

    setup_local_player_systems(engine);

    auto render_system = engine->ecs.register_system<RenderSystem>();

    Signature render_signature;
    render_signature.set(engine->ecs.get_component_type_id<GeometryComponent>());
    render_signature.set(engine->ecs.get_component_type_id<TransformComponent>());

    engine->ecs.set_system_signature<RenderSystem>(render_signature);

    //have to define systems before hand for now
       
    setup_local_player_entity(engine);
    setup_untextured_plane(engine, 15, 9);

    Entity test_cube = engine->ecs.create_entity();
    GeometryComponent cube_geometry;

    cube_geometry.vertices = {
        { 0.0f, 0.0f, 0.0f},    {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f },
    };

    cube_geometry.clr = PACK(255, 100, 20, 255);

    engine->ecs.add_component(test_cube, TransformComponent{{0, 5, 0}});
    engine->ecs.add_component(test_cube, cube_geometry);
/*
    Entity local_player = engine->ecs.create_entity();
    engine->ecs.add_component(local_player, TransformComponent{{-5, 0, -5}});
    engine->ecs.add_component(local_player, CameraComponent(90.0f, engine->get_window_config().width, engine->get_window_config().height, 0.1f, 100.0f));
    engine->ecs.add_component(local_player, LocalPlayerComponent{});
    engine->ecs.add_component(local_player, PhysicsComponent{});

    CameraComponent* main_camera = engine->ecs.get_component<CameraComponent>(local_player);
    std::cout << "comp yaw : " << main_camera->yaw << std::endl;

    //setup player

    engine->render_manager.set_active_view_matrix(&(main_camera->view_projection_matrix));
    */
}
