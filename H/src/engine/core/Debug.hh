#ifndef DEBUG_HH
#define DEBUG_HH

#if _HAS_EXCEPTIONS
#include <stdexcept>
#endif // _HAS_EXCEPTIONS

#include "Log.hh"

namespace Core
{
	/// <summary>
	/// Displays the text one way or another, and terminates the program.
	/// </summary>
	void TerminateOnFatalError(const char* text);

#ifdef ENABLE_LOG
	extern Log log;
#endif // !ENABLE_LOG

#if DEBUG
#define IFDBG(exp) exp
#else // !DEBUG
#define IFDBG(exp)
#endif // !DEBUG

/// <summary>
/// UNUSED_EXPR will tell the compiler not to warn about a given
/// variable being unused. "yeah, we know - this is unused."
/// </summary>
///
/// <remarks>
/// the internet says that (void)sizeof(expr) is the right way to do
/// this, but not for us, not with our compilers. the below is the
/// result of much experimentation by @lucas, who says that we have at
/// least one compiler that does not consider sizeof(expr) to be a
/// 'usage' of the variable(s) inside of expr.
///
/// also note that we do not have the 'if+const expr' warning enabled
/// because combining #if and if expression/constants (which we often
/// need to do - for example
/// 'caps->gles.requireClearAlpha = UNITY_WEBGL || UNITY_STV') is super
/// noisy.
///
/// This macro and its documentation are copied verbatim after a code
/// snippet posted by Unity's Aras Pranckevicius on Twitter.
/// https://twitter.com/aras_p/status/794952299240181760
/// </remarks>
#define UNUSED_EXPR(exp)			do { if (false) { (void)(exp); } } while (0)

#if DEBUG && defined(ENABLE_LOG)
#define LOG_DEBUG(format, ...)		do { ::Core::log.Add(::Core::LogLevel::Debug,	format, ##__VA_ARGS__); } while (0)
#else // !DEBUG || !defined(ENABLE_LOG)
#define LOG_DEBUG(format, ...)
#endif // !DEBUG || !defined(ENABLE_LOG)

#ifdef ENABLE_LOG
#define LOG_RAW(format, ...)		do { ::Core::log.Add(::Core::LogLevel::Raw,		format, ##__VA_ARGS__); } while (0)
#define LOG_INFO(format, ...)		do { ::Core::log.Add(::Core::LogLevel::Info,	format, ##__VA_ARGS__); } while (0)
#define LOG_WARNING(format, ...)	do { ::Core::log.Add(::Core::LogLevel::Warning,	format, ##__VA_ARGS__); } while (0)
#define LOG_ERROR(format, ...)		do { ::Core::log.Add(::Core::LogLevel::Error,	format, ##__VA_ARGS__); } while (0)
#define LOG_FATAL(format, ...)		do { ::Core::log.Add(::Core::LogLevel::Fatal,	format, ##__VA_ARGS__); } while (0)
#else // !ENABLE_LOG
#define LOG_RAW(format, ...)
#define LOG_INFO(format, ...)
#define LOG_WARNING(format, ...)
#define LOG_ERROR(format, ...)
#define LOG_FATAL(format, ...)
#endif // !ENABLE_LOG

#if _HAS_EXCEPTIONS
#define NOT_IMPLEMENTED				do { throw new ::Core::NotImplementedException(); } while (0)
	class NotImplementedException : public std::logic_error
	{
	public:
		NotImplementedException();
	};
#else // !_HAS_EXCEPTIONS
#define NOT_IMPLEMENTED
#endif // !_HAS_EXCEPTIONS
}

#endif // DEBUG_HH
