#ifndef LOCAL_PLAYER_INPUT_H
#define LOCAL_PLAYER_INPUT_H

#include "engine/core/engine.h"
#include "game/components/include_components.h"

class LocalPlayerInputSystem : public System
{
public:
    LocalPlayerInputSystem() {}

    void update(void* engine_core, float dt) override
    {
        EngineCore* engine = static_cast<EngineCore*>(engine_core);

        for (auto entity : entities)
        {
            CameraComponent* player_camera = engine->ecs.get_component<CameraComponent>(entity);
            TransformComponent* car_transform = engine->ecs.get_component<TransformComponent>(entity);
            RealRotationComponent* car_rotation = engine->ecs.get_component<RealRotationComponent>(entity);
            PhysicsComponent* car_physics = engine->ecs.get_component<PhysicsComponent>(entity);
            PositionComponent* cam_position = engine->ecs.get_component<PositionComponent>(entity);

            float yaw_rads = car_rotation->rotation.y * (3.14159 / 180.f);
            float pitch_rads = car_rotation->rotation.x * (3.14159 / 180.f);

            Vector3 forward;
            forward.x = sin(yaw_rads) * cos(pitch_rads);
            forward.y = 0;
            forward.z = cos(yaw_rads) * cos(pitch_rads);
            forward = forward.normalized();

            float movement_speed = 20;
            float acceleration_rate = 5;
            float rotation_speed = 180;
            float friction_coeff = 0.4;

            Vector3 new_car_pos = car_transform->position;
//            Vector3 new_car_rot = car_transform->rotation;
            Vector3 new_real_rotation = car_rotation->rotation;

            //have real rotation and fake rotation for car, if not drifting fake/render rotation should interpolate to try and match real rotation (or overshoot), if drifting fake rotation will stay at an angle to real rotation


            int drift_dir = 0; // Test, need actual like timed drift etc.
            // Further if init drift on say left, should be able to press right (D) to hold the drift and kind of retain momentum rather than turning
            car_physics->acceleration = Vector3({0, 0, 0});
            if (engine->input_manager.is_key_pressed(0x57))
            {
                car_physics->acceleration = forward * acceleration_rate;
            //    new_car_pos = new_car_pos + forward * movement_speed * dt;
            }

            if (engine->input_manager.is_key_pressed(0x53))
            {
             //   new_car_pos = new_car_pos - forward * movement_speed * dt;
                car_physics->acceleration = (forward * -1) * acceleration_rate;
            }
            
            if (engine->input_manager.is_key_pressed(0x44)) //D
            {
                new_real_rotation.y += rotation_speed * dt; // yaw
                drift_dir = 2;
            }
           
            if (engine->input_manager.is_key_pressed(0x41)) //A
            {
                new_real_rotation.y -= rotation_speed * dt; // yaw
                drift_dir = 1;
            }

            bool test_drift = false;
            if (engine->input_manager.is_key_pressed(VK_SHIFT))
            {
                test_drift = true;
            }

            car_physics->velocity = car_physics->velocity - car_physics->velocity * friction_coeff * dt;
            car_physics->velocity = car_physics->velocity + car_physics->acceleration * dt;
            float max_speed = 30;
            if (car_physics->velocity.magnitude() > max_speed)
            {
                car_physics->velocity = car_physics->velocity.normalized() * max_speed;
            }

            new_car_pos = new_car_pos + car_physics->velocity * dt;
            car_rotation->rotation = new_real_rotation;

            car_transform->position = new_car_pos;
            //interpolation, drifting
            car_transform->rotation = new_real_rotation;
            if (test_drift && drift_dir != 0)
            {
                if (drift_dir == 1)
                {
                    car_transform->rotation.y -= 40;
                }
                else
                {
                    car_transform->rotation.y += 40;
                }
            }

            float camera_yaw_interp = new_real_rotation.y;

            Vector3 new_pos;
            new_pos = car_transform->position - forward * 5;
            new_pos.y = car_transform->position.y + 4;
            cam_position->position = new_pos;

            player_camera->pitch = -20;
            player_camera->yaw = camera_yaw_interp;
            player_camera->clamp_rotation();
            player_camera->update_view(cam_position->position);
            player_camera->last_position = cam_position->position;
        }
/*        EngineCore* engine = static_cast<EngineCore*>(engine_core);

        for (auto entity : entities)
        {
            CameraComponent* player_camera = engine->ecs.get_component<CameraComponent>(entity);
            TransformComponent* player_transform = engine->ecs.get_component<TransformComponent>(entity);
            PhysicsComponent* player_physics = engine->ecs.get_component<PhysicsComponent>(entity);

            bool has_changed = false;
            bool has_changed2 = false;
            has_changed = update_camera_rotation(engine, player_camera, dt);
            has_changed2 = update_player_position(engine, player_camera, player_physics, player_transform, dt);
            if (has_changed || has_changed2)
            {
                player_camera->clamp_rotation();
                Vector3 new_position = player_transform->position;
                player_camera->update_view(new_position);
                player_camera->last_position = new_position;
            }
       }
       */

    }

private:
    const float MOUSE_SENSITIVITY = 90.f;
    const float MOVEMENT_SPEED = 5.0f;
    const float MOVEMENT_ACCELERATION = 10.0f;

    bool update_camera_rotation(EngineCore* engine, CameraComponent* camera, float dt)
    {
        bool has_changed = false;

        if (engine->input_manager.is_key_pressed(VK_LEFT))
        {
            camera->yaw += -MOUSE_SENSITIVITY * dt;
            has_changed = true;
        }
        if (engine->input_manager.is_key_pressed(VK_RIGHT))
        {
            camera->yaw += MOUSE_SENSITIVITY * dt;
            has_changed = true;
        }
        if (engine->input_manager.is_key_pressed(VK_UP))
        {
            camera->pitch += MOUSE_SENSITIVITY * dt;
            has_changed = true;
        }
        if (engine->input_manager.is_key_pressed(VK_DOWN))
        {
            camera->pitch -= MOUSE_SENSITIVITY * dt;
            has_changed = true;
        }

        return has_changed;
    }

    bool update_player_position(EngineCore* engine, CameraComponent* camera, PhysicsComponent* physics, TransformComponent* transform, float dt)
    {
        //should be based on velocity should perhaps just update the camera if the position has changed
        //but allow the physics systems to do the movement
        //or just add the velocity here

        bool has_changed = false;
        Vector3 front = camera->get_front();
        Vector3 right = Vector3::new_cross(Vector3(0, 1, 0), front).normalized();
        Vector3 movement_acceleration = 0;

        if (engine->input_manager.is_key_pressed(VK_SPACE))
        {
            transform->position.y += MOVEMENT_SPEED * dt;
            has_changed = true;
        }
        if (engine->input_manager.is_key_pressed(VK_CONTROL))
        {
            transform->position.y -= MOVEMENT_SPEED * dt;
            has_changed = true;
        }
        if (engine->input_manager.is_key_pressed(0x57))  // W Key
        {
            movement_acceleration = movement_acceleration + front;
            //movement_acceleration = front * MOVEMENT_ACCELERATION;
//            transform->position = transform->position + front * MOVEMENT_SPEED * dt;
//            has_changed = true;
        }
        if (engine->input_manager.is_key_pressed(0x53))  // S Key
        {
//            transform->position = transform->position - front * MOVEMENT_SPEED * dt;
//            has_changed = true;
//
            movement_acceleration = movement_acceleration - front;
        }
        if (engine->input_manager.is_key_pressed(0x44))  // D Key
        {
            //transform->position = transform->position + right * MOVEMENT_SPEED * dt;
       //     has_changed = true;
            movement_acceleration = movement_acceleration + right;
        }
        if (engine->input_manager.is_key_pressed(0x41))  // A Key
        {
            //transform->position = transform->position - right * MOVEMENT_SPEED * dt;
            //has_changed = true;
            movement_acceleration = movement_acceleration - right;
        }
        
        if (transform->position != camera->last_position)
        {
            has_changed = true;
        }


physics->acceleration = movement_acceleration.normalized() * MOVEMENT_ACCELERATION;

        //physics->acceleration = movement_acceleration;

        return has_changed;
    }
};

#endif
