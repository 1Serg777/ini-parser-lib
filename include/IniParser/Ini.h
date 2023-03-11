#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "IniError.h"
#include "IniParserApi.h"

namespace inip
{
	enum class IniOptionType
	{
		STRING,
		INTEGER,
		FLOAT
	};

	// Helper functions

	std::string IniOptionTypeToString(IniOptionType optionType);

	template <typename T>
	std::string Stringify(const T& value)
	{
		std::stringstream sstream{};
		sstream << value;
		return sstream.str();
	}
	template <> inline
	std::string Stringify<std::string>(const std::string& value)
	{
		return value;
	}

	//template <>
	//std::string Stringify<std::string>(const std::string& value);

	// Ini Option

	class IniOption
	{
	public:

		INI_PARSER_API IniOption(
			const std::string& key,
			const std::string& value,
			IniOptionType optionType);

		INI_PARSER_API const std::string& GetKey() const;

		template <typename T>
		T GetValue() const
		{
			T val{};
			try
			{
				switch (optionType)
				{
				case IniOptionType::INTEGER:
					val = static_cast<T>(std::stoi(value));
					break;
				case IniOptionType::FLOAT:
					val = static_cast<T>(std::stof(value));
					break;
				}
			}
			catch (std::invalid_argument iae)
			{
				throw IniSettingValueCastError(
					key, value, IniOptionTypeToString(optionType));
			}
			catch (std::out_of_range oore)
			{
				throw IniSettingValueCastError(
					key, value, IniOptionTypeToString(optionType));
			}
			return val;
		}
		template <>
		std::string GetValue<std::string>() const
		{
			return value;
		}

		INI_PARSER_API IniOptionType GetOptionType() const;

	private:

		std::string key;
		std::string value;

		IniOptionType optionType{};
	};

	// Ini Group

	class IniGroup
	{
	public:

		INI_PARSER_API IniGroup(const std::string& iniGroupName);

		template <typename T>
		void AddOption(const std::string& key, const T& value)
		{
			std::shared_ptr<IniOption> option;
			if (std::is_integral_v<T>)
				option = std::make_shared<IniOption>(key, Stringify(value), IniOptionType::INTEGER);
			else if (std::is_floating_point_v<T>)
				option = std::make_shared<IniOption>(key, Stringify(value), IniOptionType::FLOAT);
			else
				option = std::make_shared<IniOption>(key, Stringify(value), IniOptionType::STRING);
			AddOption(option);
		}
		INI_PARSER_API void AddOption(std::shared_ptr<IniOption> option);

		INI_PARSER_API bool OptionExists(const std::string& key) const;

		INI_PARSER_API std::shared_ptr<IniOption> GetOption(const std::string& key) const;

		template <typename T>
		T GetOptionValue(const std::string& key) const
		{
			std::shared_ptr<IniOption> option = GetOption(key);
			if (!option)
				throw IniSettingOptionNotFoundError{ key };
			return option->GetValue<T>();
		}

		INI_PARSER_API std::vector<std::shared_ptr<IniOption>> GetGroupOptions() const;

		INI_PARSER_API const std::string& GetGroupName() const;

	private:

		std::unordered_map<std::string, std::shared_ptr<IniOption>> options;
		std::string iniGroupName;
	};

	// Ini Settings

	class IniSettings
	{
	public:

		INI_PARSER_API IniSettings(const std::string& iniSettingsName);

		INI_PARSER_API void AddGroup(std::shared_ptr<IniGroup> iniGroup);
		INI_PARSER_API std::shared_ptr<IniGroup> GetGroup(const std::string& groupName) const;

		INI_PARSER_API std::vector<std::shared_ptr<IniGroup>> GetSettingsGroups() const;

		INI_PARSER_API const std::string& GetIniSettingsName() const;

	private:

		std::unordered_map<std::string, std::shared_ptr<IniGroup>> groups;
		std::string iniSettingsName;
	};

	// Ini Settings printer?

	class IniSettingsPrinter
	{
	public:
		INI_PARSER_API static void PrintIniSettings(std::ostream& outputStream, std::shared_ptr<IniSettings> iniSettings);
		INI_PARSER_API static void PrintIniGroup(std::ostream& outputStream, std::shared_ptr<IniGroup> iniGroup);
		INI_PARSER_API static void PrintIniOption(std::ostream& outputStream, std::shared_ptr<IniOption> iniOption);
	};
}