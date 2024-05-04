#ifndef LOG_H
#define LOG_H

#include <cstdarg>

namespace Core
{
	struct LogLevel
	{
		enum Enum {
			Raw,
			Debug,
			Info,
			Warning,
			Error,
			Fatal,
			MaxLevel,
		};
	};

	class Log
	{
	public:
		Log();

		/// <summary>
		/// Adds an entry to the log.
		/// The reason for using "Add" instead of something like "Print"
		/// is so output can be delayed, and some entries can be discarded
		/// on certain conditions.
		/// </summary>
		void Add(const LogLevel::Enum& level, const char* format, ...);

		/// <summary>
		/// Adds an entry to the log.
		/// </summary>
		void AddList(const LogLevel::Enum& level, const char* format, va_list argList);

	public:
		int	count[LogLevel::MaxLevel];
	};
}

#endif // LOG_H
