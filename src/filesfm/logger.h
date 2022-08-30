#pragma once
#define UNICODE 1

#include <string>
#include <fstream>
#include <iostream>

#define ENABLE_LOGGER

#ifdef ENABLE_LOGGER
 
#define LOGGER_START(MIN_PRIORITY, FILE) Logger::Start(MIN_PRIORITY, FILE);
#define LOGGER_STOP() Logger::Stop();
#define LOGGER_WRITE(PRIORITY, MESSAGE) Logger::Write(PRIORITY, MESSAGE);
#define LOG Logger::GetStream()


#else
 
#define LOGGER_START(MIN_PRIORITY, FILE)
#define LOGGER_STOP()
#define LOGGER_WRITE(PRIORITY, MESSAGE)
#define LOG Logger::GetStream()

#endif

using namespace std;

namespace FilesFM {
	class Logger
	{
	public:
		Logger();
		~Logger();
		
		// Holds a reference to the only instance of this object (hopefully, not strong singleton pattern has been implemented)
		static Logger* Instance;

		// Returns the stream that should be used for logging
		static wostream& Out();

	private:
		wstring const outFilename = L"filesfm_sync.log";

		wofstream _outStream;
		wstring _outPath;
	};
}