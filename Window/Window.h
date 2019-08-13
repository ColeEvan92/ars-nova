#pragma once

#include <map>
#include <vector>
#include <functional>
#include <optional>
#include <mutex>
#include <thread>
#include <future>
#include <type_traits>

#include "WindowsHider.h"
#include "Pair.h"
#include "GraphicObject.h"

namespace bleak
{
	namespace graphics
	{
		class Draw;
	}
	namespace window
	{
		enum class WindowsMessage : windows::UINT
		{
			CREATE = windows::WM_CREATE
			, DESTROY = windows::WM_DESTROY
			, MOVE = windows::WM_MOVE
			, SIZE = windows::WM_SIZE
			, ACTIVATE = windows::WM_ACTIVATE
			, SETFOCUS = windows::WM_SETFOCUS
			, KILLFOCUS = windows::WM_KILLFOCUS
			, ENABLE = windows::WM_ENABLE
			, SETREDRAW = windows::WM_SETREDRAW
			, SETTEXT = windows::WM_SETTEXT
			, GETTEXT = windows::WM_GETTEXT
			, GETTEXTLENGTH = windows::WM_GETTEXTLENGTH
			, PAINT = windows::WM_PAINT
			, CLOSE = windows::WM_CLOSE
			, QUERYENDSESSION = windows::WM_QUERYENDSESSION
			, QUERYOPEN = windows::WM_QUERYOPEN
			, ENDSESSION = windows::WM_ENDSESSION
			, QUIT = windows::WM_QUIT
			, ERASEBKGND = windows::WM_ERASEBKGND
			, SYSCOLORCHANGE = windows::WM_SYSCOLORCHANGE
			, SHOWWINDOW = windows::WM_SHOWWINDOW
			, WININICHANGE = windows::WM_WININICHANGE
			, SETTINGCHANGE = windows::WM_SETTINGCHANGE
			, DEVMODECHANGE = windows::WM_DEVMODECHANGE
			, ACTIVATEAPP = windows::WM_ACTIVATEAPP
			, FONTCHANGE = windows::WM_FONTCHANGE
			, TIMECHANGE = windows::WM_TIMECHANGE
			, CANCELMODE = windows::WM_CANCELMODE
			, SETCURSOR = windows::WM_SETCURSOR
			, MOUSEACTIVATE = windows::WM_MOUSEACTIVATE
			, CHILDACTIVATE = windows::WM_CHILDACTIVATE
			, QUEUESYNC = windows::WM_QUEUESYNC
			, GETMINMAXINFO = windows::WM_GETMINMAXINFO
			, PAINTICON = windows::WM_PAINTICON
			, ICONERASEBKGND = windows::WM_ICONERASEBKGND
			, NEXTDLGCTL = windows::WM_NEXTDLGCTL
			, SPOOLERSTATUS = windows::WM_SPOOLERSTATUS
			, DRAWITEM = windows::WM_DRAWITEM
			, MEASUREITEM = windows::WM_MEASUREITEM
			, DELETEITEM = windows::WM_DELETEITEM
			, VKEYTOITEM = windows::WM_VKEYTOITEM
			, CHARTOITEM = windows::WM_CHARTOITEM
			, SETFONT = windows::WM_SETFONT
			, GETFONT = windows::WM_GETFONT
			, SETHOTKEY = windows::WM_SETHOTKEY
			, GETHOTKEY = windows::WM_GETHOTKEY
			, QUERYDRAGICON = windows::WM_QUERYDRAGICON
			, COMPAREITEM = windows::WM_COMPAREITEM
			, GETOBJECT = windows::WM_GETOBJECT
			, COMPACTING = windows::WM_COMPACTING
			, COMMNOTIFY = windows::WM_COMMNOTIFY
			, WINDOWPOSCHANGING = windows::WM_WINDOWPOSCHANGING
			, WINDOWPOSCHANGED = windows::WM_WINDOWPOSCHANGED
			, POWER = windows::WM_POWER
			, COPYDATA = windows::WM_COPYDATA
			, CANCELJOURNAL = windows::WM_CANCELJOURNAL
			, NOTIFY = windows::WM_NOTIFY
			, INPUTLANGCHANGEREQUEST = windows::WM_INPUTLANGCHANGEREQUEST
			, INPUTLANGCHANGE = windows::WM_INPUTLANGCHANGE
			, TCARD = windows::WM_TCARD
			, HELP = windows::WM_HELP
			, USERCHANGED = windows::WM_USERCHANGED
			, NOTIFYFORMAT = windows::WM_NOTIFYFORMAT
			, CONTEXTMENU = windows::WM_CONTEXTMENU
			, STYLECHANGING = windows::WM_STYLECHANGING
			, STYLECHANGED = windows::WM_STYLECHANGED
			, DISPLAYCHANGE = windows::WM_DISPLAYCHANGE
			, GETICON = windows::WM_GETICON
			, SETICON = windows::WM_SETICON
			, NCCREATE = windows::WM_NCCREATE
			, NCDESTROY = windows::WM_NCDESTROY
			, NCCALCSIZE = windows::WM_NCCALCSIZE
			, NCHITTEST = windows::WM_NCHITTEST
			, NCPAINT = windows::WM_NCPAINT
			, NCACTIVATE = windows::WM_NCACTIVATE
			, GETDLGCODE = windows::WM_GETDLGCODE
			, SYNCPAINT = windows::WM_SYNCPAINT
			, NCMOUSEMOVE = windows::WM_NCMOUSEMOVE
			, NCLBUTTONDOWN = windows::WM_NCLBUTTONDOWN
			, NCLBUTTONUP = windows::WM_NCLBUTTONUP
			, NCLBUTTONDBLCLK = windows::WM_NCLBUTTONDBLCLK
			, NCRBUTTONDOWN = windows::WM_NCRBUTTONDOWN
			, NCRBUTTONUP = windows::WM_NCRBUTTONUP
			, NCRBUTTONDBLCLK = windows::WM_NCRBUTTONDBLCLK
			, NCMBUTTONDOWN = windows::WM_NCMBUTTONDOWN
			, NCMBUTTONUP = windows::WM_NCMBUTTONUP
			, NCMBUTTONDBLCLK = windows::WM_NCMBUTTONDBLCLK
			, NCXBUTTONDOWN = windows::WM_NCXBUTTONDOWN
			, NCXBUTTONUP = windows::WM_NCXBUTTONUP
			, NCXBUTTONDBLCLK = windows::WM_NCXBUTTONDBLCLK
			, INPUT_DEVICE_CHANGE = windows::WM_INPUT_DEVICE_CHANGE
			, INPUT = windows::WM_INPUT
			, KEYFIRST = windows::WM_KEYFIRST
			, KEYDOWN = windows::WM_KEYDOWN
			, KEYUP = windows::WM_KEYUP
			, CHAR = windows::WM_CHAR
			, DEADCHAR = windows::WM_DEADCHAR
			, SYSKEYDOWN = windows::WM_SYSKEYDOWN
			, SYSKEYUP = windows::WM_SYSKEYUP
			, SYSCHAR = windows::WM_SYSCHAR
			, SYSDEADCHAR = windows::WM_SYSDEADCHAR
			, UNICHAR = windows::WM_UNICHAR
			, KEYLAST = windows::WM_KEYLAST
			, IME_STARTCOMPOSITION = windows::WM_IME_STARTCOMPOSITION
			, IME_ENDCOMPOSITION = windows::WM_IME_ENDCOMPOSITION
			, IME_COMPOSITION = windows::WM_IME_COMPOSITION
			, IME_KEYLAST = windows::WM_IME_KEYLAST
			, INITDIALOG = windows::WM_INITDIALOG
			, COMMAND = windows::WM_COMMAND
			, SYSCOMMAND = windows::WM_SYSCOMMAND
			, TIMER = windows::WM_TIMER
			, HSCROLL = windows::WM_HSCROLL
			, VSCROLL = windows::WM_VSCROLL
			, INITMENU = windows::WM_INITMENU
			, INITMENUPOPUP = windows::WM_INITMENUPOPUP
			, GESTURE = windows::WM_GESTURE
			, GESTURENOTIFY = windows::WM_GESTURENOTIFY
			, MENUSELECT = windows::WM_MENUSELECT
			, MENUCHAR = windows::WM_MENUCHAR
			, ENTERIDLE = windows::WM_ENTERIDLE
			, MENURBUTTONUP = windows::WM_MENURBUTTONUP
			, MENUDRAG = windows::WM_MENUDRAG
			, MENUGETOBJECT = windows::WM_MENUGETOBJECT
			, UNINITMENUPOPUP = windows::WM_UNINITMENUPOPUP
			, MENUCOMMAND = windows::WM_MENUCOMMAND
			, CHANGEUISTATE = windows::WM_CHANGEUISTATE
			, UPDATEUISTATE = windows::WM_UPDATEUISTATE
			, QUERYUISTATE = windows::WM_QUERYUISTATE
			, CTLCOLORMSGBOX = windows::WM_CTLCOLORMSGBOX
			, CTLCOLOREDIT = windows::WM_CTLCOLOREDIT
			, CTLCOLORLISTBOX = windows::WM_CTLCOLORLISTBOX
			, CTLCOLORBTN = windows::WM_CTLCOLORBTN
			, CTLCOLORDLG = windows::WM_CTLCOLORDLG
			, CTLCOLORSCROLLBAR = windows::WM_CTLCOLORSCROLLBAR
			, CTLCOLORSTATIC = windows::WM_CTLCOLORSTATIC
			, MOUSEFIRST = windows::WM_MOUSEFIRST
			, MOUSEMOVE = windows::WM_MOUSEMOVE
			, LBUTTONDOWN = windows::WM_LBUTTONDOWN
			, LBUTTONUP = windows::WM_LBUTTONUP
			, LBUTTONDBLCLK = windows::WM_LBUTTONDBLCLK
			, RBUTTONDOWN = windows::WM_RBUTTONDOWN
			, RBUTTONUP = windows::WM_RBUTTONUP
			, RBUTTONDBLCLK = windows::WM_RBUTTONDBLCLK
			, MBUTTONDOWN = windows::WM_MBUTTONDOWN
			, MBUTTONUP = windows::WM_MBUTTONUP
			, MBUTTONDBLCLK = windows::WM_MBUTTONDBLCLK
			, MOUSEWHEEL = windows::WM_MOUSEWHEEL
			, XBUTTONDOWN = windows::WM_XBUTTONDOWN
			, XBUTTONUP = windows::WM_XBUTTONUP
			, XBUTTONDBLCLK = windows::WM_XBUTTONDBLCLK
			, MOUSEHWHEEL = windows::WM_MOUSEHWHEEL
			, MOUSELAST = windows::WM_MOUSELAST
			, PARENTNOTIFY = windows::WM_PARENTNOTIFY
			, ENTERMENULOOP = windows::WM_ENTERMENULOOP
			, EXITMENULOOP = windows::WM_EXITMENULOOP
			, NEXTMENU = windows::WM_NEXTMENU
			, SIZING = windows::WM_SIZING
			, CAPTURECHANGED = windows::WM_CAPTURECHANGED
			, MOVING = windows::WM_MOVING
			, POWERBROADCAST = windows::WM_POWERBROADCAST
			, DEVICECHANGE = windows::WM_DEVICECHANGE
			, MDICREATE = windows::WM_MDICREATE
			, MDIDESTROY = windows::WM_MDIDESTROY
			, MDIACTIVATE = windows::WM_MDIACTIVATE
			, MDIRESTORE = windows::WM_MDIRESTORE
			, MDINEXT = windows::WM_MDINEXT
			, MDIMAXIMIZE = windows::WM_MDIMAXIMIZE
			, MDITILE = windows::WM_MDITILE
			, MDICASCADE = windows::WM_MDICASCADE
			, MDIICONARRANGE = windows::WM_MDIICONARRANGE
			, MDIGETACTIVE = windows::WM_MDIGETACTIVE
			, MDISETMENU = windows::WM_MDISETMENU
			, ENTERSIZEMOVE = windows::WM_ENTERSIZEMOVE
			, EXITSIZEMOVE = windows::WM_EXITSIZEMOVE
			, DROPFILES = windows::WM_DROPFILES
			, MDIREFRESHMENU = windows::WM_MDIREFRESHMENU
			, POINTERDEVICECHANGE = windows::WM_POINTERDEVICECHANGE
			, POINTERDEVICEINRANGE = windows::WM_POINTERDEVICEINRANGE
			, POINTERDEVICEOUTOFRANGE = windows::WM_POINTERDEVICEOUTOFRANGE
			, TOUCH = windows::WM_TOUCH
			, NCPOINTERUPDATE = windows::WM_NCPOINTERUPDATE
			, NCPOINTERDOWN = windows::WM_NCPOINTERDOWN
			, NCPOINTERUP = windows::WM_NCPOINTERUP
			, POINTERUPDATE = windows::WM_POINTERUPDATE
			, POINTERDOWN = windows::WM_POINTERDOWN
			, POINTERUP = windows::WM_POINTERUP
			, POINTERENTER = windows::WM_POINTERENTER
			, POINTERLEAVE = windows::WM_POINTERLEAVE
			, POINTERACTIVATE = windows::WM_POINTERACTIVATE
			, POINTERCAPTURECHANGED = windows::WM_POINTERCAPTURECHANGED
			, TOUCHHITTESTING = windows::WM_TOUCHHITTESTING
			, POINTERWHEEL = windows::WM_POINTERWHEEL
			, POINTERHWHEEL = windows::WM_POINTERHWHEEL
			, POINTERROUTEDTO = windows::WM_POINTERROUTEDTO
			, POINTERROUTEDAWAY = windows::WM_POINTERROUTEDAWAY
			, POINTERROUTEDRELEASED = windows::WM_POINTERROUTEDRELEASED
			, IME_SETCONTEXT = windows::WM_IME_SETCONTEXT
			, IME_NOTIFY = windows::WM_IME_NOTIFY
			, IME_CONTROL = windows::WM_IME_CONTROL
			, IME_COMPOSITIONFULL = windows::WM_IME_COMPOSITIONFULL
			, IME_SELECT = windows::WM_IME_SELECT
			, IME_CHAR = windows::WM_IME_CHAR
			, IME_REQUEST = windows::WM_IME_REQUEST
			, IME_KEYDOWN = windows::WM_IME_KEYDOWN
			, IME_KEYUP = windows::WM_IME_KEYUP
			, MOUSEHOVER = windows::WM_MOUSEHOVER
			, MOUSELEAVE = windows::WM_MOUSELEAVE
			, NCMOUSEHOVER = windows::WM_NCMOUSEHOVER
			, NCMOUSELEAVE = windows::WM_NCMOUSELEAVE
			, WTSSESSION_CHANGE = windows::WM_WTSSESSION_CHANGE
			, TABLET_FIRST = windows::WM_TABLET_FIRST
			, TABLET_LAST = windows::WM_TABLET_LAST
			, DPICHANGED = windows::WM_DPICHANGED
			, DPICHANGED_BEFOREPARENT = windows::WM_DPICHANGED_BEFOREPARENT
			, DPICHANGED_AFTERPARENT = windows::WM_DPICHANGED_AFTERPARENT
			, GETDPISCALEDSIZE = windows::WM_GETDPISCALEDSIZE
			, CUT = windows::WM_CUT
			, COPY = windows::WM_COPY
			, PASTE = windows::WM_PASTE
			, CLEAR = windows::WM_CLEAR
			, UNDO = windows::WM_UNDO
			, RENDERFORMAT = windows::WM_RENDERFORMAT
			, RENDERALLFORMATS = windows::WM_RENDERALLFORMATS
			, DESTROYCLIPBOARD = windows::WM_DESTROYCLIPBOARD
			, DRAWCLIPBOARD = windows::WM_DRAWCLIPBOARD
			, PAINTCLIPBOARD = windows::WM_PAINTCLIPBOARD
			, VSCROLLCLIPBOARD = windows::WM_VSCROLLCLIPBOARD
			, SIZECLIPBOARD = windows::WM_SIZECLIPBOARD
			, ASKCBFORMATNAME = windows::WM_ASKCBFORMATNAME
			, CHANGECBCHAIN = windows::WM_CHANGECBCHAIN
			, HSCROLLCLIPBOARD = windows::WM_HSCROLLCLIPBOARD
			, QUERYNEWPALETTE = windows::WM_QUERYNEWPALETTE
			, PALETTEISCHANGING = windows::WM_PALETTEISCHANGING
			, PALETTECHANGED = windows::WM_PALETTECHANGED
			, HOTKEY = windows::WM_HOTKEY
			, PRINT = windows::WM_PRINT
			, PRINTCLIENT = windows::WM_PRINTCLIENT
			, APPCOMMAND = windows::WM_APPCOMMAND
			, THEMECHANGED = windows::WM_THEMECHANGED
			, CLIPBOARDUPDATE = windows::WM_CLIPBOARDUPDATE
			, DWMCOMPOSITIONCHANGED = windows::WM_DWMCOMPOSITIONCHANGED
			, DWMNCRENDERINGCHANGED = windows::WM_DWMNCRENDERINGCHANGED
			, DWMCOLORIZATIONCOLORCHANGED = windows::WM_DWMCOLORIZATIONCOLORCHANGED
			, DWMWINDOWMAXIMIZEDCHANGE = windows::WM_DWMWINDOWMAXIMIZEDCHANGE
			, DWMSENDICONICTHUMBNAIL = windows::WM_DWMSENDICONICTHUMBNAIL
			, DWMSENDICONICLIVEPREVIEWBITMAP = windows::WM_DWMSENDICONICLIVEPREVIEWBITMAP
			, GETTITLEBARINFOEX = windows::WM_GETTITLEBARINFOEX
			, HANDHELDFIRST = windows::WM_HANDHELDFIRST
			, HANDHELDLAST = windows::WM_HANDHELDLAST
			, AFXFIRST = windows::WM_AFXFIRST
			, AFXLAST = windows::WM_AFXLAST
			, PENWINFIRST = windows::WM_PENWINFIRST
			, PENWINLAST = windows::WM_PENWINLAST
			, APP = windows::WM_APP
			, USER = windows::WM_USER
		};
		typedef windows::UINT WindowsMessage_t;
		constexpr WindowsMessage_t operator+(WindowsMessage e)
		{
			return static_cast<WindowsMessage_t>(e);
		}
		namespace WindowClassStyle
		{
			enum WindowClassStyle : windows::UINT
			{
				NONE = 0
				, ENABLE_DOUBLE_CLICK = windows::CS_DBLCLKS //Allows the receiving of doubleclick messages.
				, X_RESIZE_REDRAW = windows::CS_HREDRAW //Redraw entire window when resizing horizontally.
				, Y_RESIZE_REDRAW = windows::CS_VREDRAW //Redraw entire window when resizing vertically.
				, NO_CLOSE = windows::CS_NOCLOSE //Disables the close button.
				, USE_PARENT_DRAW = windows::CS_PARENTDC //This window's clipping region is the entire parent. Faster, but now you can graffiti yourself. Child window only.
				, PRESERVE_OBSCURED = windows::CS_SAVEBITS //The area this window covers is stored in a bitmap and won't need to be redrawn.
			};
		}
		namespace WindowStyle
		{
			enum WindowStyle : windows::DWORD
			{
				RESIZEABLE = 0x1
				, RAISED_EDGE = 0x2 //Outside border appears raised
				, SUNKEN_EDGE = 0x4 //Inside border appears sunken
				, TITLE = 0x8 //Visible title
				, CLIP_SIBLING_BOUNDS = 0x10 //Remove sibling window bounds from draw area (you won't paint over them either way)
				, CLIP_CHILD_BOUNDS = 0x20 //Remove child window bounds from draw area (you won't paint over them either way)
				, HORIZ_SCROLL = 0x40
				, VERT_SCROLL = 0x80
				, MINIMIZE_BUTTON = 0x108
				, MAXIMIZE_BUTTON = 0x208
			};
		}

		class Window;

		struct Message
		{
			typedef windows::WPARAM IntParam;
			typedef windows::LPARAM LongParam;
			typedef windows::DWORD Time;
			typedef pair::_pair<long> Point;
			typedef windows::LRESULT Result;

			Window* window = nullptr;
			WindowsMessage_t message = 0;
			IntParam wparam = 0;
			LongParam lparam = 0;
			std::optional<Time> time;
			std::optional<Point> cursor_pos;

			Message() = default;
			Message(Window*, WindowsMessage_t, IntParam, LongParam);
			Message(Window*, WindowsMessage_t, IntParam, LongParam, Time, Point);
			Message(const Message&) = default;
			~Message() = default;
			Message& operator=(const Message&) = default;

			static Point translate_lparam_as_point(LongParam);
		};

		class Listener
		{
		public:
			enum class Location
			{
				QUEUE //Message will be sent to this Listener's queue.
				, CALLBACK //Message will be sent to this Listener's callback function.
			};
			enum class Priority
			{
				HERE_ONLY //Ignore other handler requests. In case of tie, Window > WindowClass > WindowManager.
				, ANYWHERE //Handling can be shared.
				, IF_ONLY_HERE //Handle here if nowhere else does. In case of tie, WindowManager > WindowClass > Window.
			};
			struct ListenInit
			{
				WindowsMessage_t id = 0;
				Location l = Location::QUEUE;
				Priority p = Priority::ANYWHERE;
			};
			Listener() = default;
			Listener(const Listener&) = delete;
			~Listener() = default;
			Listener& operator=(const Listener&) = delete;
			template <class Func> void set_callback(Func&& callback)
			{
				std::lock_guard<std::mutex> lock(_callback_guard);
				_callback = Callback(std::forward<Func>(callback));
			}
			void listen(WindowsMessage_t, Location = Location::QUEUE, Priority = Priority::ANYWHERE);
			void listen(std::initializer_list<WindowsMessage_t>, Location = Location::QUEUE, Priority = Priority::ANYWHERE);
			void listen(std::initializer_list<ListenInit>);
			template <class InIterB, class InIterE> auto listen(InIterB&& begin, InIterE&& end, Location loc, Priority pri)
			{
				std::lock_guard<std::mutex> lock(_map_guard);
				std::for_each(begin, end, [=, this](WindowsMessage_t id) { this->_listening[id] = { loc, pri }; });
			}
			template <class InIterB, class InIterE> auto listen(InIterB&& begin, InIterE&& end)
			{
				std::lock_guard<std::mutex> lock(_map_guard);
				std::for_each(begin, end, [this](const ListenInit& init) { _listening[init.id] = { init.l, init.p }; });
			}
			void stop_listen(WindowsMessage_t);
			void stop_listen(std::initializer_list<WindowsMessage_t>);
			template <class InIterB, class InIterE> void stop_listen(InIterB&& begin, InIterE&& end)
			{
				std::lock_guard<std::mutex> lock(_map_guard);
				std::for_each(begin, end, [this](WindowsMessage_t id) { auto it = _listening.find(id); if (it != _listening.end()) _listening.erase(it); });
			}
			std::vector<Message> get_messages();
			void wait_for_message() const;
		private:
			void _queue_message(const Message&);
			windows::LRESULT _callback_message(const Message&);
			typedef std::function<windows::LRESULT(Message)> Callback;
			Callback _callback;
			std::vector<Message> _messages;
			std::map<WindowsMessage_t, std::pair<Location, Priority>> _listening;
			mutable std::mutex _message_guard, _map_guard, _callback_guard;
			mutable std::condition_variable _cv;
			friend class WindowManager;
			friend class Window;
		};

		class WindowManager
		{
		public:
			Listener listener;
			WindowManager();
			WindowManager(const WindowManager&) = delete;
			~WindowManager();
			WindowManager& operator=(const WindowManager&) = delete;
			//Message::Result send_message(const Message&);
			//std::future<Message::Result> send_message_async(const Message&);
			void quit();
			bool has_quit() const;
			operator bool() const;
			const WindowManager& wait_for_message() const;
			static Message::Result handle_message_default(const Message&);
		private:
			static windows::LRESULT __stdcall _wndproc(windows::HWND, windows::UINT, windows::WPARAM, windows::LPARAM);
			std::optional<windows::LRESULT> _dispatch_message(const Message&, bool in_pump);
			void _pump_func();
			void _send_function(struct _package*, bool cleanup);
			void _send_message(windows::UINT, windows::WPARAM, windows::LPARAM);
			std::atomic_bool _quit = false, _ready = false;
			std::thread _pump{ std::bind(&WindowManager::_pump_func, this) };
			friend class WindowClass;
			friend class Window;
		};

		class WindowClass
		{
		public:
			typedef windows::UINT Style;
			static constexpr Style DEFAULT_STYLE = WindowClassStyle::ENABLE_DOUBLE_CLICK | WindowClassStyle::USE_PARENT_DRAW | WindowClassStyle::X_RESIZE_REDRAW | WindowClassStyle::Y_RESIZE_REDRAW;
			Listener listener;
			WindowClass(const StartupData&, Style, graphics::Brush background, graphics::Cursor);
			WindowClass(const StartupData&, Style, graphics::Brush background, graphics::Cursor, graphics::Icon large, graphics::Icon small);
			WindowClass(const WindowClass&) = delete;
			~WindowClass();
			WindowClass& operator=(const WindowClass&) = delete;
		private:
			static size_t _name_counter;
			static bstring _make_name(size_t);
			void _init(Style);
			bstring _name;
			const  StartupData& _data;
			const graphics::Brush _brush;
			const graphics::Cursor _cursor;
			const std::optional<std::pair<graphics::Icon, graphics::Icon>> _icons;
			friend class Window;
		};

		class Window
		{
		public:
			typedef windows::DWORD Style;
			typedef pair::_pair<long> Point, Size;
			typedef pair::_rectangle<long> Rectangle;
			class Canvas
			{
			public:
				typedef pair::_pair<long double> Scale;
				Canvas() = default;
				Canvas(const Canvas&);
				Canvas(Canvas&&);
				~Canvas() = default;
				Canvas& operator=(const Canvas&);
				Canvas& operator=(Canvas&&);
				Scale scale() const;
				unsigned long buffer() const;
				Size size() const;
				bool empty() const;
				void set_def_text_color(graphics::Color);
				void set_def_text_back(graphics::Color);
				void set_def_text_back_transparent(bool);
				void set_def_brush(const graphics::Brush&);
				void set_def_pen(const graphics::Pen&);
			private:
				Canvas(Window&, Size);
				std::mutex _guard;
				Window* _window = nullptr;
				std::optional<graphics::Bitmap> _bitmap;
				Scale _scale;
				unsigned long _buffer = 0;
				//HDC stuff
				std::optional<graphics::Color> _textc, _textb;
				std::optional<bool> _textm;
				std::optional<graphics::Brush> _brush;
				std::optional<graphics::Pen> _pen;
				friend class Window;
				friend class ::bleak::graphics::Draw;
			};
			enum class ShownState
			{
				NORMAL,
				MINIMIZED,
				MAXIMIZED,
				HIDDEN
			};
			static constexpr Style DEFAULT_STYLE = WindowStyle::RESIZEABLE | WindowStyle::MAXIMIZE_BUTTON | WindowStyle::MINIMIZE_BUTTON | WindowStyle::RAISED_EDGE | WindowStyle::SUNKEN_EDGE;
			Listener listener;
			Canvas canvas;
			Window(WindowManager&, WindowClass&, Style = DEFAULT_STYLE, const bstring& title = STR(""), Size canvas_size = { 0, 0 }, graphics::AlignMode_t canvas_alignment = graphics::AlignMode::FIT, ShownState = ShownState::NORMAL, Window* parent = nullptr);
			Window(WindowManager&, WindowClass&, Point, Size, Style = DEFAULT_STYLE, const bstring& title = STR(""), Size canvas_size = { 0, 0 }, graphics::AlignMode_t canvas_alignment = graphics::AlignMode::FIT, ShownState = ShownState::NORMAL, Window* parent = nullptr);
			Window(const Window&) = delete;
			~Window();
			Window& operator=(const Window&) = delete;
			WindowManager& manager() const;
			WindowClass& wclass() const;
			bool closed() const;
			operator bool() const;
			Rectangle bounds() const;
			Size inner_size() const;
			ShownState shown() const;
			bool enabled() const;
			bool active() const;
			void move(Point, Size, bool desired_inner_size = true);
			void show(ShownState);
			void enable(bool);
			void activate();
			void set_title(const bstring&);
			void close(bool unstoppable);
			Point screen_to_inner(Point) const;
			Point inner_to_screen(Point) const;
			Point inner_to_canvas(Point) const;
			windows::HWND native_handle() const;
			std::vector<Message> get_messages_from_all();
		private:
			static ShownState _translate_shown_state(windows::UINT);
			static windows::UINT _translate_shown_state(ShownState);
			windows::LRESULT _wndproc(windows::HWND, windows::UINT, windows::WPARAM, windows::LPARAM);
			WindowManager& _manager;
			WindowClass& _class;
			windows::HWND _hwnd = nullptr;
			std::mutex _bitmap_guard;
			graphics::AlignMode_t _align_mode;
			bool _last_msg_seen = false;
			//Resizing/moving wndproc stuff
			enum MoveSizeMode
			{
				NORMAL,
				MOVE,
				SIZE
			};
			MoveSizeMode _mode = MoveSizeMode::NORMAL;
			windows::WPARAM _last_ht;
			windows::POINTS _last_pos;
			unsigned char _resize_flags;
			friend class WindowManager;
			friend class graphics::Draw;
		};
	}
}