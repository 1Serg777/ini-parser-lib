#include "../../include/IniParser/IniScanner.h"

#include <sstream>
#include <unordered_map>

namespace inip
{
	static std::unordered_map<TokenType, std::string_view> tokenTypeToStr
	{
		{ TokenType::LEFT_SQUARE_BRACKET, "LEFT_SQUARE_BRACKET" },
		{ TokenType::RIGHT_SQUARE_BRACKET, "RIGHT_SQUARE_BRACKET" },

		{ TokenType::EQUAL, "EQUAL" },

		{ TokenType::IDENTIFIER, "IDENTIFIER" },
		{ TokenType::STRING, "STRING" },
		{ TokenType::INTEGER, "INTEGER" },
		{ TokenType::FLOAT, "FLOAT" },

		{ TokenType::END_OF_FILE, "END_OF_FILE" },
	};

	std::string_view TokenTypeToString(TokenType tokenType)
	{
		auto find = tokenTypeToStr.find(tokenType);
		if (find == tokenTypeToStr.end())
			return std::string_view{ "UNIDENTIFIED" };
		return find->second;
	}

	// IniScannerError

	IniScannerError::IniScannerError(std::string_view errMsg, int errLine)
		: std::runtime_error(""),
		errLine(errLine)
	{
		CreateErrorMessage(errMsg);
	}

	const char* IniScannerError::what() const
	{
		return errMsg.c_str();
	}

	void IniScannerError::CreateErrorMessage(std::string_view errMsg)
	{
		std::stringstream sstream;
		sstream << "IniScannerError error has occurred!\n";
		sstream << "Error line: " << errLine << "\n";
		sstream << "Error message: " << errMsg;

		this->errMsg = sstream.str();
	}

	// IniScanner

	void IniScanner::Scan(const std::string& iniSource)
	{
		this->iniSource = iniSource;
		while (!AtEnd())
		{
			BeginToken();
			ScanToken();
		}

		Token eof_token{};
		eof_token.type = TokenType::END_OF_FILE;

		tokens.push_back(eof_token);
	}
	void IniScanner::Clear()
	{
		iniSource.clear();
		current = 0;
		line = 0;
		tokens.clear();
	}

	std::vector<Token>* IniScanner::GetTokensPtr()
	{
		return &tokens;
	}

	void IniScanner::ScanToken()
	{
		char c = Advance();
		switch (c)
		{
		case '=':
		{
			AddToken(TokenType::EQUAL);
		}
		break;

		case '[':
		{
			AddToken(TokenType::LEFT_SQUARE_BRACKET);
		}
		break;
		case ']':
		{
			AddToken(TokenType::RIGHT_SQUARE_BRACKET);
		}
		break;

		// Comments
		case '/':
		{
			// Single line comment
			if (Match('/'))
			{
				while (!AtEnd() && Peek() != '\n')
					Advance();
			}
			// Multi line comment
			else if (Match('*'))
			{
				while (!AtEnd())
				{
					if (Peek() == '*' && PeekNext() == '/')
					{
						Advance(); Advance();
						break;
					}

					if (Match('\n'))
					{
						line++;
					}
				}

				if (AtEnd())
				{
					throw IniScannerError{ "Unterminated multi line comment!", line };
				}
			}
			else
			{
				throw IniScannerError{ "Unexpected symbol while trying to parse comments!", line };
			}
		}
		break;

		case ' ':
		case '\r':
		case '\t':
			// Skip them all
			break;

		case '\n':
		{
			line++;
		}
		break;

		case '\0':
		{
			AddToken(TokenType::END_OF_FILE);
		}
		break;

		case '"':
		{
			String();
		}
		break;

		default:
		{
			if (IsDigit(c))
			{
				Number();
			}
			else if (IsAlpha(c))
			{
				Identifier();
			}
			else
			{
				throw IniScannerError{ "Unexpected symbol!", line };
			}
		}
		break;
		}
	}

	void IniScanner::BeginToken()
	{
		start = current;
	}
	void IniScanner::AddToken(TokenType type)
	{
		int charsCount = current - start;
		std::string literal = iniSource.substr(start, charsCount);

		Token token{};
		token.type = type;
		token.literal = literal;
		token.line = line;

		if (type == TokenType::STRING)
		{
			token.value = iniSource.substr(start + 1, charsCount - 2);
		}

		tokens.push_back(token);
	}

	char IniScanner::Advance()
	{
		return iniSource.at(current++);
	}
	bool IniScanner::Match(char expected)
	{
		if (AtEnd())
			return false;
		if (Peek() == expected)
		{
			Advance();
			return true;
		}
		else
		{
			return false;
		}
	}
	char IniScanner::Peek()
	{
		if (AtEnd())
			return '\0';
		return iniSource.at(current);
	}
	char IniScanner::PeekNext()
	{
		if (current + 1 >= iniSource.size())
			return '\0';
		return iniSource.at(current + 1);
	}

	void IniScanner::String()
	{
		while (!AtEnd() && Peek() != '"')
		{
			if (Peek() == '\n')
				line++;
			Advance();
		}

		if (AtEnd())
		{
			throw IniScannerError{ "Forgot to close the string with a \"!", line };
		}

		Advance();
		AddToken(TokenType::STRING);
	}
	void IniScanner::Number()
	{
		// It's either integer or floating point number
		// For both types we're going to use the biggest types in their class:
		// 'long long' for integer and 'double' for a floating point number

		while (IsDigit(Peek()))
		{
			Advance();
		}

		if (Peek() == '.' && IsDigit(PeekNext()))
		{
			Advance();
			while (IsDigit(Peek()))
			{
				Advance();
			}
			AddToken(TokenType::FLOAT);
		}
		else
		{
			AddToken(TokenType::INTEGER);
		}
	}
	void IniScanner::Identifier()
	{
		// !AtEnd() is redundant
		while (IsAlphaNumeric(Peek()))
		{
			Advance();
		}

		AddToken(TokenType::IDENTIFIER);
	}

	bool IniScanner::IsAlpha(char c) const
	{
		return std::isalpha(c) || (c == '_');
	}
	bool IniScanner::IsDigit(char c) const
	{
		return std::isdigit(c);
	}
	bool IniScanner::IsAlphaNumeric(char c) const
	{
		return IsAlpha(c) || IsDigit(c);
	}
	bool IniScanner::IsSpace(char c) const
	{
		return std::isspace(c);
	}

	bool IniScanner::AtEnd() const
	{
		if (current >= iniSource.size())
			return true;
		return false;
	}
}