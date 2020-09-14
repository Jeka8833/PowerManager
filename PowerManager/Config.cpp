#include "Config.h"

Config Config::config = Config();

Config::Config() {
	read();
	Core::currentProfile = active_Profile;
	Core::updateTriggerThread();
	Core::useProfile(true);
}

void Config::read(bool recurce) {
	std::ifstream in("config.json");
	std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	ParseResult ok = config.document.Parse(text.c_str());
	if (ok) {
		if (!std::filesystem::exists("config-back.json"))
			std::filesystem::copy("config.json", "config-back.json", std::filesystem::copy_options::overwrite_existing);

		config.active_Profile = config.document["ActiveProfile"].GetInt();
		config.enable_Triggers = config.document["ActiveTrigger"].GetBool();
		config.trigger_Tick = config.document["TriggerSleep"].GetInt();

		const Value& profiles = config.document["profiles"];
		config.count_Profiles = profiles.Size();
		config.profiles = new Profile[config.count_Profiles];
		for (int i = 0; i < config.count_Profiles; i++) {
			config.profiles[i].name = profiles[i]["Name"].GetString();
			config.profiles[i].powerPlan = profiles[i]["PowerPlan"].GetString();
			config.profiles[i].pressetPath = profiles[i]["PresetPath"].GetString();
		}

		const Value& triggers = config.document["triggers"];
		config.count_Triggers = triggers.Size();
		config.triggers = new Trigger[config.count_Triggers];
		for (int i = 0; i < config.count_Triggers; i++) {
			config.triggers[i].profile = triggers[i]["Profile"].GetInt();
			config.triggers[i].priority = triggers[i]["Priority"].GetInt();
			config.triggers[i].fullScreen = triggers[i]["FullScreen"].GetBool();
			config.triggers[i].fileName = triggers[i]["FileName"].GetString();
			config.triggers[i].title = triggers[i]["Title"].GetString();
		}
	}
	else {
		if (!std::filesystem::exists("config-back.json") || recurce) {
			MessageBox(NULL, L"Fail read config", L"Error", MB_ICONERROR);
			exit(EXIT_FAILURE);
		}
		std::filesystem::copy("config-back.json", "config.json", std::filesystem::copy_options::overwrite_existing);
		read(true);
	}
}

void Config::write() {
	config.document["ActiveProfile"].SetInt(config.active_Profile);
	config.document["ActiveTrigger"].SetBool(config.enable_Triggers);

	StringBuffer buffer{};
	Writer<StringBuffer> writer(buffer);
	config.document.Accept(writer);

	std::string text(buffer.GetString());

	std::ofstream myfile("config.json");
	myfile << text;
	myfile.close();
}