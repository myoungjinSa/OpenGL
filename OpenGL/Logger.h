#pragma once
#include <iostream>
#include <functional>
#include <consoleapi2.h>
#include "String/String.h"
#include "Thread.h"

//template<typename Func>
//class Logger {
//public:
//	Logger(const Func& _func, const WString& _content) 
//		:func{_func}, content{_content}
//	{
//
//	}
//
//	void operator()() const {
//		std::cout << content << std::endl;
//		func();
//	}
//private:
//	Func func;
//	WString content;
//};
//
namespace GeneralLogger {
	enum {
		MAX_LOG_PREFIX_SIZE = 256,
		MAX_LOG_BUFFER_SIZE = 20480,
	};

	enum class LogType : int8_t
	{
		Debug, 
		Info,
		Warning,
		Error,
		Count
	};

	enum class LogCategory : int8_t 
	{
		Normal,
		Aubsing,
		Contents,
		Count
	};


	enum class ConsoleColor
	{
		BLACK = 0,
		DARKBLUE = FOREGROUND_BLUE,
		DARKGREEN = FOREGROUND_GREEN,
		DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DARKRED = FOREGROUND_RED,
		DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
		DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		DARKGRAY = FOREGROUND_INTENSITY,
		GRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
		MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	};
	constexpr std::wostream& operator<<(std::wostream& lhs, LogType level) {
		switch (level) {
		case LogType::Info:
			lhs << "Info";
			break;
		case LogType::Warning:
			lhs << "Warning";
			break;
		case LogType::Error:
			lhs << "Error";
			break;
		default:
			lhs << "None";
			break;
		}
		return lhs;
	}

	class LogData : public std::enable_shared_from_this<LogData>
	{
	public:
		LogData()
			: LogData(LogType::Info, LogCategory::Normal, ConsoleColor::WHITE) {

		}
		LogData(LogType level, ConsoleColor color)
			: LogData(level, LogCategory::Normal, color)
		{}
		LogData(LogType level, LogCategory category, ConsoleColor color)
			: Level(level)
			, Category(category)
			, Color(color)
		{}
		~LogData()
		{
			this->PrefixBuffer.Clear();
			this->Buffer.Clear();
		}
	public:
		LogType Level;
		LogCategory Category;
		ConsoleColor Color;
		WString PrefixBuffer;
		WString Buffer;
	};

	class ILogger : public std::enable_shared_from_this<ILogger>
	{
	public:
		virtual ~ILogger(){}
	public:
		virtual void WriteLog(const std::shared_ptr<LogData>& logData) {}
	};
	
	class Logger final {
	public:
		enum {
			LogWaitTime = 5000,
		};

		static void Initialize();
		static void Finalize();
		static void AddLogWriter(std::shared_ptr<ILogger> logger);

		static void Debug(const wchar_t* format, ...);
		static void Info(ConsoleColor color, const wchar_t* format, ...);
		static void Info(const wchar_t* format, ...);
		static void Info(LogCategory category, const wchar_t* format, ...);

		static void Warning(const wchar_t* format, ...);
		static void Warning(LogCategory category, const wchar_t* format, ...);

		static void Error(const wchar_t* format, ...);
		static void Error(LogCategory category, const wchar_t* format, ...);

		static void Info(ConsoleColor color, const char& format, ...);
		static void Info(const char& format, ...);
		static void Info(LogCategory category, const char& format, ...);
		static void Warning(const char& format, ...);
		static void Warning(LogCategory category, const char& format, ...);
		static void Error(const char& format, ...);
		static void Error(LogCategory category, const char& format, ...);

		static Logger& GetInstance() {
			return instance;
		}

	protected:
		explicit Logger() 
			: active(false)
			, logCount(0)
		{
			Initialize();
		}
		~Logger() {
			Finalize();
		}

		void Start();
		void End();

		void Run();

		void Write(const std::shared_ptr<LogData> logData);
		
		void RequestWork(const std::shared_ptr<LogData>& logData);

		static void Write(const std::shared_ptr<LogData>& logData, const wchar_t* format, va_list args);

	private:
		static Logger instance;
		std::vector<std::shared_ptr<ILogger>> logWriter;
		Concurrent::SafeQueue<std::shared_ptr<LogData>> logList;
		std::unique_ptr<std::thread> logThread = nullptr;
		bool active;
		volatile long logCount;
	};

}


