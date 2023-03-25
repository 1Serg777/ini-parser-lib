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
	class IniOption;
	class IniGroup;
	class IniSettings;

	// Helper functions

	template <
		typename T,
		std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = true>
	std::string Stringify(const T& value)
	{
		std::stringstream sstream;
		sstream << value;
		return sstream.str();
	}

	template <
		typename T,
		std::enable_if_t<
			std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>,
			bool> = true>
	std::string Stringify(const std::string& value)
	{
		return value;
	}

	// Ini Option

	class IniOption
	{
	public:

		INI_PARSER_API IniOption(
			const std::string& key,
			const std::string& value)
			: key(key), value(value) {}

		INI_PARSER_API const std::string& GetKey() const
		{
			return key;
		}

		template <
			typename T,
			std::enable_if_t<std::is_integral_v<T>, bool> = true>
		T GetValue() const
		{
			T val{};
			try
			{
				val = static_cast<T>(std::stoll(value));
			}
			catch (std::invalid_argument iae)
			{
				throw IniSettingValueCastError(key, value, "INTEGER");
			}
			catch (std::out_of_range oore)
			{
				throw IniSettingValueCastError(key, value, "INTEGER");
			}
			return val;
		}
		
		template <
			typename T,
			std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		T GetValue() const
		{
			T val{};
			try
			{
				val = static_cast<T>(std::stold(value));
			}
			catch (std::invalid_argument iae)
			{
				throw IniSettingValueCastError(key, value, "FLOAT");
			}
			catch (std::out_of_range oore)
			{
				throw IniSettingValueCastError(key, value, "FLOAT");
			}
			return val;
		}
		
		template <
			typename T,
			std::enable_if_t<
				std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>,
				bool> = true>
		T GetValue() const
		{
			return value;
		}

		template <
			typename T,
			std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, bool> = true>
		void SetValue(const T& numericValue)
		{
			std::stringstream ostream;
			ostream << numericValue;
			this->value = ostream.str();
		}
		
		template <
			typename T,
			std::enable_if_t<
				std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>,
				bool> = true>
		void SetValue(const std::string& strValue)
		{
			this->value = strValue;
		}

	private:

		std::string key;
		std::string value;
	};

	// Ini Group

	class IniGroup
	{
	public:

		INI_PARSER_API IniGroup(const std::string& iniGroupName);

		template <typename T>
		void AddOption(const std::string& key, const T& value)
		{
			std::shared_ptr<IniOption> option = std::make_shared<IniOption>(key, Stringify(value));
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