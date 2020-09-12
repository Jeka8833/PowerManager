#pragma once
#pragma comment(lib, "powrprof.lib")

#include <string>
#include <thread>
#include <atomic>
#include <regex>
#include <filesystem>

#include <Windows.h>
#include "Profile.h"
#include "Config.h"
#include <Psapi.h>
#include <powrprof.h>

class Core {
public:
	static int currentProfile;

	static void updateTriggerThread();
	static void runProgramm(bool isShow);

	static void useProfile(bool force);
private:
	static int lastProfile;

	static std::atomic_bool forceStop;
	static std::atomic_bool isRunning;

	static void triggerLoop();

};

