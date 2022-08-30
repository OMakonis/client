#include "Logger.h"

#include <Windows.h>
// #include <filesystem>
#include <sstream>
#include <string>
#include <stdio.h>
#include "helpers.h"
#include "debug.h"
#include <fileapi.h>

using namespace std;

namespace FilesFM {
	Logger* Logger::Instance = NULL;

	Logger::Logger()
	{
		Instance = this;
		
		wstring tmpDir;
        wchar_t wcPath[MAX_PATH];
		if (GetTempPathW(MAX_PATH, wcPath)) {
            tmpDir = wcPath;
				
		}


		// Open out stream
		_outPath = tmpDir + L"\\" + outFilename;
		_outStream.open(_outPath.c_str());
	}

	Logger::~Logger()
	{
		D(_outStream << L"Logger destructor" << endl);

		if ( _outStream.is_open() )
		{
			_outStream.close();
		}

		if ( Helpers::FileExists(_outPath) )
		{
			DeleteFile(_outPath.c_str());
		}
	}

	wostream& Logger::Out()
	{
		return Instance->_outStream;
	}

	// ostream& Logger::GetStream()
	// {
	// 	return Instance.FileStream.is_open() ? Instance.FileStream : std::cout;
	// }

	// void Logger::Write(Priority priority, const string& message)
	// {
	// 	if (Instance.active && priority >= Instance.minPriority)
	// 	{
	// 		// identify current output stream
	// 		ostream& stream
	// 			= Instance.FileStream.is_open() ? Instance.FileStream : std::cout;
	
	// 		stream  << PRIORITY_NAMES[priority]
	// 				<< ": "
	// 				<< message
	// 				<< endl;
	// 	}
	// }
}