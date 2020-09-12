#include "Core.h"

int Core::currentProfile = 0;
int Core::lastProfile = 0;

std::atomic_bool Core::isRunning = false;
std::atomic_bool Core::forceStop = false;

void Core::updateTriggerThread() {
	if (isRunning == Config::config.enable_Triggers)
		return;
	if (Config::config.enable_Triggers) {
		std::thread thread(triggerLoop);
		thread.detach();
	}
	else {
		forceStop = true;
	}
}

void Core::runProgramm(bool isShow) {
	ShellExecuteA(nullptr, "open", std::filesystem::current_path().append("k15tk.exe").string().c_str(), isShow ? "-e" : nullptr, nullptr, SW_SHOW);
}

void Core::triggerLoop() {
	isRunning = true;
	while (!forceStop) {
		RECT a, b;
		const HWND currentWindow = GetForegroundWindow();
		GetWindowRect(currentWindow, &a);
		GetWindowRect(GetDesktopWindow(), &b);
		bool fullScreen = a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
		DWORD dwProcId;
		GetWindowThreadProcessId(GetForegroundWindow(), &dwProcId);
		HANDLE processInfo = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
		std::wstring fileName;
		if (processInfo != NULL) {
			TCHAR szName[MAX_PATH];
			GetModuleBaseName(processInfo, NULL, szName, MAX_PATH);
			fileName = std::wstring(szName);
			CloseHandle(processInfo);
		}
		int len = GetWindowTextLength(currentWindow) + 1;
		LPWSTR titleBuffer = new TCHAR[len];
		GetWindowText(currentWindow, titleBuffer, len);
		std::wstring title(titleBuffer);
		int index = 0;
		int priority = 0;
		for (int i = 0; i < Config::config.count_Triggers; i++) {
			Trigger trigger = Config::config.triggers[i];
			if ((trigger.title.empty() ? true : std::regex_search(title.begin(), title.end(), std::regex(trigger.title))) &&
				(trigger.fileName.empty() ? true : std::regex_search(fileName.begin(), fileName.end(), std::regex(trigger.fileName))) &&
				(!trigger.fullScreen || (trigger.fullScreen && fullScreen))) {

				if (trigger.priority < 0 || priority < 0) {
					if (trigger.priority < priority) {
						priority = trigger.priority;
						index = trigger.profile;
					}
				}
				else if (trigger.priority > priority) {
					priority = trigger.priority;
					index = trigger.profile;
				}
			}
		}
		currentProfile = priority == 0 ? Config::config.active_Profile : priority < 0 || index < Config::config.active_Profile ? index : Config::config.active_Profile;
		useProfile(false);
		Sleep(Config::config.trigger_Tick);
	}
	currentProfile = Config::config.active_Profile;
	useProfile(false);
	forceStop = false;
	isRunning = false;
}

void Core::useProfile(bool force) {
	if (force || currentProfile != lastProfile) {
		lastProfile = currentProfile;
		Profile profile = Config::config.profiles[currentProfile];
		std::filesystem::copy(profile.pressetPath, "default.cfg", std::filesystem::copy_options::overwrite_existing);
		runProgramm(true);

		Sleep(5000);

		const GUID real;
		std::wstring out(L"{" + std::wstring(profile.powerPlan.begin(), profile.powerPlan.end()) + L"}");
		if (CLSIDFromString(out.c_str(), (LPCLSID)&real) == NOERROR)
			PowerSetActiveScheme(0, &real);
		else
			MessageBox(NULL, L"Fail use power plan", L"Error", MB_ICONERROR);
		out.clear();
	}
}
