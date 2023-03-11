#pragma once

#if defined(INI_PARSER_API_EXPORT)
#define INI_PARSER_API __declspec(dllexport)
#elif defined(INI_PARSER_API_IMPORT)
#define INI_PARSER_API __declspec(dllimport)
#endif