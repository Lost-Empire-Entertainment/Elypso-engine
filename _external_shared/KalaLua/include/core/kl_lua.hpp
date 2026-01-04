//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <variant>
#include <type_traits>
#include <utility>

extern "C"
{
#include "lua.h"
}

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "core/kl_core.hpp"

namespace KalaLua::Core
{
	using std::string;
	using std::function;
	using std::vector;
	using std::variant;
	using std::is_void_v;
	using std::is_pointer_v;
	using std::is_same_v;
	using std::is_invocable_r_v;
	using std::index_sequence;
	using std::index_sequence_for;
	using std::get;
	using std::bad_variant_access;
	using std::holds_alternative;
	using std::forward;
	using std::optional;
	using std::nullopt;

	using KalaHeaders::KalaLog::Log;
	using KalaHeaders::KalaLog::LogType;

	using LuaVar = variant
	<
		int,
		float,
		double,
		bool,
		string
	>;

	//Returns false if variable not found in LuaVar is used
	template<typename T>
	constexpr bool IsLuaVarCompatible =
		is_same_v<T, int> 
		|| is_same_v<T, float>
		|| is_same_v<T, double>
		|| is_same_v<T, bool>
		|| is_same_v<T, string>;

	class LIB_API Lua
	{
	public:
		//Initialize KalaLua, does not load scripts or functions
		static bool Initialize();

		static inline bool IsInitialized() { return isInitialized; };

		//Get the pointer to lua state stored within KalaLua
		//after it has initialized, recommended only for advanced users
		static inline lua_State* GetLuaState() { return isInitialized ? state : nullptr; }

		//Load and compile a lua script for use via CallFunction
		static bool LoadScript(const string& script);

		//Call a function from one of the loaded lua scripts with N number of args,
		//default void-only return type, cannot return any LuaVar types,
		//empty namespace calls function in global namespace,
		//no dot in namespace calls function in parent namespace,
		//dotted namespace allows nesting namespace calls (my.name.space.function)
		static void CallFunction(
			const string& functionName,
			const string& functionNamespace,
			const vector<LuaVar>& args = {})
		{
			_CallFunction(
				functionName,
				functionNamespace,
				args);
		}

		//Call a function from one of the loaded lua scripts with N number of args,
		//returns void on failure, can return any LuaVar type,
		//empty namespace calls function in global namespace,
		//no dot in namespace calls function in parent namespace,
		//dotted namespace allows nesting namespace calls (my.name.space.function)
		template<typename R>
		static optional<R> CallFunction(
			const string& functionName,
			const string& functionNamespace,
			const vector<LuaVar>& args = {})
		{
			static_assert(
				IsLuaVarCompatible<R>,
				"Unsupported return type was passed to CallFunction");

			LuaVar ret{};
			if (!_CallFunction(
				functionName,
				functionNamespace,
				args,
				&ret))
			{
				return nullopt;
			}

			try
			{
				return ExtractLuaVar<R>(ret);
			}
			catch (...)
			{
				Log::Print(
					"Unsupported variable type was passed to CallFunction function '" + functionName + "'!",
					"KALALUA_CALL_FUNCTION",
					LogType::LOG_ERROR,
					2);

				return nullopt;
			}
		}

		//Register a function into KalaLua for lua to use externally,
		//this overload accepts functionals and targetFunction can return LuaVar or nothing,
		//accepts N number of any args defined in LuaVar,
		//empty namespace moves function to global namespace,
		//no dot in namespace moves function to parent namespace,
		//dotted namespace allows nesting namespaces (my.name.space)
		template<typename... Args, typename R>
		static inline void RegisterFunction(
			const string& functionName,
			const string& functionNamespace,
			const function<R(Args...)>& targetFunction)
		{
			static_assert(
				(IsLuaVarCompatible<Args> && ...),
				"Unsupported argument type was passed to RegisterFunction");

			static_assert(
				is_void_v<R>
				|| IsLuaVarCompatible<R>,
				"Unsupported return type was passed to RegisterFunction");

			auto invoker = [functionName, targetFunction](const vector<LuaVar>& args) -> optional<LuaVar>
				{
					if (args.size() != sizeof...(Args))
					{
						Log::Print(
							"Argument count mismatch when invoking function '" + functionName + "'!",
							"KALALUA_REGISTER_FUNCTION",
							LogType::LOG_ERROR,
							2);

						return nullopt;
					}

					if constexpr (is_void_v<R>)
					{
						InvokeTyped(
							targetFunction,
							args,
							index_sequence_for<Args...>{});

						return nullopt;
					}
					else
					{
						R result = InvokeTypedReturn(
							targetFunction,
							args,
							index_sequence_for<Args...>{});

						return LuaVar{ result };
					}
				};

			_RegisterFunction(
				functionName,
				functionNamespace,
				invoker);
		}

		//Register a function into KalaLua for lua to use externally,
		//this overload accepts free functions and targetFunction can return LuaVar or nothing,,
		//accepts N number of any args defined in LuaVar,
		//empty namespace moves function to global namespace,
		//no dot in namespace moves function to parent namespace,
		//dotted namespace allows nesting namespaces (my.name.space)
		template<typename... Args, typename R>
		static inline void RegisterFunction(
			const string& functionName,
			const string& functionNamespace,
			R (*func)(Args...))
		{
			RegisterFunction(
				functionName,
				functionNamespace,
				function<R(Args...)>(func));
		}

		//Register a function into KalaLua for lua to use externally,
		//this overload accepts custom lua functions, recommended only for advanced users,
		//empty namespace moves function to global namespace,
		//no dot in namespace moves function to parent namespace,
		//dotted namespace allows nesting namespaces (my.name.space)
		static void RegisterFunction(
			const string& functionName,
			const string& functionNamespace,
			const function<int(lua_State*)>& targetFunction);

		//Shut down KalaLua and the Lua runtime
		static void Shutdown();
	private:
		static inline bool isInitialized{};
		static inline lua_State* state{};

		template<typename... Args, size_t... I>
		static inline void InvokeTyped(
			const function<void(Args...)>& targetFunction,
			const vector<LuaVar>& args,
			index_sequence<I...>)
		{
			targetFunction(ExtractLuaVar<Args>(args[I])...);
		}

		template<typename... Args, size_t... I, typename R>
		static inline R InvokeTypedReturn(
			const function<R(Args...)>& targetFunction,
			const vector<LuaVar>& args,
			index_sequence<I...>)
		{
			return targetFunction(ExtractLuaVar<Args>(args[I])...);
		}

		//Numeric extraction helper to help lua cast into int/float/double correctly
		template<typename T>
		static T ExtractLuaVar(const LuaVar& v)
		{
			if constexpr (is_same_v<T, int>)
			{
				if (holds_alternative<int>(v))     return get<int>(v);
				if (holds_alternative<double>(v))  return scast<int>(get<double>(v));
				if (holds_alternative<float>(v))   return scast<int>(get<float>(v));
			}
			else if constexpr (is_same_v<T, float>)
			{
				if (holds_alternative<float>(v))   return get<float>(v);
				if (holds_alternative<double>(v))  return scast<float>(get<double>(v));
				if (holds_alternative<int>(v))     return scast<float>(get<int>(v));
			}
			else if constexpr (is_same_v<T, double>)
			{
				if (holds_alternative<double>(v)) return get<double>(v);
				if (holds_alternative<int>(v))    return scast<double>(get<int>(v));
				if (holds_alternative<float>(v))  return scast<double>(get<float>(v));
			}
			else return get<T>(v);

			KalaLuaCore::ForceClose(
				"KalaLua type cast error",
				"ExtractLuaVar failed to cast unsupported type!");
		}

		//The internal true function caller
		static bool _CallFunction(
			const string& functionName,
			const string& functionNamespace,
			const vector<LuaVar>& args,
			LuaVar* outReturn = nullptr);

		//The internal true register function that is used
		//to register the function after parsing args
		static bool _RegisterFunction(
			const string& functionName,
			const string& functionNamespace,
			function<optional<LuaVar>(const vector<LuaVar>&)> invoker);
	};
}