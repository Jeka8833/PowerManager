#include "Main.h"

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_POWERBROADCAST:
		if (wParam == PBT_APMRESUMEAUTOMATIC)
			Core::useProfile(true);
		break;
	case WM_COMMAND:
		switch (wParam) {
		case BTN_ENABLE_TRIGGERS:
			Config::config.enable_Triggers = !Config::config.enable_Triggers;
			Core::updateTriggerThread();
			Config::write();
			break;
		case BTN_RUN:
			Core::runProgramm(false);
			break;
		case BTN_UPDATE:
			Config::read();
			Core::useProfile(true);
			break;
		case BTN_EXIT:
			DestroyWindow(window);
			break;
		default:
			int number = wParam - 8833;
			if (number >= 0 && Config::config.count_Profiles >= number) {
				Config::config.active_Profile = number;
				Core::currentProfile = number;
				Core::useProfile(false);
				Config::write();
			}
		}
		break;
	case WM_USER:
		switch (lParam) {
		case WM_RBUTTONUP:
			HMENU hMenu = CreatePopupMenu();
			AppendMenuA(hMenu, MF_STRING | MF_GRAYED, 0, ("Active: " + Config::config.profiles[Core::currentProfile].name).c_str());
			AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);

			HMENU profilesList = CreatePopupMenu();
			for (int i = 0; i < Config::config.count_Profiles; i++) {
				if (i == Config::config.active_Profile)
					AppendMenuA(profilesList, MF_STRING | MF_GRAYED, 833 + i, ("> " + Config::config.profiles[i].name).c_str());
				else
					AppendMenuA(profilesList, MF_STRING, 8833 + i, Config::config.profiles[i].name.c_str());
			}
			AppendMenuA(hMenu, MF_POPUP, (UINT)profilesList, "Profiles");
			if (Config::config.enable_Triggers)
				AppendMenuA(hMenu, MF_STRING | MF_CHECKED, BTN_ENABLE_TRIGGERS, "Enable triggers");
			else
				AppendMenuA(hMenu, MF_STRING, BTN_ENABLE_TRIGGERS, "Enable triggers");
			AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenuA(hMenu, MF_STRING, BTN_RUN, "Run k15tk");
			AppendMenuA(hMenu, MF_STRING, BTN_UPDATE, "Force update");
			AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenuA(hMenu, MF_STRING, BTN_EXIT, "Exit");

			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(window);
			TrackPopupMenu(hMenu, 0, curPoint.x, curPoint.y, 0, window, NULL);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int) {
	WNDCLASSEX main = { 0 };
	main.cbSize = sizeof(WNDCLASSEX);
	main.hInstance = instance;
	main.lpszClassName = L"Power Manager";
	main.lpfnWndProc = WndProc;
	RegisterClassEx(&main);

	HWND window = CreateWindowEx(0, L"Power Manager", NULL, 0, 0, 0, 0, 0, NULL, NULL, instance, NULL);

	NOTIFYICONDATA Icon = { 0 };
	Icon.cbSize = sizeof(NOTIFYICONDATA);
	Icon.hWnd = window;
	Icon.uVersion = NOTIFYICON_VERSION;
	Icon.uCallbackMessage = WM_USER;
	Icon.hIcon = LoadIcon(instance, MAKEINTRESOURCE(MAINICON));
	Icon.uFlags = NIF_MESSAGE | NIF_ICON;
	Shell_NotifyIcon(NIM_ADD, &Icon);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	Shell_NotifyIcon(NIM_DELETE, &Icon);
	return 0;
}
