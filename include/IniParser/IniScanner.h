#pragma once

#include "IniParserApi.h"

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace inip
{
	enum class TokenType
	{
		LEFT_SQUARE_BRACKET, RIGHT_SQUARE_BRACKET,

		EQUAL,

		IDENTIFIER, STRING, INTEGER, FLOAT,

		END_OF_FILE,
	};

	std::string_view TokenTypeToString(TokenType tokenType);

	struct Token
	{
		TokenType type;

		int line{ 0 };

		std::string literal;
		std::string value;
	};

	class IniScannerError : public std::runtime_error
	{
	public:

		INI_PARSER_API IniScannerError(std::string_view errMsg, int errLine);

		INI_PARSER_API _NODISCARD const char* what() const override;

	private:

		void CreateErrorMessage(std::string_view errMsg);

		std::string errMsg;
		int errLine{ 0 };
	};

	class IniScanner
	{
	public:

		void Scan(const std::string& iniSource);
		void Clear();

		std::vector<Token>* GetTokensPtr();

	private:

		void ScanToken();

		void BeginToken();
		void AddToken(TokenType type);

		char Advance();
		bool Match(char expected);
		char Peek();
		char PeekNext();

		void String();
		void Number();
		void Identifier();

		bool IsAlpha(char c) const;
		bool IsDigit(char c) const;
		bool IsAlphaNumeric(char c) const;
		bool IsSpace(char c) const;

		bool AtEnd() const;

		std::string iniSource;

		int current{ 0 };
		int start{ 0 };
		int line{ 0 };

		std::vector<Token> tokens;
	};
}