#include <irrlicht.h>
#include <iostream>
#include "driverChoice.h"
#include "ShaderSetup.h"
#include "MyEventReceiver.h"
#include "Quadcopter.h"
#include "PlatformNode.h"

using namespace irr;

#define _METER * 100

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif




IrrlichtDevice* device = 0;
bool UseHighLevelShaders = false;



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

	smgr->addSkyBoxSceneNode(
		driver->getTexture("../media/Yokohama2/posy.jpg"),
		driver->getTexture("../media/Yokohama2/negy.jpg"),
		driver->getTexture("../media/Yokohama2/negz.jpg"),
		driver->getTexture("../media/Yokohama2/posz.jpg"),
		driver->getTexture("../media/Yokohama2/negx.jpg"),
		driver->getTexture("../media/Yokohama2/posx.jpg"));

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// add a camera and disable the mouse cursor

	scene::ICameraSceneNode* cameras[2];
	cameras[0] = smgr->addCameraSceneNode();
	cameras[1] = smgr->addCameraSceneNodeFPS();

	smgr->getActiveCamera()->setPosition(core::vector3df(10, 0, 0));
	smgr->getActiveCamera()->setTarget(core::vector3df(0, 0, 0));
	//device->getCursorControl()->setVisible(false);


	receiver.UpdateEventReceiver(device, smgr, cameras, 2);
	

	// add other objects
	Quadcopter quadcopter(0.4 _METER, 0.7, 12000, 9.81 _METER, smgr->getRootSceneNode(), smgr, 1001);

	PlatformNode* platform = new PlatformNode(10 _METER, 10 _METER,
		driver->getTexture("../media/wall.bmp"),
		smgr->getRootSceneNode(), 
		smgr, 1000);

	int lastFPS = -1;
	u32 now, then;
	then = device->getTimer()->getTime();

	float speed []= { 0.01f, 0.01f, 0.01f, 0.01f };
	quadcopter.setMotorSpeed(speed);
	while (device->run())
		if (device->isWindowActive())
		{
			now = device->getTimer()->getTime();
			f64 elapsedTime = (now - then) / 1000.;
			then = now;

			quadcopter.update(elapsedTime);

			core::vector3df pos = quadcopter.getPosition();
			pos.X += 2 _METER;
			pos.Y += 2 _METER;
			pos.Z += 1.5 _METER;
			if (smgr->getActiveCamera() == cameras[0]) {
				cameras[0]->setPosition(pos);
				cameras[0]->setTarget(quadcopter.getPosition());

			}

			driver->beginScene(true, true, video::SColor(255, 0, 0, 0));
			smgr->drawAll();
			driver->endScene();

			
			int fps = driver->getFPS();

			if (lastFPS != fps)
			{
				core::stringw str = L"Irrlicht Engine - Vertex and pixel shader example [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;

				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		}

	platform->drop();
	device->drop();

	return 0;
}
