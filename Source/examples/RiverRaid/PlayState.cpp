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
    srand( time(NULL) );

    playSprite1.load("data/img/spaceship.png");
    playSprite1.setPosition(369, 430);
    playSprite1.setFrameRange(0,15);
    playSprite1.setAnimRate(15);
    playSprite1.setLooped(true);
    playSprite1.pause();

    fuelLeft = 1.0;
    sf::Vector2f fuelSize(300 * fuelLeft, 50);
    fuel.setSize(fuelSize);
    fuel.setPosition(250, 530);
    fuel.setFillColor(sf::Color::Green);

    sf::Vector2f fuelBGSize(320, 70);
    fuelBackground.setSize(fuelBGSize);
    fuelBackground.setPosition(240, 520);
    fuelBackground.setFillColor(sf::Color::Red);

     if (!font.loadFromFile("data/fonts/arial.ttf")) {
        cout << "Cannot load arial.ttf font!" << endl;
        exit(1);
    }
    fuelText.setFont(font);
    fuelText.setString("Fuel");
    fuelText.setCharacterSize(34);
    fuelText.setFillColor(sf::Color::Black);
    fuelText.setPosition(375, 533);
    fuelText.setStyle(sf::Text::Bold);

    lifesLeft = 3;

    lifesText.setFont(font);
    lifesText.setString(std::to_string(lifesLeft));
    lifesText.setCharacterSize(50);
    lifesText.setFillColor(sf::Color::White);
    lifesText.setPosition(190, 525);
    lifesText.setStyle(sf::Text::Bold);

    dirx = 0;

    im = cgf::InputManager::instance();

    im->addKeyInput("left", sf::Keyboard::Left);
    im->addKeyInput("right", sf::Keyboard::Right);
    im->addKeyInput("space", sf::Keyboard::Space);
    im->addKeyInput("quit", sf::Keyboard::Escape);
    im->addKeyInput("stats", sf::Keyboard::S);

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

        if(event.type == sf::Event::KeyPressed && im->testEvent("space")) {
            cgf::Sprite bulletSprite;
            bulletSprite.load("data/img/bullet.png");
            bulletSprite.setScale(0.5, 0.5);
            float xPos = playSprite1.getPosition().x + (playSprite1.getSize().x / 2) - (bulletSprite.getSize().x / 2) * 0.5;
            bulletSprite.setPosition(xPos, playSprite1.getPosition().y);
            bulletSprite.setYspeed(-200.0);
            bullets.push_back(bulletSprite);
        }
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

    if(im->testEvent("quit"))
        game->quit();

    if(im->testEvent("stats"))
        game->toggleStats();

    if(dirx == 0 && playSprite1.isPlaying()) playSprite1.pause();

    playSprite1.setXspeed(200*dirx);
}

void PlayState::update(cgf::Game* game)
{
    playSprite1.update(game->getUpdateInterval());

    if (fuelLeft <= 0) {
        fuelLeft = 1.0;
        lifesLeft--;
        if (lifesLeft < 0) game->quit();
        else lifesText.setString(std::to_string(lifesLeft));
    } else {
        fuelLeft -= 0.001;
    }
    sf::Vector2f fuelSize(300 * fuelLeft, 50);
    fuel.setSize(fuelSize);

    for(int i = 0; i < bullets.size(); i++) {
        bool isErased = false;
        for(int j = 0; j < enemies.size(); j++) {
            if (bullets[i].bboxCollision(enemies[j])) {
                bullets.erase(bullets.begin() + i);
                enemies.erase(enemies.begin() + j);
                isErased = true;
                break;
            }
        }
        if (isErased) continue;

        for(int k = 0; k < fuelTanks.size(); k++) {
            if (bullets[i].bboxCollision(fuelTanks[k])) {
                bullets.erase(bullets.begin() + i);
                fuelTanks.erase(fuelTanks.begin() + k);
                isErased = true;
                break;
            }
        }
        if (isErased) continue;

        if (bullets[i].getPosition().y <= 0) {
            bullets.erase(bullets.begin() + i);
        } else {
            bullets[i].update(game->getUpdateInterval());
        }
    }

    for(int i = 0; i < enemies.size(); i++) {
        if (playSprite1.circleCollision(enemies[i])) {
            enemies.erase(enemies.begin() + i);
            lifesLeft--;
            if (lifesLeft < 0) game->quit();
            else lifesText.setString(std::to_string(lifesLeft));
        } else if (enemies[i].getPosition().y + enemies[i].getSize().y >= 600) {
            enemies.erase(enemies.begin() + i);
        } else if (enemies[i].getPosition().x <= 100) {
            enemies[i].setMirror(false);
            enemies[i].setXspeed(100.0);
        } else if (enemies[i].getPosition().x + enemies[i].getSize().x >= 700) {
            enemies[i].setMirror(true);
            enemies[i].setXspeed(-100.0);
        }
        enemies[i].update(game->getUpdateInterval());
    }

    for(int i = 0; i < fuelTanks.size(); i++) {
        if(playSprite1.circleCollision(fuelTanks[i])) {
            fuelTanks.erase(fuelTanks.begin() + i);
            fuelLeft = min(fuelLeft + 0.2, 1.0);
            sf::Vector2f fuelSize(300 * fuelLeft, 50);
            fuel.setSize(fuelSize);
        } else if (fuelTanks[i].getPosition().y + fuelTanks[i].getSize().y >= 600) {
            fuelTanks.erase(fuelTanks.begin() + i);
        }
        fuelTanks[i].update(game->getUpdateInterval());
    }

    if (enemies.size() <= 3 && randomNumber(1, 500) <= 5) {
        enemies.push_back(createEnemy());
    }
    if (fuelTanks.size() <= 1 && randomNumber(1,500) <= 3) {
        fuelTanks.push_back(createFuelTank());
    }
}

void PlayState::draw(cgf::Game* game)
{
    screen = game->getScreen();
    screen->draw(playSprite1);

    for(int i = 0; i < bullets.size(); i++) {
        screen->draw(bullets[i]);
    }
    for(int i = 0; i < enemies.size(); i++) {
        screen->draw(enemies[i]);
    }
    for(int i = 0; i < fuelTanks.size(); i++) {
        screen->draw(fuelTanks[i]);
    }

    screen->draw(fuelBackground);
    screen->draw(fuel);
    screen->draw(fuelText);
    screen->draw(lifesText);
}

int PlayState::randomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}

cgf::Sprite PlayState::createEnemy() {
    cgf::Sprite enemySprite;
    string enemyFilePath = "data/img/enemy" + std::to_string(randomNumber(1, 3)) + ".png";
    enemySprite.load(enemyFilePath.c_str());
    float xPos = randomNumber(150, 600);
    enemySprite.setPosition(xPos, 5);
    enemySprite.setYspeed(30.0);
    enemySprite.setXspeed(100.0);
    return enemySprite;
}

cgf::Sprite PlayState::createFuelTank() {
    cgf::Sprite fuelSprite;
    fuelSprite.load("data/img/fuel.png");
    float xPos = randomNumber(150, 600);
    fuelSprite.setPosition(xPos, 5);
    fuelSprite.setYspeed(40.0);
    return fuelSprite;
}
