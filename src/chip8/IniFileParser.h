#ifndef INIFILEPARSER_H
#define INIFILEPARSER_H

#include <string>
#include <stdexcept>

namespace chip8
{
	namespace
	{ const char *WS = " \n\r\t\f"; }

	template<typename Handler>
	class IniFileParser
	{
	private:
		std::string _currentSection;

		size_t Section(const std::string &text, size_t pos)
		{
			auto end = text.find(']', pos);
			if (end == text.npos)
				throw std::runtime_error("unexpected end while reading section name");
			
			_currentSection = text.substr(pos + 1, end - pos - 1);
			return NextLine(text, end + 1);
		}

		size_t Value(const std::string &text, size_t pos)
		{
			auto eq = text.find('=', pos);
			if (eq == text.npos)
				throw std::runtime_error("unexpected end while reading value name");

			auto nameEnd = text.find_last_not_of(WS, eq - 1, eq - pos);
			std::string name = text.substr(pos, nameEnd - pos + 1);

			pos = SkipWS(text, eq + 1);

			auto lineEnd = NextLine(text, pos);
			auto valueEnd = text.find_last_not_of(WS, lineEnd - 1, lineEnd - pos);
			std::string value = text.substr(pos, valueEnd - pos + 1);
			//printf("'%s' '%s'\n", name.c_str(), value.c_str());
			static_cast<Handler *>(this)->OnValue(_currentSection, name, value);
			return lineEnd;
		}

		using size_type = std::string::size_type;

		size_t SkipWS(const std::string &text, size_t pos)
		{
			auto next = text.find_first_not_of(WS, pos);
			return next != text.npos? next: text.size();
		}

		size_t NextLine(const std::string &text, size_t pos)
		{
			auto next = text.find('\n', pos);
			return next != text.npos? next + 1: text.size();
		}

	public:
		void Parse(const std::string &text)
		{
			for(size_type pos = 0; pos < text.size(); )
			{
				pos = SkipWS(text, pos);
				switch(text[pos])
				{
				case ';':
				case '#':
					pos = NextLine(text, pos);
					break;
				case '[':
					pos = Section(text, pos);
					break;
				default:
					pos = Value(text, pos);
				}
			}
		}
	};
}

#endif
