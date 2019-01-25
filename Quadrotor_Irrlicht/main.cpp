#include <irrlicht.h>
#include <iostream>
#include <vector>
#include <string>
#include "driverChoice.h"
#include "ShaderSetup.h"
#include "MyEventReceiver.h"
#include "Quadrotor.h"
#include "PlatformNode.h"
#include "Graph.h"
#include "FuzzyGraph.h"

using namespace irr;

#define _METER *100

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#define WIDTH 1024
#define HEIGHT 768


IrrlichtDevice* device = 0;
bool UseHighLevelShaders = false;
float fpsMax = 250;


int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;// driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	UseHighLevelShaders = true;

	MyEventReceiver receiver;

	// create device
	device = createDevice(driverType, core::dimension2d<u32>(WIDTH, HEIGHT), 
		32, false, false, false, &receiver);


	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* gui = device->getGUIEnvironment();

	io::path vsFileName; // filename for the vertex shader
	io::path psFileName; // filename for the pixel shader

	setupShader(device, UseHighLevelShaders, driver, driverType,
		psFileName, vsFileName);

	// add a nice skybox
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	smgr->addSkyDomeSceneNode(driver->getTexture("../media/Sky_horiz_3.jpg"));
	/*smgr->addSkyBoxSceneNode(
		driver->getTexture("../media/Yokohama2/posy.jpg"),
		driver->getTexture("../media/Yokohama2/negy.jpg"),
		driver->getTexture("../media/Yokohama2/negz.jpg"),
		driver->getTexture("../media/Yokohama2/posz.jpg"),
		driver->getTexture("../media/Yokohama2/negx.jpg"),
		driver->getTexture("../media/Yokohama2/posx.jpg"));
*/
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	//add a light source
	smgr->addLightSceneNode(0, core::vector3df(10 _METER, 10 _METER, 10 _METER), video::SColor(255, 255, 255, 255), 50 _METER);

	scene::ICameraSceneNode* cameras[2];
	cameras[0] = smgr->addCameraSceneNode();
	cameras[1] = smgr->addCameraSceneNodeFPS();

	receiver.setCameras(cameras, 2);
	

	// add other objects
	Quadrotor quadrotor(0.4 _METER, 0.7, 12000/60.f, 9.81f _METER, smgr->getRootSceneNode(), smgr, 1001);
	float speed[] = { 0.8f, 0.4f, 0.4f, 0.8f };
	quadrotor.setMotorSpeed(speed);

	PlatformNode* platform = new PlatformNode(20 _METER, 20 _METER,
		driver->getTexture("../media/wall.bmp"), smgr->getRootSceneNode(), smgr, 1000);

	smgr->addSphereSceneNode(20, 16, &quadrotor, -1, core::vector3df(100, 0, 0))->getMaterial(0).EmissiveColor = video::SColor(150, 255, 0, 0);
	smgr->addSphereSceneNode(20, 16, &quadrotor, -1, core::vector3df(0, 100, 0))->getMaterial(0).EmissiveColor = video::SColor(150, 0, 255, 0);
	smgr->addSphereSceneNode(20, 16, &quadrotor, -1, core::vector3df(0, 0, 100))->getMaterial(0).EmissiveColor = video::SColor(150, 0, 0, 255);
	// setup Graphs and GUI
	gui::IGUIFont* font = gui->getFont("../media/fonthaettenschweiler.bmp");

	Graph* motorGraphLin[4];
	FuzzyGraph* motorGraphFuzzy[4];
	for (int i = 0; i < 4; ++i) {
		int sizeWidth = 0.22*WIDTH, sizeHeight = 0.22*HEIGHT;
		int x = i % 2, y = i / 2;
		core::rect<s32> pos;
		pos.UpperLeftCorner = core::vector2d<s32>(x*(WIDTH - sizeWidth), y*(HEIGHT - sizeHeight - 1));
		pos.LowerRightCorner = core::vector2d<s32>(x*(WIDTH - sizeWidth) + sizeWidth, y*(HEIGHT - sizeHeight - 1) + sizeHeight);
		std::wstring caption = L"Motor ";
		caption += std::to_wstring(i);
		motorGraphLin[i] = new Graph(caption.c_str(), pos, 1.f, 2, 30, font);
		//motorGraphFuzzy[i] = FuzzyGraph(caption.c_str(), pos, 2, smgr, -1);
	}


	// Camera stuff
	smgr->getActiveCamera()->setPosition(core::vector3df(1 _METER, 1 _METER, 1 _METER));
	smgr->getActiveCamera()->setTarget(quadrotor.getAbsolutePosition());
	bool isCameraHeightFixed = false;
	bool isPaused = false, wasPaused = false;
	receiver.registerSwap('1', &isCameraHeightFixed);
	receiver.registerSwap(' ', &isPaused);

	bool showFuzzySets = false;
	receiver.registerSwap('F', &showFuzzySets);
	receiver.setQuadrotor(&quadrotor);


	int lastFPS = -1;
	float maxElapsedTimeMs = 1 / fpsMax * 1000.f;

	u32 now, then;
	now = device->getTimer()->getTime();
	u32 lastUpdate = 0;
	u32 timeWorld = 0;

	core::vector3df delayedPos, delayedRot, delayedSpeed, delayedRotSpeed;

	while (device->run())
		if (device->isWindowActive())
		{
			then = now;
			now = device->getTimer()->getTime();
			u32 elapsedTimeMs = now - then;
			f32 elapsedTime = elapsedTimeMs / 1000.f;


			// World updates
			if (!isPaused) {
				timeWorld += elapsedTimeMs;
				// Continuous updates
				quadrotor.update(elapsedTime);

				// Delayed updates
				if (now - lastUpdate > 150) {
					lastUpdate = now;
					for (int i = 0; i < 4; ++i) {
						motorGraphLin[i]->addVal(0, core::vector2df(timeWorld, quadrotor.getMotorSpeed(i)));
						motorGraphLin[i]->addVal(1, core::vector2df(timeWorld, quadrotor.getWantedMotorSpeed(i)));
					}

					delayedPos = quadrotor.getAbsolutePosition();
					delayedRot = quadrotor.getRotation();
					delayedSpeed = quadrotor.getSpeed();
					delayedRotSpeed = quadrotor.getAngularSpeed();
				}
			} 

			// Drawing stuff:
			// Update camera
			if (smgr->getActiveCamera() == cameras[0]) {

				cameras[0]->setPosition(quadrotor.getAbsolutePosition() + core::vector3df(1 _METER, 1 _METER, 1 _METER));
				cameras[0]->setTarget(quadrotor.getAbsolutePosition());
			} 
			else if (isCameraHeightFixed) {
				core::vector3df camPos = cameras[1]->getPosition();
				camPos.Y = quadrotor.getAbsolutePosition().Y + 1.5f _METER;
				cameras[1]->setPosition(camPos);
			}

			// Draw scene
			driver->beginScene(true, true, video::SColor(255, 0, 0, 0));
			smgr->drawAll();


			// Draw info graphics + text
			for (int i = 0; i < 4; ++i) {
				motorGraphLin[i]->render(driver);
			}
			wchar_t posStr[100], rotStr[100];
			swprintf(posStr, 100, L"Position: (%.2f, %.2f, %.2f),\tSpeed: (%.2f, %.2f, %.2f)", 
				delayedPos.X, delayedPos.Y, delayedPos.Z, delayedSpeed.X, delayedSpeed.Y, delayedSpeed.Z);
			swprintf(rotStr, 100, L"Rotation: (%.2f, %.2f, %.2f),\tAngularSpeed: (%.2f, %.2f, %.2f)", 
				delayedRot.X, delayedRot.Y, delayedRot.Z, delayedRotSpeed.X, delayedRotSpeed.Y, delayedRotSpeed.Z);
			font->draw(posStr, core::rect<s32>(WIDTH / 2 - 500, 0, WIDTH / 2 + 500, 30), video::SColor(255, 255, 255, 255), true, true);
			font->draw(rotStr, core::rect<s32>(WIDTH / 2 - 500, 20, WIDTH / 2 + 500, 50), video::SColor(255, 255, 255, 255), true, true);
				
			driver->endScene();

			int fps = driver->getFPS();
			
			if (lastFPS != fps) {
				core::stringw str = L"Irrlicht Engine - Quadrotor Controller [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;

				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}

			// cap FPS
			u32 restTime = maxElapsedTimeMs - (device->getTimer()->getTime() - now);
			if (restTime > 0) {
				_sleep(restTime);
			}
		}

	for (int i = 0; i < 4; ++i)
		delete motorGraphLin[i];
	smgr->drop();
	platform->drop();
	quadrotor.drop();
	device->drop();

	return 0;
}
