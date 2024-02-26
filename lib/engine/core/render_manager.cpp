#include "engine/core/render_manager.h"
#include <iostream>
#include <array>

RenderManager::RenderManager()
{
    view_projection_matrix = &temporary_matrix;
}

RenderManager::~RenderManager()
{

}

void RenderManager::set_active_view_matrix(Matrix4* view_projection_matrix)
{
    this->view_projection_matrix = view_projection_matrix;
}

Matrix4* RenderManager::get_active_view_matrix()
{
    return view_projection_matrix;
}

ScreenVertex RenderManager::world_to_screen(const Vector3& vec)
{
    Matrix4* matrix = view_projection_matrix;
    Vector3 result;
    float* m = matrix->m;
    result.x = m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12];
    result.y = m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13];
    result.z = m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14];
    float w  = m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15];

    if (w < 0.01f)
        return {0, 0, 0};

    Vector3 NDC;
    NDC.x = result.x / w;
    NDC.y = result.y / w;
    NDC.z = result.z / w;

    ScreenVertex screen;
    int width = window_config->width;
    int height = window_config->height;

    screen.x = static_cast<int>((1 + NDC.x) * width * 0.5);
    screen.y = static_cast<int>((1 - NDC.y) * height * 0.5);
    screen.z = w;
    return screen;
}

Vector4 RenderManager::world_to_vec4(const Vector3& vec)
{
    Matrix4* matrix = view_projection_matrix;
    Vector4 result;
    float* m = matrix->m;
    result.x = m[0] * vec.x + m[4] * vec.y + m[8] * vec.z + m[12];
    result.y = m[1] * vec.x + m[5] * vec.y + m[9] * vec.z + m[13];
    result.z = m[2] * vec.x + m[6] * vec.y + m[10] * vec.z + m[14];
    result.w  = m[3] * vec.x + m[7] * vec.y + m[11] * vec.z + m[15];
    return result;
}

ScreenVertex RenderManager::vec4_to_screen(const Vector4& vec)
{
    Vector3 NDC;
    NDC.x = vec.x / vec.w;
    NDC.y = vec.y / vec.w;
    NDC.z = vec.z / vec.w;

    ScreenVertex screen;
    int width = window_config->width;
    int height = window_config->height;

    screen.x = static_cast<int>((1 + NDC.x) * width * 0.5);
    screen.y = static_cast<int>((1 - NDC.y) * height * 0.5);
    screen.z = vec.w;
    return screen;
}

void RenderManager::queue_geometry(GeometryComponent geometry)
{
    geometry_queue.push_back(geometry);
}

void RenderManager::clear_queue()
{
    geometry_queue.clear();
}

#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<uint32_t> dis(0, 0xFFFF);

uint32_t temp_random_uint32()
{
    uint32_t randomPart = dis(gen);
    uint32_t result = (0x64 << 16) | randomPart;
    return result;
}

void RenderManager::render_geometry(std::size_t index)
{
    GeometryComponent geometry = geometry_queue[index];

    if (geometry.render_vertices.size() % 3 != 0)
    {
        std::cout << "geometry vertices not multiple of 3." << std::endl;
        return;
    }

    for (std::size_t i = 0; i < geometry.render_vertices.size(); i += 3)
    {
        /*std::array<ScreenVertex, 3> list_screen;
        std::transform(geometry.vertices.begin() + i, geometry.vertices.begin() + i + 3, list_screen.begin(),
                       [this](const Vector3& pt) {
                           return world_to_screen(pt);
                       });

        primitive_renderer->draw_triangle(list_screen[0], list_screen[1], list_screen[2], geometry.clr);*/

        std::array<ScreenVertex, 3> list_screen;
        int num_clipped = 0;
 /*       std::transform(geometry.vertices.begin() + i, geometry.vertices.begin() + i + 3, list_screen.begin(), [this, &num_clipped](const Vector3& pt) {
            Vector4 mvec = world_to_vec4(pt);
            if (mvec.z + mvec.w < 0)
            {
                num_clipped++;

            }
            return vec4_to_screen(mvec);
});*/
        //std::array<ScreenVertex, 3> list_screen;
        std::array<Vector4, 3> manipulated_v;
        std::vector<int> clipped_indices;
        std::vector<Vector4> clipped_vertices;


        std::size_t n = 3;
        for (std::size_t j = 0; j < 3; ++j)
        {
            Vector3 pt = geometry.render_vertices[i + j];
            manipulated_v[j] = world_to_vec4(pt);
            if (manipulated_v[j].z + manipulated_v[j].w < 0)
                num_clipped++;
            list_screen[j] = vec4_to_screen(manipulated_v[j]);
        }

//        bool clipped = clip_triangle(manipulated_v, clipped_vertices, 0.1f);
//        if (!clipped)
//           continue;
//        for (std::size_t j = 0; j < clipped_vertices.size(); ++j)
//        {
//            if (j > 2)
//                break;
//            list_screen[j] = vec4_to_screen(clipped_vertices[j]);
//        }

        uint32_t temp_clr = geometry.clr;
/*        if (num_clipped == 0)
            temp_clr = 0xFFeb34e8;
        if (num_clipped == 1)
            temp_clr = 0xFF344feb;
        if (num_clipped == 2)
            temp_clr = 0xFFd9eb34;
            */

        if (num_clipped > 0)
            continue;
            
        primitive_renderer->draw_triangle(list_screen[0], list_screen[1], list_screen[2], temp_clr);


//        uint32_t random_clr = temp_random_uint32();
//        primitive_renderer->draw_triangle(list_screen[0], list_screen[1], list_screen[2], random_clr);

    }
}

void RenderManager::render_geometry_queue()
{
    //need to sort triangles by z.
    std::size_t n = geometry_queue.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        render_geometry(i);
    }
}
