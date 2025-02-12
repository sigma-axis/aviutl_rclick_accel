/*
The MIT License (MIT)

Copyright (c) 2025 sigma-axis

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <cstdint>
#include <string>
#include <vector>
#include <bit>
#include <map>
#include <set>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using byte = uint8_t;
#include <exedit.hpp>

#include "memory_protect.hpp"


////////////////////////////////
// 文字列操作．
////////////////////////////////
template<uint32_t codepage>
struct Encode {
	constexpr static uint32_t CodePage = codepage;

	// conversion between wide character string.
	static int cnt_wide_str(char const* str, int cnt_str = -1) {
		return to_wide_str(nullptr, 0, str, cnt_str);
	}
	static int to_wide_str(wchar_t* wstr, int cnt_wstr, char const* str, int cnt_str = -1) {
		return ::MultiByteToWideChar(CodePage, 0, str, cnt_str, wstr, cnt_wstr);
	}
	template<size_t cnt_wstr>
	static int to_wide_str(wchar_t(&wstr)[cnt_wstr], char const* str, int cnt_str = -1) {
		return to_wide_str(wstr, int{ cnt_wstr }, str, cnt_str);
	}
	static std::wstring to_wide_str(char const* str, int cnt_str = -1) {
		size_t cntw = cnt_wide_str(str, cnt_str);
		if (cntw == 0 || (cnt_str >= 0 && str[cnt_str - 1] != '\0')) cntw++;
		std::wstring ret(cntw - 1, L'\0');
		to_wide_str(ret.data(), cntw, str, cnt_str);
		return ret;
	}
	static std::wstring to_wide_str(std::string const& str) { return to_wide_str(str.c_str()); }

	static int cnt_narrow_str(wchar_t const* wstr, int cnt_wstr = -1) {
		return from_wide_str(nullptr, 0, wstr, cnt_wstr);
	}
	static int from_wide_str(char* str, int cnt_str, wchar_t const* wstr, int cnt_wstr = -1) {
		return ::WideCharToMultiByte(CodePage, 0, wstr, cnt_wstr, str, cnt_str, nullptr, nullptr);
	}
	template<size_t cnt_str>
	static int from_wide_str(char(&str)[cnt_str], wchar_t const* wstr, int cnt_wstr = -1) {
		return from_wide_str(str, int{ cnt_str }, wstr, cnt_wstr);
	}
	static std::string from_wide_str(wchar_t const* wstr, int cnt_wstr = -1) {
		size_t cnt = cnt_narrow_str(wstr, cnt_wstr);
		if (cnt == 0 || (cnt_wstr >= 0 && wstr[cnt_wstr - 1] != L'\0')) cnt++;
		std::string ret(cnt - 1, '\0');
		from_wide_str(ret.data(), cnt, wstr, cnt_wstr);
		return ret;
	}
	static std::string from_wide_str(std::wstring const& wstr) { return from_wide_str(wstr.c_str()); }
};
using encode_sjis = Encode<932>;
using encode_utf8 = Encode<CP_UTF8>;
using encode_sys = Encode<CP_ACP>;


////////////////////////////////
// 主要情報源の変数アドレス．
////////////////////////////////
inline constinit struct ExEdit092 {
	AviUtl::FilterPlugin* fp;
	constexpr static char const* info_exedit092 = "拡張編集(exedit) version 0.92 by ＫＥＮくん";
	bool init(AviUtl::FilterPlugin* this_fp)
	{
		if (fp != nullptr) return true;
		AviUtl::SysInfo si; this_fp->exfunc->get_sys_info(nullptr, &si);
		for (int i = 0; i < si.filter_n; i++) {
			auto that_fp = this_fp->exfunc->get_filterp(i);
			if (that_fp->information != nullptr &&
				0 == std::strcmp(that_fp->information, info_exedit092)) {
				fp = that_fp;
				return true;
			}
		}
		return false;
	}

	HMENU get_menu_handle(ptrdiff_t offset) const {
		return *reinterpret_cast<HMENU*>(reinterpret_cast<uintptr_t>(fp->dll_hinst) + offset);
	}
	uintptr_t get_code_address(ptrdiff_t offset) const {
		return reinterpret_cast<uintptr_t>(fp->dll_hinst) + offset;
	}
} exedit;


////////////////////////////////
// 文字列置き換え対象のデータ．
////////////////////////////////
struct modify_target {
	char const* section;
	std::vector<ptrdiff_t> const offsets; // HMENU が格納されている exedit.fp->dll_hinst からの相対アドレス．
	std::vector<wchar_t const*> const pending_roots;

	// セパレータ追加を指定する接頭辞．
	constexpr static std::wstring_view sep_prefix = L"sep:";
};

static modify_target const menu_data[] = {
	{
		"timeline_blank",
		{
			0x15917c, // プロジェクト未ロード時に右クリック．
			0x168598, // プロジェクトロード時に空白を右クリック．
		},
		{ L"メディアオブジェクトの追加", L"フィルタオブジェクトの追加", },
	},
	{
		"timeline_object",
		{ 0x159170, }, // タイムライン上のオブジェクトを右クリック．
		{},
	},
	{
		"layer",
		{ 0x158f5c, }, // レイヤーを右クリック．
		{},
	},
	{
		"scene",
		{ 0x158f64, }, // タイムライン左上のシーン部分を右クリック．
		{},
	},
	{
		"setting_dialog",
		{
			0x158d2c, // 図形やテキストオブジェクト，グループ制御など．
			0x158d24, // 音声系オブジェクト．
			0x167d40, // カメラ制御と時間制御．
			0x158d20, // 画像系フィルタオブジェクト（シーンチェンジなど）．
			0x167d44, // 音声系フィルタオブジェクト．
		},
		{ L"フィルタ効果の追加", },
	},
	{
		"setting_dialog_interval",
		{ 0x159184, }, // 設定ダイアログ上部の中間点配置などの表示を右クリック．
		{},
	},
};
static modify_target const filter_menu_data = {
	"filters",
	{
		0x167d78, // 図形やテキストオブジェクトなどの，入力フィルタ切り替えボタン．
		0x167d3c, // 音声系オブジェクトの，入力フィルタ切り替えボタン．
	},
	{},
};

constexpr static struct {
	constexpr static ptrdiff_t offset_root_menu = 0x159178;
	constexpr static UINT id_acc = 0x41, id_dec = 0x21, id_param = 0x462; // 加速 / 減速 / 設定．
	constexpr static char const* setting_section = "easing_settings", *item_section = "easings",
		*acc_name = "加速", *dec_name = "減速", *param_name = "設定";
	constexpr static ptrdiff_t
		offset_code_param_1 = 0x2d417, // ee + 0x2d417: 68 xx yy zz ww	// push	wwzzyyxx ("設定 [%d]")
		offset_code_param_0 = 0x2d461; // ee + 0x2d461: 68 xx yy zz ww	// push	wwzzyyxx ("設定")

	constexpr static char const*
		param_alt_text_1_fmt = "設定 (&%c) [%%d]", * param_alt_text_0_fmt = "設定 (&%c)";
#ifdef NDEBUG
	constinit
#endif
	static inline std::string param_alt_text_1{}, param_alt_text_0{}; // memory storage for the modified text.
} easing_menu;


////////////////////////////////
// メニューテキストの置き換え．
////////////////////////////////
inline static char read_accel_key(char const* ini_file, char const* section, char const* item_name)
{
	// read one character from .ini file.
	char buf[4], &key = buf[0];
	::GetPrivateProfileStringA(section, item_name, "", buf, std::size(buf), ini_file);

	// check if the key is valid.
	if ('\x2a' <= key && key <= '\x7e') key = std::toupper(key);
	else key = '\0';
	return key;
}
struct sep_pos {
	int8_t raw;
	constexpr static int8_t flag_above = 0b01, flag_below = 0b10;
	constexpr bool above() const { return (raw & flag_above) != 0; }
	constexpr bool below() const { return (raw & flag_below) != 0; }
	constexpr operator bool() const { return raw != 0; }
};
inline static sep_pos read_insert_sep(char const* ini_file, char const* section, char const* sep_item_name)
{
	return { static_cast<decltype(sep_pos::raw)>(
		::GetPrivateProfileIntA(section, sep_item_name, 0, ini_file)) };
}
inline static auto read_item_modification(char const* ini_file, char const* section, std::wstring const& item_name)
{
	auto sep_item_name = encode_sys::from_wide_str(std::wstring{ modify_target::sep_prefix } + item_name);

	// load the accelerator key from the .ini file.
	char key = read_accel_key(ini_file, section, sep_item_name.c_str() + modify_target::sep_prefix.size());

	// load whether the separator should be inserted.
	auto sep = read_insert_sep(ini_file, section, sep_item_name.c_str());

	return std::pair{
		key == '\0' ? L"" : item_name + L" (&" + static_cast<wchar_t>(key) + L")" ,
		sep,
	};
}

inline static bool allow_separator_at(HMENU hmenu, int pos)
{
	if (pos < 0 || pos >= ::GetMenuItemCount(hmenu)) return false;

	MENUITEMINFOW mii{
		.cbSize = sizeof(mii),
		.fMask = MIIM_FTYPE,
	};
	return ::GetMenuItemInfoW(hmenu, pos, TRUE, &mii) != FALSE
		&& (mii.fType & MFT_SEPARATOR) == 0;
}
inline static void modify_menu_items(HMENU hmenu, char const* ini_file, char const* section,
	std::map<std::wstring, std::pair<std::wstring, sep_pos>>& ini_cache,
	std::set<HMENU>* pendings, std::set<std::wstring> const* pending_roots)
{
	std::set<HMENU> handled{}; // to avoid potential infinite loop.
	[&](this auto&& self, HMENU hmenu) -> void {
		handled.emplace(hmenu);

		for (int i = ::GetMenuItemCount(hmenu); --i >= 0; ) {
			// get the name and submenu of the menu item.
			std::wstring name; HMENU submenu;
			MENUITEMINFOW mii{
				.cbSize = sizeof(mii),
				.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE,
			};
			if (wchar_t buf[256]; mii.dwTypeData = buf, mii.cch = std::size(buf),
				::GetMenuItemInfoW(hmenu, i, TRUE, &mii) == FALSE ||
				buf[0] == L'\0' || (mii.fType & MFT_SEPARATOR) != 0) continue;
			else {
				name = buf;
				submenu = mii.hSubMenu;
			}

			// find or create the cache data of the .ini file.
			auto it = ini_cache.find(name);
			if (it == ini_cache.end())
				it = ini_cache.emplace(name, read_item_modification(ini_file, section, name)).first;
			auto const& [repl, sep] = it->second;

			// replace the text if valid.
			if (!repl.empty()) {
				mii.fMask = MIIM_STRING;
				mii.dwTypeData = const_cast<wchar_t*>(repl.c_str());
				::SetMenuItemInfoW(hmenu, i, TRUE, &mii);
			}

			// insert a separator if specified.
			if (sep) {
				mii.fMask = MIIM_FTYPE;
				mii.fType = MFT_SEPARATOR;

				if (sep.below() && allow_separator_at(hmenu, i + 1))
					::InsertMenuItemW(hmenu, i + 1, TRUE, &mii);
				if (sep.above() && allow_separator_at(hmenu, i - 1))
					::InsertMenuItemW(hmenu, i, TRUE, &mii);
			}

			// to the deeper level.
			if (submenu != nullptr) {
				if (pendings != nullptr && pending_roots != nullptr && pending_roots->contains(name))
					// handle this later.
					pendings->emplace(submenu);
				else if (!handled.contains(submenu))
					// recursively handle this branch.
					self(submenu);
			}
		}
	} (hmenu);
}

inline static void modify_menu_text(char const* ini_file, modify_target const& target,
	std::set<HMENU>& pendings)
{
	std::map<std::wstring, std::pair<std::wstring, sep_pos>> ini_cache{};
	std::set<std::wstring> filter_roots{};

	// prepare filter_roots.
	for (auto const& parent : target.pending_roots)
		filter_roots.emplace(parent);

	// apply modification to all targets.
	for (auto const& offset : target.offsets) {
		modify_menu_items(::GetSubMenu(exedit.get_menu_handle(offset), 0),
			ini_file, target.section,
			ini_cache, &pendings, &filter_roots);
	}
}

inline static void modify_easing_menu_text(char const* ini_file)
{
	HMENU menu = ::GetSubMenu(exedit.get_menu_handle(easing_menu.offset_root_menu), 0);

	// list of easing items.
	{
		std::map<std::wstring, std::pair<std::wstring, sep_pos>> ini_cache{};
		modify_menu_items(menu, ini_file, easing_menu.item_section, ini_cache, nullptr, nullptr);
	}

	// acceleration and deceleration settings.
	auto handle_acc_dec = [&](UINT id, char const* name) {
		if (char key = read_accel_key(ini_file, easing_menu.setting_section, name);
			key != '\0') {
			// get the name of the menu (however it's most likely equivalent to `name`).
			wchar_t buf[32]; MENUITEMINFOW mii{
				.cbSize = sizeof(mii),
				.fMask = MIIM_STRING,
				.dwTypeData = buf, .cch = std::size(buf),
			};
			if (::GetMenuItemInfoW(menu, id, FALSE, &mii) != FALSE && buf[0] != L'\0') {
				// append the accelerator key.
				auto tmp = std::wstring{ buf } + L" (&" + static_cast<wchar_t>(key) + L")";
				mii.dwTypeData = tmp.data();

				// place it to the menu.
				::SetMenuItemInfoW(menu, id, FALSE, &mii);
			}
		}
	};
	handle_acc_dec(easing_menu.id_acc, easing_menu.acc_name);
	handle_acc_dec(easing_menu.id_dec, easing_menu.dec_name);

	// paramter setting.
	if (char key = read_accel_key(ini_file, easing_menu.setting_section, easing_menu.param_name);
		key != '\0') {
		auto replace_code_ref = [key](ptrdiff_t offset, char const* fmt, std::string& storage) {
			char buf[32]; ::sprintf_s(buf, fmt, key);
			storage = buf;
			memory::ProtectHelper::write(
				// rewriting the code to refer to the new text.
				// 68 xx yy zz ww	: push	wwzzyyxx
				exedit.get_code_address(offset) + 1,
				storage.c_str());
		};

		// "設定 [%d]" -> "設定 (&X) [%d]"
		replace_code_ref(easing_menu.offset_code_param_1,
			easing_menu.param_alt_text_1_fmt, easing_menu.param_alt_text_1);

		// "設定" -> "設定 (&X)"
		replace_code_ref(easing_menu.offset_code_param_0,
			easing_menu.param_alt_text_0_fmt, easing_menu.param_alt_text_0);
	}
}


////////////////////////////////
// AviUtlに渡す関数の定義．
////////////////////////////////
BOOL func_init(AviUtl::FilterPlugin* fp)
{
	// 情報源確保．
	if (!exedit.init(fp)) {
		::MessageBoxA(fp->hwnd, "拡張編集0.92が見つかりませんでした．",
			fp->name, MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// message-only window を作成，登録．これで NoConfig でも AviUtl からメッセージを受け取れる.
	fp->hwnd = ::CreateWindowExW(0, L"AviUtl", L"", 0, 0, 0, 0, 0,
		HWND_MESSAGE, nullptr, fp->hinst_parent, nullptr);

	return TRUE;
}

BOOL func_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
{
	switch (message) {
		using Message = AviUtl::FilterPlugin::WindowMessage;
	case Message::ChangeWindow:
	{
		// find the path to the ini file.
		char ini_file[MAX_PATH];
		auto len = ::GetModuleFileNameA(fp->dll_hinst, ini_file, std::size(ini_file));
		::strncpy_s(ini_file + len - 3, 4, "ini", 3);

		// apply the modifications to menus.
		std::set<HMENU> pendings{};

		// generic ones.
		for (auto& target : menu_data) modify_menu_text(ini_file, target, pendings);

		// filter menu.
		for (auto ofs : filter_menu_data.offsets)
			pendings.emplace(::GetSubMenu(exedit.get_menu_handle(ofs), 0));
		for (std::map<std::wstring, std::pair<std::wstring, sep_pos>> ini_cache{};
			auto hmenu : pendings) modify_menu_items(hmenu, ini_file, filter_menu_data.section,
				ini_cache, nullptr, nullptr);

		// easing menu.
		modify_easing_menu_text(ini_file);

		// もう必要ないので，message-only window を削除．コールバックも解除．
		fp->hwnd = nullptr; ::DestroyWindow(hwnd);
		fp->func_WndProc = nullptr;
		break;
	}
	default: break;
	}
	return FALSE;
}


////////////////////////////////
// Entry point.
////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hinst);
		break;
	}
	return TRUE;
}


////////////////////////////////
// 看板．
////////////////////////////////
#define PLUGIN_NAME		"右クリメニューショトカ追加"
#define PLUGIN_VERSION	"v1.12"
#define PLUGIN_AUTHOR	"sigma-axis"
#define PLUGIN_INFO_FMT(name, ver, author)	(name##" "##ver##" by "##author)
#define PLUGIN_INFO		PLUGIN_INFO_FMT(PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_AUTHOR)

extern "C" __declspec(dllexport) AviUtl::FilterPluginDLL* __stdcall GetFilterTable(void)
{
	// （フィルタとは名ばかりの）看板．
	using Flag = AviUtl::FilterPlugin::Flag;
	static constinit AviUtl::FilterPluginDLL filter{
		.flag = Flag::NoConfig | Flag::AlwaysActive | Flag::ExInformation,
		.name = PLUGIN_NAME,

		.func_init = func_init,
		.func_WndProc = func_wndproc,
		.information = PLUGIN_INFO,
	};
	return &filter;
}
