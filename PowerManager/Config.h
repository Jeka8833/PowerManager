#pragma once

#include <string>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

#include "Core.h"
#include "Profile.h"
#include "Trigger.h"

using namespace rapidjson;

class Config {

public:
	static Config config;

	Config();

	static void read(bool recurce = false);
	static void write();

	int active_Profile;
	int count_Profiles;
	int count_Triggers;
	int trigger_Tick;
	Profile* profiles;
	Trigger* triggers;
	bool enable_Triggers;

private:
	Document document;
};