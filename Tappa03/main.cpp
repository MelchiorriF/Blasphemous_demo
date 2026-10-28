//////////////
// includes //
//////////////

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

//////////////////////
// Global constants //
//////////////////////

// Window consts
const char* window_title = "Blasphemous demo";
const int window_width = 1280;
const int window_height = 864;
const float max_frame_rate = 60;

// Physics consts
const float gravity = 1;
const float ground = 715;

// Player consts
const sf::Vector2f player_starting_position = {window_width/2, ground};

//Background consts
sf::Texture background;
const sf::IntRect bg_dim({6,4}, {640,432});

// World consts
sf::Texture tileset;
const sf::IntRect small_platform({0,228}, {75,48});
const sf::IntRect big_platform({203,304}, {142,48});

const int num_platforms = 2;
const std::vector<bool> platform_types = {false, true};
const std::vector<sf::Vector2f> platforms_positions = {{0,0}, {500,500}};

/////////////////////
// Texture loading //
/////////////////////

void Load() {
    if(!background.loadFromFile("assets/sprites/other/background1.png")) {
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
    : bg_sprite(background) { // Member Initializer List (non esiste costruttore vuoto per sf::Sprite)
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
    : sprite(tileset) { // Member Initializer List (non esiste costruttore vuoto per sf::Sprite)
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
    for (int i = 0; i < num_platforms; i++) {
        platforms.push_back(Platform(platform_types[i], platforms_positions[i]));
    }
}

void World::draw(sf::RenderWindow& window) {
    for (auto& p : platforms)
        p.draw (window);
}

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

Spritesheet::Spritesheet(std::string path, std::vector<sf::IntRect> frames, bool looping) {
    if (!texture.loadFromFile(path)) {
        std::cerr << "Errore nel caricamento di " << path << std::endl;
        exit(1);
    }
    this->frames = frames;
    this->looping = looping;
}

Spritesheet idle = Spritesheet("assets/sprites/player/idle.png", {
    sf::IntRect({17,40}, {67,72}),
        //sf::IntRect({145,40}, {67,72}),
            //sf::IntRect({273,40}, {67,72}),
        //sf::IntRect({401,40}, {67,72}),
    sf::IntRect({529,40}, {67,72}),
        //sf::IntRect({657,40}, {67,72}),
            //sf::IntRect({785,40}, {67,72}),
        //sf::IntRect({913,40}, {67,72}),
    sf::IntRect({17,168}, {67,72}),
        //sf::IntRect({145,168}, {67,72}),
            //sf::IntRect({273,168}, {67,72}),
        //sf::IntRect({401,168}, {67,72}),
    //sf::IntRect({529,168}, {67,72})
}, true);

Spritesheet walking = Spritesheet("assets/sprites/player/walking.png", {
            //sf::IntRect({0,186}, {77,70}),
        //sf::IntRect({223,191}, {76,65}),
    sf::IntRect({78,51}, {72,67}),
        //sf::IntRect({150,121}, {72,67}),
            //sf::IntRect({0,119}, {77,67}),
        //sf::IntRect({0,52}, {77,67}),
    sf::IntRect({150,188}, {73,68}),
        //sf::IntRect({77,118}, {72,69}),
            //sf::IntRect({223,126}, {72,65}),
        //sf::IntRect({366,190}, {68,66}),
    sf::IntRect({223,59}, {72,67}),
        //sf::IntRect({299,190}, {67,66}),
            //sf::IntRect({150,54}, {72,67})
        //sf::IntRect({78,187}, {72,69})
}, true);

Spritesheet jumping = Spritesheet("assets/sprites/player/jumping.png", {
    sf::IntRect({294,58}, {46,79}),
        //sf::IntRect({374,115}, {43,75}),
            //sf::IntRect({225,47}, {47,72}),
        //sf::IntRect({225,119}, {51,68}),
    sf::IntRect({158,48}, {63,64}),
        //sf::IntRect({294,137}, {80,53}),
            //sf::IntRect({365,202}, {81,54}),
        //sf::IntRect({78,191}, {80,64}),
    sf::IntRect({0,41}, {67,73}),
        //sf::IntRect({158,184}, {67,72}),
            //sf::IntRect({78,119}, {67,72}),
        //sf::IntRect({158,112}, {67,72}),
    sf::IntRect({78,47}, {67,72}),
        //sf::IntRect({0,114}, {67,73})
}, false);

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

    Spritesheet* spritesheet = &idle;
    sf::Clock animation_clock;

    Player(sf::Vector2f pos);

    void move(float delta);
    void start_jump();
    void manage_spritesheet();
    void animate();
    void draw(sf::RenderWindow& window);
};

Player::Player(sf::Vector2f pos) {
    position = pos;
}

void Player::move(float delta) {
    if (position.y < ground) {
        on_ground = false;
        velocity_y += gravity;
    }
    else {
        on_ground = true;
        position.y = ground;
    }

    position.y += velocity_y;

    if (holding_left) position.x -= speed*delta;
    if (holding_right) position.x += speed*delta;
    if (holding_jump && on_ground) start_jump();
}

void Player::start_jump() {
    if (on_ground) {
        spritesheet = &jumping;
        spritesheet->current_frame = 0;
        velocity_y = -20;
    }
}

void Player::manage_spritesheet() {
    if (on_ground) {
        if ((holding_left && !holding_right) || (!holding_left && holding_right)) spritesheet = &walking;
        else spritesheet = &idle;
    }
    else {
        spritesheet = &jumping;
    }
}

void Player::animate() {
    if (holding_left && !holding_right) direction = -1;
    else if (!holding_left && holding_right) direction = 1;
    
    if (animation_clock.getElapsedTime().asSeconds() > 0.25) {
        if (spritesheet->looping) {
            spritesheet->current_frame = (spritesheet->current_frame + 1) % spritesheet->frames.size();
        }
        else if (spritesheet->current_frame < spritesheet->frames.size()-1) {
            spritesheet->current_frame++;
        }
        animation_clock.restart();
    }
}

void Player::draw(sf::RenderWindow& window) {
    sf::Sprite sprite(spritesheet->texture, spritesheet->frames[spritesheet->current_frame]);
    if (direction == -1) sprite.setScale({-2.f, 2.f});
    else sprite.setScale({2.f, 2.f});
    sprite.setOrigin({spritesheet->frames[spritesheet->current_frame].size.x / 2.f, static_cast<float>(spritesheet->frames[spritesheet->current_frame].size.y)});
    sprite.setPosition(position);
    window.draw(sprite);
}

///////////////
// GameState //
///////////////

struct GameState {
    Player player = Player(player_starting_position);
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

// Keyboard input handle
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

    // Texture loading, Gamestate and clock
    Load();
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