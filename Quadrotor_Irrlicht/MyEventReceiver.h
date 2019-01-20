#pragma once
#include <irrlicht.h>

using namespace irr;


class MyEventReceiver : public IEventReceiver
{
private:
	IrrlichtDevice* device = NULL;
	scene::ICameraSceneNode** cameras = NULL;
	int numCameras = 0;
	scene::ISceneManager* smgr = NULL;

public:
	// We'll create a struct to record info on the mouse state
	struct SMouseState
	{
		core::position2di Position;
		bool LeftButtonDown;
		SMouseState() : LeftButtonDown(false) { }
	} MouseState;

	void UpdateEventReceiver(
		IrrlichtDevice* device,
		scene::ISceneManager* smgr,
		scene::ICameraSceneNode** cameras,
		int numCameras)
	{
		this->device = device;
		this->smgr = smgr;
		this->cameras = cameras;
		this->numCameras = numCameras;
	}

	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
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

	const SMouseState & GetMouseState(void) const
	{
		return MouseState;
	}
};