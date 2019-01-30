#include <irrlicht.h>
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include "driverChoice.h"
#include "ShaderSetup.h"
#include "MyEventReceiver.h"
#include "Quadrotor.h"
#include "PlatformNode.h"
#include "Graph.h"
#include "FuzzyGraph.h"

#include "FuzzyPDController.h"
#include "QuadrotorController.h"
#include "QuadrotorTrajectoryController.h"

using namespace irr;

#define _METER *100

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


void drawCoordinateSystem(Quadrotor* quadrotor, video::IVideoDriver *driver);

IrrlichtDevice* device = 0;
bool UseHighLevelShaders = false;
float fpsMax = 200;

int gScreenWidth = 1366, gScreenHeight = 740;

int main(int argc, char **argv)
{
	if (argc > 1) {
		gScreenWidth = atoi(argv[1]);
		gScreenHeight = atoi(argv[2]);
	}
	// ask user for driver
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;// driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	UseHighLevelShaders = true;

	MyEventReceiver receiver;

	// create device
	device = createDevice(driverType, core::dimension2d<u32>(gScreenWidth, gScreenHeight),
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
	//smgr->addSkyDomeSceneNode(driver->getTexture("../media/Sky_horiz_3.jpg"));
	//TODO: Skybox löst beim Beenden Reference-Error aus !?
	smgr->addSkyBoxSceneNode(
		driver->getTexture("../media/irrlicht2_up.jpg"),
		driver->getTexture("../media/irrlicht2_dn.jpg"),
		driver->getTexture("../media/irrlicht2_lf.jpg"),
		driver->getTexture("../media/irrlicht2_rt.jpg"),
		driver->getTexture("../media/irrlicht2_ft.jpg"),
		driver->getTexture("../media/irrlicht2_bk.jpg"));

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	scene::ICameraSceneNode* cameras[2];
	cameras[0] = smgr->addCameraSceneNode();
	cameras[1] = smgr->addCameraSceneNodeFPS();

	receiver.setCameras(cameras, 2);


	// add other objects
	Quadrotor quadrotor(0.4 _METER, 0.7f, 12000 / 60.f, 9.81f _METER, smgr->getRootSceneNode(), smgr, 1001);
	float speed[] = { 0.01f, 0.01f, 0.01f, 0.01f };
	quadrotor.setMotorSpeed(speed);

	QuadrotorController quadrotorControllerPD(PDController(1, .8f), PDController(1, .1f, .05f), PDController(1, .1f, .2f), &quadrotor);
	QuadrotorTrajectoryController trajectoryController(&quadrotorControllerPD, &quadrotor);
	receiver.setTrajectoryController(&trajectoryController);

	PlatformNode* platform = new PlatformNode(20 _METER, 20 _METER,
		driver->getTexture("../media/wall.bmp"), smgr->getRootSceneNode(), smgr, 1000);

	// add a light source
	scene::ILightSceneNode* light = smgr->addLightSceneNode(0, core::vector3df(1000 _METER, 1000 _METER, 1000 _METER),
		video::SColor(255, 255, 255, 255), 10000 _METER);

	//smgr->addSphereSceneNode(20, 16, &quadrotor, -1, core::vector3df(100, 0, 0))->getMaterial(0).EmissiveColor = video::SColor(150, 255, 0, 0);
	//smgr->addSphereSceneNode(20, 16, &quadrotor, -1, core::vector3df(0, 100, 0))->getMaterial(0).EmissiveColor = video::SColor(150, 0, 255, 0);
	//smgr->addSphereSceneNode(20, 16, &quadrotor, -1, core::vector3df(0, 0, 100))->getMaterial(0).EmissiveColor = video::SColor(150, 0, 0, 255);
	// setup Graphs and GUI
	gui::IGUIFont* font = gui->getFont("../media/fonthaettenschweiler.bmp");

	Graph* motorGraphLin[4];
	FuzzyGraph* motorGraphFuzzy[4];
	for (int i = 0; i < 4; ++i) {
		int sizeWidth = (int)(0.25*gScreenWidth), sizeHeight = (int)(0.25*gScreenHeight);
		int x = i % 2, y = i / 2;
		core::rect<s32> pos;
		pos.UpperLeftCorner = core::vector2d<s32>(x*(gScreenWidth - sizeWidth), y*(gScreenHeight - sizeHeight - 1));
		pos.LowerRightCorner = core::vector2d<s32>(x*(gScreenWidth - sizeWidth) + sizeWidth, y*(gScreenHeight - sizeHeight - 1) + sizeHeight);
		std::wstring caption = L"Motor ";
		caption += std::to_wstring(i);
		motorGraphLin[i] = new Graph(caption.c_str(), pos, 1.f, 0.f, 2, 30, font);
		//motorGraphFuzzy[i] = FuzzyGraph(caption.c_str(), pos, 2, smgr, -1);
	}

	Graph* quadrotorGraph[4];
	quadrotorGraph[0] = new Graph(L"Height", core::rect<s32>(0, 0.252*gScreenHeight, 0.25*gScreenWidth, 0.5*gScreenHeight),
		50.f _METER, 0.f, 2, 30, font);
	quadrotorGraph[1] = new Graph(L"Roll", core::rect<s32>(0.75*gScreenWidth, 0.252*gScreenHeight, gScreenWidth, 0.5*gScreenHeight),
		180.f, -180.f, 2, 30, font);
	quadrotorGraph[2] = new Graph(L"Yaw", core::rect<s32>(0, 0.502*gScreenHeight, 0.25*gScreenWidth, 0.748*gScreenHeight),
		180.f, -180.f, 2, 30, font);
	quadrotorGraph[3] = new Graph(L"Pitch", core::rect<s32>(0.75*gScreenWidth, 0.502*gScreenHeight, gScreenWidth, 0.748*gScreenHeight),
		180.f, -180.f, 2, 30, font);




	// Camera stuff
	smgr->setActiveCamera(cameras[0]);
	cameras[0]->setFarValue(10000 _METER);
	cameras[1]->setFarValue(10000 _METER);
	cameras[0]->setParent(&quadrotor);
	cameras[0]->setPosition(core::vector3df(150, 200, 150));
	cameras[0]->setTarget(quadrotor.getAbsolutePosition());
	cameras[1]->setPosition(core::vector3df(150, 200, 150));

	bool isCameraTargetFixed = true, isPaused = false, drawCoordSys = false;
	receiver.registerSwap('l', &isCameraTargetFixed);
	receiver.registerSwap(' ', &isPaused);
	receiver.registerSwap('c', &drawCoordSys);

	bool showFuzzySets = false;
	receiver.registerSwap('f', &showFuzzySets);
	receiver.setQuadrotor(&quadrotor);


	u32 lastFPS = -1;
	u32 maxElapsedTimeMs = (u32)round(1000.f / fpsMax);

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
			
				trajectoryController.update(elapsedTime);
				quadrotor.update(elapsedTime);

				// Delayed updates
				if (now - lastUpdate > 150) {
					lastUpdate = now;
					for (int i = 0; i < 4; ++i) {
						motorGraphLin[i]->addVal(0, core::vector2df((f32)timeWorld, quadrotor.getMotorSpeed(i)));
						motorGraphLin[i]->addVal(1, core::vector2df((f32)timeWorld, quadrotor.getWantedMotorSpeed(i)));
					}

					float quadrotorRot[3];
					quadrotor.getRotation().getAs3Values(quadrotorRot);
					const float *const trajectoryParams = trajectoryController.getParams();

					quadrotorGraph[0]->addVal(0, core::vector2df((f32)timeWorld, quadrotor.getAbsolutePosition().Y));
					if (trajectoryController.getTrajectory() != QT_NONE)
						quadrotorGraph[0]->addVal(1, core::vector2df((f32)timeWorld, trajectoryParams[0]));
					for (int i = 0; i < 3; ++i) {
						quadrotorRot[i] -= 360 * (int)(quadrotorRot[i] / 360);
						if (fabs(quadrotorRot[i]) > 180)
							quadrotorRot[i] = (quadrotorRot[i] > 0 ? -360 : 360) + quadrotorRot[i];
						quadrotorGraph[i+1]->addVal(0, core::vector2df((f32)timeWorld, quadrotorRot[i]));
						if (trajectoryController.getTrajectory() != QT_NONE)
							quadrotorGraph[i+1]->addVal(1, core::vector2df((f32)timeWorld, trajectoryParams[i+1]));
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
				cameras[0]->setTarget(quadrotor.getAbsolutePosition());
			}
			else if (isCameraTargetFixed) {
				cameras[1]->setTarget(quadrotor.getAbsolutePosition());
				/*core::vector3df camPos = cameras[1]->getPosition();
				camPos.Y = quadrotor.getAbsolutePosition().Y + 1.5f _METER;
				cameras[1]->setPosition(camPos);
				cameras[1]->updateAbsolutePosition();*/
			}
			// Draw scene
			driver->beginScene(true, true, video::SColor(255, 0, 0, 0));
			smgr->drawAll();

			if (drawCoordSys)
				drawCoordinateSystem(&quadrotor, driver);

			// Draw info graphics + text
			for (int i = 0; i < 4; ++i) {
				motorGraphLin[i]->render(driver);
				quadrotorGraph[i]->render(driver);
			}
			wchar_t posStr[100], rotStr[100];
			swprintf(posStr, 100, L"Position: (%.2f, %.2f, %.2f),\tSpeed: (%.2f, %.2f, %.2f)", 
				delayedPos.X, delayedPos.Y, delayedPos.Z, delayedSpeed.X, delayedSpeed.Y, delayedSpeed.Z);
			swprintf(rotStr, 100, L"Rotation: (%.2f, %.2f, %.2f),\tAngularSpeed: (%.2f, %.2f, %.2f)", 
				delayedRot.X, delayedRot.Y, delayedRot.Z, delayedRotSpeed.X, delayedRotSpeed.Y, delayedRotSpeed.Z);
			font->draw(posStr, core::rect<s32>(gScreenWidth / 2 - 500, 0, gScreenWidth / 2 + 500, 30), video::SColor(255, 255, 255, 255), true, true);
			font->draw(rotStr, core::rect<s32>(gScreenWidth / 2 - 500, 20, gScreenWidth / 2 + 500, 50), video::SColor(255, 255, 255, 255), true, true);

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
				Sleep(restTime);
			}
			
		}
	
	for (int i = 0; i < 4; ++i)
		delete motorGraphLin[i];
	smgr->drop();
	platform->drop();
	device->drop();
	return 0;
}

void drawCoordinateSystem(Quadrotor* quadrotor, video::IVideoDriver *driver) {
	// TODO: Gewünschte Farbe wird nicht gemalt
	video::SColor color;
	core::vector3df startPos(0, 0, 0), endPos;
	video::SMaterial material;
	material.Lighting = false;
	driver->setMaterial(material);
	driver->setTransform(video::ETS_WORLD, quadrotor->getAbsoluteTransformation());
	for (int i = 0; i < 3; ++i) {
		color.set(255, i == 0 ? 255 : 0, i == 1 ? 255 : 0, i == 2 ? 255 : 0);
 		endPos.set(i == 0 ? 1.f _METER : 0.f, i == 1 ? 1.f _METER : 0.f, i == 2 ? 1.f _METER : 0.f);
		driver->draw3DLine(startPos, endPos, color);
	}
}