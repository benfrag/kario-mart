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
            Vector3 right = Vector3::new_cross(Vector3(0, 1, 0), forward).normalized();

            // constants, we can #define later
            float max_speed = 35;
            float accel_rate = 15;
            float friction_coeff = 0.3;

            Vector3 result_car_pos = car_transform->position;
            Vector3 result_real_car_rot = car_rotation->rotation; // real rotation

            float frame_speed = car_physics->velocity.magnitude(); // pre input
            
            car_physics->acceleration = Vector3({0, 0, 0});

            if (engine->input_manager.is_key_pressed(0x57)) // W
            {
                car_physics->acceleration = car_physics->acceleration + forward * accel_rate;
            }

            if (engine->input_manager.is_key_pressed(0x53)) // S
            {
                car_physics->acceleration = car_physics->acceleration - forward * accel_rate;
            }
            float rotation_speed = 40;
            rotation_speed = 40 + ((180 - 40) * (1 - (frame_speed / max_speed)));
            //at 0 should be 180, at max_speed should be 40

            if (engine->input_manager.is_key_pressed(0x44)) //D
            {
                result_real_car_rot.y += rotation_speed * dt;
            }
           
            if (engine->input_manager.is_key_pressed(0x41)) //A
            {
                result_real_car_rot.y -= rotation_speed * dt;
            }
            
            
            yaw_rads = result_real_car_rot.y * (3.14159 / 180.f);
            pitch_rads = result_real_car_rot.x * (3.14159 / 180.f);

            Vector3 new_forward;
            new_forward.x = sin(yaw_rads) * cos(pitch_rads);
            new_forward.y = 0;
            new_forward.z = cos(yaw_rads) * cos(pitch_rads);
            new_forward = new_forward.normalized();
            float directionality = car_physics->velocity.normalized().dot(new_forward);
            Vector3 target_direction = directionality >= 0 ? new_forward : new_forward * -1;
            // implement lerping
//            car_physics->velocity = target_direction * car_physics->velocity.magnitude();
//            car_physics->velocity = Vector3::lerp(car_physics->velocity, target_direction, traction) * car_physics->velocity.magnitude();
            
           /* float deg_change = Vector3::angle_diff(car_physics->velocity.normalized(), target_direction);
            float changeThisFrame = deg_change * 0.5 * dt;
            car_physics->velocity = Vector3::rotateAroundAxis(car_physics->velocity.normalized(), Vector3(0, 1, 0), changeThisFrame) * car_physics->velocity.magnitude();
            //Vector3 lerped_forward = Vector3::lerp(car_physics->velocity, target_direction, deg_change);
            */

//            Vector3 targetVelocity = Vector3::slerp(car_physics->velocity, target_direction * car_physics->velocity.magnitude(), 0.5);
//car_physics->velocity = targetVelocity;


            car_physics->velocity = target_direction * car_physics->velocity.magnitude();


            // set the new velocity
            car_physics->velocity = car_physics->velocity - car_physics->velocity * friction_coeff * dt;
            car_physics->velocity = car_physics->velocity + car_physics->acceleration * dt;
            if (car_physics->velocity.magnitude() > max_speed) car_physics->velocity = car_physics->velocity.normalized() * max_speed;

            // set position using velocity
            result_car_pos = result_car_pos + car_physics->velocity * dt;


            // setters for after rotation
            car_rotation->rotation = result_real_car_rot; // set real rotation
//            car_transform->rotation = result_real_car_rot; // set fake rotation, this should be interped

            Vector3 vel_rot_test = {0, 0, 0};
            vel_rot_test.y = atan2(car_physics->velocity.normalized().x, car_physics->velocity.normalized().z) * 180.f / 3.14159;

            car_transform->rotation = vel_rot_test; // set fake rotation, this should be interped
            car_transform->position = result_car_pos;

            float interp_yaw = result_real_car_rot.y;

            // set camera to follow car rotation
            Vector3 new_camera_pos = car_transform->position - forward * 5;
            new_camera_pos.y = car_transform->position.y + 4;
            cam_position->position = new_camera_pos;
            player_camera->pitch = -20;
            player_camera->yaw = interp_yaw;
            player_camera->clamp_rotation();
            player_camera->update_view(cam_position->position);
            player_camera->last_position = cam_position->position;

/*
            float movement_speed = 20;
            float acceleration_rate = 15;
            float rotation_speed = 180;
            float max_speed = 35;
            float friction_coeff = 0.3;

            Vector3 new_car_pos = car_transform->position;
            Vector3 new_real_rotation = car_rotation->rotation;

            //have real rotation and fake rotation for car, if not drifting
            //fake/render rotation should interpolate to try and match real
            //rotation (or overshoot), if drifting fake rotation will stay at an angle to real rotation

            float speed = car_physics->velocity.magnitude();
            float acceleration_mod= 5.0f - (speed / max_speed);
            if (acceleration_mod < 0.2f)
                acceleration_mod = 0.2f;

            int drift_dir = 0;
            car_physics->acceleration = Vector3({0, 0, 0});
            if (engine->input_manager.is_key_pressed(0x57))
            {
                car_physics->acceleration = forward * acceleration_rate * acceleration_mod;
            }

            if (engine->input_manager.is_key_pressed(0x53))
            {
                car_physics->acceleration = (forward * -1) * acceleration_rate;
            }
           
            bool test_drift = false;
            if (engine->input_manager.is_key_pressed(VK_SHIFT))
            {
                test_drift = true;
            }

            car_physics->velocity = car_physics->velocity - car_physics->velocity * friction_coeff * dt;
            car_physics->velocity = car_physics->velocity + car_physics->acceleration * dt;
            if (car_physics->velocity.magnitude() > max_speed)
            {
                car_physics->velocity = car_physics->velocity.normalized() * max_speed;
            }
 
            float minTurningRadius = 5.0f; // Minimum turning radius
            float maxTurningRadius = 20.0f; // Maximum turning radius
            float currentSpeed = car_physics->velocity.magnitude();
            float turningRadius;
            float someSpeedFactor = 1.f;
            if (currentSpeed > 0) {
                float calculatedRadius = maxTurningRadius / (currentSpeed / someSpeedFactor + 1);
                if (calculatedRadius < minTurningRadius) {
                    turningRadius = minTurningRadius;
                } else {
                    turningRadius = calculatedRadius;
                }
            } else {
                turningRadius = maxTurningRadius;
            }

            float turningAngle = (currentSpeed / turningRadius) * dt; // dt is the delta time for the frame

            if (engine->input_manager.is_key_pressed(0x44)) //D
            {
                new_real_rotation.y += rotation_speed * dt; // yaw
//                new_real_rotation.y += adjusted_rot_speed * dt; // yaw
//        new_real_rotation.y += turningAngle * (180.0f / 3.14159); // Convert from radians to degrees if needed
                drift_dir = 2;
            }
           
            if (engine->input_manager.is_key_pressed(0x41)) //A
            {
                new_real_rotation.y -= rotation_speed * dt; // yaw
//                new_real_rotation.y -= adjusted_rot_speed * dt; // yaw
//        new_real_rotation.y -= turningAngle * (180.0f / 3.14159); // Convert from radians to degrees if needed
                drift_dir = 1;
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
            */
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
