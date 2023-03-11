#include "../../include/IniParser/IniError.h"

#include <sstream>

namespace inip
{
	// IniSettingValueCastError

	IniSettingValueCastError::IniSettingValueCastError(
		const std::string& key,
		const std::string& value,
		const std::string& castType)
		: std::runtime_error("")
	{
		SetupErrorMessage(key, value, castType);
	}

	char const* IniSettingValueCastError::what() const
	{
		return message.c_str();
	}

	void IniSettingValueCastError::SetupErrorMessage(
		const std::string& key,
		const std::string& value,
		const std::string& castType)
	{
		std::stringstream stream{};
		stream << "Unable to cast the value of a key to [" << castType << "]\n"
			<< "key: [" << key << "] "
			<< "value: [" << value << "] ";
		this->message = stream.str();
	}

	// IniSettingKeyNotFoundError

	IniSettingOptionNotFoundError::IniSettingOptionNotFoundError(const std::string& key)
		: std::runtime_error("")
	{
		SetupErrorMessage(key);
	}

	_NODISCARD char const* IniSettingOptionNotFoundError::what() const
	{
		return message.c_str();
	}

	void IniSettingOptionNotFoundError::SetupErrorMessage(const std::string& key)
	{
		std::stringstream stream{};
		stream << "Unable to find a key. " << "Key: [" << key << "]";
		this->message = stream.str();
	}
}