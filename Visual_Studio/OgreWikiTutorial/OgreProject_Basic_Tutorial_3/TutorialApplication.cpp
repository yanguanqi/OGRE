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
	//�趨���λ��
	mCamera->setPosition(Ogre::Vector3(1683,50,2116));
	mCamera->lookAt(Ogre::Vector3(1963,50,1660));
	mCamera->setNearClipDistance(0.1);
	//�ж���Ⱦϵͳ�Ƿ��������Զ�Ĳü���
	bool infiniteClip = mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE);
	
	if(infiniteClip)
	{
		mCamera->setFarClipDistance(0);
	}
	else
	{
		mCamera->setFarClipDistance(50000);
	}
	//���û�����
	mSceneMgr->setAmbientLight(Ogre::ColourValue(.2,.2,.2));
	//���÷����
	Ogre::Vector3 lightdir(.55,-.3,.75);
	lightdir.normalise();
	Ogre::Light* light = mSceneMgr->createLight("TestLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(lightdir);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue(1,0,0));
	//���õ���
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr,Ogre::Terrain::ALIGN_X_Z,513,12000.0);
	mTerrainGroup->setFilenameConvention(Ogre::String("terrain"),Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
	configureTerrainDefaults(light);
	for (int x=0;x<=0;++x)//������
	{
		for (int y=0;y<=0;++y)
		{
			defineTerrain(x,y);
		}
	}
	mTerrainGroup->loadAllTerrains(true);//���ص���

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
	//��ʵ���κ����Ǵ�����mesh����������ֵԽС��ζ�Ÿ���Ķ��㣬meshԼ��ϸ
	mTerrainGlobals->setMaxPixelError(8);
	//���û��ӳ����룬ֵԽ�󣬹���Ӱ��ľ���ԽԶ
	mTerrainGlobals->setCompositeMapDistance(3000);
	//���ù���ӳ��ķ���
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	//����ӳ��Ļ�����
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	//����ӳ�����������ɫ
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
	importData.terrainSize=513;
	importData.worldSize = 12000.0;
	importData.inputScale=600;
	importData.minBatchSize =33;
	importData.maxBatchSize = 65;

	//��������
	importData.layerList.resize(3);
	//ÿһ��ͼ�㶼�����������ֱ��ǻҶ�ͼ������ͼƬ
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
