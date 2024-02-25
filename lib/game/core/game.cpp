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
            plane_geo.clr = black ? PACK(255, 255, 255, 255) : PACK(11, 81, 176, 255);
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
    local_player_input_signature.set(engine->ecs.get_component_type_id<RealRotationComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<PositionComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<CameraComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<PhysicsComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<CarPhysicsComponent>());
    local_player_input_signature.set(engine->ecs.get_component_type_id<GeometryComponent>());

    engine->ecs.set_system_signature<LocalPlayerInputSystem>(local_player_input_signature);
}

void lazy_center_programmatically(std::vector<Vector3>* verts, float xoff, float zoff, float yoff = 0)
{
    // to be later removed
    for (std::size_t i = 0; i < verts->size(); ++i)
    {
        (*verts)[i].x += xoff;
        (*verts)[i].z += zoff;
        (*verts)[i].y += yoff;
    }
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

    lazy_center_programmatically(&(car_geo.vertices), -0.5f, -1.f);

//    car_geo.clr = PACK(240, 240, 248, 255);
    car_geo.clr = PACK(255, 100, 20, 255);



    Entity local_player = engine->ecs.create_entity();
    engine->ecs.add_component(local_player, TransformComponent{{-5, 1, 5}}); //transform for geo
    engine->ecs.add_component(local_player, CarPhysicsComponent{}); //transform for geo
    engine->ecs.add_component(local_player, RealRotationComponent{{0, 0, 0}}); //transform for geo

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


void lazy_scale(std::vector<Vector3>* verts, float scale)
{
    // to be later removed
    for (std::size_t i = 0; i < verts->size(); ++i)
    {
        (*verts)[i].x *= scale;
        (*verts)[i].y *= scale;
        (*verts)[i].z *= scale;
    }
}


void create_item_box(EngineCore* engine, Vector3 position)
{
    Entity item_box = engine->ecs.create_entity();
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

    lazy_scale(&(cube_geometry.vertices), 1.5);
    lazy_center_programmatically(&(cube_geometry.vertices), -0.75, -0.75, -0.75);

    cube_geometry.clr = PACK(243,204,10, 255);

    TransformComponent transform(position);
    AnimationComponent animation;

    engine->ecs.add_component(item_box, transform);
    engine->ecs.add_component(item_box, animation);
    engine->ecs.add_component(item_box, cube_geometry);

    
    LoopingFlipGenericAnimTask* movement_anim = new LoopingFlipGenericAnimTask(&(engine->ecs.get_component<TransformComponent>(item_box)->position.y), 2, 2.25, 0.5); 
    float rot_time = 10;
    LoopingFlipGenericAnimTask* rotation_anim = new LoopingFlipGenericAnimTask(&(engine->ecs.get_component<TransformComponent>(item_box)->rotation.y), 0, 360, rot_time); 
    LoopingFlipGenericAnimTask* rotation_anim2 = new LoopingFlipGenericAnimTask(&(engine->ecs.get_component<TransformComponent>(item_box)->rotation.x), 0, 360, rot_time); 
//    ColourInterpTask* col_task = new ColourInterpTask(&(engine->ecs.get_component<GeometryComponent>(item_box)->clr), RGB{233,98,95}, RGB{225,126,72}, 5); 
    std::vector<RGB> box_cols = {
        RGB{233,98,95},
        RGB{225,126,72},
        RGB{241,224,119},
        RGB{170,248,132},
        RGB{113,225,241},
        RGB{105,109,237},
        RGB{211,98,236},
    };

    LoopColourInterpTask* col_task = new LoopColourInterpTask(&(engine->ecs.get_component<GeometryComponent>(item_box)->clr), box_cols, 1); 
    engine->ecs.get_component<AnimationComponent>(item_box)->add_task(movement_anim);
    engine->ecs.get_component<AnimationComponent>(item_box)->add_task(rotation_anim);
    engine->ecs.get_component<AnimationComponent>(item_box)->add_task(col_task);
    engine->ecs.get_component<AnimationComponent>(item_box)->add_task(rotation_anim2);
}

void Game::setup()
{
/*    auto physics_system = engine->ecs.register_system<PhysicsSystem>();

    Signature physics_signature;
    physics_signature.set(engine->ecs.get_component_type_id<TransformComponent>());
    physics_signature.set(engine->ecs.get_component_type_id<PhysicsComponent>());

    engine->ecs.set_system_signature<PhysicsSystem>(physics_signature);
*/


    auto animation_system = engine->ecs.register_system<AnimationSystem>();

    Signature animation_signature;
    animation_signature.set(engine->ecs.get_component_type_id<AnimationComponent>());

    engine->ecs.set_system_signature<AnimationSystem>(animation_signature);

    setup_local_player_systems(engine);

    auto render_system = engine->ecs.register_system<RenderSystem>();

    Signature render_signature;
    render_signature.set(engine->ecs.get_component_type_id<GeometryComponent>());
    render_signature.set(engine->ecs.get_component_type_id<TransformComponent>());

    engine->ecs.set_system_signature<RenderSystem>(render_signature);

    //have to define systems before hand for now
       
    setup_untextured_plane(engine, 20, 5);
    setup_local_player_entity(engine);

    create_item_box(engine, {0, 1, 0});

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

    cube_geometry.clr = PACK(243,204,10, 255);

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
