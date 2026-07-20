//////////////
// includes //
//////////////

#include <SFML/Graphics.hpp>

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
const sf::Texture idle("assets/sprites/player/idle.png");

////////////
// Player //
////////////

struct Player {
    sf::Vector2f position;
    float speed = 300;      //In pixels per seconds
    float life = 100;

    sf::Texture spritesheet = idle;
    int row = 0;
    int col = 0;
    int missing = 3;
    sf::Vector2i sprite_dim = {128, 128};
    sf::Clock clock;

    Player(sf::Vector2f pos);
    void animate();
    void draw(sf::RenderWindow& window);
};

Player::Player(sf::Vector2f pos) {
    position = pos;
}

void Player::animate() {
    if (clock.getElapsedTime().asSeconds() > 0.1) {
        if (col == static_cast<int>(spritesheet.getSize().x)/sprite_dim.x - 1 || (row == static_cast<int>(spritesheet.getSize().y)/sprite_dim.y - 1 && col == static_cast<int>(spritesheet.getSize().x)/sprite_dim.x - missing - 1)) {
            col = 0;
            row = (row + 1) % (static_cast<int>(spritesheet.getSize().y) / sprite_dim.y);
        }
        else {
            col++;
        }
        clock.restart();
    }
}

void Player::draw(sf::RenderWindow& window) {
    sf::IntRect section({col*sprite_dim.x, row*sprite_dim.y}, {sprite_dim});
    sf::Sprite sprite(spritesheet, section);
    sprite.setOrigin({static_cast<float>(sprite_dim.x)/2, static_cast<float>(sprite_dim.y)});
    sprite.setPosition(position);
    window.draw(sprite);
}

///////////////
// GameState //
///////////////

struct GameState {
    Player player = Player(player_starting_position);
    bool is_running = false;

    //void pause_game();
    //void reset_game();
    //void game_over();
    void update(float delta);
    //void collisions();
    void draw(sf::RenderWindow& window);
};

void GameState::update(float delta) {
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
        state.update(game_clock.getElapsedTime().asSeconds());

        // Clear, draw, display
        window.clear(sf::Color::Black);
        state.draw(window);
        window.display();
    }
}