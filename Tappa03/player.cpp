#include "header.hpp"

/////////////////
// Spritesheet //
/////////////////

Spritesheet::Spritesheet(std::string path, std::vector<sf::IntRect> frames, bool looping) {
    if (!texture.loadFromFile(path)) {
        std::cerr << "Errore nel caricamento di " << path << std::endl;
        exit(1);
    }
    this->frames = frames;
    this->looping = looping;
}

////////////
// Player //
////////////

Player::Player(sf::Vector2f pos, Spritesheet* sp) {
    position = pos;
    spritesheet = sp;
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