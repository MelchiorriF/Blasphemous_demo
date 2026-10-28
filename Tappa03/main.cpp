#include "header.hpp"

////////////////////////
// External variables //
////////////////////////

float gravity = 1;
float ground = 715;

Spritesheet idle("assets/sprites/player/idle.png", {
    sf::IntRect({17,40}, {67,72}),
    sf::IntRect({529,40}, {67,72}),
    sf::IntRect({17,168}, {67,72})
}, true);

Spritesheet walking("assets/sprites/player/walking.png", {
    sf::IntRect({78,51}, {72,67}),
    sf::IntRect({150,188}, {73,68}),
    sf::IntRect({223,59}, {72,67})
}, true);

Spritesheet jumping("assets/sprites/player/jumping.png", {
    sf::IntRect({294,58}, {46,79}),
    sf::IntRect({158,48}, {63,64}),
    sf::IntRect({0,41}, {67,73}),
    sf::IntRect({78,47}, {67,72})
}, false);

//////////////////////
// Global constants //
//////////////////////

// Window consts
const char* window_title = "Blasphemous demo";
const int window_width = 1280;
const int window_height = 864;
const float max_frame_rate = 60;

// Player consts
const sf::Vector2f player_starting_position = {window_width/2, ground};

// Background consts
sf::Texture background;
const sf::IntRect bg_dim({6,4}, {640,432});

// World consts
sf::Texture tileset;
const sf::IntRect small_platform({0,228}, {75,48});
const sf::IntRect big_platform({203,304}, {142,48});

// Element positioning
const std::vector<std::pair<bool, sf::Vector2f>> all_platforms = {
    {false, {0 , 0}},
    {true, {500, 500}}
};

/////////////////////
// Texture loading //
/////////////////////

void Load() {
    if(!background.loadFromFile("assets/sprites/other/background.png")) {
        std::cerr << "Errore nel caricamento del background" << std::endl;
        exit(1);
    }

    if(!tileset.loadFromFile("assets/sprites/other/tileset.png")) {
        std::cerr << "Errore nel caricamento del tileset" << std::endl;
        exit(1);
    }
}

////////////////
// Background //
////////////////

struct Background {
    sf::Sprite bg_sprite;

    Background(sf::IntRect rect);

    void draw(sf::RenderWindow& window);
};

Background::Background(sf::IntRect rect)
    : bg_sprite(background) { // Member Initializer List (no empty constructor sf::Sprite)
    bg_sprite.setTextureRect(rect);
}

void Background::draw(sf::RenderWindow& window) {
    bg_sprite.setScale({2.f, 2.f});
    window.draw(bg_sprite);
}

//////////////
// Platform //
//////////////

struct Platform {
    sf::Vector2f position;
    sf::Sprite sprite;

    Platform(bool big, sf::Vector2f pos);

    void draw(sf::RenderWindow& window);
};

Platform::Platform(bool big, sf::Vector2f pos)
    : sprite(tileset) { // Member Initializer List (no empty constructor sf::Sprite)
    if (big) sprite.setTextureRect(big_platform);
    else sprite.setTextureRect(small_platform);
    position = pos;
}

void Platform::draw(sf::RenderWindow& window) {
    sprite.setScale({2.f, 2.f});
    sprite.setPosition(position);
    window.draw(sprite);
}

///////////
// World //
///////////

struct World {
    std::vector<Platform> platforms;

    World();

    void draw(sf::RenderWindow& window);
};

World::World() {
    for (auto& p : all_platforms)
        platforms.push_back(Platform(p.first, p.second));
}

void World::draw(sf::RenderWindow& window) {
    for (auto& p : platforms)
        p.draw (window);
}

///////////////
// GameState //
///////////////

struct GameState {
    Player player = Player(player_starting_position, &idle);
    Background background = Background(bg_dim);
    World world;
    bool focus = false;
    bool game_running = true;

    void pause_game();
    //void reset_game();
    //void game_over();
    void update(float delta);
    //void collisions();
    void draw(sf::RenderWindow& window);
};

void GameState::pause_game() {
    game_running = !game_running;
}

void GameState::update(float delta) {
    if(!game_running) return;

    player.move(delta);
    player.manage_spritesheet();
    player.animate();
}

void GameState::draw(sf::RenderWindow& window) {
    background.draw(window);
    world.draw(window);
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

// Keyboard input press handle
void handle (const sf::Event::KeyPressed& key, GameState& state) {
    if (!state.focus)
        return;

    switch (key.scancode) {
    case sf::Keyboard::Scancode::Escape:
        state.pause_game();
        break;
    case sf::Keyboard::Scancode::Left:
        state.player.holding_left = true;
        break;
    case sf::Keyboard::Scancode::Right:
        state.player.holding_right = true;
        break;
    case sf::Keyboard::Scancode::Space:
        state.player.holding_jump = true;
        break;
    default:
        return;
    }
}

// Keyboard input release handle
void handle (const sf::Event::KeyReleased& key, GameState& state) {
    if (!state.focus)
        return;

    switch (key.scancode) {
    case sf::Keyboard::Scancode::Left:
        state.player.holding_left = false;
        break;
    case sf::Keyboard::Scancode::Right:
        state.player.holding_right = false;
        break;
    case sf::Keyboard::Scancode::Space:
        state.player.holding_jump = false;
        break;
    default:
        return;
    }
}

// Generic handle
template <typename T>
void handle (T& event, GameState& state) {}

///////////
// Main //
///////////

int main() {
    // Create the window
    sf::RenderWindow window (sf::VideoMode ({window_width, window_height}), window_title);
    window.setFramerateLimit (max_frame_rate);
    window.setMinimumSize(window.getSize());
    
    // Center the window
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    window.setPosition({(static_cast<int>(desktopMode.size.x) - window_width)/2, (static_cast<int>(desktopMode.size.y) - window_height)/2});

    // Texture loading, Gamestate and clock
    Load();
    GameState state;
    sf::Clock game_clock;

    // Main loop
    while(window.isOpen()) {
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