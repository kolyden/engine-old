#pragma once
#include "core/uutApplication.h"
#include <windows.h>
#include "video/uutColor.h"
#include "math/uutVector2.h"

namespace uut
{
	class Texture;
	class VideoBuffer;
	class Model;

	class MyApp : public Application
	{
	public:
		MyApp();

	protected:
		SharedPtr<Texture> _tex0;
		SharedPtr<VideoBuffer> _vbuf;
		SharedPtr<VideoBuffer> _ibuf;
		SharedPtr<Model> _model0;

		Color _color;
		Vector2i _pos;
		Vector2i _size;
		bool _paused;

		virtual void OnInit() override;
		virtual void OnStart() override;
		virtual void OnUpdate() override;
		virtual void OnRender() override;

		void UpdateBuffer(const Vector2& pos, const Vector2& size);
	};
}