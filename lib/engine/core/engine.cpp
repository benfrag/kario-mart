#include "engine/core/engine.h"

EngineCore::EngineCore() : is_running(false)
{

}

EngineCore::~EngineCore()
{
    shutdown();
}

WindowConfig EngineCore::get_window_config()
{
    return window_config;
}

void EngineCore::configure_window(WindowConfig window_config)
{
    this->window_config = window_config;
}

void EngineCore::reconfigure_window(WindowConfig window_config)
{
    //need to update window
    //need to update renderer, frame buffer and tiles
    //this needs to be performed prior to a render cycle
    //need to update renderer manager etc.

}

void EngineCore::initialize()
{
    //rather than giving the rendermanager a pointer to the renderer, the rendermanager should create the renderer. this philosophy should be followed throughout for other things as well.

    ecs.engine_core = this;
    render_manager.primitive_renderer = &primitive_renderer;
    render_manager.window_config = &window_config;
    is_running = true;
    window_manager.input_manager = &input_manager;
    window_manager.create_window(&window_config, &primitive_renderer);
}

void EngineCore::run()
{
    last_frame_time = std::chrono::high_resolution_clock::now();
    while (is_running)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float, std::chrono::seconds::period>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        process_input();
        update(delta);
        render(delta);
    }
}

void EngineCore::process_input()
{
    window_manager.process_messages();
}

void EngineCore::update(float dt)
{
    ecs.update_systems(dt);
}

void EngineCore::render(float dt)
{
    primitive_renderer.cycle_start();

    render_manager.render_geometry_queue();

    primitive_renderer.shader_filter_hook(dt);
    
    Vector3 xpoint = {5, 0, 0};
    Vector3 zpoint = {0, 0, 5};
    Vector3 anchor = {0, 0, 0};
    Vector4 v4_anchor;
    Vector4 v4_x;
    Vector4 v4_z;
    v4_anchor = render_manager.world_to_vec4(anchor);
    v4_x = render_manager.world_to_vec4(xpoint);
    v4_z = render_manager.world_to_vec4(zpoint);
    if (!(v4_anchor.z + v4_anchor.w < 0))
    {
        ScreenVertex sc_anchor = render_manager.vec4_to_screen(v4_anchor);
        if (!(v4_x.z + v4_x.w < 0))
        {
            ScreenVertex sc_x = render_manager.vec4_to_screen(v4_x);
            primitive_renderer.draw_line(sc_anchor.x, sc_anchor.y, sc_x.x, sc_x.y, PACK(255, 0, 0, 255), 1);
        }
        if (!(v4_z.z + v4_z.w < 0))
        {
            ScreenVertex sc_z = render_manager.vec4_to_screen(v4_z);
            primitive_renderer.draw_line(sc_anchor.x, sc_anchor.y, sc_z.x, sc_z.y, PACK(0, 0, 255, 255), 1);
        }
    }


    primitive_renderer.cycle_end();
    window_manager.update_window();
    render_manager.clear_queue();
}

void EngineCore::shutdown()
{

}
