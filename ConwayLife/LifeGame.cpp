#include <iostream>
#include <chrono>

#include "Window.h"
#include "KeyCodes.h"
#include "Life.h"
#include "Draw.h"
using namespace std::chrono_literals;
using namespace bleak;
using namespace window;
using namespace graphics;

const Color ALIVE{ 0, 0xFF, 0 };
const Color DEAD{ 0, 0, 0 };
const Brush BACK(Color{0x10, 0x10, 0x10});
const Brush FILL(Color{});

void setup(Window& window, Life& life)
{
	while (window)
	{
		window.manager().listener.wait_for_message();
		auto msgs = window.manager().listener.get_messages();
		Draw d(window);
		for (auto& msg : msgs)
		{
			switch (msg.message)
			{
			case +WindowsMessage::LBUTTONDOWN:
			{
				auto pt = window.inner_to_canvas(Message::translate_lparam_as_point(msg.lparam));
				life.set(pt, !life.get(pt));
				d.pixel(pt, (life.get(pt) ? ALIVE : DEAD));
			}
				break;
			case +WindowsMessage::KEYDOWN:
				switch (msg.wparam)
				{
				case +KeyCode::ESCAPE:
					life.clear();
					d.fill(Draw::Rectangle({ 0, 0 }, window.canvas.size()), FILL);
					break;
				case +KeyCode::RETURN:
					return;
				}
			}
		}
	}
}
void run(Window& window, Life& life)
{
	std::chrono::steady_clock clock;
	while (window)
	{
		auto now = clock.now();
		auto msgs = window.manager().listener.get_messages();
		for (auto& msg : msgs)
		{
			if (msg.message == +WindowsMessage::KEYDOWN && msg.wparam == +KeyCode::RETURN)
				return;
		}
		life.advance(1);
		{
			Draw d(window);
			d.fill(Draw::Rectangle({ 0, 0 }, window.canvas.size()), FILL);
			auto sz = life.size();
			for (size_t y = 0; y < sz.y_value; ++y)
			{
				for (size_t x = 0; x < sz.y_value; ++x)
				{
					if (life.get({ x, y }))
						d.pixel(Draw::Point( x, y ), ALIVE);
				}
			}
		}
		std::this_thread::sleep_until(now + 33ms);
	}
}

int BleakMain(StartupData data)
{
	WindowManager manager;
	manager.listener.listen({+WindowsMessage::DESTROY, +WindowsMessage::LBUTTONDOWN, +WindowsMessage::RBUTTONDOWN, +WindowsMessage::KEYDOWN });
	WindowClass wc(data, WindowClass::DEFAULT_STYLE, BACK, Cursor(SystemCursor::NORMAL, true));
	Window window(manager, wc, Window::DEFAULT_STYLE ^ WindowStyle::RESIZEABLE, STR("Life"), { 100, 100 }, bleak::graphics::AlignMode::STRETCH);
	window.move((+window.bounds()).first, { 400, 400 }, true);
	Life life({ 100, 100 });
	while (window)
	{
		setup(window, life);
		run(window, life);
	}
	return 0;
}