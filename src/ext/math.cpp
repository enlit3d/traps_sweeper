#include "math.h"
#include <cmath>
#include <algorithm>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

namespace engine::math
{
	f32 sin(f32 x)
	{
		return std::sin(x);
	}
	f32 cos(f32 x)
	{
		return std::cos(x);
	}

	f32 sqrt(f32 x)
	{
		return std::sqrt(x);
	}

	f32 min(f32 a, f32 b)
	{
		return std::min(a, b);
	}

	f32 max(f32 a, f32 b)
	{
		return std::max(a, b);
	}

	f32 ceil(f32 x)
	{
		return std::ceil(x);
	}

	f32 floor(f32 x)
	{
		return std::floor(x);
	}


	f32 lerp(f32 a, f32 b, f32 ratio)
	{
		return a + (b - a) * ratio;
	}

	i32 toInt(f32 x)
	{
		return static_cast<i32>(round(x));
	}



	/*! \class small_prng
	\brief From http://burtleburtle.net/bob/rand/smallprng.html
	*/
	class SmallPRNG
	{
		u32 a;
		u32 b;
		u32 c;
		u32 d;

		static inline u32 rot(u32 x, u32 k) noexcept { return (((x) << (k)) | ((x) >> (32 - (k)))); }
	public:
		explicit SmallPRNG(u32 seed = 0xdeadbeef) noexcept
		{
			a = 0xf1ea5eed;
			b = c = d = seed;
			for (size_t i = 0; i < 20; ++i)
				(*this)();
		}

		inline u32 operator()() noexcept
		{
			u32 e = a - rot(b, 27);
			a = b ^ rot(c, 17);
			b = c + d;
			c = d + e;
			d = e + a;
			return d;
		}

		static constexpr u32 min() { return std::numeric_limits<u32>::min(); }
		static constexpr u32 max() { return std::numeric_limits<u32>::max(); }
	};

	template <typename T, typename T2>
	T remap(T2 srcV, T2 srcMin, T2 srcMax, T destMin, T destMax) {
		f32 v = static_cast<f32>(srcV);
		f32 sMin = static_cast<f32>(srcMin);
		f32 sMax = static_cast<f32>(srcMax);
		f32 dMin = static_cast<f32>(destMin);
		f32 dMax = static_cast<f32>(destMax);
		return static_cast<T>((v - sMin) / (sMax - sMin) * (dMax - dMin) + dMin);
	}

	
	static SmallPRNG rng = []() { //TODO: maybe seed this better later
		u32 seed = time(nullptr);
		return SmallPRNG{ seed };
	}();;

	u32 randBits()
	{
		return rng();
	}

	f32 randFloat(f32 min, f32 max)
	{
		return remap(rng(), rng.min(), rng.max(), min, max);
	}
	i32 randInt(i32 min, i32 maxInclusive)
	{
		int n = maxInclusive - min + 1;
		int remainder = rng.max() % n;
		int x;
		do
		{
			x = rand();
		} while (x >= rng.max() - remainder);
		return min + x % n;
	}
}