#include "game/core/game.h"

struct LocalPlayerComponent
{

};

Game::Game(EngineCore* engine) : engine(engine)
{

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


void create_track(EngineCore* engine)
{
    /*bool black = false;
    float size = 500;
    int num_planes = 80;
    float indiv_size = size / num_planes;
    float width = 20;
    for (int x = 0; x < num_planes; ++x)
    {
        float zstart = -(size / 2.f) + (indiv_size * x);
        for (int z = 0; z < 1; ++z)
        {
            float currentx = -(width / 2.f);
            Entity new_plane = engine->ecs.create_entity();
            GeometryComponent plane_geo;
            plane_geo.vertices = {
                {0, 0, 0}, {width, 0, indiv_size}, {width, 0, 0},
                {0, 0, 0}, {0, 0, indiv_size}, {width, 0, indiv_size},
            };
            plane_geo.clr = black ? PACK(204,204,204, 255) : PACK(128,126,120,255);
            black = !black;
            engine->ecs.add_component(new_plane, TransformComponent({currentx, 0, zstart}));
            engine->ecs.add_component(new_plane, plane_geo);
        }

    }
    */
    bool black = false;
    float size = 250;
    int num_planes = 40;
    float indiv_size = size / num_planes;
    float width = 20;
    for (int x = 0; x < num_planes; ++x)
    {
        float zstart = 10 + (indiv_size * x);
        for (int z = 0; z < 1; ++z)
        {
            float currentx = -(width / 2.f);
            Entity new_plane = engine->ecs.create_entity();
            GeometryComponent plane_geo;
            plane_geo.vertices = {
                {0, 0, 0}, {width, 0, indiv_size}, {width, 0, 0},
                {0, 0, 0}, {0, 0, indiv_size}, {width, 0, indiv_size},
            };
            plane_geo.clr = black ? PACK(204,204,204, 255) : PACK(128,126,120,255);
            black = !black;
            engine->ecs.add_component(new_plane, TransformComponent({currentx, 0, zstart}));
            engine->ecs.add_component(new_plane, plane_geo);
        }

    }

    for (int x = 0; x < num_planes; ++x)
    {
        float zstart = -10 - (indiv_size * (x + 1));
        for (int z = 0; z < 1; ++z)
        {
            float currentx = -(width / 2.f);
            Entity new_plane = engine->ecs.create_entity();
            GeometryComponent plane_geo;
            plane_geo.vertices = {
                {0, 0, 0}, {width, 0, indiv_size}, {width, 0, 0},
                {0, 0, 0}, {0, 0, indiv_size}, {width, 0, indiv_size},
            };
            plane_geo.clr = black ? PACK(204,204,204, 255) : PACK(128,126,120,255);
            black = !black;
            engine->ecs.add_component(new_plane, TransformComponent({currentx, 0, zstart}));
            engine->ecs.add_component(new_plane, plane_geo);
        }

    }


    for (int i = 0; i < 5; ++i)
    {
        create_item_box(engine, Vector3{-10.f / 2.f + 2.5f * static_cast<float>(i), 0, 30});
    }
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
    //easily fixed by changing ecs to update entity lists
       
    setup_untextured_plane(engine, 20, 5);
    create_track(engine);
    setup_local_player_entity(engine);

    create_item_box(engine, {0, 1, 0});

    /*
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
    */
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
