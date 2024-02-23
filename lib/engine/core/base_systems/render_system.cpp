#include "engine/core/engine.h"
#include "engine/core/base_systems/render_system.h"
#include <iostream>

Matrix4 rotationX(float angleRadians)
{
    float c = cos(angleRadians);
    float s = sin(angleRadians);
    Matrix4 result = Matrix4::identity();
    result.m[5] = c;
    result.m[6] = -s;
    result.m[9] = s;
    result.m[10] = c;
    return result;
}

Matrix4 rotationY(float angleRadians)
{
    float c = cos(angleRadians);
    float s = sin(angleRadians);
    Matrix4 result = Matrix4::identity();
    result.m[0] = c;
    result.m[2] = -s;
    result.m[8] = s;
    result.m[10] = c;
    return result;
}

Matrix4 rotationZ(float angleRadians)
{
    float c = cos(angleRadians);
    float s = sin(angleRadians);
    Matrix4 result = Matrix4::identity();
    result.m[0] = c;
    result.m[1] = -s;
    result.m[4] = s;
    result.m[5] = c;
    return result;
}

void RenderSystem::update(void* engine_core, float dt)
{
    EngineCore* engine = static_cast<EngineCore*>(engine_core);

    for (auto entity : entities)
    {
        GeometryComponent* geometry = engine->ecs.get_component<GeometryComponent>(entity);
        TransformComponent* transform = engine->ecs.get_component<TransformComponent>(entity);
        if (geometry->render_vertices.empty())
        {
            geometry->render_vertices = geometry->vertices;

        }
        if (!(geometry->has_init) || geometry->old_transform != *transform)
        {
            geometry->has_init = true;
//            Vector3 delta_position = transform->position - geometry->old_transform.position;
            Vector3 delta_position = transform->position;
            std::size_t n = geometry->vertices.size();
            float yawRadians = (transform->rotation.y) * (3.14159 / 180.f);
            float pitchRadians = (transform->rotation.x) * (3.14159 / 180.f);
            float rollRadians = (transform->rotation.z) * (3.14159 / 180.f);

            Matrix4 rotX = rotationX(pitchRadians);
            Matrix4 rotY = rotationY(yawRadians);
            Matrix4 rotZ = rotationZ(rollRadians);

            // Combine rotation matrices
//            Matrix4 rotationMatrix = rotZ.multiply_mat(&rotY, &rotZ).multiply_mat(&rotX, &rotZ); // Note: Adjust this line according to your matrix multiplication method

            /*
            Matrix4 rotationMatrix = rotZ.multiply_mat(&rotY, &rotZ);
            rotationMatrix = rotationMatrix.multiply_mat(&rotationMatrix, &rotX);
            */

            Matrix4 rotationMatrix = rotZ.multiply_mat(&rotY, &rotZ);
rotationMatrix = rotationMatrix.multiply_mat(&rotationMatrix, &rotX);

            geometry->render_vertices = geometry->vertices;
            for (std::size_t i = 0; i < n; ++i)
            {
                /*geometry->vertices[i] = geometry->vertices[i] + delta_position;
                geometry->vertices[i] = rotationMatrix.multiply(geometry->vertices[i]);
                */
                geometry->render_vertices[i] = rotationMatrix.multiply(geometry->vertices[i]);
                geometry->render_vertices[i] = geometry->render_vertices[i] + delta_position;
            }

            geometry->old_transform = *transform;
        }

        //apply transform then send to queue
        engine->render_manager.queue_geometry(*geometry);
    }
}
