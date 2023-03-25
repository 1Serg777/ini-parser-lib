#include "../../include/IniParser/Ini.h"

namespace inip
{
	// Helper functions

	std::string IniOptionTypeToString(IniOptionType optionType)
	{
		switch (optionType)
		{
		case IniOptionType::INTEGER:
			return "INTEGER";
			break;
		case IniOptionType::FLOAT:
			return "FLOAT";
			break;
		case IniOptionType::STRING:
			return "STRING";
			break;
		}
		return "UNIDENTIFIED";
	}

	// Ini Group

	IniGroup::IniGroup(const std::string& iniGroupName)
		: iniGroupName(iniGroupName)
	{
	}

	void IniGroup::AddOption(std::shared_ptr<IniOption> iniOption)
	{
		options.insert({ iniOption->GetKey(), iniOption });
	}

	bool IniGroup::OptionExists(const std::string& key) const
	{
		std::shared_ptr<IniOption> option = GetOption(key);
		return option ? true : false;
	}

	std::shared_ptr<IniOption> IniGroup::GetOption(const std::string& key) const
	{
		auto find = options.find(key);
		if (find == options.end())
			return std::shared_ptr<IniOption>{};
		return find->second;
	}

	std::vector<std::shared_ptr<IniOption>> IniGroup::GetGroupOptions() const
	{
		std::vector<std::shared_ptr<IniOption>> groupOptions;
		for (const auto& [optionName, option] : options)
		{
			groupOptions.push_back(option);
		}
		return groupOptions;
	}

	const std::string& IniGroup::GetGroupName() const
	{
		return iniGroupName;
	}

	// Ini Settings

	IniSettings::IniSettings(const std::string& iniSettingsName)
		: iniSettingsName(iniSettingsName)
	{
	}

	void IniSettings::AddGroup(std::shared_ptr<IniGroup> iniGroup)
	{
		groups.insert({ iniGroup->GetGroupName(), iniGroup });
	}
	std::shared_ptr<IniGroup> IniSettings::GetGroup(const std::string& groupName) const
	{
		auto find = groups.find(groupName);
		if (find == groups.end())
			return std::shared_ptr<IniGroup>{};
		return find->second;
	}

	std::vector<std::shared_ptr<IniGroup>> IniSettings::GetSettingsGroups() const
	{
		std::vector<std::shared_ptr<IniGroup>> settingsGroups;
		for (const auto& [groupName, group] : groups)
		{
			settingsGroups.push_back(group);
		}
		return settingsGroups;
	}

	const std::string& IniSettings::GetIniSettingsName() const
	{
		return iniSettingsName;
	}

	// Ini Settings Printer

	void IniSettingsWriter::PrintIniSettings(std::ostream& outputStream, std::shared_ptr<IniSettings> iniSettings)
	{
		for (const auto& group : iniSettings->GetSettingsGroups())
		{
			PrintIniGroup(outputStream, group);
			outputStream << "\n";
		}
	}
	void IniSettingsWriter::PrintIniGroup(std::ostream& outputStream, std::shared_ptr<IniGroup> iniGroup)
	{
		outputStream << "[" << iniGroup->GetGroupName() << "]\n";
		for (const auto& option : iniGroup->GetGroupOptions())
		{
			PrintIniOption(outputStream, option);
		}
	}
	void IniSettingsWriter::PrintIniOption(std::ostream& outputStream, std::shared_ptr<IniOption> iniOption)
	{
		outputStream << iniOption->GetKey() << " = ";

		if (iniOption->GetOptionType() == IniOptionType::STRING)
		{
			outputStream << "\"" << iniOption->GetValue<std::string>() << "\"" << "\n";
		}
		else
		{
			outputStream << iniOption->GetValue<std::string>() << "\n";
		}
	}
}