#include "../../include/IniParser/IniParser.h"

#include <cassert>
#include <fstream>
#include <sstream>

namespace inip
{
	// IniParserError

	IniParserError::IniParserError(const Token& errorToken, std::string_view errMsg)
		: std::runtime_error(""),
		errorToken(errorToken)
	{
		CreateErrorMessage(errMsg);
	}

	const char* IniParserError::what() const
	{
		return errMsg.c_str();
	}

	void IniParserError::CreateErrorMessage(std::string_view errMsg)
	{
		std::stringstream sstream;
		sstream << "IniParserError error has occurred!\n";
		sstream << "TokenType: " << TokenTypeToString(errorToken.type) << "\n";
		sstream << "Literal: " << errorToken.literal << "\n";
		sstream << "Value: " << errorToken.value << "\n";
		sstream << "Error message: " << errMsg;

		this->errMsg = sstream.str();
	}

	// IniParser

	IniParser::IniParser()
	{
		InitializeIniParser();
	}
	IniParser::~IniParser()
	{
		Clear();
	}

	void IniParser::Parse(const std::filesystem::path& iniFilePath)
	{
		std::string iniSrc = GetIniFileSrc(iniFilePath);
		std::filesystem::path iniFileName = iniFilePath;

		Parse(iniSrc, iniFileName.replace_extension().generic_string());
	}
	void IniParser::Parse(const std::string& iniSource, const std::string& iniSettingsName)
	{
		Clear();

		iniSettings = std::make_shared<IniSettings>(iniSettingsName);

		iniScanner->Scan(iniSource);
		tokens = iniScanner->GetTokensPtr();

		while (!AtEnd())
		{
			std::shared_ptr<IniGroup> iniGroup = Group();
			iniSettings->AddGroup(iniGroup);
		}
	}

	std::shared_ptr<IniSettings> IniParser::GetIniSettings() const
	{
		return iniSettings;
	}

	void IniParser::InitializeIniParser()
	{
		iniScanner = std::make_unique<IniScanner>();
	}

	std::string IniParser::GetIniFileSrc(const std::filesystem::path& iniFilePath) const
	{
		assert(!iniFilePath.empty() && "The path to an ini file must not be empty!");
		std::ifstream file{ iniFilePath , std::ios::binary };

		if (file.fail())
		{
			throw std::ifstream::failure{ "I/O runtime error while openning a file!" };
		}

		std::ostringstream ostream{};
		ostream << file.rdbuf();

		std::string iniSrc = ostream.str();
		return iniSrc;
	}

	std::shared_ptr<IniGroup> IniParser::Group()
	{
		std::string groupId = GroupId();

		std::shared_ptr<IniGroup> iniGroup = std::make_shared<IniGroup>(groupId);

		while (Peek().type == TokenType::IDENTIFIER)
		{
			std::shared_ptr<IniOption> iniOption = Option();
			iniGroup->AddOption(iniOption);
		}

		return iniGroup;
	}
	std::string IniParser::GroupId()
	{
		Token leftSquareBracket =
			Consume(
				TokenType::LEFT_SQUARE_BRACKET,
				"Group ID is expected to begin with a '[' symbol!");

		std::string groupId{};
		Token idStr = Advance();
		if (idStr.type == TokenType::IDENTIFIER)
		{
			groupId = idStr.literal;
		}
		else if (idStr.type == TokenType::STRING)
		{
			groupId = idStr.value;
		}
		else
		{
			throw IniParserError(Peek(), "Unexpected Group ID! It must be either [IDENTIFIER] or [STRING]!");
		}

		Token rightSquareBracket =
			Consume(
				TokenType::RIGHT_SQUARE_BRACKET,
				"Group ID is expected to end with a ']' symbol!");

		return groupId;
	}
	std::shared_ptr<IniOption> IniParser::Option()
	{
		std::shared_ptr<IniOption> iniOption{};

		Token optionKey =
			Consume(
				TokenType::IDENTIFIER,
				"An option's key is expected to be an IDENTIFIER!");

		Token equalSign =
			Consume(
				TokenType::EQUAL,
				"Expected to delimit an option's 'key' and 'value' with a '=' sign!");

		Token value = Advance();
		switch (value.type)
		{
		case TokenType::STRING:
			iniOption = std::make_shared<IniOption>(optionKey.literal, value.value);
			break;
		case TokenType::INTEGER:
			iniOption = std::make_shared<IniOption>(optionKey.literal, value.literal);
			break;
		case TokenType::FLOAT:
			iniOption = std::make_shared<IniOption>(optionKey.literal, value.literal);
			break;
		case TokenType::IDENTIFIER:
			iniOption = std::make_shared<IniOption>(optionKey.literal, value.literal);
			break;
		default:
			throw IniParserError(Peek(), "Unexpected 'value' token! Must be either STRING, INTEGER, FLOAT or IDENTIFIER!");
		}

		return iniOption;
	}

	Token IniParser::Advance()
	{
		if (AtEnd())
			return Peek();
		current++;
		return Previous();
	}
	Token IniParser::Peek() const
	{
		return (*tokens)[current];
	}
	Token IniParser::Previous() const
	{
		assert(current != 0 &&
			"Cannot call 'Previous' while at the beginning of the stream!");
		return (*tokens)[current - 1];
	}
	Token IniParser::Consume(TokenType type, std::string_view errMsg)
	{
		if (Check(type))
			return Advance();
		throw IniParserError(Peek(), errMsg);
	}

	bool IniParser::Check(TokenType type)
	{
		if (AtEnd())
			return false;
		return Peek().type == type;
	}
	bool IniParser::AtEnd() const
	{
		if (Peek().type == TokenType::END_OF_FILE)
			return true;
		return false;
	}

	void IniParser::Clear()
	{
		iniScanner->Clear();
		current = 0;
		tokens = nullptr;
		iniSettings.reset();
	}
}