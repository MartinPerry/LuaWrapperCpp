#include "./Logger.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#ifdef _WIN32
	//#include <Windows.h>

	//#include <DbgHelp.h>
	//#pragma comment( lib, "dbghelp.lib" )
#endif

using namespace MyUtils;

Logger * Logger::instanceLogger = NULL;




Logger::Logger()
{
	memset(this->enableErrors, true, 3 * sizeof(bool));
	memset(this->enableWarnings, true, 3 * sizeof(bool));
	memset(this->enableInfo, true, 3 * sizeof(bool));

#if defined(_DEBUG) || defined(DEBUG)		
	memset(this->enableInfo, true, 3 * sizeof(bool));	
#else	
	memset(this->enableInfo, false, 3 * sizeof(bool));	
#endif

	memset(this->loggerOutput, NULL, 3 * sizeof(FILE *));
	
	this->LogToStdout();
}

Logger::~Logger()
{
	if (this->loggerOutput[LOG_FILE] != NULL)
	{
		fclose(this->loggerOutput[1]);
	}
}

void Logger::Initialize()
{
	instanceLogger = new Logger();
}

void Logger::Destroy()
{
	SAFE_DELETE(instanceLogger);
}

Logger * Logger::GetInstance()
{	
	if (instanceLogger == NULL)
	{
		Logger::Initialize();
	}

	return instanceLogger;
}


void Logger::LogToFile(const MyStringAnsi & fileName)
{
	
	this->loggerOutput[LOG_FILE] = fopen(fileName.GetConstString(), "w");
}

void Logger::LogToStdout()
{
	this->loggerOutput[LOG_STDOUT] = stdout;
}

void Logger::LogToStder()
{
	this->loggerOutput[LOG_STDERR] = stderr;
}


void Logger::LogError(const char * message, ...)
{
	Logger *l = Logger::GetInstance();

	
	va_list vl;
	va_start(vl, message);
	
	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == NULL)
		{
			continue;
		}
		if (l->enableErrors[i] == false)
		{
			continue;
		}

		fprintf(l->loggerOutput[i], "[Error] ");
		vfprintf(l->loggerOutput[i], message, vl);
		fprintf(l->loggerOutput[i], "\n");

		fflush(l->loggerOutput[i]);
	}
	va_end(vl);

	
	
}


void Logger::LogWarning(const char * message, ...)
{
	Logger *l = Logger::GetInstance();

	
	va_list vl;
	va_start(vl, message);
	 
	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == NULL)
		{
			continue;
		}
		if (l->enableWarnings[i] == false)
		{
			continue;
		}

		fprintf(l->loggerOutput[i], "[Warning] ");
		vfprintf(l->loggerOutput[i], message, vl);
		fprintf(l->loggerOutput[i], "\n");
		fflush(l->loggerOutput[i]);
	}

	va_end(vl);

	
}

void Logger::LogInfo(const char * message, ...)
{
	Logger *l = Logger::GetInstance();

	

	va_list vl;
	va_start(vl, message);
	
	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == NULL)
		{
			continue;
		}
		if (l->enableInfo[i] == false)
		{
			continue;
		}


		fprintf(l->loggerOutput[i], "[Info] ");
		vfprintf(l->loggerOutput[i], message, vl);
		fprintf(l->loggerOutput[i], "\n");
		fflush(l->loggerOutput[i]);
	}

	va_end(vl);

	
}

void Logger::LogMessage(const char * message, ...)
{	
	Logger *l = Logger::GetInstance();

	va_list vl;
	va_start(vl, message);

	for (int i = 0; i < 3; i++)
	{
		if (l->loggerOutput[i] == NULL)
		{
			continue;
		}
		vfprintf(l->loggerOutput[i], message, vl);
		fflush(l->loggerOutput[i]);
	}

	va_end(vl);

	
}



void Logger::DisableErrorLogging(LOG_OUTPUT type)
{
	this->enableErrors[type] = false;
}

void Logger::DisableWarningLogging(LOG_OUTPUT type)
{
	this->enableWarnings[type] = false;
}

void Logger::DisableInfoLogging(LOG_OUTPUT type)
{
	this->enableInfo[type] = false;
}


void Logger::EnableErrorLogging(LOG_OUTPUT type)
{
	this->enableErrors[type] = true;
}

void Logger::EnableWarningLogging(LOG_OUTPUT type)
{
	this->enableWarnings[type] = true;
}

void Logger::EnableInfoLogging(LOG_OUTPUT type)
{
	this->enableInfo[type] = true;
}