#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdint>
#include <string>
#include <cassert>
#include <cstring>
#include <chrono>
#include <stdexcept>

#ifndef CPP_UTILITY_NAMESPACE
#define CPP_UTILITY_NAMESPACE cpp
#endif

// #define CPP_UTILITY_IMPLEMENTATION in one of your cpp files (only one)
// #define LOGGER_DISABLE_LOGGING to disable logging macro

// #define CPP_UTILITY_IMPLEMENTATION

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef LOGGER_DISABLE_LOGGING
#define LOG(logLevel, x, ...) CPP_UTILITY_NAMESPACE ::Logger::GetGlobalLogger().print(CPP_UTILITY_NAMESPACE ::LogLevel::logLevel, __FILENAME__, __LINE__,  x, __VA_ARGS__)
#else
#define LOG(logLevel, x, ...)
#endif

#define LOGEXCEPT(x, ...) { auto err = CPP_UTILITY_NAMESPACE ::Format(x, __VA_ARGS__); LOG(ERR, err); throw std::runtime_error(err); }

#ifdef CPP_UTILITY_IMPLEMENTATION
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <signal.h>
#endif
#include <signal.h>
#include <regex>
#include <filesystem>
#include <cwctype>
#include <mutex>
#endif

#ifdef CPP_UTILITY_IMPLEMENTATION
namespace NMB
{

	enum Result
	{
		CANCEL,
		OK
	};

	enum Icon
	{
		ICON_INFO,
		ICON_WARNING,
		ICON_ERROR
	};


	static Result show(const char* p_title, const char* p_message, Icon icon)
	{
#if defined(_WIN32)

		int icon_flag = 0;

		switch (icon)
		{
		case NMB::Icon::ICON_INFO:
			icon_flag = MB_ICONINFORMATION;
			break;
		case NMB::Icon::ICON_WARNING:
			icon_flag = MB_ICONWARNING;
			break;
		case NMB::Icon::ICON_ERROR:
			icon_flag = MB_ICONERROR;
			break;
		}

		int result = MessageBoxA(nullptr, p_message, p_title, MB_OKCANCEL | MB_SYSTEMMODAL | icon_flag);

		if (result == IDOK)
			return NMB::Result::OK;
		else
			return NMB::Result::CANCEL;

#elif defined(__APPLE__)

		CFOptionFlags cf_alert_icon;

		switch (icon)
		{
		case NMB::Icon::INFO:
			cf_alert_icon = kCFUserNotificationNoteAlertLevel;
			break;
		case NMB::Icon::WARNING:
			cf_alert_icon = kCFUserNotificationCautionAlertLevel;
			break;
		case NMB::Icon::ERROR:
			cf_alert_icon = kCFUserNotificationStopAlertLevel;
			break;
		}

		CFStringRef cf_title = CFStringCreateWithCString(kCFAllocatorDefault, p_title, kCFStringEncodingUTF8);
		CFStringRef cf_message = CFStringCreateWithCString(kCFAllocatorDefault, p_message, kCFStringEncodingUTF8);

		CFOptionFlags result;

		CFUserNotificationDisplayAlert(0, cf_alert_icon, nullptr, nullptr, nullptr, cf_title, cf_message, CFSTR("OK"), CFSTR("Cancel"), nullptr, &result);

		CFRelease(cf_title);
		CFRelease(cf_message);

		if (result == kCFUserNotificationDefaultResponse)
			return NMB::Result::OK;
		else
			return NMB::Result::CANCEL;

#elif defined(__linux__)

		GtkMessageType gtk_message_type;

		switch (icon)
		{
		case NMB::Icon::ICON_INFO:
			gtk_message_type = GTK_MESSAGE_INFO;
			break;
		case NMB::Icon::ICON_WARNING:
			gtk_message_type = GTK_MESSAGE_WARNING;
			break;
		case NMB::Icon::ICON_ERROR:
			gtk_message_type = GTK_MESSAGE_ERROR;
			break;
		}

		GtkWidget* p_dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, gtk_message_type, GTK_BUTTONS_OK_CANCEL, "%s\n\n%s", p_title, p_message);
		gint result = gtk_dialog_run(GTK_DIALOG(p_dialog));
		gtk_widget_destroy(p_dialog);

		if (result == GTK_RESPONSE_OK)
			return NMB::Result::OK;
		else
			return NMB::Result::CANCEL;

#else

#error "Platform not supported!"

#endif
	}

}
#endif


namespace CPP_UTILITY_NAMESPACE
{
	template <typename T>
	inline static T clamp(T& value, T min, T max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	void DebugBreak();

	void ShowInfoBox(std::string_view title, std::string_view text);
	void ShowWarningBox(std::string_view title, std::string_view text);
	void ShowErrorBox(std::string_view title, std::string_view text);

	// std::string contains find (IndexOf) rfind(LastIndexOf) substring
	// for more info lookup std::string::find() and std::string::rfind()

	std::string Replace(std::string_view source, std::string_view find, std::string_view Replace);
	std::string ReplaceAll(std::string_view source, std::string_view find, std::string_view Replace);
	std::wstring Replace(std::wstring_view source, std::wstring_view find, std::wstring_view Replace);
	std::wstring ReplaceAll(std::wstring_view source, std::wstring_view find, std::wstring_view Replace);

	inline size_t LastIndexOf(std::string_view source, std::string_view find) { return source.rfind(find.data()); }
	inline size_t IndexOf(std::string_view source, std::string_view find) { return source.find(find.data()); }

	inline size_t LastIndexOf(std::wstring_view source, std::wstring_view find) { return source.rfind(find.data()); }
	inline size_t IndexOf(std::wstring_view source, std::wstring_view find) { return source.find(find.data()); }

	std::vector<std::string> Split(const std::string& source, std::string_view regex);
	std::vector<std::wstring> Split(const std::wstring& source, std::wstring_view regex);

	std::string LowerCase(std::string_view source);
	std::string UpperCase(std::string_view source);

	std::wstring LowerCase(std::wstring_view source);
	std::wstring UpperCase(std::wstring_view source);

	std::wstring ToWideString(std::string_view source);
	std::string ToAsciiString(std::wstring_view source);

	std::string LTrim(std::string_view source);
	std::string RTrim(std::string_view source);
	std::string Trim(std::string_view source);

	std::wstring LTrim(std::wstring_view source);
	std::wstring RTrim(std::wstring_view source);
	std::wstring Trim(std::wstring_view source);

	bool EqualIgnoreCase(std::string_view a, std::string_view b);
	bool StartsWith(std::string_view source, std::string_view find);
	bool EndsWith(std::string_view source, std::string_view find);
	bool Contains(std::string_view source, std::string_view find);

	bool EqualIgnoreCase(std::wstring_view a, std::wstring_view b);
	bool StartsWith(std::wstring_view source, std::wstring_view find);
	bool EndsWith(std::wstring_view source, std::wstring_view find);
	bool Contains(std::wstring_view source, std::wstring_view find);

	size_t ToInt64(std::string_view source);
	bool TryToInt64(std::string_view source, size_t& RefInt64);

	size_t ToInt64(std::wstring_view source);
	bool TryToInt64(std::wstring_view source, size_t& RefInt64);

	double ToDouble(std::string_view source);
	bool TryToDouble(std::string_view source, double& RefDouble);

	double ToDouble(std::wstring_view source);
	bool TryToDouble(std::wstring_view source, double& RefDouble);

	bool WriteAllBytes(std::string_view path, const void* data, size_t size);
	bool WriteAllText(std::string_view path, std::string_view text);

	/// <summary>
	/// Reads file binary into vector. First element in vector determines success status.
	/// </summary>
	/// <returns>First element in vector determines success status. 1 == success, 0 == fail</returns>
	std::optional<std::vector<uint8_t>> ReadAllBytes(const std::string& path);

	/// <summary>
	/// Reads file binary into string.
	/// </summary>
	/// <returns></returns>
	std::optional<std::string> ReadAllText(const std::string& path);

	bool Exists(std::string_view path);
	std::string ReadLine(FILE* file);
	bool CreateEmptyFile(std::string_view path);

	struct any {
		enum type { Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64, Float, Double, String, Ptr };
		any(int8_t   e) { m_data.INT8 = e; m_type = Int8; }
		any(int16_t   e) { m_data.INT16 = e; m_type = Int16; }
		any(int32_t   e) { m_data.INT32 = e; m_type = Int32; }
		any(int64_t   e) { m_data.INT64 = e; m_type = Int64; }
		any(uint8_t   e) { m_data.UINT8 = e; m_type = UInt8; }
		any(uint16_t   e) { m_data.UINT16 = e; m_type = UInt16; }
		any(uint32_t   e) { m_data.UINT32 = e; m_type = UInt32; }
		any(uint64_t   e) { m_data.UINT64 = e; m_type = UInt64; }
#ifdef _MSC_VER
		// For DWORD
		any(wchar_t e) { m_data.INT16 = (int16_t)e; m_type = Int16; }
		any(unsigned long e) { m_data.UINT16 = (uint16_t)e; m_type = UInt16; }
#endif
		any(float e) { m_data.FLOAT = e; m_type = Float; }
		any(double e) { m_data.DOUBLE = e; m_type = Double; }
		any(const char* e) { m_data.STRING = e; m_type = String; }
		any(const std::string& e) { m_data.STRING = e.c_str(); m_type = String; }
		any(void* e) { m_data.PTR = e; m_type = Ptr; }
		inline type get_type() const { return m_type; }
		inline int8_t get_int8() const { return m_data.INT8; }
		inline int16_t get_int16() const { return m_data.INT16; }
		inline int32_t get_int32() const { return m_data.INT32; }
		inline int64_t get_int64() const { return m_data.INT64; }
		inline uint8_t get_uint8() const { return m_data.UINT8; }
		inline uint16_t get_uint16() const { return m_data.UINT16; }
		inline uint32_t get_uint32() const { return m_data.UINT32; }
		inline uint64_t get_uint64() const { return m_data.UINT64; }
		inline float get_float() const { return m_data.FLOAT; }
		inline double get_double() const { return m_data.DOUBLE; }
		inline const char* get_string() const { return m_data.STRING; }
		inline void* get_ptr() const { return m_data.PTR; }
	private:
		type m_type;
		union {
			int8_t   INT8;
			int16_t   INT16;
			int32_t   INT32;
			int64_t   INT64;
			uint8_t    UINT8;
			uint16_t   UINT16;
			uint32_t   UINT32;
			uint64_t   UINT64;
			float FLOAT;
			double DOUBLE;
			const char* STRING;
			void* PTR;
		} m_data;
	};

	template<typename ... Arg>
	std::string Format(const std::string& input, Arg... arguments)
	{
		using namespace std;
		std::vector<any> vec{ arguments... };
		std::string result;
		result.reserve((size_t)(input.size() * 1.5));

		bool openbracket = false;
		bool closebracket = false;
		bool formatSpecifier = false;
		bool clearLine = false;
		int argumentId = 0;
		int argumentIdCounter = 0;
		bool hasArgumentId = false;
		std::string argumentFormatSpecifier{};
		int formatSpecifierIndex = 0;
		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '{') {
				openbracket = !openbracket;
				if (!openbracket) result.append("{");
				continue;
			}
			if (input[i] == '}' && !openbracket) {
				closebracket = !closebracket;
				if (!closebracket) result.append("}");
				continue;
			}
			if (input[i] == ':' && openbracket) {
				formatSpecifier = true;
				argumentFormatSpecifier.clear();
				argumentFormatSpecifier.reserve(10);
				continue;
			}
			if (formatSpecifier && openbracket && input[i] != '}') {
				argumentFormatSpecifier.push_back(input[i]);
				continue;
			}
			if (openbracket) {
				if (argumentId >= vec.size()) {
					result.append("{OUT_OF_RANGE:" + std::to_string(argumentId) + "}");
					openbracket = false;
					argumentId = 0;
					continue;
				}
				switch (input[i]) {
				case '0': argumentId = argumentId * 10 + 0; hasArgumentId = true; break;
				case '1': argumentId = argumentId * 10 + 1; hasArgumentId = true; break;
				case '2': argumentId = argumentId * 10 + 2; hasArgumentId = true; break;
				case '3': argumentId = argumentId * 10 + 3; hasArgumentId = true; break;
				case '4': argumentId = argumentId * 10 + 4; hasArgumentId = true; break;
				case '5': argumentId = argumentId * 10 + 5; hasArgumentId = true; break;
				case '6': argumentId = argumentId * 10 + 6; hasArgumentId = true; break;
				case '7': argumentId = argumentId * 10 + 7; hasArgumentId = true; break;
				case '8': argumentId = argumentId * 10 + 8; hasArgumentId = true; break;
				case '9': argumentId = argumentId * 10 + 9; hasArgumentId = true; break;
				case '}': {
					auto& e = vec[hasArgumentId ? argumentId : argumentIdCounter];
					std::string argument_output;
						#define set_argument_output(type) \
						{\
							if (formatSpecifier) {\
								if(argumentFormatSpecifier[0] != '%') { argumentFormatSpecifier = "%" + argumentFormatSpecifier; }\
								size_t nbytes = snprintf(nullptr, 0, argumentFormatSpecifier.c_str(), type);\
								argument_output.resize(nbytes);\
								snprintf((char*)argument_output.c_str(), nbytes, argumentFormatSpecifier.c_str(), type);\
								argument_output.pop_back();\
							}\
							else\
								argument_output = std::to_string(type);\
							break;\
						}

					switch (e.get_type()) {
					case any::Int8: set_argument_output(e.get_int8());
					case any::Int16: set_argument_output(e.get_int16());
					case any::Int32: set_argument_output(e.get_int32());
					case any::Int64: set_argument_output(e.get_int64());

					case any::UInt8: set_argument_output(e.get_uint8());
					case any::UInt16: set_argument_output(e.get_uint16());
					case any::UInt32: set_argument_output(e.get_uint32());
					case any::UInt64: set_argument_output(e.get_uint64());

					case any::Float: set_argument_output(e.get_float());
					case any::Double: set_argument_output(e.get_double());
					case any::String: {
						if (formatSpecifier) {
							size_t nbytes = snprintf(nullptr, 0, argumentFormatSpecifier.c_str(), e.get_string());
							argument_output.resize(nbytes);
							snprintf((char*)argument_output.c_str(), nbytes, argumentFormatSpecifier.c_str(), e.get_string());
							argument_output.pop_back();
						}
						else
							argument_output = e.get_string();
						break;
					}
					case any::Ptr: {
						if (!formatSpecifier) {
							argumentFormatSpecifier += "%p";
						}
						size_t nbytes = snprintf(nullptr, 0, argumentFormatSpecifier.c_str(), e.get_ptr());
						argument_output.resize(nbytes);
						snprintf((char*)argument_output.c_str(), nbytes, argumentFormatSpecifier.c_str(), e.get_ptr());
						argument_output.pop_back();
						break;
					}
					}
					result.append(argument_output);
					openbracket = false;
					if (hasArgumentId)
						argumentIdCounter = argumentId + 1;
					else
						argumentIdCounter++;
					argumentId = 0;
					hasArgumentId = false;
					formatSpecifier = false;
#undef set_argument_output
					break;
				}
				default:
					result += "{INVALID}";
					argumentId = 0;
					openbracket = false;
					hasArgumentId = false;
				}
			}
			else {
				result += input[i];
			}
		}
		if (clearLine) {
			result = "\33[2K\r" + result;
		}
		return result;
	}

	enum LoggerFlags : int32_t {
		LOGGER_TYPE_CONSOLE = 0b01,
		LOGGER_TYPE_FILE = 0b10
	};

	enum class LogLevel {
		INFO,
		INFOBOLD,
		WARNING,
		ERR
	};

	struct LoggerOptions {
		int32_t LoggerType = LOGGER_TYPE_CONSOLE;
		bool IncludeDate = false;
		bool IncludeFileAndLine = true;
		std::string LoggerOutputFileName;
		bool ShowMessageBoxOnError = false;
		bool DebugBreakOnError = false;
		bool VerboseMode = false;
	};

	class Logger {
	public:
		Logger(LoggerOptions options);
		Logger(Logger&) = delete;
		Logger(Logger&&) noexcept;
		~Logger();

		void AddFileLogging(const char* FileName);

		static Logger& GetGlobalLogger();
		static std::string Escape(const std::string& input);

		template<typename ... Arg>
		void print(LogLevel logLevel, const char* FileName, int LineNumber, const std::string& input, Arg... arguments) {
			using namespace std;
			std::vector<any> vec{ arguments... };
			char currentTime[80]{};
			{
				time_t rawTime = time(NULL);
				if (Options.IncludeDate) {
					strftime(currentTime, 80, "%Y/%m/%d %H:%M:%S %p", localtime(&rawTime));
				}
				else {
					strftime(currentTime, 80, "%H:%M:%S %p", localtime(&rawTime));
				}
			}
			double timeSinceStart = double(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _start_timestamp).count()) * 1e-9;
			uint64_t threadId = _get_current_thread_id();
			std::string result;
			{
				FileName = FileName ? FileName : ".";
				char fileAndNumber[125]{};

				if (Options.IncludeFileAndLine) {
					sprintf(fileAndNumber, " %s:%03d", FileName, LineNumber);
				}

				const char* INFOLabel = "INFO";
				const char* WARNINGLabel = "WARNING";
				const char* ERRORLabel = "ERROR";
				const char* UNKNOWNLabel = "UNKNOWN";
				const char* pLogLevel = "";

				switch (logLevel) {
				case LogLevel::INFO:
				case LogLevel::INFOBOLD:
					pLogLevel = INFOLabel;
					break;
				case LogLevel::WARNING:
					pLogLevel = WARNINGLabel;
					break;
				case LogLevel::ERR:
					pLogLevel = ERRORLabel;
					break;
				default:
					pLogLevel = UNKNOWNLabel;
					break;
				}
				if (Options.VerboseMode) {
					result = Format("{0} ({1:%.2fs}) | {2} {3} {4:%-8s} {5}", currentTime, timeSinceStart, fileAndNumber, threadId, pLogLevel,
						Format(input, arguments...));
				}
				else {
					result = Format("[{0:%.2fs} |{1}] {2}", timeSinceStart, fileAndNumber,
						Format(input, arguments...));
				}
			}
			result += "\n";
			_internal_log(logLevel, result);
		}

		Logger& operator<<(const char* input) {
			print(LogLevel::INFO, nullptr, 0, input);
			return *this;
		}

		Logger& operator<<(const std::string& input) {
			print(LogLevel::INFO, nullptr, 0, input);
			return *this;
		}

	public:
		static LoggerOptions GlobalLoggerOptions;
		LoggerOptions Options;

	private:
		void _internal_log(LogLevel logLevel, const std::string& input);
		uint64_t _get_current_thread_id();

	private:
		std::fstream _file_stream;
		static std::chrono::steady_clock::time_point _start_timestamp;
	};

#ifdef CPP_UTILITY_IMPLEMENTATION
	static std::mutex _console_lock;

	std::string Replace(std::string_view source, std::string_view find, std::string_view Replace)
	{
		std::string result = source.data();
		size_t start_pos = source.find(find);
		if (start_pos == std::string::npos)
			return result;
		result.replace(start_pos, Replace.length(), Replace);
		return result;
	}

	std::string ReplaceAll(std::string_view source, std::string_view find, std::string_view Replace)
	{
		std::string result = source.data();
		if (find.empty())
			return result;
		size_t start_pos = 0;
		while ((start_pos = result.find(find, start_pos)) != std::string::npos)
		{
			result.replace(start_pos, find.length(), Replace);
			start_pos += Replace.length();
		}
		return result;
	}

	std::wstring Replace(std::wstring_view source, std::wstring_view find, std::wstring_view Replace)
	{
		std::wstring result = source.data();
		size_t start_pos = source.find(find);
		if (start_pos == std::string::npos)
			return result;
		result.replace(start_pos, find.length(), Replace);
		return result;
	}

	std::wstring ReplaceAll(std::wstring_view source, std::wstring_view find, std::wstring_view Replace)
	{
		std::wstring result = source.data();
		if (find.empty())
			return result;
		size_t start_pos = 0;
		while ((start_pos = result.find(find, start_pos)) != std::string::npos)
		{
			result.replace(start_pos, result.length(), Replace);
			start_pos += Replace.length();
		}
		return result;
	}

	std::vector<std::string> Split(const std::string& source, std::string_view regex)
	{
		std::vector<std::string> Split_content;
		std::regex pattern(regex.data());
		std::copy(std::sregex_token_iterator(source.begin(), source.end(), pattern, -1),
			std::sregex_token_iterator(), back_inserter(Split_content));
		return Split_content;
	}

	std::vector<std::wstring> Split(const std::wstring& source, std::wstring_view regex)
	{
		std::vector<std::wstring> Split_content;
		std::wregex pattern(regex.data());
		std::copy(std::wsregex_token_iterator(source.begin(), source.end(), pattern, -1),
			std::wsregex_token_iterator(), back_inserter(Split_content));
		return Split_content;
	}

	std::string LowerCase(std::string_view source)
	{
		std::string lw = source.data();
		for (int i = 0; i < lw.size(); i++)
			lw[i] = std::tolower(lw[i]);
		return lw;
	}

	std::string UpperCase(std::string_view source)
	{
		std::string up = source.data();
		for (int i = 0; i < up.length(); i++)
			up[i] = std::toupper(up[i]);
		return up;
	}

	std::wstring LowerCase(std::wstring_view source)
	{
		std::wstring lw = source.data();
		for (int i = 0; i < lw.size(); i++)
			lw[i] = std::towlower(lw[i]);
		return lw;
	}

	std::wstring UpperCase(std::wstring_view source)
	{
		std::wstring up = source.data();
		for (int i = 0; i < up.length(); i++)
			up[i] = std::towupper(up[i]);
		return up;
	}

	std::wstring ToWideString(std::string_view source)
	{
		std::wstring result;
		result.resize(source.size());
		::mbstowcs(result.data(), source.data(), source.size());
		return result;
	}

	std::string ToAsciiString(std::wstring_view source)
	{
		std::string result;
		result.resize(source.size());
		::wcstombs(result.data(), source.data(), source.size());
		return result;
	}

	std::string LTrim(std::string_view source)
	{
		std::string s = source.data();
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
			{ return !std::isspace(ch); }));
		return s;
	}

	std::string RTrim(std::string_view source)
	{
		std::string s = source.data();
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
			{ return !std::isspace(ch); })
			.base(),
			s.end());
		return s;
	}

	std::string Trim(std::string_view source)
	{
		return RTrim(LTrim(source));
	}

	std::wstring LTrim(std::wstring_view source)
	{
		std::wstring s = source.data();
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wchar_t ch)
			{ return !std::isspace(ch); }));
		return s;
	}

	std::wstring RTrim(std::wstring_view source)
	{
		std::wstring s = source.data();
		s.erase(std::find_if(s.rbegin(), s.rend(), [](wchar_t ch)
			{ return !std::isspace(ch); })
			.base(),
			s.end());
		return s;
	}

	std::wstring Trim(std::wstring_view source)
	{
		return RTrim(LTrim(source));
	}

	bool EqualIgnoreCase(std::string_view a, std::string_view b)
	{
		return LowerCase(a) == LowerCase(b);
	}

	bool StartsWith(std::string_view source, std::string_view find)
	{
		return source.rfind(find, 0) != std::string_view::npos;
	}

	bool EndsWith(std::string_view source, std::string_view find)
	{
		return source.rfind(find) != std::string_view::npos;
	}

	bool Contains(std::string_view source, std::string_view find)
	{
		return source.find(find) != std::string_view::npos;
	}

	bool EqualIgnoreCase(std::wstring_view a, std::wstring_view b)
	{
		return LowerCase(a) == LowerCase(b);
	}

	bool StartsWith(std::wstring_view source, std::wstring_view find)
	{
		return source.rfind(find, 0);
	}

	bool EndsWith(std::wstring_view source, std::wstring_view find)
	{
		return source.rfind(find);
	}

	bool Contains(std::wstring_view source, std::wstring_view find)
	{
		return source.find(find) != std::string_view::npos;
	}

	size_t ToInt64(std::string_view source)
	{
		return std::stoull(source.data());
	}

	bool TryToInt64(std::string_view source, size_t& RefInt64)
	{
		try
		{
			RefInt64 = std::stoull(source.data());
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	size_t ToInt64(std::wstring_view source)
	{
		return std::stoull(source.data());
	}

	bool TryToInt64(std::wstring_view source, size_t& RefInt64)
	{
		try
		{
			RefInt64 = std::stoull(source.data());
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	double ToDouble(std::string_view source)
	{
		return std::stod(source.data());
	}

	bool TryToDouble(std::string_view source, double& RefDouble)
	{
		try
		{
			RefDouble = std::stod(source.data());
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	double ToDouble(std::wstring_view source)
	{
		return std::stod(source.data());
	}

	bool TryToDouble(std::wstring_view source, double& RefDouble)
	{
		try
		{
			RefDouble = std::stod(source.data());
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	std::optional<std::string> ReadAllText(const std::string& path)
	{
		// Open the file for reading
		std::ifstream file(path);

		// Check if the file was opened successfully
		if (!file.is_open()) {
			return std::nullopt;
		}

		// Read the contents of the file into a stringstream
		std::stringstream ss;
		ss << file.rdbuf();

		// Close the file
		file.close();

		// Return the contents of the stringstream as an std::string
		return ss.str();
	}

	bool Exists(std::string_view path)
	{
		return std::filesystem::exists(path);
	}

	std::string ReadLine(FILE* file)
	{
		int character = '\n';
		std::string line = "";
		do
		{
			character = fgetc(file);
			if (character == '\n')
				break;
			line += character;
		} while (!feof(file));
		return line;
	}

	bool CreateEmptyFile(std::string_view path)
	{
		FILE* file = fopen(path.data(), "w");
		if (!file)
			return false;
		fflush(file);
		fclose(file);
		return true;
	}

	std::optional<std::vector<uint8_t>> ReadAllBytes(const std::string& path)
	{
		// Open the file for reading in binary mode
		std::ifstream file(path, std::ios::binary);

		// Check if the file was opened successfully
		if (!file.is_open()) {
			return std::nullopt;
		}

		// Determine the size of the file
		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// Read the contents of the file into a vector
		std::vector<uint8_t> buffer(fileSize);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

		// Close the file
		file.close();

		// Return the vector containing the contents of the file
		return buffer;
	}

	bool WriteAllBytes(std::string_view path, const void* data, size_t size)
	{
		FILE* output = fopen(path.data(), "wb");
		if (!output)
			return false;
		fwrite(data, 1, size, output);
		fclose(output);
		return true;
	}

	bool WriteAllText(std::string_view path, std::string_view text)
	{
		FILE* output = fopen(path.data(), "w");
		if (!output)
			return false;
		fwrite(text.data(), 1, text.size(), output);
		fclose(output);
		return true;
	}

	void DebugBreak()
	{
#if defined(_WIN32)
		::DebugBreak();
#else
		raise(SIGTRAP);
#endif
	}

	void ShowInfoBox(std::string_view title, std::string_view text)
	{
		NMB::show(title.data(), text.data(), NMB::ICON_INFO);
	}

	void ShowWarningBox(std::string_view title, std::string_view text)
	{
		NMB::show(title.data(), text.data(), NMB::ICON_WARNING);
	}

	void ShowErrorBox(std::string_view title, std::string_view text)
	{
		NMB::show(title.data(), text.data(), NMB::ICON_ERROR);
	}

	Logger::Logger(LoggerOptions options) :
		Options(options)
	{
		if (Options.LoggerType & LOGGER_TYPE_FILE) {
			if (Options.LoggerOutputFileName.length() == 0)
				Options.LoggerOutputFileName = (std::to_string(time(NULL)) + ".log.txt");
			_file_stream.open(Options.LoggerOutputFileName, std::ios::out);
		}
	}

	Logger::Logger(Logger&& move) noexcept
	{
		Options = move.Options;
		_file_stream = std::move(move._file_stream);
	}

	Logger::~Logger()
	{
		_file_stream.close();
	}

	Logger& Logger::GetGlobalLogger()
	{
		static Logger logger(Logger::GlobalLoggerOptions);
		return logger;
	}

	std::string Logger::Escape(const std::string& input)
	{
		std::string out;
		out.reserve(input.size());
		for (size_t i = 0; i < input.size(); i++) {
			if (input[i] == '{')
				out += "{{";
			else if (input[i] == '}')
				out += "}}";
			else
				out += input[i];
		}
		return out;
	}

	void Logger::_internal_log(LogLevel logLevel, const std::string& output)
	{
		std::lock_guard<std::mutex> lock(_console_lock);
		if (Options.LoggerType & LOGGER_TYPE_CONSOLE) {
#ifdef _WIN32
			static HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
			switch (logLevel) {
			case LogLevel::INFOBOLD:
				SetConsoleTextAttribute(hStdOut, 15);
				break;
			case LogLevel::WARNING:
				SetConsoleTextAttribute(hStdOut, 6);
				break;
			case LogLevel::ERR:
				SetConsoleTextAttribute(hStdOut, 4);
				break;
			}
#else
			switch (logLevel) {
			case LogLevel::INFOBOLD:
				std::cout << "\033[1;47;35m";
				break;
			case LogLevel::WARNING:
				std::cout << "\x1B[33m";
				break;
			case LogLevel::ERR:
				std::cout << "\x1B[31m";
				break;
			}
#endif
#ifdef _WIN32
			std::cout << output;
			SetConsoleTextAttribute(hStdOut, 7);
#else
			std::cout << output << "\033[0m";
#endif
		}
		if (Options.LoggerType & LOGGER_TYPE_FILE) {
			_file_stream << output;
			_file_stream.flush();
		}
		if (logLevel == LogLevel::ERR) {
			if (Options.ShowMessageBoxOnError) {
				CPP_UTILITY_NAMESPACE::ShowErrorBox("Encountered an Error", output);
			}
			if (Options.DebugBreakOnError) {
				CPP_UTILITY_NAMESPACE::DebugBreak();
			}
		}
	}

	void Logger::AddFileLogging(const char* FileName)
	{
		_file_stream = std::fstream(FileName, std::ios::out);
		Options.LoggerType |= LOGGER_TYPE_FILE;
	}

	uint64_t Logger::_get_current_thread_id()
	{
#ifdef _WIN32
		return GetCurrentThreadId();
#else
#error "Not Implemented"
#endif
	}

	std::chrono::steady_clock::time_point Logger::_start_timestamp = std::chrono::high_resolution_clock::now();
	LoggerOptions Logger::GlobalLoggerOptions;

#endif

}