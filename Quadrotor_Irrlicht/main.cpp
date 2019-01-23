#include <irrlicht.h>
#include <iostream>
#include <vector>
#include "driverChoice.h"
#include "ShaderSetup.h"
#include "MyEventReceiver.h"
#include "Quadrotor.h"
#include "PlatformNode.h"
#include "Graph.h"
#include "FuzzyGraph.h"
using namespace irr;

#define _METER * 100

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif




IrrlichtDevice* device = 0;
bool UseHighLevelShaders = false;
float fpsMax = 200;


int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL; // driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	UseHighLevelShaders = true;

	MyEventReceiver receiver;

	// create device
	device = createDevice(driverType, core::dimension2d<u32>(1024, 768), 
		32, false, false, true, &receiver);

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
	smgr->addLightSceneNode(0, core::vector3df(10 _METER, 10 _METER, 10 _METER), video::SColor(255, 255, 255, 255), 20 _METER);

	scene::ICameraSceneNode* cameras[2];
	cameras[0] = smgr->addCameraSceneNode();
	cameras[1] = smgr->addCameraSceneNodeFPS();

	receiver.setCameras(cameras, 2);
	

	// add other objects
	Quadrotor quadrotor(0.4 _METER, 0.7, 12000/60.f, 9.81f _METER, smgr->getRootSceneNode(), smgr, 1001);
	float speed[] = { 0.8f, 0.7f, 0.701f, 0.8f };
	quadrotor.setMotorSpeed(speed);

	PlatformNode* platform = new PlatformNode(20 _METER, 20 _METER,
		driver->getTexture("../media/wall.bmp"), smgr->getRootSceneNode(), smgr, 1000);

	// setup Graphs and GUI
	std::vector<Graph> motorGraphLin;
	std::vector<FuzzyGraph> motorGraphFuzzy;


	// Camera stuff
	smgr->getActiveCamera()->setPosition(core::vector3df(1 _METER, 1 _METER, 1 _METER));
	smgr->getActiveCamera()->setTarget(quadrotor.getAbsolutePosition());
	bool isCameraHeightFixed = false;
	bool isPaused = false;
	receiver.registerSwap('1', &isCameraHeightFixed);
	receiver.registerSwap(' ', &isPaused);

	int lastFPS = -1;
	u32 now, then;
	then = device->getTimer()->getTime();

	while (device->run())
		if (device->isWindowActive())
		{
			now = device->getTimer()->getTime();
			f64 elapsedTime = (now - then) / 1000.;
			then = now;

			if (!isPaused)
				quadrotor.update(elapsedTime);

			core::vector3df pos = quadrotor.getPosition();
			pos.X += 1 _METER;
			pos.Y += 1 _METER;
			pos.Z += 0.5 _METER;
			if (smgr->getActiveCamera() == cameras[0]) {
				cameras[0]->setPosition(pos);
				cameras[0]->setTarget(quadrotor.getAbsolutePosition());
			}
			else if (isCameraHeightFixed) {
				core::vector3df camPos = cameras[1]->getPosition();
				camPos.Y = pos.Y + 0.5 _METER;
				cameras[1]->setPosition(camPos);
			}

			driver->beginScene(true, true, video::SColor(255, 0, 0, 0));
			smgr->drawAll();

			driver->endScene();

			
			int fps = driver->getFPS();

			if (lastFPS != fps)
			{
				core::stringw str = L"Irrlicht Engine - Quadrotor Controller [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;

				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}

			// cap FPS
			now = device->getTimer()->getTime();
			float elapsedTimeMs = (now - then);
			float maxElapsedTimeMs = 1 / fpsMax * 1000.f;
			if (elapsedTimeMs < maxElapsedTimeMs) {
				_sleep(maxElapsedTimeMs - elapsedTimeMs);
			}
		}

	platform->drop();
	device->drop();

	return 0;
}
