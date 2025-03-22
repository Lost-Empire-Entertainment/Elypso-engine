//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef CRASHHANDLER_DLL_EXPORT
		#define CRASHHANDLER_API __declspec(dllexport)
	#else
		#define CRASHHANDLER_API __declspec(dllimport)
	#endif
#else
	#define CRASHHANDLER_API
#endif

#include <string>
#include <sstream>
#include <functional>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace ElypsoUtils
{
	using std::string;
	using std::ostringstream;
	using std::function;

	class CRASHHANDLER_API CrashHandler
	{
	public:
		static void SetProgramName(const string& newName) { name = newName; }

		static string GetProgramName() { return name; }

		static inline function<void()> ShutdownCallback;

		/// <summary>
		/// Assign a shutdown function with this function 
		/// that is called internally by CrashHandler whenever 
		/// a crash is detected after the error popup has beem closed.
		/// </summary>
		static inline void SetShutdownCallback(function<void()> callback) 
		{
			ShutdownCallback = move(callback);
		}

		/// <summary>
		/// Internal shutdown function, not meant to be called by end user.
		/// </summary>
		static inline void Shutdown() { if (ShutdownCallback) ShutdownCallback(); }

#ifdef _WIN32
		/// <summary>
		/// Windows crash handler that calls the minidump creator function and sends error info to error popup.
		/// </summary>
		static LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info);
#endif

		/// <summary>
		/// Return current day as number, current hour, minute and second.
		/// </summary>
		static string GetCurrentTimeStamp();
	private:
		/// <summary>
		/// The name of the program this crash handler is attached to
		/// </summary>
		static inline string name;

#ifdef _WIN32
		/// <summary>
		/// Creates a windows crash .dmp file to exe location.
		/// </summary>
		static string WriteMiniDump(EXCEPTION_POINTERS* info);

		/// <summary>
		/// Appends up to to last 10 frames of the call stack upon crash.
		/// </summary>
		static void AppendCallStackToStream(ostringstream& oss, CONTEXT* context);
#endif
	};
}