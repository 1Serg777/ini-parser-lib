#pragma once

#include "Ini.h"
#include "IniParserApi.h"
#include "IniScanner.h"

#include <filesystem>
#include <memory>

namespace inip
{
	// Parser related errors

	class IniParserError : public std::runtime_error
	{
	public:

		INI_PARSER_API IniParserError(const Token& errorToken, std::string_view errMsg);

		INI_PARSER_API _NODISCARD const char* what() const override;

	private:

		void CreateErrorMessage(std::string_view errMsg);

		Token errorToken;
		std::string errMsg;
	};

	// Ini Parser

	class IniParser
	{
	public:

		INI_PARSER_API IniParser();
		INI_PARSER_API ~IniParser();

		INI_PARSER_API void Parse(const std::filesystem::path& iniFilePath);
		INI_PARSER_API void Parse(const std::string& iniSource, const std::string& iniSettingsName);

		INI_PARSER_API std::shared_ptr<IniSettings> GetIniSettings() const;

	private:

		void InitializeIniParser();

		std::string GetIniFileSrc(const std::filesystem::path& iniFilePath) const;

		std::shared_ptr<IniGroup> Group();
		std::string GroupId();
		std::shared_ptr<IniOption> Option();

		Token Advance();
		Token Peek() const;
		Token Previous() const;
		Token Consume(TokenType type, std::string_view errMsg);

		bool Check(TokenType type);
		bool AtEnd() const;

		void Clear();

		std::unique_ptr<IniScanner> iniScanner;
		std::vector<Token>* tokens{ nullptr };

		std::shared_ptr<IniSettings> iniSettings;

		int current{ 0 };
	};
}