#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <string.h>
#include <string>

struct streqpred
{
   bool operator()(const char* a, const char* b) const
   {
      return strcmp(a, b) == 0;
   }
};

struct stdstreqpred
{
	bool operator()(std::string a, std::string b) const
   {
      return a == b;
   }
};

/*
_STLP_BEGIN_NAMESPACE

_STLP_TEMPLATE_NULL struct hash<std::string>
{
	size_t operator()(std::string __s) const {
		unsigned long __h = 0; 
		for (unsigned int i = 0; i < __s.length(); ++i)
			__h = 5*__h + __s[i];

		return size_t(__h);
	}
};

_STLP_END_NAMESPACE
*/

namespace WWCOMMON {

	template <typename T, typename tScalar>
		inline void SmoothCD(T &val,          // in/out: value to be smoothed
		T &valRate,      // in/out: rate of change of the value
		const tScalar timeDelta, // in: time interval
		const T &to,     // in: the target value
		const tScalar smoothTime)// in: timescale for smoothing
	{
		if (smoothTime > 0.0f)
		{
			tScalar omega = 2.0f / smoothTime;
			tScalar x = omega * timeDelta;
			tScalar exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
			T change = val - to;
			T temp = (valRate + omega * change) * timeDelta;
			valRate = (valRate - omega * temp) * exp;
			val = to + (change + temp) * exp;
		}
		else if (timeDelta > 0.0f)
		{
			valRate = (to - val) / timeDelta;
			val = to;
		}
		else
		{
			val = to;
			valRate -= valRate; // zero it...
		}
	}

	/**
	 * Solves a quadratic polynomial 3 equation system for the 3 unknown constants.
	 **/
	inline void solveQuadraticInterp3f(const float t[3], const float v[3], float& a, float& b, float& c) {
		float t__10 = t[1] - t[0];
		float t__20 = t[2] - t[0];
		float t2__0 = t[0]*t[0];
		float t__20_10 = t__20 / t__10;
		float t2__10 = t[1]*t[1] - t2__0;

		float v__10 = v[1] - v[0];
		a = ((v[2] - v[0]) - (t__20_10*v__10))/((t[2]*t[2] - t2__0) - (t2__10*t__20_10));
		b = (v__10 - (a*t2__10))/t__10;
		c = v[0] - (a*t[0]*t[0]) - (b*t[0]);
	}

	/**
	 * Interpolates/extrapolates using a quadratic equation defined by the constants a, b and c.
	 **/
	inline float quadraticInterp(const float& t, const float& a, const float& b, const float& c) {
		return (a*t*t) + (b*t) + c;
	}



}; // END OF NAMESPACE WWCOMMON

#endif // __GENERAL_H__