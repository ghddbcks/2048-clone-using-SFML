#include <SFML/Graphics.hpp>
#include <vector>
#include "Animation.h"
#include "const.h"

using namespace std;
using namespace sf;

AnimTarget::AnimTarget(Drawable* target, Transformable& trans, Vector2f goal, float speed, float accel) : trans(trans), goal(goal), speed(speed), done(false), target(target), accelation(accel), currentSpeed(0)
{
    dir = goal - trans.getPosition();
    dir = dir / sqrt(dir.x * dir.x + dir.y * dir.y);

    if (trans.getPosition() == goal)
    {
        done = true;
    }
}

void AnimTarget::update(float dt)
{
    if (!done)
    {
        trans.setPosition(trans.getPosition() + currentSpeed * dt * dir);
        Vector2f diffDir = goal - trans.getPosition();
        diffDir = diffDir / sqrt(diffDir.x * diffDir.x + diffDir.y * diffDir.y);
        if ((diffDir.x * dir.x + diffDir.y * dir.y) < 0)
        {
            done = true;
            trans.setPosition(goal);
        }

        if (currentSpeed < speed)
        {
            currentSpeed += accelation * dt;
        }
    }
}

bool AnimTarget::isDone()
{
    return done;
}

Drawable* AnimTarget::getDrawTarget()
{
    return target;
}

Animation::Animation(RenderWindow& window) : 
    targets(vector<AnimTarget>()), window(window), background(), texture(), done(false)
{
    texture.create(SWIDTH, SHEIGHT);
}

void Animation::addTarget(AnimTarget target)
{
    targets.push_back(target);
}

void Animation::start()
{
    texture.update(window);
    background.setTexture(texture);

    Clock clock;
    float dt;
    bool done = false;

    Event event;

    while (!(done || !window.isOpen()))
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;
            }
        }

        window.clear();

        window.draw(background);

        dt = clock.restart().asSeconds();

        done = true;
        for (AnimTarget& elem : targets)
        {
            elem.update(dt);
            done = done && elem.isDone();
            window.draw(*(elem.getDrawTarget()));
        }

        window.display();
    }
    targets = vector<AnimTarget>();
}

bool Animation::targetLeft()
{
    return !targets.empty();
}
