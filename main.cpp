#include <SFML/Graphics.hpp>
#include <deque>
#include <utility>
#include <iterator>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <Windows.h>

#define SNAKE 1
#define APPLE 2
#define EMPTY 0
#define ILLEGAL 3

#define BWIDTH 100
#define BHEIGHT 100
#define BMARGIN 2 // px

#define SWIDTH 1000
#define SHEIGHT 1000

#define DIED 2
#define GROWN 1
#define NOTHING 0

#define SPEED .25f // second per move

typedef int Square;

using namespace std;
using namespace sf;

class Board
{
private:
    Square board[BWIDTH][BHEIGHT];

public:
    Board() : board{ 0 } {};

    bool isLegal(int X, int Y)
    {
        return 0 <= X && X < BWIDTH && 0 <= Y && Y < BHEIGHT;
    }

    Square getSquare(int X, int Y)
    {
        return board[X][Y];
    }

    void setSquare(int X, int Y, Square value)
    {
        board[X][Y] = value;
    }
};

class Snake
{
private:
    deque<pair<int, int>> body; // first element is head, last element is tail
    int dirX;
    int dirY;
    Board &board;

public:
    Snake(int X, int Y, Board& board, int length = 5) : dirX(1), dirY(0), board(board)
    {
        for (int i = 0;i < length;i++)
        {
            board.setSquare(X + dirX * i, Y + dirY * i, SNAKE);
            body.push_front(pair<int, int>(X + dirX * i, Y + dirY * i));
        }
    }
    
    Snake(Board& board) : dirX(1), dirY(0), body(deque<pair<int, int>>()), board(board) {};

    Snake(const Snake& lhs) : dirX(lhs.dirX), dirY(lhs.dirY), board(lhs.board), body(lhs.body) {};

    Snake& operator=(const Snake& lhs)
    {
        dirX = lhs.dirX;
        dirY = lhs.dirY;
        board = lhs.board;
        body = lhs.body;
        return *this;
    }

    auto getBody()
    {
        return begin(body);
    }

    auto getHead()
    {
        return body.front();
    }

    int move()
    {
        auto head = body.front();
        auto tail = body.back();
        int goalX = head.first + dirX;
        int goalY = head.second + dirY;
        bool grow = false;

        if (not board.isLegal(goalX, goalY))
        {
            return DIED;
        }

        Square element = board.getSquare(goalX, goalY);
        if (element == SNAKE)
        {
            return DIED;
        }

        grow = element == APPLE;

        body.push_front(pair<int, int>(goalX, goalY));
        board.setSquare(goalX, goalY, SNAKE);

        if (not grow)
        {
            body.pop_back();
            board.setSquare(tail.first, tail.second, EMPTY);
            return NOTHING;
        }
        return GROWN;
    }

    void setXdir(int value)
    {
        dirX = value;
        dirY = 0;
    }

    void setYdir(int value)
    {
        dirY = value;
        dirX = 0;
    }
};

void newApple(Board& board)
{
    int i = 0;
    int j = 0;

    do
    {
        i = rand() % BWIDTH;
        j = rand() % BHEIGHT;
    } while (board.getSquare(i, j) != EMPTY);

    board.setSquare(i, j, APPLE);
}

void reset(Board& board, Snake& snake)
{
    board = Board();
    snake = Snake(5, BHEIGHT / 2 + 1, board);
    newApple(board);
}

int main()
{
    unsigned int t = GetTickCount64();
    srand(t);

    RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"크고♂아름다운♂게이ㅁ");

    Event event;

    Board board;
    Snake snake(board);
    reset(board, snake);

    Clock clock;

    int result = 0;

    vector<vector<RectangleShape>> squares = vector<vector<RectangleShape>>(BWIDTH, vector<RectangleShape>(BHEIGHT, RectangleShape()));
    for (int i = 0;i < BWIDTH;i++)
    {
        for (int j = 0;j < BHEIGHT;j++)
        {
            squares[i][j] = RectangleShape(Vector2f(SWIDTH / BWIDTH, SHEIGHT / BHEIGHT));
            squares[i][j].setFillColor(Color::Black);
            squares[i][j].setOutlineColor(Color::White);
            squares[i][j].setOutlineThickness(BMARGIN / 2);
            squares[i][j].setPosition(SWIDTH / BWIDTH * i, SHEIGHT / BHEIGHT * j);
        }
    }

    Color colorTheme[3] = { Color::Black, Color::Green, Color::Red }; // empty, snake, apple

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
                case Keyboard::W:
                    snake.setYdir(-1);
                    break;
                case Keyboard::S:
                    snake.setYdir(1);
                    break;
                case Keyboard::A:
                    snake.setXdir(-1);
                    break;
                case Keyboard::D:
                    snake.setXdir(1);
                    break;
                }
                break;

            case Event::KeyReleased:
                break;
            }
        }

        if (clock.getElapsedTime().asSeconds() > SPEED)
        {
            clock.restart();
            result = snake.move();

            switch (result)
            {
            case DIED:
                reset(board, snake);
                break;

            case GROWN:
                newApple(board);
            }
        }

        for (int i = 0;i < BWIDTH;i++)
        {
            for (int j = 0;j < BHEIGHT;j++)
            {
                squares[i][j].setFillColor(colorTheme[board.getSquare(i, j)]);
                window.draw(squares[i][j]);
            }
        }
        window.display();
    }
    return 0;
}