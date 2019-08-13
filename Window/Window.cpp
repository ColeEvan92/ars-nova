#include "WindowsError.h"

#include "Window.h"

namespace bleak
{
	namespace window
	{
		constexpr windows::UINT WM_FUNCTION_NOCLEAN = windows::WM_USER + 1;
		constexpr windows::UINT WM_FUNCTION_CLEAN = windows::WM_USER + 2;
		constexpr windows::UINT WM_MANAGER_DESTROY = windows::WM_USER + 3;
		constexpr windows::UINT WM_WINDOW_DESTROY = windows::WM_USER + 4;

		/*template <class> struct _invoke_expander {};
		template <size_t... Is> struct _invoke_expander<std::index_sequence<Is...>> { template <class F, typename... Ts> static auto _invoke(F& func, std::tuple<Ts...>& args) { return func(std::get<Is>(args)...); } };
		template <class F, typename... Ts> auto _invoke(F& func, std::tuple<Ts...>& args)
		{
			return _invoke_expander<std::make_index_sequence<sizeof...(Ts)>>::_invoke(func, args);
		}*/

		struct _package_noreturn_tag {} _package_noreturn;
		struct _package
		{
			virtual ~_package() = default;
			virtual void operator()() = 0;
		};
		template<class R, class F, typename... Ts> struct _package_call : public _package
		{
			std::promise<R> promise;
			F func;
			std::tuple<Ts...> args;
			template <class Fx, typename... Tsx> _package_call(std::promise<R>&& promise, Fx&& func, Tsx&& ... args) : promise(std::move(promise)), func(std::forward<Fx>(func)), args(std::make_tuple(std::forward<Tsx>(args)...)) {}
			_package_call(_package_call&& rhs) : promise(std::move(rhs.promise)), func(std::move(rhs.func)), args(std::move(rhs.args)) {}
			virtual ~_package_call() override = default;
			virtual void operator()() override
			{
				_really<R>();
			}
			template <typename T> void _really()
			{
				promise.set_value(std::apply(func, args));
			}
			template <> void _really<void>()
			{
				std::apply(func, args);
				promise.set_value();
			}
		};
		template <class F, typename... Ts> struct _package_call<_package_noreturn_tag, F, Ts...> : public _package
		{
			F func;
			std::tuple<Ts...> args;
			template <class Fx, typename... Tsx> _package_call(Fx&& func, Tsx&& ... args) : func(std::forward<Fx>(func)), args(std::forward<Tsx>(args)...) {}
			_package_call(_package_call&& rhs) : func(std::move(rhs._func)), args(std::move(rhs.args)) {}
			virtual ~_package_call() override = default;
			virtual void operator()() override
			{
				std::apply(func, args);
			}
		};
		template <class R, class F, typename... Ts> auto _make_package(std::promise<R>&& prom, F&& func, Ts&& ... args)
		{
			return new _package_call<R, std::remove_reference_t<F>, std::remove_reference_t<Ts>...>(std::move(prom), std::forward<F>(func), std::forward<Ts>(args)...);
		}
		template <class F, typename... Ts> auto _make_package(F&& func, Ts&& ... args)
		{
			return new _package_call<_package_noreturn_tag, std::remove_reference_t<F>, std::remove_reference_t<Ts>...>(std::forward<F>(func), std::forward<Ts>(args)...);
		}
		template <class R, class F, typename... Ts> auto _make_package_noalloc(std::promise<R>&& prom, F&& func, Ts&& ... args)
		{
			return _package_call<R, std::remove_reference_t<F>, std::remove_reference_t<Ts>...>(std::move(prom), std::forward<F>(func), std::forward<Ts>(args)...);
		}
		template <class F, typename... Ts> auto _make_package_noalloc(F&& func, Ts&& ... args)
		{
			return _package_call<_package_noreturn_tag, std::remove_reference_t<F>, std::remove_reference_t<Ts>...>(std::forward<F>(func), std::forward<Ts>(args)...);
		}

		Message::Message(Window* h, WindowsMessage_t m, IntParam w, LongParam l) : window(h), message(m), wparam(w), lparam(l) {}
		Message::Message(Window* h, WindowsMessage_t m, IntParam w, LongParam l, Time t, Point p) : window(h), message(m), wparam(w), lparam(l), time(t), cursor_pos(p) {}
		Message::Point Message::translate_lparam_as_point(LongParam l)
		{
			return { l & 0xFFFF, (l & 0xFFFF0000) >> 16 };
		}

		void Listener::listen(WindowsMessage_t id, Location l, Priority p)
		{
			std::lock_guard<std::mutex> lock(_map_guard);
			_listening[id] = { l, p };
		}
		void Listener::listen(std::initializer_list<WindowsMessage_t> il, Location l, Priority p)
		{
			listen(il.begin(), il.end(), l, p);
		}
		void Listener::listen(std::initializer_list<ListenInit> il)
		{
			listen(il.begin(), il.end());
		}
		void Listener::stop_listen(WindowsMessage_t id)
		{
			auto it = _listening.find(id);
			if (it != _listening.end())
			{
				std::lock_guard<std::mutex> lock(_map_guard);
				_listening.erase(it);
			}
		}
		void Listener::stop_listen(std::initializer_list<WindowsMessage_t> il)
		{
			stop_listen(il.begin(), il.end());
		}
		std::vector<Message> Listener::get_messages()
		{
			std::lock_guard<std::mutex> lock(_message_guard);
			std::vector<Message> r;
			std::swap(r, _messages);
			return r;
		}
		void Listener::wait_for_message() const
		{
			std::unique_lock<std::mutex> lock(_message_guard);
			while (_messages.empty())
			{
				_cv.wait(lock);
			}
		}
		void Listener::_queue_message(const Message& msg)
		{
			{
				std::lock_guard<std::mutex> lock(_message_guard);
				_messages.push_back(msg);
				_cv.notify_one();
			}
		}
		windows::LRESULT Listener::_callback_message(const Message& msg)
		{
			std::lock_guard<std::mutex> lock(_callback_guard);
			return _callback(msg);
		}

		WindowManager::WindowManager()
		{
			while (!_ready);
		}
		WindowManager::~WindowManager()
		{
			_send_message(WM_MANAGER_DESTROY, 0, 0);
			_pump.join();
		}
		void WindowManager::quit()
		{
			_package* package = _make_package(&windows::PostQuitMessage, 0);
			_send_function(package, true);
		}
		bool WindowManager::has_quit() const
		{
			return _quit;
		}
		WindowManager::operator bool() const
		{
			return !_quit;
		}
		const WindowManager& WindowManager::wait_for_message() const
		{
			listener.wait_for_message();
			return *this;
		}
		Message::Result WindowManager::handle_message_default(const Message& msg)
		{
			return windows::DefWindowProc(msg.window ? msg.window->_hwnd : nullptr, msg.message, msg.wparam, msg.lparam);
		}
		thread_local std::map<windows::HWND, Window*> _wndproc_map;
		windows::LRESULT WindowManager::_wndproc(windows::HWND hwnd, windows::UINT msg, windows::WPARAM wparam, windows::LPARAM lparam)
		{
			if (msg == windows::WM_CREATE)
				_wndproc_map[hwnd] = reinterpret_cast<Window*>(reinterpret_cast<windows::CREATESTRUCT*>(lparam)->lpCreateParams);
			auto it = _wndproc_map.find(hwnd), end = _wndproc_map.end();
			windows::LRESULT r = 0;
			if (it != end)
				r = it->second->_wndproc(hwnd, msg, wparam, lparam);
			else
				r = windows::DefWindowProc(hwnd, msg, wparam, lparam);
			if (msg == windows::WM_DESTROY && it != end)
			{
				it->second->_hwnd = nullptr;
				_wndproc_map.erase(it);
			}
			return r;
		}
		std::optional<windows::LRESULT> WindowManager::_dispatch_message(const Message& msg, bool in_pump)
		{
			typedef decltype(listener._listening)::iterator MapIter;
			//typedef std::map<WindowsMessage_t, std::pair<Listener::Location, Listener::Priority>>::iterator MapIter;
			MapIter w, wc, wm;
			bool wb = false, wcb = false, wmb = false;
			{
				std::lock_guard<std::mutex> lock(listener._map_guard);
				wm = listener._listening.find(msg.message);
				wmb = wm != listener._listening.end();
			}
			if (msg.window)
			{
				{
					std::lock_guard<std::mutex> lock(msg.window->listener._map_guard);
					w = msg.window->listener._listening.find(msg.message);
					wb = w != msg.window->listener._listening.end();
				}
				{
					std::lock_guard<std::mutex> lock(msg.window->_class.listener._map_guard);
					wc = msg.window->_class.listener._listening.find(msg.message);
					wcb = wc != msg.window->_class.listener._listening.end();
				}
			}
			if (wb && w->second.second == Listener::Priority::HERE_ONLY)
			{
				wcb = false;
				wmb = false;
			}
			else if (wcb && wc->second.second == Listener::Priority::HERE_ONLY)
			{
				wb = false;
				wmb = false;
			}
			else if (wmb && wm->second.second == Listener::Priority::HERE_ONLY)
			{
				wb = false;
				wcb = false;
			}
			if ((wb && (wcb || wmb)) && w->second.second == Listener::Priority::IF_ONLY_HERE)
				wb = false;
			if ((wcb && (wb || wmb)) && wc->second.second == Listener::Priority::IF_ONLY_HERE)
				wcb = false;
			if ((wmb && (wb || wcb)) && wm->second.second == Listener::Priority::IF_ONLY_HERE)
				wmb = false;
			std::optional<windows::LRESULT> r;
			if (in_pump && msg.window)
				msg.window->_last_msg_seen = true;
			if (wmb)
			{
				if (wm->second.first == Listener::Location::CALLBACK)
					r = listener._callback_message(msg);
				else
					listener._queue_message(msg);
			}
			if (wcb)
			{
				if (wc->second.first == Listener::Location::CALLBACK)
					r = msg.window->_class.listener._callback_message(msg);
				else
					msg.window->_class.listener._queue_message(msg);
			}
			if (wb)
			{
				if (w->second.first == Listener::Location::CALLBACK)
					r = msg.window->listener._callback_message(msg);
				else
					msg.window->listener._queue_message(msg);
			}
			return r;
		}
		void WindowManager::_pump_func()
		{
			_ready = true;
			windows::MSG msg;
			while (windows::GetMessage(&msg, nullptr, 0, 0) > 0)
			{
				switch (msg.message)
				{
				case WM_FUNCTION_NOCLEAN:
				case WM_FUNCTION_CLEAN:
				{
					_package& package = *reinterpret_cast<_package*>(msg.lParam);
					package();
					if (msg.message == WM_FUNCTION_CLEAN)
						delete& package;
				}
				break;
				case WM_MANAGER_DESTROY:
					windows::PostQuitMessage(0);
					break;
				case WM_WINDOW_DESTROY:
				{
					windows::HWND hwnd = reinterpret_cast<windows::HWND>(msg.lParam);
					auto it = _wndproc_map.find(hwnd);
					if (it != _wndproc_map.end())
					{
						_wndproc_map.erase(it);
						if (windows::IsWindow(hwnd))
							windows::DestroyWindow(hwnd);
					}
					break;
				}
				}
				{
					auto it = _wndproc_map.find(msg.hwnd);
					_dispatch_message(Message(it == _wndproc_map.end() ? nullptr : it->second, msg.message, msg.wParam, msg.lParam, msg.time, msg.pt), true);
				}
				windows::TranslateMessage(&msg);
				windows::DispatchMessage(&msg);
			}
			_quit = true;
			listener._queue_message(Message(nullptr, windows::WM_QUIT, 0, 0));
		}
		void WindowManager::_send_function(_package* package, bool cleanup)
		{
			_send_message(WM_FUNCTION_NOCLEAN + (cleanup == true), 0, reinterpret_cast<intptr_t>(package));
		}
		void WindowManager::_send_message(windows::UINT msg, windows::WPARAM wParam, windows::LPARAM lParam)
		{
			windows::PostThreadMessage(windows::GetThreadId(_pump.native_handle()), msg, wParam, lParam);
		}

		size_t WindowClass::_name_counter = 0;
		WindowClass::WindowClass(const StartupData& data, Style style, graphics::Brush background, graphics::Cursor cursor) : _data(data), _brush(background), _cursor(cursor), _name(_make_name(_name_counter++))
		{
			_init(style);
		}
		WindowClass::WindowClass(const StartupData& data, Style style, graphics::Brush background, graphics::Cursor cursor, graphics::Icon large, graphics::Icon small) : _data(data), _brush(background), _cursor(cursor), _name(_make_name(_name_counter++)), _icons({ large, small })
		{
			_init(style);
		}
		WindowClass::~WindowClass()
		{
			windows::UnregisterClass(_name.c_str(), _data.hInstance);
		}
		bstring WindowClass::_make_name(size_t id)
		{
			return STR("bleak::WindowClass") + to_bstring(id);
		}
		void WindowClass::_init(Style style)
		{
			windows::WNDCLASSEX wc{};
			wc.cbSize = sizeof(wc);
			wc.hbrBackground = _brush.get_object();
			wc.hCursor = _cursor.get_object();
			if (_icons)
			{
				wc.hIcon = _icons->first.get_object();
				wc.hIconSm = _icons->second.get_object();
			}
			wc.hInstance = _data.hInstance;
			wc.lpfnWndProc = &WindowManager::_wndproc;
			wc.lpszClassName = _name.c_str();
			wc.style = style;
			windows::RegisterClassEx(&wc);
		}

		Window::Canvas::Canvas(const Canvas& rhs) : _bitmap(rhs._bitmap), _scale(rhs._scale), _buffer(rhs._buffer) {}
		Window::Canvas::Canvas(Canvas&& rhs) : _bitmap(std::move(rhs._bitmap)), _scale(rhs._scale), _buffer(rhs._buffer) {}
		Window::Canvas& Window::Canvas::operator=(const Canvas& rhs)
		{
			std::lock_guard<std::mutex> lock(_guard);
			this->~Canvas();
			new (this) Canvas(rhs);
			return *this;
		}
		Window::Canvas& Window::Canvas::operator=(Canvas&& rhs)
		{
			std::lock_guard<std::mutex> lock(_guard);
			this->~Canvas();
			new (this) Canvas(std::move(rhs));
			return *this;
		}
		Window::Canvas::Scale Window::Canvas::scale() const
		{
			return _scale;
		}
		unsigned long Window::Canvas::buffer() const
		{
			return _buffer;
		}
		Window::Size Window::Canvas::size() const
		{
			return (_bitmap ? _bitmap->size() : Size{ 0, 0 });
		}
		bool Window::Canvas::empty() const
		{
			return !_bitmap;
		}
		void Window::Canvas::set_def_text_color(graphics::Color c)
		{
			_textc = c;
		}
		void Window::Canvas::set_def_text_back(graphics::Color c)
		{
			_textb = c;
		}
		void Window::Canvas::set_def_text_back_transparent(bool b)
		{
			_textm = b;
		}
		void Window::Canvas::set_def_brush(const graphics::Brush& b)
		{
			_brush = b;
		}
		void Window::Canvas::set_def_pen(const graphics::Pen& p)
		{
			_pen = p;
		}
		Window::Canvas::Canvas(Window& w, Size sz) : _window(&w)
		{
			if (sz.x_value > 0 && sz.y_value > 0)
				_bitmap = graphics::Bitmap(sz);
		}

		Window::Window(WindowManager& wm, WindowClass& wc, Style style, const bstring& title, Size canvas_size, graphics::AlignMode_t canvas_alignment, ShownState shown, Window* parent) : Window(wm, wc, { windows::CW_USEDEFAULT, windows::CW_USEDEFAULT }, { windows::CW_USEDEFAULT, windows::CW_USEDEFAULT }, style, title, canvas_size, canvas_alignment, shown, parent) {}
		Window::Window(WindowManager& wm, WindowClass& wc, Point pos, Size sz, Style style, const bstring& title, Size canvas_size, graphics::AlignMode_t canvas_alignment, ShownState shown, Window* parent) : _manager(wm), _class(wc), _align_mode(canvas_alignment), canvas(*this, canvas_size)
		{
			windows::DWORD ws = 0, ws_ex = 0;
			if (style & WindowStyle::CLIP_CHILD_BOUNDS)
				ws |= windows::WS_CLIPCHILDREN;
			if (style & WindowStyle::CLIP_SIBLING_BOUNDS)
				ws |= windows::WS_CLIPSIBLINGS;
			if (style & WindowStyle::HORIZ_SCROLL)
				ws |= windows::WS_HSCROLL;
			if (style & WindowStyle::VERT_SCROLL)
				ws |= windows::WS_VSCROLL;
			if (style & WindowStyle::MAXIMIZE_BUTTON)
				ws |= windows::WS_MAXIMIZEBOX;
			if (style & WindowStyle::MINIMIZE_BUTTON)
				ws |= windows::WS_MINIMIZEBOX;
			if (style & WindowStyle::RAISED_EDGE)
				ws_ex |= windows::WS_EX_WINDOWEDGE;
			if (style & WindowStyle::SUNKEN_EDGE)
				ws_ex |= windows::WS_EX_CLIENTEDGE;
			if (style & WindowStyle::RESIZEABLE)
				ws |= windows::WS_THICKFRAME;
			if (style & WindowStyle::TITLE)
				ws |= windows::WS_CAPTION;
			if (style & (WindowStyle::MAXIMIZE_BUTTON | WindowStyle::MINIMIZE_BUTTON))
				ws |= windows::WS_SYSMENU | windows::WS_CAPTION;
			if (parent)
				ws |= windows::WS_CHILD;
			std::promise<windows::HWND> prom;
			std::future<windows::HWND> fut(prom.get_future());
			auto package = _make_package_noalloc(std::move(prom), &windows::CreateWindowEx, ws_ex, _class._name.c_str(), title.c_str(), ws, pos.x_value, pos.y_value, sz.x_value, sz.y_value, (parent ? parent->_hwnd : nullptr), nullptr, _class._data.hInstance, this);
			_manager._send_function(&package, false);
			_hwnd = fut.get();
			windows::ShowWindow(_hwnd, _translate_shown_state(shown));
			windows::UpdateWindow(_hwnd);
		}
		Window::~Window()
		{
			if (_hwnd)
			{
				_manager._send_message(WM_WINDOW_DESTROY, 0, reinterpret_cast<intptr_t>(_hwnd));
			}
		}
		WindowManager& Window::manager() const
		{
			return _manager;
		}
		WindowClass& Window::wclass() const
		{
			return _class;
		}
		bool Window::closed() const
		{
			return !_hwnd;
		}
		Window::operator bool() const
		{
			return !closed();
		}
		Window::Rectangle Window::bounds() const
		{
			windows::RECT r;
			windows::GetWindowRect(_hwnd, &r);
			return r;
		}
		Window::Size Window::inner_size() const
		{
			windows::RECT r;
			windows::GetClientRect(_hwnd, &r);
			return { r.right, r.bottom };
		}
		Window::ShownState Window::shown() const
		{
			windows::WINDOWPLACEMENT wp;
			wp.length = sizeof(wp);
			windows::GetWindowPlacement(_hwnd, &wp);
			return _translate_shown_state(wp.showCmd);
		}
		bool Window::enabled() const
		{
			return windows::IsWindowEnabled(_hwnd);
		}
		bool Window::active() const
		{
			windows::WINDOWINFO wi;
			wi.cbSize = sizeof(wi);
			windows::GetWindowInfo(_hwnd, &wi);
			return wi.dwWindowStatus & 0x1;
		}
		void Window::move(Point pos, Size sz, bool desired_inner_size)
		{
			if (desired_inner_size)
			{
				auto r = bounds();
				sz += Size{ r.x_value_second - r.x_value_first, r.y_value_second - r.y_value_first } -inner_size();
			}
			windows::MoveWindow(_hwnd, pos.x_value, pos.y_value, sz.x_value, sz.y_value, false);
		}
		void Window::show(ShownState show)
		{
			windows::ShowWindow(_hwnd, _translate_shown_state(show));
		}
		void Window::enable(bool e)
		{
			windows::EnableWindow(_hwnd, e);
		}
		void Window::activate()
		{
			_manager._send_function(_make_package(&windows::SetActiveWindow, _hwnd), true);
		}
		void Window::set_title(const bstring& str)
		{
			windows::SetWindowText(_hwnd, str.c_str());
			//std::unique_lock<std::mutex> lock(canvas._guard, std::try_to_lock);
			//if (!lock.owns_lock())
			//	throw std::runtime_error("Calling set_title inside of a Draw block invokes a race condition.");
			//lock.release();
			//_manager._send_function(_make_package(&windows::SetWindowText, _hwnd, str.c_str()), true);
		}
		void Window::close(bool unstoppable)
		{
			if (unstoppable)
				windows::DestroyWindow(_hwnd);
			else
				windows::CloseWindow(_hwnd);
		}
		Window::Point Window::screen_to_inner(Point pos) const
		{
			windows::POINT p{ pos.x_value, pos.y_value };
			windows::ScreenToClient(_hwnd, &p);
			return p;
		}
		Window::Point Window::inner_to_screen(Point pos) const
		{
			windows::POINT p{ pos.x_value, pos.y_value };
			windows::ClientToScreen(_hwnd, &p);
			return p;
		}
		Window::Point Window::inner_to_canvas(Point pos) const
		{
			if (canvas.empty())
				return {};
			Size sz = inner_size();
			if (_align_mode != graphics::AlignMode::FIT && _align_mode != graphics::AlignMode::STRETCH)
			{
				Rectangle r = graphics::Bitmap::calculate_blt_bounds(sz, canvas.size() * canvas.scale(), _align_mode);
				if (_align_mode & graphics::AlignMode::LEFT)
					r.x_value_first += canvas.buffer();
				else if (_align_mode & graphics::AlignMode::RIGHT)
					r.x_value_first -= canvas.buffer();
				if (_align_mode & graphics::AlignMode::TOP)
					r.y_value_first += canvas.buffer();
				else if (_align_mode & graphics::AlignMode::BOTTOM)
					r.y_value_first -= canvas.buffer();
				pos -= {r.x_value_first, r.y_value_first};
				pos /= canvas.scale();
				return pos;
			}
			if (_align_mode == graphics::AlignMode::STRETCH)
			{
				return pos / (sz / canvas.size());
			}
			Rectangle r = graphics::Bitmap::calculate_blt_bounds(sz, canvas.size(), _align_mode);
			return (pos - Point{ r.x_value_first, r.y_value_first }) / (Size{ r.x_value_second, r.y_value_second } / canvas.size());
		}
		windows::HWND Window::native_handle() const
		{
			return _hwnd;
		}
		std::vector<Message> Window::get_messages_from_all()
		{
			auto msgs1 = listener.get_messages(), msgs2 = _class.listener.get_messages(), msgs3 = _manager.listener.get_messages();
			msgs1.reserve(msgs1.size() + msgs2.size() + msgs3.size());
			msgs1.insert(msgs1.end(), msgs2.begin(), msgs2.end());
			msgs1.insert(msgs1.end(), msgs3.begin(), msgs3.end());
			return msgs1;
		}
		Window::ShownState Window::_translate_shown_state(windows::UINT ss)
		{
			ShownState shown;
			if (ss == windows::SW_HIDE)
				shown = ShownState::HIDDEN;
			else if (ss == windows::SW_MAXIMIZE || ss == windows::SW_SHOWMAXIMIZED)
				shown = ShownState::MAXIMIZED;
			else if (ss == windows::SW_MINIMIZE || ss == windows::SW_SHOWMINIMIZED || ss == windows::SW_SHOWMINNOACTIVE)
				shown = ShownState::MAXIMIZED;
			else if (ss == windows::SW_SHOW || ss == windows::SW_RESTORE || ss == windows::SW_SHOWNA || ss == windows::SW_SHOWNORMAL || ss == windows::SW_SHOWNOACTIVATE)
				shown = ShownState::NORMAL;
			return shown;
		}
		windows::UINT Window::_translate_shown_state(ShownState ss)
		{
			windows::UINT show;
			switch (ss)
			{
			case ShownState::HIDDEN:
				show = windows::SW_HIDE;
				break;
			case ShownState::MAXIMIZED:
				show = windows::SW_MAXIMIZE;
				break;
			case ShownState::MINIMIZED:
				show = windows::SW_MINIMIZE;
				break;
			case ShownState::NORMAL:
				show = windows::SW_RESTORE;
				break;
			}
			return show;
		}
		windows::LRESULT Window::_wndproc(windows::HWND hwnd, windows::UINT msg, windows::WPARAM wparam, windows::LPARAM lparam)
		{
			enum MoveDir : unsigned char
			{
				LEFT = 0x1,
				UP = 0x2,
				RIGHT = 0x4,
				DOWN = 0x8
			};
			std::optional<windows::LRESULT> return_val;
			switch (msg)
			{
			case windows::WM_NCLBUTTONDOWN:
				if (_mode == MoveSizeMode::NORMAL)
				{
					_last_pos = *reinterpret_cast<windows::POINTS*>(&lparam);
					_last_ht = wparam;
				}
				break;
			case windows::WM_NCLBUTTONUP:
			case windows::WM_LBUTTONUP:
				if (_mode != MoveSizeMode::NORMAL)
				{
					_mode = MoveSizeMode::NORMAL;
					windows::ReleaseCapture();
				}
				break;
			case windows::WM_NCMOUSEMOVE:
			case windows::WM_MOUSEMOVE:
				if (_mode != MoveSizeMode::NORMAL)
				{
					windows::POINTS pos;
					if (msg == windows::WM_MOUSEMOVE)
					{
						pos.x = static_cast<windows::SHORT>(lparam & 0xFFFF);
						pos.y = static_cast<windows::SHORT>((lparam & 0xFFFF0000) >> 16);
						windows::POINT p{ pos.x, pos.y };
						windows::ClientToScreen(hwnd, &p);
						pos.x = static_cast<windows::SHORT>(p.x);
						pos.y = static_cast<windows::SHORT>(p.y);
					}
					else
						pos = *reinterpret_cast<windows::POINTS*>(&lparam);
					windows::POINTS off{ pos.x - _last_pos.x, pos.y - _last_pos.y };
					windows::RECT r;
					windows::GetWindowRect(hwnd, &r);
					if (_mode == MoveSizeMode::MOVE)
					{
						windows::MoveWindow(hwnd, r.left + off.x, r.top + off.y, r.right - r.left, r.bottom - r.top, false);
						windows::InvalidateRect(hwnd, nullptr, true);
					}
					else
					{
						windows::RECT r2 = r;
						if (_resize_flags & LEFT)
						{
							r2.left += off.x;
						}
						if (_resize_flags & UP)
						{
							r2.top += off.y;
						}
						if (_resize_flags & RIGHT)
							r2.right += off.x;
						if (_resize_flags & DOWN)
							r2.bottom += off.y;
						windows::MoveWindow(hwnd, r2.left, r2.top, r2.right - r2.left, r2.bottom - r2.top, false);
						windows::InvalidateRect(hwnd, nullptr, true);
					}
					_last_pos = pos;
				}
				break;
			case windows::WM_SYSCOMMAND:
				if ((wparam & 0xFFF0) == windows::SC_SIZE)
				{
					_resize_flags = 0;
					if (_last_ht == windows::HTLEFT || _last_ht == windows::HTTOPLEFT || _last_ht == windows::HTBOTTOMLEFT)
						_resize_flags |= LEFT;
					if (_last_ht == windows::HTTOP || _last_ht == windows::HTTOPLEFT || _last_ht == windows::HTTOPRIGHT)
						_resize_flags |= UP;
					if (_last_ht == windows::HTRIGHT || _last_ht == windows::HTTOPRIGHT || _last_ht == windows::HTBOTTOMRIGHT)
						_resize_flags |= RIGHT;
					if (_last_ht == windows::HTBOTTOM || _last_ht == windows::HTBOTTOMLEFT || _last_ht == windows::HTBOTTOMRIGHT)
						_resize_flags |= DOWN;
					_mode = MoveSizeMode::SIZE;
					windows::SetCapture(hwnd);
					return 0;
				}
				else if ((wparam & 0xFFF0) == windows::SC_MOVE)
				{
					_mode = MoveSizeMode::MOVE;
					windows::SetCapture(hwnd);
					return 0;
				}
				break;
			case windows::WM_PAINT:
				std::lock_guard<std::mutex> lock(canvas._guard);
				if (canvas._bitmap)
				{
					windows::PAINTSTRUCT ps;
					windows::HDC dest = windows::BeginPaint(hwnd, &ps);
					windows::HDC src = windows::CreateCompatibleDC(dest);
					windows::HGDIOBJ old = windows::SelectObject(src, canvas._bitmap->get_object());
					windows::RECT sz;
					windows::GetClientRect(hwnd, &sz);
					auto bmsz = canvas._bitmap->size();
					auto bounds = graphics::Bitmap::calculate_blt_bounds({ sz.right, sz.bottom }, bmsz, _align_mode);
					if (_align_mode == graphics::AlignMode::STRETCH || _align_mode == graphics::AlignMode::FIT)
						windows::StretchBlt(dest, bounds.x_value_first, bounds.y_value_first, bounds.x_value_second - bounds.x_value_first, bounds.y_value_second - bounds.y_value_first, src, 0, 0, bmsz.x_value, bmsz.y_value, windows::SRCCOPY);
					else
						windows::BitBlt(dest, bounds.x_value_first, bounds.y_value_first, bounds.x_value_second - bounds.x_value_first, bounds.y_value_second - bounds.y_value_first, src, 0, 0, windows::SRCCOPY);
					windows::SelectObject(src, old);
					windows::DeleteDC(src);
					windows::EndPaint(hwnd, &ps);
					return_val = 0;
				}
				else
					return_val = windows::DefWindowProc(hwnd, msg, wparam, lparam);
				break;
			}
			if (!_last_msg_seen)
				return_val = _manager._dispatch_message(Message(this, msg, wparam, lparam), false);
			_last_msg_seen = false;
			if (!return_val)
				return_val = windows::DefWindowProc(hwnd, msg, wparam, lparam);
			return *return_val;
		}

	}
}