#pragma once

#include "common.h"
#include "game_resources.h"
#include "texture.h"

struct Animation
{
    Point start_pos, vec;
    float angle_deg;
    int frames_count;
    float renders_per_animation_frame;
    float animation_time_renders;
    float cur_render;
    texture_t texture;

    Animation(const Point& start_pos, const Point& vec, const float angle_deg, const int frames_count,
              const float renders_per_frame, const float animation_time_renders, const float cur_render,
              const texture_t& texture);

    void Render(std::shared_ptr<SDL_Renderer> r);

    bool ToDelete();

    // positions in sector space
    static std::unique_ptr<Animation> MissileAnimation(const Point& s, const Point& e,
                                                       const std::shared_ptr<game_resources>& res);

    static std::unique_ptr<Animation> Explosion1Animation(const Point& p, const std::shared_ptr<game_resources>& res);
    static std::unique_ptr<Animation> Explosion2Animation(const Point& p, const std::shared_ptr<game_resources>& res);
};
