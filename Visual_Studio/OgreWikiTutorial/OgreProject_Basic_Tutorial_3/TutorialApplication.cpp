/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#include "TutorialApplication.h"

//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void): mInfoLabel(0),mTerrainGroup(0),mTerrainGlobals(0)
{
	
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//---------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	//设定相机位置
	mCamera->setPosition(Ogre::Vector3(1683,50,2116));
	mCamera->lookAt(Ogre::Vector3(1963,50,1660));
	mCamera->setNearClipDistance(0.1);
	//判断渲染系统是否存在无穷远的裁剪面
	bool infiniteClip = mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE);
	
	if(infiniteClip)
	{
		mCamera->setFarClipDistance(0);
	}
	else
	{
		mCamera->setFarClipDistance(50000);
	}
	//设置环境光
	mSceneMgr->setAmbientLight(Ogre::ColourValue(.2,.2,.2));
	//设置方向光
	Ogre::Vector3 lightdir(.55,-.3,.75);
	lightdir.normalise();
	Ogre::Light* light = mSceneMgr->createLight("TestLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(lightdir);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue(1,0,0));
	//设置地形
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr,Ogre::Terrain::ALIGN_X_Z,513,12000.0);
	mTerrainGroup->setFilenameConvention(Ogre::String("terrain"),Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
	configureTerrainDefaults(light);
	for (int x=0;x<=0;++x)//填充地形
	{
		for (int y=0;y<=0;++y)
		{
			defineTerrain(x,y);
		}
	}
	mTerrainGroup->loadAllTerrains(true);//加载地形

	if (mTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();

		while (ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}
	mTerrainGroup->freeTemporaryResources();
	/*mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300);
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);*/

	// Sky Techniques
	// mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	// Ogre::Plane plane;
	// plane.d = 1000;
	// plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;

	// mSceneMgr->setSkyPlane(
	//   true, plane, "Examples/SpaceSkyPlane", 1500, 40, true, 1.5, 150, 150);
}

void TutorialApplication::createFrameListener()
{
	BaseApplication::createFrameListener();
	mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TerrainInfo", "", 350);

}

void TutorialApplication::destroyScene()
{
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobals;
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
	bool ret =BaseApplication::frameRenderingQueued(fe);
	if (mTerrainGroup->isDerivedDataUpdateInProgress())
	{
		mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
		mInfoLabel->show();

		if (mTerrainsImported)
			mInfoLabel->setCaption("Building terrain...");
		else   
			mInfoLabel->setCaption("Updating terrain...");
	}
	else
	{
		mTrayMgr->removeWidgetFromTray(mInfoLabel);
		mInfoLabel->hide();

		if (mTerrainsImported)
		{
			mTerrainGroup->saveAllTerrains(true);
			mTerrainsImported = false;
		}
	}
	return ret;
}

void TutorialApplication::defineTerrain(long x,long y)
{
	Ogre::String filename = mTerrainGroup->generateFilename(x,y);
	bool exists = Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(),filename);
	if(exists)
	{
		mTerrainGroup->defineTerrain(x,y);
	}
	else
	{
		Ogre::Image img;
		getTerrainImage(x%2!=0,y%2!=0,img);
		mTerrainGroup->defineTerrain(x,y,&img);
		mTerrainsImported=true;
	}
}

void TutorialApplication::getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("terrain.png",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if(flipX)
	{
		img.flipAroundY();
	}
	if(flipY)
	{
		img.flipAroundX();
	}
}

void TutorialApplication::initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::Real minHeight0=70;
	Ogre::Real fadeDist0=40;
	Ogre::Real minHeight1=70;
	Ogre::Real fadeDist1=15;

	Ogre::TerrainLayerBlendMap* blendMap0=terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1=terrain->getLayerBlendMap(2);

	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1=blendMap1->getBlendPointer();

	for(Ogre::uint16 y=0;y<terrain->getLayerBlendMapSize();++y)
	{
		for (Ogre::uint16 x=0;x<terrain->getLayerBlendMapSize();++x)
		{
			Ogre::Real tx,ty;

			blendMap0->convertImageToTerrainSpace(x,y,&tx,&ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx,ty);
			Ogre::Real val = (height-minHeight0)/fadeDist0;
			val=Ogre::Math::Clamp(val,(Ogre::Real)0,(Ogre::Real)1);
			*pBlend0++=val;

			val=(height-minHeight1)/fadeDist1;
			val=Ogre::Math::Clamp(val,(Ogre::Real)0,(Ogre::Real)1);
			*pBlend1++=val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

void TutorialApplication::configureTerrainDefaults(Ogre::Light* light)
{
	//真实地形和我们创建的mesh允许的最大误差，值越小意味着更多的顶点，mesh约精细
	mTerrainGlobals->setMaxPixelError(8);
	//设置混合映射距离，值越大，光照影像的距离越远
	mTerrainGlobals->setCompositeMapDistance(3000);
	//设置光照映射的方向
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	//设置映射的环境光
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	//设置映射的漫反射颜色
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
	importData.terrainSize=513;
	importData.worldSize = 12000.0;
	importData.inputScale=600;
	importData.minBatchSize =33;
	importData.maxBatchSize = 65;

	//加入纹理
	importData.layerList.resize(3);
	//每一个图层都有两个纹理，分别是灰度图和纹理图片
	importData.layerList[0].worldSize=100;
	importData.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	importData.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	importData.layerList[1].worldSize=30;
	importData.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	importData.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	importData.layerList[2].worldSize=200;
	importData.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	importData.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

}


//---------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch(Ogre::Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
