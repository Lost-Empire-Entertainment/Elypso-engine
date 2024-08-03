//This script is a part of the Game Template that is distributed by Elypso Engine.
//If you have access to this script then you have full freedom to do anything with it.
//A copy of the MIT license has been included in LICENSE.md

#pragma once

#include <string>

using std::string;

namespace Core
{
	class Console
	{
	public:
		enum Caller
		{
			FILE,
			INPUT
		};

		enum Type
		{
			INFO,
			DEBUG,
			EXCEPTION
		};

		static void InitializeLogger();

		/// <summary>
		/// Print selected message to in-game console.
		/// </summary>
		/// <param name="caller">What called the message.</param>
		/// <param name="type">What kind of a message is it.</param>
		/// <param name="message">The message itself.</param>
		/// <param name="onlyMessage">Do we only send the message without message caller, type and timestamp?</param>
		/// <param name="internalMessage">Do we also print this message to the in-game console?</param>
		static void WriteConsoleMessage(Caller caller, Type type, const string& message, bool onlyMessage = false, bool internalMessage = true);
	};
}