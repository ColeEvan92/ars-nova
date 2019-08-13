#include <chrono>

#include "Window.h"
#include "KeyCodes.h"
#include "Draw.h"
#include "SnakeGame.h"
using namespace bleak::window;
using namespace bleak::graphics;
using namespace std::chrono_literals;

int BleakMain(bleak::StartupData data)
{
	WindowManager wm;
	wm.listener.listen({ +WindowsMessage::KEYDOWN, +WindowsMessage::DESTROY });
	WindowClass wc(data, WindowClass::DEFAULT_STYLE, Brush(Color{}), Cursor(SystemCursor::NORMAL, true));
	Window w(wm, wc, Window::DEFAULT_STYLE, STR(""), { 490, 490 }, AlignMode::CENTER);
	w.move((+w.bounds()).first, { 530, 530 }, true); //This is why operator+ works like that for _rectangle
	std::chrono::steady_clock clock;
	auto time = clock.now();
	bool paused = false;
	bool game_over = false;

	while (w)
	{
	RESTART: //Pressing Enter restarts the game. This is the point in the code that it jumps to.
		//I don't like Goto any more than the next guy, but I like it more than having a bunch of bool variables.

		Game snake({ 49, 49 }, 10, { 0, 24 }, Game::Direction::RIGHT, 4, 16, 3, Color(0x7F, 0x7F, 0x7F), Color(0xFF, 0xFF, 0xFF), Color(0, 0x7F, 0x7F), Color(0x38, 0x38, 0x38));
		w.activate();
		while (w && snake)
		{
			if (paused)
				wm.wait_for_message();
			else
			{
				{
					Draw d(w);
					snake.draw(d);
					w.set_title(STR("~~!Snake!~~ Score: ") + bleak::to_bstring(snake.score()));
				}

			}
			{
				auto messages = wm.listener.get_messages();
				for (const auto& message : messages)
				{
					if (message.message == +WindowsMessage::KEYDOWN)
					{
						switch (message.wparam)
						{
						case +KeyCode::LEFT:
						case +KeyCode::KeyA:
							snake.change_direction(Game::Direction::LEFT);
							break;
						case +KeyCode::UP:
						case +KeyCode::KeyW:
							snake.change_direction(Game::Direction::UP);
							break;
						case +KeyCode::RIGHT:
						case +KeyCode::KeyD:
							snake.change_direction(Game::Direction::RIGHT);
							break;
						case +KeyCode::DOWN:
						case +KeyCode::KeyS:
							snake.change_direction(Game::Direction::DOWN);
							break;
						case +KeyCode::SPACE:
							paused = !paused;
							break;
						case +KeyCode::RETURN:
							goto RESTART;
						}
					}
				}
			}

			if (!paused)
			{
				snake.advance(33ms);
				std::this_thread::sleep_until(time + 33ms);
				time = clock.now();
			}
		}
		game_over = true;
		w.set_title(STR("GAME OVER - ") + bleak::to_bstring(snake.score()));
		while (w && game_over)
		{
			wm.wait_for_message();
			auto messages = wm.listener.get_messages();
			for (const auto& message : messages)
			{
				if (message.message == +WindowsMessage::KEYDOWN && message.wparam == +KeyCode::RETURN)
				{
					game_over = false;
					paused = false;
				}
			}
		}
	}
	return 0;
}