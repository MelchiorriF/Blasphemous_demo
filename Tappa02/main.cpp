//////////////
// includes //
//////////////

#include <SFML/Graphics.hpp>
#include <vector>

//////////////////////
// Global constants //
//////////////////////

// Window consts
const char* window_title = "Blasphemous demo";
const int window_width = 800;
const int window_height = 600;
const float max_frame_rate = 60;

// Background consts
const float ground = 500;

// Player consts
const sf::Vector2f player_starting_position = {window_width/2, ground};

/////////////////
// Spritesheet //
/////////////////

struct Spritesheet {
    sf::Texture texture;
    std::vector<sf::IntRect> frames;
    unsigned current_frame = 0;

    Spritesheet(sf::Texture texture, std::vector<sf::IntRect> frames);
};

Spritesheet::Spritesheet(sf::Texture texture, std::vector<sf::IntRect> frames) {
    this->texture = texture;
    this->frames = frames;
}

Spritesheet idle = Spritesheet(sf::Texture("assets/sprites/player/idle.png"), {
    sf::IntRect({0,0}, {128,128}),
    sf::IntRect({128,0}, {128,128}),
    sf::IntRect({256,0}, {128,128}),
    sf::IntRect({384,0}, {128,128}),
    sf::IntRect({512,0}, {128,128}),
    sf::IntRect({640,0}, {128,128}),
    sf::IntRect({768,0}, {128,128}),
    sf::IntRect({896,0}, {128,128}),
    sf::IntRect({0,128}, {128,128}),
    sf::IntRect({128,128}, {128,128}),
    sf::IntRect({256,128}, {128,128}),
    sf::IntRect({384,128}, {128,128}),
    sf::IntRect({512,128}, {128,128})
});

////////////
// Player //
////////////

struct Player {
    sf::Vector2f position;
    float speed = 100;      //In pixels per seconds
    float life = 100;

    Spritesheet* spritesheet = &idle;
    sf::Clock animation_clock;

    int direction = 1;
    bool is_moving = false;

    Player(sf::Vector2f position);
    void move(float delta);
    void animate();
    void draw(sf::RenderWindow& window);
};

Player::Player(sf::Vector2f position) {
    this->position = position;
}

void Player::move(float delta) {
    if (is_moving) position.x += direction * speed * delta;
}

void Player::animate() {
    if (animation_clock.getElapsedTime().asSeconds() > 0.1) {
        spritesheet->current_frame = (spritesheet->current_frame + 1) % spritesheet->frames.size();
        animation_clock.restart();
    }
}

void Player::draw(sf::RenderWindow& window) {
    sf::Sprite sprite(spritesheet->texture, spritesheet->frames[spritesheet->current_frame]);
    sprite.setOrigin({spritesheet->frames[spritesheet->current_frame].size.x / 2.f, spritesheet->frames[spritesheet->current_frame].size.y});
    sprite.setPosition(position);
    if (direction == -1) sprite.setScale({-1, 1});
    window.draw(sprite);
}

///////////////
// GameState //
///////////////

struct GameState {
    Player player = Player(player_starting_position);
    bool focus = false;

    //void pause_game();
    //void reset_game();
    //void game_over();
    void update(float delta);
    //void collisions();
    void draw(sf::RenderWindow& window);
};

void GameState::update(float delta) {
    player.move(delta);
    player.animate();
}

void GameState::draw(sf::RenderWindow& window) {
    player.draw(window);
}

////////////
// Events //
////////////

// Close the window
void handle_close (sf::RenderWindow& window) {
    window.close();
}

// Constrain aspect ratio and map always the same portion of the world
void handle_resize (const sf::Event::Resized& resized, sf::RenderWindow& window) {
    float aspect = static_cast<float>(window_width)/static_cast<float>(window_height);
    sf::Vector2u ws = resized.size;
    float new_aspect = static_cast<float>(ws.x)/static_cast<float>(ws.y);
    if (new_aspect < aspect) {
        ws = {ws.x,static_cast<unsigned>(ws.x/aspect)};
    }
    else {
        ws = {static_cast<unsigned>(ws.y*aspect),ws.y};
    }
    window.setSize(ws);
}

// Gain focus
void handle (const sf::Event::FocusGained&, GameState& state) {
    state.focus = true;
}

// Lose focus
void handle (const sf::Event::FocusLost&, GameState& state) {
    state.focus = false;
}

// Keyboard input handle
void handle (const sf::Event::KeyPressed& key, GameState& state) {
    if (!state.focus)
        return;

    switch (key.scancode) {
    case sf::Keyboard::Scancode::Left:
        state.player.direction = -1;
        state.player.is_moving = true;
        return;
    case sf::Keyboard::Scancode::Right:
        state.player.direction = 1;
        state.player.is_moving = true;
        return;
    default:
        return;
    }
}

// Keyboard input release handle
void handle (const sf::Event::KeyReleased& key, GameState& state)
{
    if (!state.focus)
        return;

    switch (key.scancode) {
    case sf::Keyboard::Scancode::Left:
        state.player.is_moving = false;
        return;
    case sf::Keyboard::Scancode::Right:
        state.player.is_moving = false;
        return;
    default:
        return;
    }
}

// Generic handle
template <typename T>
void handle (T& event, GameState& state) {}

//////////
// Loop //
//////////

int main() {
    // Create the window
    sf::RenderWindow window (sf::VideoMode ({window_width, window_height}), window_title);
    window.setFramerateLimit (max_frame_rate);
    window.setMinimumSize(window.getSize());
    
    // Center the window
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    window.setPosition({(static_cast<int>(desktopMode.size.x) - window_width)/2, (static_cast<int>(desktopMode.size.y) - window_height)/2});

    // Gamestate and clock
    GameState state;
    sf::Clock game_clock;

    // Main loop
    while(window.isOpen()) {
        // Events
        window.handleEvents (
                            [&window](const sf::Event::Closed&) { handle_close (window); },
                            [&window](const sf::Event::Resized& event) { handle_resize (event, window); },
                            [&state] (const auto& event) { handle (event, state); }
        );

        // Update
        state.update(game_clock.restart().asSeconds());

        // Clear, draw, display
        window.clear(sf::Color::Black);
        state.draw(window);
        window.display();
    }
}