#define _USE_MATH_DEFINES
#include "animation.h"
#include <cmath>
#include "assets.h"
#include "size_settings.h"

static Point sector_to_screen_space(const Point &p)
{
    return Point{size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * p.x,
                 size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * p.y};
}

Animation::Animation(const Point &start_pos, const Point &vec, const float angle_deg, const int frames_count,
                     const float renders_per_animation_frame, const float animation_time_renders,
                     const float cur_render, const texture_t &texture)
    : start_pos(start_pos),
      vec(vec),
      angle_deg(angle_deg),
      frames_count(frames_count),
      renders_per_animation_frame(renders_per_animation_frame),
      animation_time_renders(animation_time_renders),
      cur_render(cur_render),
      texture(texture)
{
}

void Animation::Render(std::shared_ptr<SDL_Renderer> r)
{
    const int w = texture.w / frames_count;
    const Point pos = start_pos + vec * (cur_render / animation_time_renders);
    const SDL_Rect s = {(int(cur_render / renders_per_animation_frame) % frames_count) * w, 0, w, texture.h};
    const SDL_Rect d = {static_cast<int>(pos.x - 0.5f * w), static_cast<int>(pos.y - 0.5f * texture.h), w, texture.h};

    SDL_RenderCopyEx(r.get(), texture.t.get(), &s, &d, angle_deg, nullptr, SDL_FLIP_NONE);
    cur_render++;
}

bool Animation::ToDelete() { return cur_render >= animation_time_renders; }

std::unique_ptr<Animation> Animation::MissileAnimation(const Point &s, const Point &e,
                                                       const std::shared_ptr<game_resources> &res)
{
    const Point start_pos = sector_to_screen_space(s);
    const Point v = sector_to_screen_space(e) - start_pos;
    const float angle = std::atan2(v.y, v.x) * 360.0f / (2.0f * float(M_PI));
    return std::make_unique<Animation>(start_pos, v, angle, 1, 1, 60, 0, res->missile_texture);
}

std::unique_ptr<Animation> Animation::Explosion1Animation(const Point &p, const std::shared_ptr<game_resources> &res)
{
    const Point start_pos = sector_to_screen_space(p);
    return std::make_unique<Animation>(start_pos, Point{0, 0}, 0, 8, 8, 64, 0, res->explosion1_texture);
}

std::unique_ptr<Animation> Animation::Explosion2Animation(const Point &p, const std::shared_ptr<game_resources> &res)
{
    const Point start_pos = sector_to_screen_space(p);
    return std::make_unique<Animation>(start_pos, Point{0, 0}, 0, 8, 8, 64, 0, res->explosion2_texture);
}
