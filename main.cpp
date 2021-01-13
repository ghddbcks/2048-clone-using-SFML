#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <vector>
#include <functional>
#include "ConnectionIii-Rj3W.hpp"
#include "nanum.hpp"
#include "blur.hpp"

#define SWIDTH 600
#define SHEIGHT 600

#define BWIDTH 4
#define BHEIGHT 4

const float GWIDTH = SWIDTH / BWIDTH;
const float GHEIGHT = SHEIGHT / BHEIGHT;

const float speed = 10000.f;

using namespace std;
using namespace sf;

class Target
{
private:
    Drawable* target;
    Transformable& trans;
    Vector2f goal;
    float speed;
    Vector2f dir;
    bool done;
public:
    Target(Drawable* target, Transformable& trans, Vector2f goal, float speed) : 
        trans(trans), goal(goal), speed(speed), done(false), target(target)
    {
        dir = goal - trans.getPosition();
        dir = dir / sqrt(dir.x * dir.x + dir.y * dir.y);
    };

    void update(float dt)
    {
        trans.setPosition(trans.getPosition() + speed * dt * dir);
        Vector2f diffDir = goal - trans.getPosition();
        diffDir = diffDir / sqrt(diffDir.x * diffDir.x + diffDir.y * diffDir.y);
        if ((diffDir.x * dir.x + diffDir.y * dir.y) < 0)
        {
            done = true;
            trans.setPosition(goal);
        }
    }
    bool isDone() 
    {
        return done;
    }
    Drawable* getDrawTarget()
    {
        return target;
    }
};

class Animation
{
private:
    vector<Target> targets;
    RenderWindow& window;
    Sprite background;
    Texture texture;
    bool done;

public:
    Animation(RenderWindow& window) : targets(), window(window), background(), texture(), done(false) 
    {
        texture.create(SWIDTH, SHEIGHT);
    }

    void addTarget(Target target)
    {
        targets.push_back(target);
    }

    void start()
    {
        texture.update(window);
        background.setTexture(texture);

        Clock clock;
        float dt;
        bool done = false;

        while (!done)
        {
            window.clear();

            window.draw(background);

            dt = clock.restart().asSeconds();

            done = true;
            for (auto& elem : targets)
            {
                //window.clear();
                elem.update(dt);
                done = done && elem.isDone();
                window.draw(*(elem.getDrawTarget()));
                //window.display();
            }

            window.display();
        }
        targets = vector<Target>();
    }

    bool targetLeft()
    {
        return !targets.empty();
    }
};

int main()
{
    unsigned int t = GetTickCount64();
    srand(t);

    RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"크고♂아름다운♂게이ㅁ");

    Event event;

    Clock clock;

    vector<vector<int>> board;
    
    int Xmove;
    int Ymove;

    Font font1;
    if (!font1.loadFromMemory(ConnectionIii_Rj3W_otf, ConnectionIii_Rj3W_otf_len))
    {
        cout << "Error while loading font" << endl;
    }

    Font font2;
    if (!font2.loadFromMemory(NanumGothic_ttf, NanumGothic_ttf_len))
    {
        cout << "Error while loading font" << endl;
    }

    RectangleShape rect(Vector2f(GWIDTH, GHEIGHT));
    rect.setFillColor(Color::White);
    rect.setOutlineColor(Color::Yellow);
    rect.setOutlineThickness(2);

    Text text;
    text.setFillColor(Color::Red);
    text.setCharacterSize(SHEIGHT / BHEIGHT / 4);
    text.setFont(font1);

    Text wonText;
    wonText.setFillColor(Color::Black);
    wonText.setCharacterSize(48);
    wonText.setFont(font2);
    wonText.setPosition(SWIDTH/2, SHEIGHT/2);
    wonText.setString(L"당신 승리자이다!\n눌러 R키를 재시작!");
    wonText.setOrigin(wonText.getLocalBounds().width / 2, wonText.getLocalBounds().height / 2);

    int Xfrom;
    int Yfrom;
    int Xto;
    int Yto;
    int Xsign;
    int Ysign;

    bool won;

    Texture texture;
    if (!texture.create(SWIDTH, SHEIGHT))
    {
        cout << "Error!" << endl;
    }

    Shader shader;
    if (!shader.loadFromMemory(frag, Shader::Fragment))
    {
        cout << "Error!" << endl;
    }

    Sprite sprite(texture);

    vector<vector<bool>> moving;

    Animation animation(window);

    auto newNum = [&]()
    {
        int X;
        int Y;
        pair<int, int> newSqr;
        vector<pair<int, int>> list;

        for (int X = 0;X < BWIDTH;X++)
        {
            for (int Y = 0;Y < BHEIGHT;Y++)
            {
                if (!board[X][Y])
                {
                    list.push_back(make_pair(X, Y));
                }
            }
        }

        if (!list.size())
        {
            return;
        }

        newSqr = list[rand() % list.size()];
        X = newSqr.first;
        Y = newSqr.second;

        if (rand() % 100 < 10)
        {
            board[X][Y] = 4;
        }
        else
        {
            board[X][Y] = 2;
        }
    };

    auto reset = [&]()
    {
        board = vector<vector<int>>(BWIDTH, vector<int>(BHEIGHT, 0));
        
        Xmove = 0;
        Ymove = 0;

        won = false;

        moving = vector<vector<bool>>(BWIDTH, vector<bool>(BHEIGHT, false));

        newNum();
    };

    auto draw = [&]()
    {
        for (int X = 0; X < BWIDTH; X++)
        {
            for (int Y = 0; Y < BHEIGHT; Y++)
            {
                    rect.setPosition(SWIDTH / BWIDTH * X, SHEIGHT / BHEIGHT * Y);
                    window.draw(rect);
                if (!moving[X][Y])
                {
                    text.setPosition(SWIDTH / BWIDTH * (X + .5f), SHEIGHT / BHEIGHT * (Y + .5f));
                    text.setString(board[X][Y] ? to_string(board[X][Y]) : "");
                    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
                    window.draw(text);
                }
            }
        }
    };

    reset();

    while (window.isOpen()) 
    {
        top:

        Xmove = 0;
        Ymove = 0;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;
                
            case Event::KeyPressed:
                switch (event.key.code)
                {
                case Keyboard::Left:
                    Xmove = -1;
                    Ymove = 0;
                    break;
                case Keyboard::Right:
                    Xmove = 1;
                    Ymove = 0;
                    break;
                case Keyboard::Up:
                    Xmove = 0;
                    Ymove = -1;
                    break;
                case Keyboard::Down:
                    Xmove = 0;
                    Ymove = 1;
                    break;
                case Keyboard::R:
                    reset();
                    break;
                }
                break;
            }
        }

        if (won)
        {
            texture.update(window);
            shader.setUniform("texture", texture);
            shader.setUniform("blur_radius", 0.002f);
            sprite.setTexture(texture);

            while (window.isOpen())
            {
                while (window.pollEvent(event))
                {
                    switch (event.type)
                    {
                    case Event::Closed:
                        window.close();
                        break;

                    case Event::KeyPressed:
                        switch (event.key.code)
                        {
                        case Keyboard::R:
                            reset();
                            goto top;
                            break;
                        }
                        break;
                    }
                }

                window.clear();

                window.draw(sprite, &shader);

                window.draw(wonText);

                window.display();
            }
        }

        if (Xmove || Ymove)
        {
            vector<RectangleShape*> rects;
            vector<Text*> texts;
            Animation ani(window);

            auto addAni = [&](int X, int Y, int Xmove, int Ymove)
            {
                rects.push_back(new RectangleShape(Vector2f(GWIDTH, GHEIGHT)));
                rects.back()->setPosition(GWIDTH * X, GHEIGHT * Y);
                rects.back()->setFillColor(Color::White);
                rects.back()->setOutlineColor(Color::Yellow);
                rects.back()->setOutlineThickness(2);

                texts.push_back(new Text());
                texts.back()->setFillColor(Color::Red);
                texts.back()->setCharacterSize(GHEIGHT / 4);
                texts.back()->setFont(font1);
                texts.back()->setPosition(GWIDTH * (X + .5f), GHEIGHT * (Y + .5f));
                texts.back()->setString(to_string(board[X][Y]));
                texts.back()->setOrigin(texts.back()->getLocalBounds().width / 2, texts.back()->getLocalBounds().height / 2);

                ani.addTarget(Target(rects.back(), *rects.back(), Vector2f(GWIDTH * (X + Xmove), GHEIGHT * (Y + Ymove)), speed));
                ani.addTarget(Target(texts.back(), *texts.back(), Vector2f(GWIDTH * (X + Xmove + .5f), GHEIGHT * (Y + Ymove + .5f)), speed));
            };

            auto runAni = [&]()
            {
                window.clear();
                draw();

                ani.start();

                while (!rects.empty())
                {
                    delete rects.back();
                    rects.pop_back();
                }
                while (!texts.empty())
                {
                    delete texts.back();
                    texts.pop_back();
                }
            };

            auto slide = [&]()
            {
                bool changed = false;
                vector<vector<int>> temp(BWIDTH, vector<int>(BHEIGHT, 0));

                do
                {
                    changed = false;
                    temp = vector<vector<int>>(BWIDTH, vector<int>(BHEIGHT, 0));

                    for (int X = 0;X < BWIDTH;X++)
                    {
                        for (int Y = 0;Y < BHEIGHT;Y++)
                        {
                            if (X + Xmove != -1 && X + Xmove != BWIDTH && Y+Ymove != -1 && Y+Ymove != BHEIGHT && !board[X + Xmove][Y + Ymove] &&
                                board[X][Y] && !temp[X+Xmove][Y+Ymove])
                            {
                                addAni(X, Y, Xmove, Ymove);

                                changed = true;
                                temp[X + Xmove][Y + Ymove] = board[X][Y];
                                board[X][Y] = 0;
                            }
                            else if (!temp[X][Y])
                            {
                                temp[X][Y] = board[X][Y];
                            }
                        }
                    }

                    if (ani.targetLeft())
                    {
                        runAni();
                    }

                    board = vector<vector<int>>(temp);
                } while (changed);
            };

            auto pop = [&]()
            {
                vector<vector<int>> temp(BWIDTH, vector<int>(BHEIGHT, 0));

                for (int X = 0;X < BWIDTH;X++)
                {
                    for (int Y = 0;Y < BHEIGHT;Y++)
                    {
                        if (X + Xmove != -1 && X + Xmove != BWIDTH && Y + Ymove != -1 && Y + Ymove != BHEIGHT &&
                            board[X + Xmove][Y + Ymove] == board[X][Y] && board[X][Y])
                        {
                            addAni(X, Y, Xmove, Ymove);

                            temp[X+Xmove][Y+Ymove] = board[X + Xmove][Y + Ymove] + board[X][Y];
                            board[X][Y] = 0;
                        }
                        else if (!temp[X][Y])
                        {
                            temp[X][Y] = board[X][Y];
                        }
                    }
                }
                if (ani.targetLeft())
                {
                    runAni();
                }

                board = vector<vector<int>>(temp);
            };

            slide();
            pop();
            slide();

            newNum();

            won = false;
            for (int X = 0; X < BWIDTH; X++)
            {
                for (int Y = 0; Y < BHEIGHT; Y++)
                {
                    if (board[X][Y] >= 2048)
                    {
                        won = true;
                        break;
                    }
                }
            }
        }


        window.clear();

        draw();

        window.display();

    }
    return 0;
}