#include "main.h"
#include "video/uutVideo.h"
#include "resource/uutResourceCache.h"
#include "video/uutTexture.h"
#include "video/uutVideoBuffer.h"
#include "video/uutGraphics.h"
#include "platform/uutTime.h"
#include "io/uutInput.h"
#include "video/uutModel.h"
#include "math/uutVector4.h"
#include "Camera.h"

RUNAPP(uut::MyApp);

namespace uut
{
	static const Vector3f CAMERA_ACCELERATION(8.0f, 8.0f, 8.0f);
	static const float   CAMERA_FOVX = 90.0f;
	static const Vector3f CAMERA_POS(0.0f, 0.0f, 10.0f);
	static const float   CAMERA_SPEED_ROTATION = 0.2f;
	static const float   CAMERA_SPEED_FLIGHT_YAW = 100.0f;
	static const Vector3f CAMERA_VELOCITY(2.0f, 2.0f, 2.0f);
	static const float   CAMERA_ZFAR = 100.0f;
	static const float   CAMERA_ZNEAR = 0.1f;

	static const float   FLOOR_WIDTH = 128;
	static const float   FLOOR_HEIGHT = 128;
	static const float   FLOOR_TILE_S = 1;
	static const float   FLOOR_TILE_T = 1;

	static float g_cameraRotationSpeed = CAMERA_SPEED_ROTATION;

	MyApp::MyApp()
		: _color(0, 0, 0)
		, _pos(100, 100)
		, _paused(false)
		, _size(800, 600)
	{
	}

	void MyApp::OnInit()
	{
		const Recti rect(Vector2i::ZERO, _size);
		const float aspect = (1.0f * _size.x) / _size.y;

		_video->SetMode(_size.x, _size.y);
		_video->SetRenderState(RENDERSTATE_LIGHTNING, false);
		_video->SetRenderState(RENDERSTATE_DEPTH_TEST, false);
	}

	void MyApp::OnStart()
	{
		_tex0 = _cache->Load<Texture>("Data/zazaka.png");
		_tex1 = _cache->Load<Texture>("Data/floor.png");
		_model0 = _cache->Load<Model>("Data/house2.obj");

		_camera = new Camera(_context);
		if (_camera)
		{
			_camera->perspective(CAMERA_FOVX,
				static_cast<float>(_size.x) / static_cast<float>(_size.y),
				CAMERA_ZNEAR, CAMERA_ZFAR);

			_camera->setBehavior(Camera::CAMERA_BEHAVIOR_FIRST_PERSON);
			_camera->setPosition(CAMERA_POS);
			_camera->setAcceleration(CAMERA_ACCELERATION);
			_camera->setVelocity(CAMERA_VELOCITY);
		}
	}

	void MyApp::OnUpdate()
	{
		_paused = _input->IsKey(KEYCODE_SPACE);
		if (_paused)
			return;

		const Vector2i newPos = _input->GetMousePos();
		if (_input->IsKey(KEYCODE_ESCAPE)) 
			Stop();

		if (_camera)
		{
			static bool moveForwardsPressed = false;
			static bool moveBackwardsPressed = false;
			static bool moveRightPressed = false;
			static bool moveLeftPressed = false;
			static bool moveUpPressed = false;
			static bool moveDownPressed = false;

			const auto& velocity = _camera->getCurrentVelocity();

			Vector3f direction(0.0f, 0.0f, 0.0f);

			if (_input->IsKey(KEYCODE_W))
			{
				if (!moveForwardsPressed)
				{
					moveForwardsPressed = true;
					_camera->setCurrentVelocity(velocity.x, velocity.y, 0.0f);
				}

				direction.z += 1.0f;
			}
			else
			{
				moveForwardsPressed = false;
			}

			if (_input->IsKey(KEYCODE_S))
			{
				if (!moveBackwardsPressed)
				{
					moveBackwardsPressed = true;
					_camera->setCurrentVelocity(velocity.x, velocity.y, 0.0f);
				}

				direction.z -= 1.0f;
			}
			else
			{
				moveBackwardsPressed = false;
			}

			if (_input->IsKey(KEYCODE_D))
			{
				if (!moveRightPressed)
				{
					moveRightPressed = true;
					_camera->setCurrentVelocity(0.0f, velocity.y, velocity.z);
				}

				direction.x += 1.0f;
			}
			else
			{
				moveRightPressed = false;
			}

			if (_input->IsKey(KEYCODE_A))
			{
				if (!moveLeftPressed)
				{
					moveLeftPressed = true;
					_camera->setCurrentVelocity(0.0f, velocity.y, velocity.z);
				}

				direction.x -= 1.0f;
			}
			else
			{
				moveLeftPressed = false;
			}

			if (_input->IsKey(KEYCODE_E))
			{
				if (!moveUpPressed)
				{
					moveUpPressed = true;
					_camera->setCurrentVelocity(velocity.x, 0.0f, velocity.z);
				}

				direction.y += 1.0f;
			}
			else
			{
				moveUpPressed = false;
			}

			if (_input->IsKey(KEYCODE_Q))
			{
				if (!moveDownPressed)
				{
					moveDownPressed = true;
					_camera->setCurrentVelocity(velocity.x, 0.0f, velocity.z);
				}

				direction.y -= 1.0f;
			}
			else
			{
				moveDownPressed = false;
			}

// 			const Vector2i center(_size.x / 2, _size.y / 2);
			static Vector2i prevMouse = _input->GetMousePos();

			Vector2i curMouse = _input->GetMousePos();

			const float pitch = (prevMouse.y - curMouse.y) * g_cameraRotationSpeed;
			const float heading = -(curMouse.x - prevMouse.x) * g_cameraRotationSpeed;
			_camera->rotate(heading, pitch, 0.0f);

			prevMouse = curMouse;
			_camera->updatePosition(direction, _time->GetDelta());
		}

		_color._g += _time->GetDelta();
		while (_color._g > 1)
			_color._g -= 1.0f;
	}

	void MyApp::OnRender()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);

		_video->SetViewPort(Recti(0, 0, _size.x, _size.y));
		_video->SetColor(COLOR_CLEAR, Color::BLACK);
		_video->Clear(true, true, false);

		if (_camera)
		{
			_video->SetTransform(TRANSFORM_PROJECTION, _camera->getProjectionMatrix());
			_video->SetTransform(TRANSFORM_VIEW, _camera->getViewMatrix());
		}

		//*
		const int count = 100;
		const float side = 1;

		_graphics->ResetStates();
		_graphics->SetColor(Color::WHITE);

		_graphics->DrawQuad(_tex1,
			Vertex3(Vector3f(-FLOOR_WIDTH * 0.5f, -10.0f, FLOOR_HEIGHT * 0.5f), Vector2f(0, 0), 0xFFFFFFFF),
			Vertex3(Vector3f(-FLOOR_WIDTH * 0.5f, -10.0f, -FLOOR_HEIGHT * 0.5f), Vector2f(0.00f, FLOOR_TILE_T), 0xFFFFFFFF),
			Vertex3(Vector3f(FLOOR_WIDTH * 0.5f, -10.0f, -FLOOR_HEIGHT * 0.5f), Vector2f(FLOOR_TILE_S, FLOOR_TILE_T), 0xFFFFFFFF),
			Vertex3(Vector3f(FLOOR_WIDTH * 0.5f, -10.0f, FLOOR_HEIGHT * 0.5f), Vector2f(FLOOR_TILE_S, 0), 0xFFFFFFFF));

		{
			const auto v0 = Vector3f(-FLOOR_WIDTH * 0.5f, -10.0f, FLOOR_HEIGHT * 0.5f);
			const auto v1 = Vector3f(-FLOOR_WIDTH * 0.5f, -10.0f, -FLOOR_HEIGHT * 0.5f);
			const auto v2 = Vector3f(FLOOR_WIDTH * 0.5f, -10.0f, -FLOOR_HEIGHT * 0.5f);
			const auto v3 = Vector3f(FLOOR_WIDTH * 0.5f, -10.0f, FLOOR_HEIGHT * 0.5f);

			_graphics->DrawLine(v0, v1);
			_graphics->DrawLine(v1, v2);
			_graphics->DrawLine(v2, v3);
			_graphics->DrawLine(v0, v3);
		}

		_graphics->SetColor(Color::WHITE);
		for (int i = 0; i <= count; i++)
		{
			// XY PLANE
			_graphics->DrawLine(
				Vector3f(side * i, 0, 0),
				Vector3f(side * i, side * count, 0));
			_graphics->DrawLine(
				Vector3f(0, side * i, 0),
				Vector3f(side * count, side * i, 0));

			// XZ PLANE
// 			_graphics->DrawLine(
// 				Vector3f(side * i, 0, 0),
// 				Vector3f(side * i, 0, side * count));
// 			_graphics->DrawLine(
// 				Vector3f(0, side * i, 0),
// 				Vector3f(side * count, 0, side * i));

			// YZ PLANE
// 			_graphics->DrawLine(
// 				Vector3f(0, side * i, 0),
// 				Vector3f(0, side * i, side * count));
// 			_graphics->DrawLine(
// 				Vector3f(0, 0, side * i),
// 				Vector3f(0, side * count, side * i));
		}
		_graphics->Flush();/**/

// 		if (_model0)
// 		{
// 			_video->SetColor(COLOR_DRAW, Color::WHITE);
// 			_model0->Draw();
// 		}
 
		_graphics->ResetStates();
		_graphics->SetColor(Color::WHITE);
		_graphics->DrawTriangle(0,
			Vertex3(Vector3f(0, 0, 0), 0xFFFFFFFF),
			Vertex3(Vector3f(0, 0, 20), 0xFFFFFFFF),
			Vertex3(Vector3f(20, 0, 20), 0xFFFFFFFF));
		_graphics->Flush();
	}
}