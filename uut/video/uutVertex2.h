#pragma once
#include "uutVideoDefs.h"
#include "math/uutVector2.h"
#include "math/uutVector3.h"

namespace uut
{
	struct Vertex2
	{
		Vector2f pos;
		Vector2f tex;
		uint32_t color;

		static const unsigned SIZE;
		static const unsigned DECLARE_COUNT = 3;
		static const SDeclareType DECLARE[DECLARE_COUNT];

		Vertex2() {}
		Vertex2(const Vector2f& pos, const uint32_t color);
		Vertex2(const Vector2f& pos, const Vector2f& tex, const uint32_t color);
	};
}