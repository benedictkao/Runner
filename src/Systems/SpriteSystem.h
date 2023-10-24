#include <entt/entt.hpp>
#include "SDL.h"

class SpriteSystem
{
private:
public:
    SpriteSystem();

    ~SpriteSystem();

    void update(entt::registry &);

    void render(entt::registry &, SDL_Renderer *);
};