#include "SDL.h"

constexpr int COOLDOWN = 2;

class SpriteComponent
{
private:
    SDL_Rect _sprite_rect_position;
    SDL_Surface *_sprite_image_surface = nullptr;
    SDL_Texture *_sprite_image_texture = nullptr;
    SDL_Texture *_sprite_image_texture_flipped = nullptr;

    int _total_rows;
    int _total_cols;
    int _sprite_row;
    int _sprite_col;

    int _cooldown = COOLDOWN;
    int _cooldown_timer = COOLDOWN;

    bool _flipped;

public:
    SpriteComponent();

    SpriteComponent(const SpriteComponent &&);

    ~SpriteComponent();

    void load_img(char const *, SDL_Renderer *, int, int);
};