#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
using namespace sf;

class AnimTarget
{
public:
    AnimTarget(Drawable* target, Transformable& trans, Vector2f goal, float speed, float accel);

    void update(float dt);
    bool isDone();
    
    Drawable* getDrawTarget();

private:
    Drawable* target;
    Transformable& trans;
    Vector2f goal;
    float speed;
    float accelation;
    float currentSpeed;
    Vector2f dir;
    bool done;
};

class Animation
{
public:
    Animation(RenderWindow& window);

    void addTarget(AnimTarget target);
    void start();
    bool targetLeft();

private:
    vector<AnimTarget> targets;
    RenderWindow& window;
    Sprite background;
    Texture texture;
    bool done;
};

#endif