#ifndef LOGGER_H
#define LOGGER_H

#include "./Strings/MyStringAnsi.h"
#include "./Macros.h"

namespace MyUtils 
{
	class Logger
	{
		public:
			typedef enum LOG_OUTPUT { LOG_STDOUT = 0, LOG_FILE = 1, LOG_STDERR = 2 } LOG_OUTPUT;

			static void Initialize();
			static void Destroy();
			static Logger * GetInstance();

			static void LogError(const char * message, ...);
			static void LogWarning(const char * message, ...);
			static void LogInfo(const char * message, ...);
			static void LogMessage(const char * message, ...);


			void LogToFile(const MyStringAnsi & fileName);
			void LogToStdout();
			void LogToStder();						

			void EnableErrorLogging(LOG_OUTPUT type);
			void EnableWarningLogging(LOG_OUTPUT type);
			void EnableInfoLogging(LOG_OUTPUT type);

			void DisableErrorLogging(LOG_OUTPUT type);
			void DisableWarningLogging(LOG_OUTPUT type);
			void DisableInfoLogging(LOG_OUTPUT type);

		private:

			static Logger * instanceLogger;

			Logger();
			~Logger();

			bool enableErrors[3];
			bool enableWarnings[3];
			bool enableInfo[3];

			FILE * loggerOutput[3];

			
			
	};
}


#endif