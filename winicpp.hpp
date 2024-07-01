/*
* MIT License
*
* Copyright (c) 2023 dujingning <djn2019x@163.com> <https://github.com/dujingning/inicpp.git>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef __JN_INICPP_H__
#define __JN_INICPP_H__

#include <algorithm>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <cwchar>
// for std::string <==> std::wstring convert
#include <codecvt>
#include <locale>
#include <filesystem>
#include <memory.h>
//#define INICPP_DEBUG
#ifdef INICPP_DEBUG

#include <array>
#include <ctime>
#include <iostream>

class TimeFormatter
{
public:
	static std::wstring format(const std::wstring& format = L"%Y-%m-%d %H:%M:%S")
	{
		std::time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);
		std::array<wchar_t, 100> buffer;
		std::wcsftime(buffer.data(), buffer.size(), format.c_str(), &tm);
		return buffer.data();
	}
};

#define _L(x) __L(x)
#define __L(x) L##x

#define CODE_INFO std::wstring(L" | Code:\'file:") + std::wstring(_L(__FILE__)) + L",function:" + std::wstring(_L(__FUNCTION__)) + L",line:" + std::to_wstring(__LINE__) + L'\''
#define INI_DEBUG(x) std::wcout << L"INICPP " << TimeFormatter::format() << L" : " << x << CODE_INFO << std::endl

#else  // #ifdef INICPP_DEBUG
#define INI_DEBUG(x)
#endif // #ifdef INICPP_DEBUG


namespace winicpp
{

	// 使用CRT库的wcstombs()函数将wstring转string，平台无关，需设定locale,这种方法可以兼容中文
	std::string wstring_to_string(const std::wstring& ws)
	{
		std::string curLocale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "");
 
		const wchar_t *_Source = ws.c_str();
		size_t _Dsize = 2 * ws.size() + 1;
		char *_Dest = new char[_Dsize];
		memset(_Dest, 0, _Dsize);
		wcstombs(_Dest, _Source, _Dsize);
 
		std::string result = _Dest;
		delete[] _Dest;
 
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}

	typedef struct KeyValueNode
	{
		std::wstring Value = L"";
		int lineNumber = -1; // text line start with 1
	} KeyValueNode;

	class section
	{
	public:
		section() : _sectionName()
		{
		}

		explicit section(const std::wstring& sectionName) : _sectionName(sectionName)
		{
		}

		const std::wstring& name()
		{
			return _sectionName;
		}

		const std::wstring getValue(const std::wstring& Key)
		{
			if (!_sectionMap.count(Key))
			{
				return L"";
			}
			return _sectionMap[Key].Value;
		}

		void setName(const std::wstring& name, const int& lineNumber)
		{
			_sectionName = name;
			_lineNumber = lineNumber;
		}

		void setValue(const std::wstring& Key, const std::wstring& Value, const int line)
		{
			_sectionMap[Key].Value = Value;
			_sectionMap[Key].lineNumber = line;
		}

		void append(section& sec)
		{
			_sectionMap.insert(sec._sectionMap.begin(), sec._sectionMap.end());
		}

		bool isKeyExist(const std::wstring& Key)
		{
			return !_sectionMap.count(Key) ? false : true;
		}

		int getEndSection()
		{
			int line = -1;

			if (_sectionMap.empty() && _sectionName != L"")
			{
				return _lineNumber;
			}

			for (const auto& data : _sectionMap)
			{
				if (data.second.lineNumber > line)
				{
					line = data.second.lineNumber;
				}
			}
			return line;
		}

		int getLine(const std::wstring& Key)
		{
			if (!_sectionMap.count(Key))
			{
				return -1;
			}
			return _sectionMap[Key].lineNumber;
		}

		const std::wstring operator[](const std::wstring& Key)
		{
			if (!_sectionMap.count(Key))
			{
				return L"";
			}
			return _sectionMap[Key].Value;
		}

		void clear()
		{
			_lineNumber = -1;
			_sectionName.clear();
			_sectionMap.clear();
		}

		bool isEmpty() const
		{
			return _sectionMap.empty();
		}

		int toInt(const std::wstring& Key)
		{
			if (!_sectionMap.count(Key))
			{
				return 0;
			}

			int result = 0;

			try
			{
				result = std::stoi(_sectionMap[Key].Value);
			}
			catch (const std::invalid_argument& e)
			{
				//INI_DEBUG("Invalid argument: " << e.what() << ",input:\'" << _sectionMap[Key].Value << "\'");
			}
			catch (const std::out_of_range& e)
			{
				//INI_DEBUG("Out of range: " << e.what() << ",input:\'" << _sectionMap[Key].Value << "\'");
			}

			return result;
		}

		std::wstring toString(const std::wstring& Key)
		{
			if (!_sectionMap.count(Key))
			{
				return L"";
			}
			return _sectionMap[Key].Value;
		}

		double toDouble(const std::wstring& Key)
		{
			if (!_sectionMap.count(Key))
			{
				return 0.0;
			}

			double result = 0.0;

			try
			{
				result = std::stod(_sectionMap[Key].Value);
			}
			catch (const std::invalid_argument& e)
			{
				INI_DEBUG(L"Invalid argument: " << e.what() << L",input:\'" << _sectionMap[Key].Value << L"\'");
			}
			catch (const std::out_of_range& e)
			{
				INI_DEBUG(L"Out of range: " << e.what() << L",input:\'" << _sectionMap[Key].Value << L"\'");
			}

			return result;
		}

		// todo : add toString() / toInt() / toDouble()

	private:
		std::wstring _sectionName;
		std::map<std::wstring, KeyValueNode> _sectionMap;
		int _lineNumber = -1; // text line start with 1
	};

	class ini
	{
	public:
		void addSection(section& sec)
		{
			if (_iniInfoMap.count(sec.name())) // if exist,need to merge
			{
				_iniInfoMap[sec.name()].append(sec);
				return;
			}
			_iniInfoMap.emplace(sec.name(), sec);
			return;
		}

		void removeSection(const std::wstring& sectionName)
		{
			if (!_iniInfoMap.count(sectionName))
			{
				return;
			}
			_iniInfoMap.erase(sectionName);
			return;
		}

		bool isSectionExists(const std::wstring& sectionName)
		{
			return !_iniInfoMap.count(sectionName) ? false : true;
		}

		// may contains default of Unnamed section with ""
		std::list<std::wstring> getSectionsList()
		{
			std::list<std::wstring> sectionList;
			for (const auto& data : _iniInfoMap)
			{
				if (data.first == L"" && data.second.isEmpty()) // default section: no section name,if empty,not count it.
				{
					continue;
				}
				sectionList.emplace_back(data.first);
			}
			return sectionList;
		}

		const section& operator[](const std::wstring& sectionName)
		{
			if (!_iniInfoMap.count(sectionName))
			{
				return _iniInfoMap[L""];
			}
			return _iniInfoMap[sectionName];
		}

		const int getSectionSize()
		{
			return _iniInfoMap.size();
		}

		std::wstring getValue(const std::wstring& sectionName, const std::wstring& Key)
		{
			if (!_iniInfoMap.count(sectionName))
			{
				return L"";
			}
			return _iniInfoMap[sectionName][Key];
		}

		// for none section
		int getLine(const std::wstring& Key)
		{
			if (!_iniInfoMap.count(L""))
			{
				return -1;
			}
			return _iniInfoMap[L""].getLine(Key);
		}

		// for section-key
		int getLine(const std::wstring& sectionName, const std::wstring& Key)
		{
			if (!_iniInfoMap.count(sectionName))
			{
				return -1;
			}
			return _iniInfoMap[sectionName].getLine(Key);
		}

		inline void clear() { _iniInfoMap.clear(); }
		inline bool empty() { return _iniInfoMap.empty(); }

	protected:
		std::map<std::wstring/*Section Name*/, section> _iniInfoMap;
	};

	// todo if file is modified,never write back
	class IniManager
	{
	public:
		explicit IniManager(const std::wstring& configFileName) : _configFileName(configFileName)
		{
			parse();
		}

		~IniManager()
		{
			_iniFile.close();
		}

		section operator[](const std::wstring& sectionName)
		{
			return _iniData[sectionName];
		}

		void parse()
		{
			_iniFile.open(wstring_to_string(_configFileName), std::ifstream::in | std::ifstream::out | std::fstream::app);
			if (!_iniFile.is_open())
			{
				INI_DEBUG(L"Failed to open the input INI file for parsing!");
				return;
			}

			_iniData.clear();

			_iniFile.seekg(0, _iniFile.beg);
			std::wstring data, sectionName;
			int sectionLine = -1;

			section sectionRecord;

			_SumOfLines = 1;
			do
			{
				std::getline(_iniFile, data);

				if (!filterData(data))
				{
					++_SumOfLines;
					continue;
				}

				if (data.find('[') == 0) // section
				{
					if (!sectionRecord.isEmpty() || sectionRecord.name() != L"")
					{
						_iniData.addSection(sectionRecord);
					}

					size_t first = data.find(L'[');
					size_t last = data.find(L']');

					if (last == std::wstring::npos)
					{
						++_SumOfLines;
						continue;
					}

					sectionName = data.substr(first + 1, last - first - 1);
					sectionLine = _SumOfLines;

					sectionRecord.clear();
					sectionRecord.setName(sectionName, sectionLine);
				}

				size_t pos = data.find(L'=');
				if (pos != std::wstring::npos)
				{ // k=v
					std::wstring key = data.substr(0, pos);
					std::wstring value = data.substr(pos + 1);

					sectionRecord.setValue(key, value, _SumOfLines);

				}

				++_SumOfLines;

			} while (!_iniFile.eof());

			if (!sectionRecord.isEmpty())
			{
				sectionRecord.setName(sectionName, -1);
				_iniData.addSection(sectionRecord);
			}

			if (_iniFile.is_open())
			{
				_iniFile.close();
			}
		}

		bool modify(const std::wstring& Section, const std::wstring& Key, const std::wstring& Value, const std::wstring& comment = L"")
		{ // todo: insert comment before k=v
			parse();

			if (Key == L"" || Value == L"")
			{
				INI_DEBUG("Invalid parameter input: Key[" << Key << "],Value[" << Value << "]");
				return false;
			}

			std::wstring keyValueData = Key + L"=" + Value + L"\n";
			if (comment.length() > 0)
			{
				keyValueData = comment + L"\n" + keyValueData;
				if (comment[0] != ';')
				{
					keyValueData = L";" + keyValueData;
				}
			}

			const std::wstring& tempFile = L".temp.ini";
			std::wfstream input(wstring_to_string( _configFileName), std::ifstream::in | std::ifstream::out | std::fstream::app);
			std::wofstream output(wstring_to_string(tempFile));

			if (!input.is_open())
			{
				INI_DEBUG("Failed to open the input INI file for modification!");
				return false;
			}

			if (!output.is_open())
			{
				INI_DEBUG("Failed to open the input INI file for modification!");
				return false;
			}

			int line_number_mark = -1;
			bool isInputDataWited = false;

			do
			{
				// exist key at one section replace it, or need to create it
				if (_iniData.isSectionExists(Section))
				{
					line_number_mark = (*this)[Section].getLine(Key);

					if (line_number_mark == -1)
					{ // section exist, key not exist
						line_number_mark = (*this)[Section].getEndSection();

						std::wstring lineData;
						int input_line_number = 0;
						while (std::getline(input, lineData))
						{
							++input_line_number;

							if (input_line_number == (line_number_mark + 1))
							{ // new line,append to next line
								isInputDataWited = true;
								output << keyValueData;
							}

							output << lineData << "\n";

						}

						if (input.eof() && !isInputDataWited)
						{
							isInputDataWited = true;
							output << keyValueData;
						}

						break;
					}
				}

				if (line_number_mark <= 0) // not found key at config file
				{
					input.seekg(0, input.beg);

					bool isHoldSection = false;
					std::wstring newLine = L"\n\n";
					if (Section != L"" && Section.find(L"[") == std::wstring::npos && Section.find(L"]") == std::wstring::npos && Section.find(L"=") == std::wstring::npos)
					{
						if (_iniData.empty() || _iniData.getSectionSize() <= 0)
						{
							newLine.clear();
						}

						isHoldSection = true;
					}

					// 1.section is exist or empty section
					if (_iniData.isSectionExists(Section) || Section == L"")
					{
						// write key/value to head
						if (isHoldSection)
						{
							output << newLine << "[" << Section << "]" << "\n";
						}
						output << keyValueData;
						// write others
						std::wstring lineData;
						while (std::getline(input, lineData))
						{
							output << lineData << "\n";
						}
					}
					// 2.section is not exist
					else
					{
						// write others
						std::wstring lineData;
						while (std::getline(input, lineData))
						{
							output << lineData << "\n";
						}
						// write key/value to end
						if (isHoldSection)
						{
							output << newLine << "[" << Section << "]" << "\n";
						}
						output << keyValueData;
					}

					break;

				}
				else { // found, replace it

					std::wstring lineData;
					int input_line_number = 0;

					while (std::getline(input, lineData))
					{
						++input_line_number;

						// delete old comment if new comment is set
						if (input_line_number == (line_number_mark - 1) && lineData.length() > 0 && lineData[0] == ';' && comment != L"")
						{
							continue;
						}

						if (input_line_number == line_number_mark)
						{ // replace to this line
							output << keyValueData;
						}
						else
						{
							output << lineData << L"\n";
						}
					}
					break;
				}

				INI_DEBUG(L"error! inicpp lost process of modify function");
				return false;

			} while (false);

			// clear work
			input.close();
			output.close();

			std::filesystem::remove(_configFileName);
			std::filesystem::rename(tempFile, _configFileName);
		
			// reload
			parse();

			return true;
		}

		bool modify(const std::wstring& Section, const std::wstring& Key, const int& Value, const std::wstring& comment = L"")
		{
			std::wstring stringValue = std::to_wstring(Value);
			return modify(Section, Key, stringValue, comment);
		}

		bool modify(const std::wstring& Section, const std::wstring& Key, const double& Value, const std::wstring& comment = L"")
		{
			std::wstring stringValue = std::to_wstring(Value);
			return modify(Section, Key, stringValue, comment);
		}

		bool modifyComment(const std::wstring& Section, const std::wstring& Key, const std::wstring& comment)
		{
			return modify(Section, Key, (*this)[Section][Key], comment);
		}

		bool isSectionExists(const std::wstring& sectionName)
		{
			return _iniData.isSectionExists(sectionName);
		}

		inline std::list<std::wstring> getSectionsList()
		{
			return _iniData.getSectionsList();
		}

	private:
		bool filterData(std::wstring& data)
		{
			// filter
			data.erase(std::remove_if(data.begin(), data.end(), [](wchar_t c)
				{ return c == ' ' || c == L'\t'; }),
				data.end());

			if (data.length() == 0)
			{
				return false;
			}

			if (data[0] == L';')
			{
				return false;
			}

			if (data[0] == L'#')
			{
				return false;
			}

			return true;
		}

	private:
		ini _iniData;
		int _SumOfLines;
		std::wfstream _iniFile;
		std::wstring _configFileName;
	};

}

#endif
