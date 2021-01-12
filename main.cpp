#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <vector>
#include <functional>
#include "ConnectionIii-Rj3W.hpp"
#include "nanum.hpp"
#include "blur.hpp"

#define SWIDTH 800
#define SHEIGHT 800

#define BWIDTH 4
#define BHEIGHT 4

using namespace std;
using namespace sf;

class Target
{
private:
    typedef function<void(float)> Setter;
    typedef function<float(void)> Getter;
    Drawable* target;
    Getter& getter;
    Setter& setter;
    float goal;
    float speed;
    float dir;
    bool done;
public:
    Target(Drawable* target, Getter& getter, Setter& setter, float goal, float speed) : 
        getter(getter), setter(setter), goal(goal), speed(speed), done(false) 
    {
        dir = speed / abs(speed);
    };

    void update(float dt)
    {
        setter(getter() + speed * dt);
        float diff = goal - getter();
        if (diff / abs(diff) != dir)
        {
            done = true;
            setter(goal);
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
    Animation(RenderWindow& window) : targets(), window(window), background(), texture(), done(false) {};
    void addTarget(Target target)
    {
        targets.push_back(target);
    }
    void ready()
    {
        texture.update(window);
    }

    void Start()
    {
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
                elem.update(dt);
                done = done && elem.isDone();
                window.draw(*elem.getDrawTarget());
            }

            window.display();
        }
    }

    bool targetLeft()
    {
        return targets.empty();
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

    RectangleShape rect(Vector2f(SWIDTH / BWIDTH, SHEIGHT / BHEIGHT));
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

    auto newNum = [&]()
    {
        int X;
        int Y;

        do
        {
            X = rand() % BWIDTH;
            Y = rand() % BHEIGHT;
        } while (board[X][Y] != 0);
        board[X][Y] = 2;
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
                if (!moving[X][Y])
                {
                    rect.setPosition(SWIDTH / BWIDTH * X, SHEIGHT / BHEIGHT * Y);
                    window.draw(rect);

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
            bool changed;
            do
            {
                changed = false;
                for (int X = 0; X < BWIDTH; X++)
                {
                    for (int Y = 0; Y < BHEIGHT; Y++)
                    {
                        if (!(X + Xmove == -1 || X + Xmove == BWIDTH || Y + Ymove == -1 || Y + Ymove == BHEIGHT))
                        {
                            if ( (board[X + Xmove][Y + Ymove] == board[X][Y] || !board[X + Xmove][Y + Ymove]) && board[X][Y])
                            {
                                int temp = board[X][Y];
                                board[X][Y] = 0;
                                board[X + Xmove][Y + Ymove] += temp;
                                changed = true;
                            }
                        }
                    }
                }
            } while (changed);

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
                    if (board[X][Y] == 0)
                    {
                        reset();
                        goto top;
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