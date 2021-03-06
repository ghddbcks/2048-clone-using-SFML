#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <functional>
#include <ctime>
#include "ConnectionIii-Rj3W.hpp"
#include "nanum.hpp"
#include "blur.hpp"
#include "Animation.h"
#include "const.h"

const float GWIDTH = SWIDTH / BWIDTH;
const float GHEIGHT = GWIDTH;

const float speed = 1000.f; // animation speed
const float accel = 5000.f; // animation accel

using namespace std;
using namespace sf;

// color scheme
const Color numberClr = Color(0xD77256ff);
const int colorCnt = 5;
const Color tileClr[colorCnt] = { Color(0xF0D0C8ff), Color(0xE3D0B3ff), Color(0xCCE3B3ff), Color(0xFAF5D2ff), Color(0xC8FDDAff) };
const Color blkTilClr = Color(0xffffffff);
const Color borderClr = Color(0xD2E0D1ff);
const Color buttonClr = Color(0x35505eff);
const Color textClr = Color(0x171C3aff);
const Color backgroundClr = Color(0xd2d0d1ff);

int lb/*lg is incorrect.*/(int powerOfTwo) // lb(2^n) = n
{
	int temp = powerOfTwo;
	int i = 0;
	while (temp != 1)
	{
		temp = temp >> 1;
		i++;
	}
	return i;
}

void setRect(RectangleShape& rect, int X, int Y, int number)
{
	rect.setPosition(GWIDTH * X, GHEIGHT * Y);
	rect.setFillColor(number ? tileClr[lb(number) % colorCnt] : blkTilClr);
	// select color from color list depending on the number on it.
}

void setText(Text& text, String string, int X, int Y)
{
	text.setPosition(GWIDTH * (X + .5f), GHEIGHT * (Y + .5f));
	text.setString(string);
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
}

void resetText(Text& text, Font& font1)
{
	text.setFillColor(textClr);
	text.setCharacterSize(GHEIGHT / 4);
	text.setFont(font1);
}

void resetRect(RectangleShape& rect)
{
	rect.setOutlineColor(borderClr);
	rect.setOutlineThickness(BORDER);
}

int main()
{
	unsigned int t = time(NULL);
	srand(t);

	RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"2048");

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
	resetRect(rect);

	RectangleShape back(Vector2f(SWIDTH, SHEIGHT));
	back.setFillColor(backgroundClr);

	Text text;
	resetText(text, font1);

	Text wonText;
	wonText.setFillColor(textClr);
	wonText.setCharacterSize(48);
	wonText.setFont(font2);
	wonText.setPosition(SWIDTH / 2, SWIDTH / 2);
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

	int btnXScale = .5f;
	int btnYScale = .5f;

	RectangleShape resetButton(Vector2f(BTWIDTH, BTHEIGHT));
	resetButton.setPosition(SWIDTH / 2 - BTWIDTH / 2, SWIDTH + (SHEIGHT - SWIDTH) / 2 - BTHEIGHT / 2);
	resetButton.setFillColor(buttonClr);

	Text resetBtnTxt;
	resetBtnTxt.setFillColor(textClr);
	resetBtnTxt.setFont(font2);
	resetBtnTxt.setString(L"재시작");
	resetBtnTxt.setCharacterSize(50);
	resetBtnTxt.setOrigin(resetBtnTxt.getLocalBounds().width / 2, resetBtnTxt.getLocalBounds().height / 2);
	resetBtnTxt.setPosition(SWIDTH / 2, SWIDTH + (SHEIGHT - SWIDTH) / 2);

	Vector2f mousePos;

	int score;

	Text scoreText;
	scoreText.setFillColor(textClr);
	scoreText.setFont(font1);
	scoreText.setString("0");
	scoreText.setCharacterSize(50);

	auto newNum = [&]()
		// place new number on random empty location. 
		// for prevent infinite-loop, all location in board that is empty is precalculated, and randomly pick one of these.
	{
		int X;
		int Y;
		pair<int, int> newSqr;
		vector<pair<int, int>> list;

		for (int X = 0;X < BWIDTH;X++)
		{
			for (int Y = 0;Y < BHEIGHT;Y++)
			{
				if (!board[X][Y]) // if (X, Y) location is empty (0)
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

		if (rand() % 100 < 10) // 4 spawns for 10% chance
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

		score = 0;
	};

	auto draw = [&](RectangleShape& rect, Text& text, bool drawTile = true)
	{
		window.draw(back);

		for (int X = 0; X < BWIDTH; X++)
		{
			for (int Y = 0; Y < BHEIGHT; Y++)
			{
				setRect(rect, X, Y, drawTile ? board[X][Y] : 0);
				window.draw(rect);
				if (!moving[X][Y])
				{
					setText(text, board[X][Y] && drawTile ? to_string(board[X][Y]) : "", X, Y);
					window.draw(text);
				}
			}
		}

		window.draw(resetButton);
		window.draw(resetBtnTxt);
		window.draw(scoreText);
	};

	auto addScore = [&](int dS)
	{
		score += dS;
		scoreText.setString(to_string(score));
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
					// always Xmove == 0 && Ymove == 0 or if Xmove != 0 then Ymove == 0, and if Ymove != 0 then Xmove == 0
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

			case Event::MouseMoved:
				mousePos = static_cast<Vector2f>(Mouse::getPosition(window));
				break;

			case Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case Mouse::Button::Left:
					if (resetButton.getGlobalBounds().contains(mousePos))
					{
						reset();
					}
				}
				break;
			}
		}

		won = false;
		for (int X = 0;X < BWIDTH;X++)
		{
			for (int Y = 0;Y < BHEIGHT;Y++)
			{
				if (board[X][Y] >= WINNMBR)
				{
					won = true;
				}
			}
		}

		if (Xmove || Ymove)
		{
			vector<RectangleShape*> rects;
			vector<Text*> texts;
			Animation ani(window);

			auto addAni = [&](int X, int Y, int Xdest, int Ydest)
			{
				rects.push_back(new RectangleShape(Vector2f(GWIDTH - BORDER, GHEIGHT - BORDER)));
				resetRect(*rects.back());
				setRect(*rects.back(), X, Y, board[X][Y]);

				texts.push_back(new Text());
				resetText(*(texts.back()), font1);
				setText(*texts.back(), board[X][Y] ? to_string(board[X][Y]) : "", X, Y);

				ani.addTarget(AnimTarget(rects.back(), *rects.back(), Vector2f(GWIDTH * (Xdest), GHEIGHT * (Ydest)), speed, accel));
				ani.addTarget(AnimTarget(texts.back(), *texts.back(), Vector2f(GWIDTH * (Xdest + .5f), GHEIGHT * (Ydest + .5f)), speed, accel));
			};

			auto runAni = [&]()
			{
				window.clear();
				draw(rect, text, false);

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

			auto inside = [&](int X, int Y) -> bool // return true whether the location (X, Y) is not out of boundary
			{
				return !(X == -1 || Y == -1 || X >= BWIDTH || Y >= BHEIGHT);
			};

			auto slide = [&]()
			{
				vector<vector<bool>> drawn = vector<vector<bool>>(BWIDTH, vector<bool>(BHEIGHT, false));

				for (int X = (Xmove == 1 ? BWIDTH - 1 : 0);X != (Xmove == 1 ? -1 : BWIDTH);X += (Xmove == 1 ? -1 : 1))
					// if Xmove == 1, this works as 'for (int X=BWIDTH-1;X>-1;X--)' and 'for (int X=0;X<BWIDTH;X++)' if Xmove == -1.
				{
					for (int Y = (Ymove == 1 ? BHEIGHT - 1 : 0);Y != (Ymove == 1 ? -1 : BHEIGHT);Y += (Ymove == 1 ? -1 : 1))
						// if Ymove == 1, this works as 'for (int Y=BHEIGHT-1;Y>-1;Y--)' and 'for (int Y=0;X<BHEIGHT;Y++)' if Ymove == -1.
					{
						if (board[X][Y])
						{
							int Xdest = X;
							int Ydest = Y;

							while (
								inside(Xdest + Xmove, Ydest + Ymove) && board[X][Y] &&
								(
									!board[Xdest + Xmove][Ydest + Ymove] || board[Xdest + Xmove][Ydest + Ymove] == board[X][Y]
									)
								) // destination is not out of boundary and destination contains 0 or same number as started point
							{
								Xdest += Xmove;
								Ydest += Ymove;
							}

							if (!drawn[X][Y])
							{
								addAni(X, Y, Xdest, Ydest);
							}

							if (Xdest != X || Ydest != Y)
							{
								if (board[X][Y])
								{
									drawn[Xdest][Ydest] = true;
									if (board[Xdest][Ydest])
									{
										addAni(Xdest, Ydest, Xdest, Ydest);
									}
								}

								board[Xdest][Ydest] += board[X][Y];

								if (board[Xdest][Ydest] > 2 && board[X][Y])
								{
									addScore(board[Xdest][Ydest]);
								}

								board[X][Y] = 0;
							}
						}
					}
				}

				if (ani.targetLeft())
				{
					runAni();
				}
			};

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

		draw(rect, text);

		if (won)
		{
			texture.update(window);
			shader.setUniform("texture", texture);
			shader.setUniform("offsetFactor", Glsl::Vec2(.002f, .002f));
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

		window.display();

	}
	return 0;
}