#pragma once

#include "IniParserApi.h"

#include <stdexcept>
#include <string>

// [GROUP]
// "key = value"

namespace inip
{
	class IniSettingValueCastError : std::runtime_error
	{
	public:

		INI_PARSER_API IniSettingValueCastError(
			const std::string& key,
			const std::string& value,
			const std::string& castType);

		INI_PARSER_API _NODISCARD char const* what() const override;

	private:

		void SetupErrorMessage(
			const std::string& key,
			const std::string& value,
			const std::string& castType);

		std::string message;
	};

	class IniSettingOptionNotFoundError : std::runtime_error
	{
	public:

		INI_PARSER_API IniSettingOptionNotFoundError(const std::string& key);

		INI_PARSER_API _NODISCARD char const* what() const override;

	private:

		void SetupErrorMessage(const std::string& key);

		std::string message;
	};
}