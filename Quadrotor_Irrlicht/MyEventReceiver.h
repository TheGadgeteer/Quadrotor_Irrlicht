#pragma once
#include <irrlicht.h>
#include "Quadrotor.h"
#include <map>

using namespace irr;

extern IrrlichtDevice* device;

class MyEventReceiver : public IEventReceiver
{
private:
	scene::ICameraSceneNode** cameras = NULL;
	int numCameras = 0;
	Quadrotor* quadrotor = NULL;

	scene::ISceneManager* smgr = NULL;
	std::map<wchar_t, bool*> keyMap;

public:
	// We'll create a struct to record info on the mouse state
	struct SMouseState
	{
		core::position2di Position;
		bool LeftButtonDown;
		SMouseState() : LeftButtonDown(false) { }
	} MouseState;

	void setCameras(
		scene::ICameraSceneNode** cameras,
		int numCameras)
	{
		this->cameras = cameras;
		this->numCameras = numCameras;
	}

	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		if (this->smgr == NULL) {
			if (device == NULL)
				return false;
			this->smgr = device->getSceneManager();
		}

		// Remember the mouse state
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch (event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				break;

			case EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				printf("Left Mouse Button Released\n");
				break;

			case EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;

			default:
				// We won't use the wheel
				break;
			}
		}
		else if (event.EventType == irr::EET_KEY_INPUT_EVENT) {

			// look up in swap map
			if (event.KeyInput.PressedDown) {
				try {
					bool* swapVar = keyMap.at(event.KeyInput.Char);
					*swapVar = !*swapVar;
				}
				catch (std::out_of_range) {};
			}
			switch (event.KeyInput.Key) {
			case KEY_END:
				if (event.KeyInput.PressedDown) {
					gui::ICursorControl* cursor = device->getCursorControl();
					cursor->setVisible(!cursor->isVisible());
				}
				break;
			case KEY_ESCAPE: // Swap cameras
				if (event.KeyInput.PressedDown) {
					scene::ICameraSceneNode* activeCam = smgr->getActiveCamera();
					// find active camera
					int idx = -1;
					for (int i = 0; i < numCameras; ++i) {
						if (cameras[i] == activeCam) {
							idx = i;
							break;
						}
					}
					if (idx >= 0)
						setActiveCamera(cameras[(idx + 1) % numCameras]);
				}
				break;
			case KEY_KEY_R:
				if (quadrotor != NULL)
					quadrotor->reset();
				break;
			case KEY_KEY_0:
				if (quadrotor != NULL) {
					float desiredSpeed[] = { 0.005f, 0.005f, 0.005f, 0.005f };
					quadrotor->setMotorSpeed(desiredSpeed);
				}
				break;
			case KEY_KEY_9:
				if (quadrotor != NULL) {
					float desiredSpeed[] = { 0.5f, 0.5f, 0.5f, 0.5f };
					quadrotor->setMotorSpeed(desiredSpeed);
				}
				break;
			case KEY_KEY_8:
				if (quadrotor != NULL) {
					float desiredSpeed[] = { 0.7f, 0.2f, 0.2f, 0.7f };
					quadrotor->setMotorSpeed(desiredSpeed);
				}
				break;
			case KEY_KEY_7:
				if (quadrotor != NULL) {
					float desiredSpeed[] = { 0.7003f, 0.7003f, 0.6997f, 0.6997f };
					quadrotor->setMotorSpeed(desiredSpeed);
				}
				break;
			case KEY_KEY_6:
				if (quadrotor != NULL) {
					float desiredSpeed[] = { 0.7003f, 0.6997f, 0.7003f, 0.6997f };
					quadrotor->setMotorSpeed(desiredSpeed);
				}
				break;
			case KEY_KEY_5:
				if (quadrotor != NULL) {
					float desiredSpeed[] = { 1.f, 1.f, 1.f, 1.f };
					quadrotor->setMotorSpeed(desiredSpeed);
				}
				break;

			}
		}



		return false;
	}

	void setActiveCamera(scene::ICameraSceneNode* newActive)
	{
		if (0 == smgr)
			return;

		scene::ICameraSceneNode * active = smgr->getActiveCamera();
		active->setInputReceiverEnabled(false);

		newActive->setInputReceiverEnabled(true);
		newActive->setPosition(active->getPosition());
		newActive->setRotation(active->getRotation());
		smgr->setActiveCamera(newActive);
	}
	void setQuadrotor(Quadrotor* quadrotor) {
		this->quadrotor = quadrotor;
	}

	const SMouseState & GetMouseState(void) const
	{
		return MouseState;
	}

	void registerSwap(char key, bool *swap) {
		keyMap.insert(std::pair<char, bool*>(key, swap));
	}

	void removeSwap(char key) {
		keyMap.erase(key);
	}
};