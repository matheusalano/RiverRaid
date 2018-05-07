/*
 *  PlayState.cpp
 *  Normal "play" state
 *
 *  Created by Marcelo Cohen on 08/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include "Game.h"
#include "PlayState.h"
#include "InputManager.h"

PlayState PlayState::m_PlayState;

using namespace std;

void PlayState::init()
{
//    playSprite1.load("data/img/smurf.png", 128, 128, 5, 5, 5, 5, 6, 3, 16);
    playSprite1.load("data/img/spaceship.png");
    playSprite1.setPosition(369, 500);
    playSprite1.setFrameRange(0,15);
    playSprite1.setAnimRate(15);
    playSprite1.setLooped(true);
    playSprite1.pause();

    bulletSprite.load("data/img/bullet.png");
    bulletSprite.setPosition(400, 300);
    bulletSprite.setScale(0.5, 0.5);


    dirx = 0; // direção do sprite: para a direita (1), esquerda (-1)

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("stats", sf::Keyboard::S);
    im->addMouseInput("rightclick", sf::Mouse::Right);

    cout << "PlayState: Init" << endl;
}

void PlayState::cleanup()
{
    cout << "PlayState: Clean" << endl;
}

void PlayState::pause()
{
    cout << "PlayState: Paused" << endl;
}

void PlayState::resume()
{
    cout << "PlayState: Resumed" << endl;
}

void PlayState::handleEvents(cgf::Game* game)
{
    screen = game->getScreen();
    sf::Event event;

    while (screen->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            game->quit();
    }

    dirx = 0;

    if(im->testEvent("left")) {
        if(playSprite1.getPosition().x > 0) {
            dirx--;
        }
        if (!playSprite1.getMirror()) playSprite1.setMirror(true);
        if (playSprite1.isPaused()) playSprite1.play();
    }


    if(im->testEvent("right")) {
        if((playSprite1.getPosition().x + playSprite1.getSize().x) < screen->getSize().x) {
            dirx++;
        }
        if (playSprite1.getMirror()) playSprite1.setMirror(false);
        if (playSprite1.isPaused()) playSprite1.play();
    }

    if(im->testEvent("quit") || im->testEvent("rightclick"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();

    if(dirx == 0 && playSprite1.isPlaying()) playSprite1.pause();

    playSprite1.setXspeed(200*dirx);
}

void PlayState::update(cgf::Game* game)
{
    playSprite1.update(game->getUpdateInterval());
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    screen->draw(playSprite1);
    screen->draw(bulletSprite);
}
