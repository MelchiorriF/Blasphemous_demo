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
const int window_width = 1366;
const int window_height = 768;
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

    Spritesheet(std::string path, std::vector<sf::IntRect> frames);
};

Spritesheet::Spritesheet(std::string path, std::vector<sf::IntRect> frames) {
    if (!texture.loadFromFile(path)) {
        std::cout << "Errore nel caricamento di " << path <<"\n";
    }
    this->frames = frames;
}

Spritesheet idle = Spritesheet("assets/sprites/player/idle.png", {
    sf::IntRect({17,40}, {67,72}),
    //sf::IntRect({145,40}, {67,72}),
    sf::IntRect({273,40}, {67,72}),
    //sf::IntRect({401,40}, {67,72}),
    sf::IntRect({529,40}, {67,72}),
    //sf::IntRect({657,40}, {67,72}),
    sf::IntRect({785,40}, {67,72}),
    //sf::IntRect({913,40}, {67,72}),
    sf::IntRect({17,168}, {67,72}),
    //sf::IntRect({145,168}, {67,72}),
    sf::IntRect({273,168}, {67,72}),
    //sf::IntRect({401,168}, {67,72}),
    sf::IntRect({529,168}, {67,72})
});

Spritesheet walking = Spritesheet("assets/sprites/player/walking.png", {
    sf::IntRect({0,186}, {77,70}),
    //sf::IntRect({223,191}, {76,65}),
    sf::IntRect({78,51}, {72,67}),
    //sf::IntRect({150,121}, {72,67}),
    sf::IntRect({0,119}, {77,67}),
    //sf::IntRect({0,52}, {77,67}),
    sf::IntRect({150,188}, {73,68}),
    //sf::IntRect({77,118}, {72,69}),
    sf::IntRect({223,126}, {72,65}),
    //sf::IntRect({366,190}, {68,66}),
    sf::IntRect({223,59}, {72,67}),
    //sf::IntRect({299,190}, {67,66}),
    sf::IntRect({150,54}, {72,67})
    //sf::IntRect({78,187}, {72,69})
});

////////////
// Player //
////////////

struct Player {
    sf::Vector2f position;
    float speed = 200;      //In pixels per seconds
    float life = 100;

    bool go_left = false;
    bool go_right = false;

    Spritesheet* spritesheet = &idle;
    sf::Clock animation_clock;

    int direction = 1;      // 1 when facing right, -1 when facing left

    Player(sf::Vector2f position);

    void move(float delta);
    void change_spritesheet();
    void animate();
    void draw(sf::RenderWindow& window);
};

Player::Player(sf::Vector2f position) {
    this->position = position;
}

void Player::move(float delta) {
    if (go_left) position.x -= speed*delta;
    if (go_right) position.x += speed*delta;
}

void Player::change_spritesheet() {
    if ((go_left && !go_right) || (!go_left && go_right)) spritesheet = &walking;
    else spritesheet = &idle;
}

void Player::animate() {
    if (go_left && !go_right) direction = -1;
    else if (!go_left && go_right) direction = 1;
    
    if (animation_clock.getElapsedTime().asSeconds() > 0.1) {
        spritesheet->current_frame = (spritesheet->current_frame + 1) % spritesheet->frames.size();
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
    player.change_spritesheet();
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
    case sf::Keyboard::Scancode::Space:
        state.pause_game();
        break;
    case sf::Keyboard::Scancode::Left:
        state.player.go_left = true;
        break;
    case sf::Keyboard::Scancode::Right:
        state.player.go_right = true;
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
        state.player.go_left = false;
        break;
    case sf::Keyboard::Scancode::Right:
        state.player.go_right = false;
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