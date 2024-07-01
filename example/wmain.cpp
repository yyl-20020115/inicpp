#include "winicpp.hpp"

#include <iostream>
#include <iomanip>


void printList(std::list<std::wstring>& theList)
{
	bool first = true;
	for (const std::wstring& sectionName : theList)
	{
		if(!first)
				std::wcout<<L"|";
	        std::wcout << sectionName;
			first = false;
	}
	std::wcout << std::endl << std::endl;
}


/**
  compile: g++ -I../ -std=c++11 main.cpp -o iniExample
*/
int main()
{
	/** 1. Load the INI file. */
	winicpp::IniManager _ini(L"config.ini");


	/** 2. Check if the key exists. */
	if (!_ini[L"rtsp"].isKeyExist(L"port"))
	{
		std::wcout << L"rtsp.port: not exist!" << L"\n\n";
	}


	/** 3. Check if the section exists. */
	if (!_ini.isSectionExists(L"math"))
	{
	        std::wcout << L"section of math: not exist" << L"\n\n";
	}


	/** 4. Get all sections as std::list<std::string>. */
	std::list<std::wstring> sectionList = _ini.getSectionsList(); // may contains Unnamed section, but keys at head of file
	std::wcout << L"Got Section Name List(size:" << sectionList.size() << L") :"; 
	printList(sectionList);


	/** 5. library head */
	_ini.modify(L"head", L"title", L"inicpp",L"thanks for your using inicpp project.");
	_ini.modify(L"head", L"license", L"MIT", L"Permissive license for open-source software distribution.");


	/** 6. Add section-key-value pairs or Modify later. */
	_ini.modify(L"rtsp", L"port", L"554");
	_ini.modify(L"rtsp", L"port", L"555");
	_ini.modify(L"rtsp", L"ip", L"127.0.0.1");

	  // - Read key-value pairs directly.
	std::wstring rtsp_port = _ini[L"rtsp"][L"port"];
	std::wcout << L"get rtsp port:" << rtsp_port << L"\n\n";;

	  // - Add a comment.
	_ini.modify(L"rtsp", L"port", L"554", L"this is the listen port for http server.");

	  // - Modify the comment.
	_ini.modifyComment(L"rtsp", L"port", L"this is the listen ip for rtsp server.");

	  // - You have obtained the key-value pair and saved it to your config file.
	std::wcout << L"to string:\trtsp.port = " << _ini[L"rtsp"][L"port"] << std::endl;


	/** 7. Convert type. */
	_ini.modify(L"math", L"PI", L"3.1415926", L"This is pi in mathematics.");

	  // Convert to string, default is string.
	std::wcout << L"to string:\tmath.PI   = " << _ini[L"math"][L"PI"] << std::endl;
	std::wcout << L"to string:\tmath.PI   = " << _ini[L"math"].toString(L"PI") << std::endl;

	  // - Convert to double
	double http_port_d = _ini[L"math"].toDouble(L"PI");
	std::wcout << L"to double:\tmath.PI   = " << std::setprecision(10) << http_port_d << std::endl;

	  // - Convert to int
	int http_port_i = _ini[L"math"].toInt(L"PI");
	std::wcout << L"to int:\t\tmath.PI   = " << http_port_i << std::endl;

	  // - convert to std::wstring.
	//std::locale::global(std::locale("")); // support wide string output

	//_ini.modify(L"other", L"desc",  L"你好，世界", L"this test for std::wstring. comment it.");

	//std::wstring wstr = _ini[L"other"].toString(L"desc");
	//std::wcout << L"to wstring:\tother.desc= " << wstr << L"\n\n";


	/** 8. Unnamed section: at head of ini file,with no section name found. */
	_ini.modify(L"a", L"noSection", L"no", L"no section test.");
	_ini.modify(L"a", L"noSection", L"yes", L"no section test:add comment later.");

	_ini.modify(L"a", L"key0", L"noSectionAndComment");
	_ini.modify(L"a", L"key1", L"noSectionAndComment");
	_ini.modify(L"a", L"key2", L"noSectionAndComment");


	/** Print all sections again */
	sectionList = _ini.getSectionsList();  // may contains Unnamed section, but keys at head of file.
	std::wcout << L"Got Section Name List(size:" << sectionList.size() << L") :"; 
	printList(sectionList);

	return 0;
}
