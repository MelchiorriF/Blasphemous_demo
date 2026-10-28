///////////////
// Libraries //
///////////////

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

/////////////////
// Spritesheet //
/////////////////

struct Spritesheet {
    sf::Texture texture;
    std::vector<sf::IntRect> frames;
    bool looping;
    unsigned current_frame = 0;

    Spritesheet(std::string path, std::vector<sf::IntRect> frames, bool looping);
};

////////////
// Player //
////////////

struct Player {
    sf::Vector2f position;
    float velocity_y = 0;       // To handle jumps
    float speed = 350;          // Horizontal walking speed (in pixels per seconds)
    int direction = 1;          // 1 when facing right, -1 when facing left
    bool on_ground = true;

    // Input hold booleans
    bool holding_left = false;
    bool holding_right = false;
    bool holding_jump = false;

    Spritesheet* spritesheet;
    sf::Clock animation_clock;

    Player(sf::Vector2f pos, Spritesheet* sp);

    void move(float delta);
    void start_jump();
    void manage_spritesheet();
    void animate();
    void draw(sf::RenderWindow& window);
};


/////////////////
// Extern vars //
/////////////////

extern Spritesheet idle;
extern Spritesheet walking;
extern Spritesheet jumping;

extern float ground;
extern float gravity;