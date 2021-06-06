#include "Logger.h"
#include <iomanip>
#include <stdarg.h>

namespace GeneralLogger {
	_declspec(thread) wchar_t DeclspecPrefixBuffer[MAX_LOG_PREFIX_SIZE];
	_declspec(thread) wchar_t DeclspecLogBuffer[MAX_LOG_BUFFER_SIZE];

	Logger Logger::instance;
}

void GeneralLogger::Logger::Initialize() {
	instance.logList.Clear();
	instance.Start();
}

void GeneralLogger::Logger::Finalize() {
	instance.End();
}

void GeneralLogger::Logger::AddLogWriter(std::shared_ptr<ILogger> logger) {
	instance.logWriter.emplace_back(logger);
}

void GeneralLogger::Logger::Debug(const wchar_t* format, ...) {
#ifdef _DEBUG
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Info, ConsoleColor::YELLOW), format, args);
	va_end(args);
#endif
}

void GeneralLogger::Logger::Info(ConsoleColor color, const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Info, color), format, args);
	va_end(args);
}

void GeneralLogger::Logger::Info(const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Info, ConsoleColor::WHITE), format, args);
	va_end(args);
}
void GeneralLogger::Logger::Info(LogCategory category, const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Info, category, ConsoleColor::WHITE), format, args);
	va_end(args);
}

void GeneralLogger::Logger::Warning(LogCategory category, const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Warning, category, ConsoleColor::YELLOW), format, args);
	va_end(args);
}



void GeneralLogger::Logger::Warning(const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Warning, ConsoleColor::YELLOW), format, args);
	va_end(args);
}

void GeneralLogger::Logger::Error(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Error, ConsoleColor::RED), format, args);
	va_end(args);
}

void GeneralLogger::Logger::Error(LogCategory category, const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	Write(std::make_shared<LogData>(LogType::Error, category, ConsoleColor::RED), format, args);
	va_end(args);
}


void GeneralLogger::Logger::Start() {
	if (this->active) {
		return;
	}

	active = true;

	this->logThread = std::make_unique<std::thread>([](Logger* logger) {
		logger->Run();
		}, &instance);
}

void GeneralLogger::Logger::End() {
	if (this->active == false)
		return;

	active = false;

	if (this->logThread != nullptr && this->logThread->joinable()) {
		this->logThread->join();
	}
}

void GeneralLogger::Logger::Run() {
	while (active) {
		std::shared_ptr<LogData> logData = nullptr;
		if (logList.Try_pop(logData) == false) {
			std::this_thread::sleep_for(std::chrono::duration<int>(1));
			continue;
		}
#if defined(_WIN32) || defined(_WIN64)
		_InterlockedDecrement(&logCount);
#else
		std::atomic_fetch_sub(&logCount, 1);
#endif
		if (logData == nullptr) {
			continue;
		}
		Write(logData);
	}
}

void GeneralLogger::Logger::Write(const std::shared_ptr<LogData> logData) {
	for (auto writer : logWriter) {
		writer->WriteLog(logData);
	}
}

void GeneralLogger::Logger::RequestWork(const std::shared_ptr<LogData>& logData) {
#if defined(_WIN32) || defined(_WIN64)
	_InterlockedIncrement(&logCount);
#else
	std::atomic_fetch_add(&logCount, 1);
#endif 
	logList.Enqueue(logData);
}

void GeneralLogger::Logger::Write(const std::shared_ptr<LogData>& logData, const wchar_t* format, va_list args)
{
	if (logData == nullptr) {
		std::cout << "[Logger] logData Error.\n";
		return;
	}

	auto curTime = time(NULL);
	struct tm localTime;
#if defined(_WIN32) || defined(_WIN64)
	localtime_s(&localTime, &curTime);
#else
	localtime_r(&curTime, localTime);
#endif
	swprintf_s(DeclspecPrefixBuffer,
		L"%04d-%02d-%02d %02d:%02d:%02d",
		localTime.tm_year + 1900,
		localTime.tm_mon + 1,
		localTime.tm_mday,
		localTime.tm_hour,
		localTime.tm_min,
		localTime.tm_sec
	);

	vswprintf_s(DeclspecLogBuffer, format, args);

	logData->PrefixBuffer.Assign(DeclspecPrefixBuffer);
	logData->PrefixBuffer.Append(L"\t");
	logData->Buffer.Assign(DeclspecLogBuffer);
	logData->Buffer.Append(L"\n");

#if defined(_WIN64) || defined(_WIN32)
	OutputDebugString((LPCTSTR)logData->PrefixBuffer.c_str());
	OutputDebugString((LPCTSTR)logData->Buffer.c_str());
#endif
}

GeneralLogger::ConsoleLogger::ConsoleLogger() 
	: showLevel(false)
	, showLocation(false)
	, showLogLevels(LogType())
{
	std::wcout.imbue(std::locale("korean"));
}

GeneralLogger::ConsoleLogger::ConsoleLogger(bool showLevel, bool showLocation) {
	std::wcout.imbue(std::locale("korean"));

	this->showLevel = showLevel;
	this->showLocation = showLocation;
	showLogLevels = LogType();
}

GeneralLogger::ConsoleLogger::~ConsoleLogger() {

}

void GeneralLogger::ConsoleLogger::WriteLog(const std::shared_ptr<LogData>& logData) {
	std::wcout << "[" << logData->PrefixBuffer.c_str() << " ]";
	std::wcout.setf(std::ios::left);
	std::wcout << "[" << std::setw(7) << logData->Level << " ]";
	std::wcout << logData->Buffer.c_str();
	if (std::wcout.fail())
	{
		std::wcout.clear();
	}
	std::wcout << std::endl;
}

