// Copyright (C) 2024 Lost Empire Entertainment
//
// This header file is distributed under the MIT License.
// You may use, modify, and distribute this file freely.
// 
// THIS FILE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// See LICENSE.md for more information.

#pragma once

#include <cmath>
#include <numbers>

using std::is_arithmetic_v;
using std::is_integral_v;
using std::is_floating_point_v;
using std::numeric_limits;
using std::invalid_argument;
using std::overflow_error;
using std::underflow_error;
using std::sin;
using std::cos;
using std::tan;
using std::ceil;
using std::round;
using std::floor;
using std::isinf;
using std::isnan;

#ifndef M_PI
#define M_PI 3.1415926535
#endif

namespace Utils
{
	class Math
	{
	public:
		/// <summary>
		/// Adds B to A.
		/// </summary>
		template <typename T1, typename T2>
		static auto Add(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1> 
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if constexpr (
				is_integral_v<T1> 
				&& is_integral_v<T2>)
			{
				if ((b > 0 && a > numeric_limits<decltype(a + b)>::max() - b) 
					|| (b < 0 && a < numeric_limits<decltype(a + b)>::min() - b))
				{
					throw overflow_error("Integer addition resulted in overflow or underflow.");
				}
			}
			
			using ResultType = decltype(
				static_cast<long double>(a) 
				+ static_cast<long double>(b));

			ResultType result = 
				static_cast<ResultType>(a) 
				+ static_cast<ResultType>(b);

			if (isinf(result))
			{
				throw overflow_error("Addition resulted in overflow.");
			}
			if (isnan(result))
			{
				throw underflow_error("Addition resulted in underflow.");
			}

			return result;
		}

		/// <summary>
		/// Subtracts B from A.
		/// </summary>
		template <typename T1, typename T2>
		static auto Subtract(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if constexpr (
				is_integral_v<T1> 
				&& is_integral_v<T2>)
			{
				if ((b > 0 && a < numeric_limits<T1>::min() + b) 
					|| (b < 0 && a > numeric_limits<T1>::max() + b))
				{
					throw overflow_error("Integer subtraction resulted in overflow or underflow.");
				}
			}

			using ResultType = decltype(
				static_cast<long double>(a) 
				- static_cast<long double>(b));

			ResultType result = static_cast<ResultType>(a) - static_cast<ResultType>(b);

			if (isinf(result))
			{
				throw overflow_error("Subtraction resulted in overflow.");
			}
			if (isnan(result))
			{
				throw underflow_error("Subtraction resulted in underflow.");
			}

			return result;
		}

		/// <summary>
		/// Multiplies B by A.
		/// </summary>
		template <typename T1, typename T2>
		static auto Multiply(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if constexpr (
				is_integral_v<T1>
				&& is_integral_v<T2>)
			{
				if (a != 0 
					&& (b > numeric_limits<decltype(a * b)>::max() / a 
					|| b < numeric_limits<decltype(a * b)>::min() / a))
				{
					throw overflow_error("Integer multiplication resulted in overflow or underflow.");
				}
			}

			using ResultType = decltype(
				static_cast<long double>(a) 
				* static_cast<long double>(b));

			ResultType result = static_cast<ResultType>(a) * static_cast<ResultType>(b);

			if (isinf(result))
			{
				throw overflow_error("Multiplication resulted in overflow.");
			}
			if (isnan(result))
			{
				throw underflow_error("Multiplication resulted in underflow.");
			}

			return result;
		}

		/// <summary>
		/// Divides B by A.
		/// </summary>
		template <typename T1, typename T2>
		static auto Divide(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if (b == 0)
			{
				throw invalid_argument("Division by zero is not allowed.");
			}

			using ResultType = decltype(
				static_cast<long double>(a) 
				/ static_cast<long double>(b));

			ResultType result = static_cast<ResultType>(a) / static_cast<ResultType>(b);

			if (isinf(result))
			{
				throw overflow_error("Division resulted in overflow.");
			}
			if (isnan(result))
			{
				throw underflow_error("Division resulted in underflow.");
			}

			return result;
		}

		/// <summary>
		/// Finds the remainder of A divided by B.
		/// </summary>
		template <typename T1, typename T2>
		static auto Mod(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1> 
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if (b == 0)
			{
				throw invalid_argument("Modulo operation with a divisor of zero is not allowed.");
			}

			if constexpr (
				is_integral_v<T1> 
				&& is_integral_v<T2>)
			{
				return a % b;
			}
			else
			{
				using ResultType = decltype(
					fmod(static_cast<long double>(a),
					static_cast<long double>(b)));

				ResultType result = static_cast<ResultType>(
					fmod(static_cast<long double>(a), 
					static_cast<long double>(b)));

				if (isinf(result))
				{
					throw overflow_error("Variable overflow occurred during modulo operation.");
				}
				if (isnan(result))
				{
					throw underflow_error("Variable underflow occurred during modulo operation.");
				}

				return result;
			}
		}

		/// <summary>
		/// Raises A to the power of B.
		/// </summary>
		template <typename T1, typename T2>
		static auto Power(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1> 
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if (a == 0 
				&& b < 0)
			{
				throw invalid_argument("Zero cannot be raised to a negative exponent.");
			}

			if constexpr (
				is_integral_v<T1> 
				&& is_integral_v<T2>)
			{
				if (b < 0) 
				{
					throw invalid_argument("Negative exponent not supported for integer powers.");
				}
				if (a == 0 
					&& b == 0) 
				{
					throw invalid_argument("Multiplying two zero values is not allowed.");
				}
				if (b == 0) return 1;
				if (a == 0) return 0;

				T1 result = 1;
				T1 base = a;

				while (b > 0) {
					if (b % 2 == 1) //if b is odd
					{ 
						if (result > numeric_limits<T1>::max() / base) 
						{
							throw overflow_error("Overflow occurred during multiplication.");
						}
						result *= base;
					}
					if (base > numeric_limits<T1>::max() / base) 
					{
						throw overflow_error("Overflow occurred during multiplication.");
					}
					base *= base;
					b /= 2;
				}

				return result;
			}

			using ResultType = decltype(
				pow(static_cast<long double>(a),
				static_cast<long double>(b)));

			ResultType result = static_cast<ResultType>(
				pow(static_cast<long double>(a), 
				static_cast<long double>(b)));

			if (isinf(result))
			{
				throw overflow_error("Variable overflow occurred during floating-point power calculation.");
			}
			if (isnan(result))
			{
				throw underflow_error("Variable underflow occurred during floating-point power calculation.");
			}

			return result;
		}

		/// <summary>
		/// Calculates the square root of A.
		/// </summary>
		template <typename T>
		static auto Sqrt(T a)
		{
			static_assert(
				is_arithmetic_v<T>,
				"T must be an arithmetic type.");

			if (a < 0)
			{
				throw invalid_argument("Cannot calculate the square root of a negative number.");
			}

			using ResultType = decltype(sqrt(static_cast<long double>(a)));
			return static_cast<ResultType>(sqrt(static_cast<long double>(a)));
		}

		/// <summary>
		/// Returns the absolute value of A.
		/// </summary>
		template <typename T>
		static auto Abs(T a)
		{
			static_assert(
				is_arithmetic_v<T>,
				"T must be an arithmetic type.");

			if constexpr (is_integral_v<T>)
			{
				return static_cast<long double>(abs(a));
			}
			else
			{
				return fabs(static_cast<long double>(a));
			}
		}

		/// <summary>
		/// Determines the maximum of A and B.
		/// </summary>
		template <typename T1, typename T2>
		static auto Max(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1> 
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			using ResultType = decltype(
				static_cast<long double>(a) 
				> static_cast<long double>(b) 
				? static_cast<long double>(a) 
				: static_cast<long double>(b));

			return 
				(static_cast<ResultType>(a) 
				> static_cast<ResultType>(b)) 
				? static_cast<ResultType>(a) 
				: static_cast<ResultType>(b);
		}

		/// <summary>
		/// Determines the minimum of A and B.
		/// </summary>
		template <typename T1, typename T2>
		static auto Min(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1> 
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			using ResultType = decltype(
				static_cast<long double>(a) 
				< static_cast<long double>(b) 
				? static_cast<long double>(a) 
				: static_cast<long double>(b));

			return 
				(static_cast<ResultType>(a) 
				< static_cast<ResultType>(b)) 
				? static_cast<ResultType>(a) 
				: static_cast<ResultType>(b);
		}

		/// <summary>
		/// Checks if A is equal to B.
		/// </summary>
		template <typename T1, typename T2>
		static auto Equals(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			using ResultType = decltype(
				static_cast<long double>(a) 
				== static_cast<long double>(b));
			return 
				(static_cast<long double>(a) 
				== static_cast<long double>(b));
		}

		/// <summary>
		/// Checks if A is less than B.
		/// </summary>
		template <typename T1, typename T2>
		static auto LessThan(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			using ResultType = decltype(
				static_cast<long double>(a)
				< static_cast<long double>(b));
			return
				(static_cast<long double>(a)
				< static_cast<long double>(b));
		}

		/// <summary>
		/// Checks if A is greater than B.
		/// </summary>
		template <typename T1, typename T2>
		static auto GreaterThan(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			using ResultType = decltype(
				static_cast<long double>(a)
				> static_cast<long double>(b));
			return
				(static_cast<long double>(a)
				> static_cast<long double>(b));
		}

		/// <summary>
		/// Determines the percentage of A relative to B.
		/// </summary>
		template <typename T1, typename T2>
		static auto Percentage(T1 a, T2 b)
		{
			static_assert(
				is_arithmetic_v<T1>
				&& is_arithmetic_v<T2>,
				"Both T1 and T2 must be arithmetic types.");

			if (b == 0) 
			{
				throw invalid_argument("Division by zero is not allowed.");
			}

			using ResultType = decltype(
				static_cast<long double>(a) 
				/ static_cast<long double>(b) * 100);

			return 
				static_cast<ResultType>(
				static_cast<long double>(a) 
				/ static_cast<long double>(b) * 100);
		}

		/// <summary>
		/// Constrains a value within the range of min and max.
		/// </summary>
		template<typename T>
		static auto Clamp(T value, T min, T max)
		{
			static_assert(
				is_arithmetic_v<T>, 
				"T must be an arithmetic type.");

			if (min > max)
			{
				throw invalid_argument("Min value is greater than max value.");
			}

			using ResultType = decltype(
				static_cast<long double>(value) 
				< static_cast<long double>(min)
				? static_cast<long double>(min)
				: (static_cast<long double>(value) 
				> static_cast<long double>(max)
				? static_cast<long double>(max)
				: static_cast<long double>(value)));

			ResultType result =
				(static_cast<long double>(value) 
				< static_cast<long double>(min))
				? static_cast<long double>(min)
				: ((static_cast<long double>(value) 
				> static_cast<long double>(max))
				? static_cast<long double>(max)
				: static_cast<long double>(value));

			if (isinf(result))
			{
				throw overflow_error("Overflow occurred while clamping the value.");
			}
			if (isnan(result))
			{
				throw underflow_error("Underflow occurred while clamping the value.");
			}

			return result;
		}

		/// <summary>
		/// Interpolates linearly between A and B based on T.
		/// </summary>
		template <typename T1, typename T2>
		static auto Lerp(T1 a, T2 b, T2 t)
		{
			static_assert(
				is_arithmetic_v<T1> 
				&& is_arithmetic_v<T2>, 
				"T1 and T2 must be arithmetic types.");

			using ResultType = decltype(
				static_cast<long double>(a) 
				+ static_cast<long double>(t) 
				* (static_cast<long double>(b) 
				- static_cast<long double>(a)));

			ResultType result = 
				static_cast<ResultType>(a)
				+ static_cast<ResultType>(t)
				* (static_cast<ResultType>(b)
				- static_cast<ResultType>(a));

			if (isinf(result))
			{
				throw overflow_error("Overflow occurred during linear interpolation.");
			}
			if (isnan(result))
			{
				throw underflow_error("Underflow occurred during linear interpolation.");
			}

			return result;
		}

		/// <summary>
		/// Rounds A up to the nearest integer that is greater than or equal to A.
		/// </summary>
		template <typename T>
		static auto Ceil(T value)
		{
			static_assert(
				is_floating_point_v<T>,
				"T must be an floating-point type.");

			using ResultType = decltype(
				ceil(static_cast<long double>(value)));

			return static_cast<ResultType>(
				ceil(static_cast<long double>(value)));
		}

		/// <summary>
		/// Rounds A down to the nearest integer if A is less than halfway to the next integer, otherwise rounds up.
		/// </summary>
		template <typename T>
		static auto Round(T value)
		{
			static_assert(
				is_floating_point_v<T>,
				"T must be an floating-point type.");

			using ResultType = decltype(
				round(static_cast<long double>(value)));

			return static_cast<ResultType>(
				round(static_cast<long double>(value)));
		}

		/// <summary>
		/// Rounds A down to the nearest integer that is less than or equal to A.
		/// </summary>
		template <typename T>
		static auto Floor(T value)
		{
			static_assert(
				is_floating_point_v<T>,
				"T must be an floating-point type.");

			using ResultType = decltype(
				floor(static_cast<long double>(value)));

			return static_cast<ResultType>(
				floor(static_cast<long double>(value)));
		}

		/// <summary>
		/// Computes the sine of an angle.
		/// </summary>
		template <typename T>
		static auto Sin(T angle)
		{
			static_assert(
				is_arithmetic_v<T>, 
				"T must be an arithmetic type.");

			using ResultType = decltype(
				sin(static_cast<long double>(angle)));

			return static_cast<ResultType>(
				sin(static_cast<long double>(angle)));
		}

		/// <summary>
		/// Computes the cosine of an angle.
		/// </summary>
		template <typename T>
		static auto Cos(T angle)
		{
			static_assert(
				is_arithmetic_v<T>, 
				"T must be an arithmetic type.");

			using ResultType = decltype(
				cos(static_cast<long double>(angle)));

			return static_cast<ResultType>(
				cos(static_cast<long double>(angle)));
		}

		/// <summary>
		/// Computes the tangent of an angle.
		/// </summary>
		template <typename T>
		static auto Tan(T angle)
		{
			static_assert(
				is_arithmetic_v<T>, 
				"T must be an arithmetic type.");

			using ResultType = decltype(
				tan(static_cast<long double>(angle)));

			ResultType result = static_cast<ResultType>(
				tan(static_cast<long double>(angle)));

			if (isinf(result))
			{
				throw overflow_error("Overflow occurred while computing the tangent.");
			}
			if (isnan(result))
			{
				throw underflow_error("Underflow occurred while computing the tangent.");
			}

			return static_cast<ResultType>(
				tan(static_cast<long double>(angle)));
		}

		/// <summary>
		/// Converts degrees to radians.
		/// </summary>
		template <typename T>
		static auto DegToRad(T degrees)
		{
			static_assert(
				is_arithmetic_v<T>, 
				"T must be an arithmetic type.");

			using ResultType = decltype(
				static_cast<long double>(degrees)
				* M_PI 
				/ 180.0);

			return static_cast<ResultType>(
				static_cast<long double>(degrees) 
				* M_PI 
				/ 180.0);
		}

		/// <summary>
		/// Converts radians to degrees.
		/// </summary>
		template <typename T>
		static auto RadToDeg(T radians)
		{
			static_assert(
				is_arithmetic_v<T>, 
				"T must be an arithmetic type.");

			using ResultType = decltype(
				static_cast<long double>(radians) 
				* 180.0 
				/ M_PI);

			return static_cast<ResultType>(
				static_cast<long double>(radians) 
				* 180.0 
				/ M_PI);
		}
	};
}