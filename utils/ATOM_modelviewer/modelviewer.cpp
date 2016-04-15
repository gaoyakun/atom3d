#define WIN32_LEAN_AND_MEAN
#include <windows.h>  
#include <commdlg.h>
#include <stdio.h>
#include <crtdbg.h>   
#include <signal.h>                     
#include <psapi.h>            
#include <direct.h>      
#include <ATOM_dbghlp.h>
#include <ATOM_engine.h> 
#include <ATOM_commondlg.h>
#include <ATOM_kernel.h>  
#include <ATOM_render.h>
#include <ATOM_image.h>   
#include <ATOM_ttfont.h>      
#include <ATOM_geometry.h>
#include <ATOM_utils.h>
#include <ATOM_vfs.h>
#include <ATOM_archive.h>
#include <ATOM3DX.h>
#include <engine/deferredscene.h>
#include <engine/atmosphere.h>

#include "trackball.h"
#include "modelio.h"

#define WINDOWWIDTH 800
#define WINDOWHEIGHT 600

static ATOM_AUTOREF(ATOM_Water) gWater;
static bool threadLoading = false;

enum
{
	MenuId_Load,
	MenuId_Reload,
	MenuId_Save,
	MenuId_SaveAs,
	MenuId_DumpMemState,
	MenuId_ToggleWireframe,
	MenuId_ChangeAmbient,
	MenuId_EditModel,
	MenuId_ToggleBoundingbox,
	MenuId_AddComponent,
	MenuId_Exit,
	MenuId_Screenshot,
	MenuId_DebugDeferredNormals,
	MenuId_About,
	MenuId_TimeScale_32x,
	MenuId_TimeScale_16x,
	MenuId_TimeScale_8x,
	MenuId_TimeScale_4x,
	MenuId_TimeScale_2x,
	MenuId_TimeScale_1x,
	MenuId_TimeScale_1_2x,
	MenuId_TimeScale_1_4x,
	MenuId_TimeScale_1_8x,
	MenuId_TimeScale_1_16x,
	MenuId_TimeScale_1_32x,
	MenuId_EnablePPL,
	MenuId_TestParticleSystem,
	MenuId_TestChar,
	MenuId_TestHud,
	MenuId_TestInstance,
	MenuId_TestAttachment,
	MenuId_TestTerrain,
	MenuId_TestWater,
	MenuId_TestWaterBug,
	MenuId_TestActor,
	MenuId_TestMtLoad,
	MenuId_TestDeferredShading,
	MenuId_TestAtmosphere,
	MenuId_DiscardLoading,
	MenuId_ChangeColor,
	MenuId_ConfigFog,
	MenuId_ConfigMaterial,
	MenuId_PostEffectClear,
	MenuId_PostEffectGray,
	MenuId_PostEffectBloom,
	MenuId_PostEffectGamma,
	MenuId_ToggleShadow,
	ButtonId_Ok,
	ButtonId_Cancel,
	ButtonId_FogColor,
	ButtonId_EnableFog,
	SliderId_Near,
	SliderId_Far,
};

static const int MenuId_ActionListStart = 500;

class MyEmitter: public ATOM_ParticleEmitter
{
	ATOM_CLASS(test, MyEmitter, MyEmitter)

public:
	virtual void initParticle (ATOM_ParticleSystem *ps, ATOM_Particle *particle, unsigned emitTime)
	{
		particle->position.set (0.f, 0.f, 0.f);
		
		float z = -1.f + 2.f * float(rand())/float(RAND_MAX);
		float radius = ATOM_sqrt (1.f - z * z);
		float t = -ATOM_Pi + 2.f * ATOM_Pi * float(rand())/float(RAND_MAX);
		float s, c;
		ATOM_sincos (t, &s, &c);
		float x = c * radius;
		float y = s * radius;
		particle->velocity.set (x, y, z);
		particle->velocity *= 0.5f;

		particle->size1 = 0.1f;
		particle->size2 = 0.2f;
		particle->lifeSpan = 8000;
		particle->acceleration = 0.f;
	}
};

class RealtimeEventTrigger;

class MyView
  {
  public:
	MyView (void);
	~MyView (void);
    void render (void);
    void initGUI (void);
    bool initFont (void);

  public:
	ATOM_GUIFont::handle _font;
    ATOM_GUIRenderer *_gui;
	ATOM_DeferredScene _scene;
	ATOM_PopupMenu *_actionMenu;
	RealtimeEventTrigger *_rttrigger;
	ATOM_RealtimeCtrl *_realtimeCtrl;
	ATOM_AUTOREF(ATOM_Node) _rootNode;
	ATOM_AUTOREF(ATOM_RenderWindow) _window;

  public:
    void onUnhandledEvent (ATOM_Event *event);
	void onWindowClose (ATOM_WindowCloseEvent *event);
	void onIdle (ATOM_AppIdleEvent *event);
	void onWMEvent (ATOM_AppWMEvent *event);

    ATOM_DECLARE_EVENT_MAP_NOPARENT(MyView)
  };

ATOM_BEGIN_EVENT_MAP_NOPARENT(MyView)
	ATOM_EVENT_HANDLER(MyView, ATOM_AppIdleEvent, onIdle)
	ATOM_EVENT_HANDLER(MyView, ATOM_WindowCloseEvent, onWindowClose)
	ATOM_EVENT_HANDLER(MyView, ATOM_AppWMEvent, onWMEvent)
	ATOM_UNHANDLED_EVENT_HANDLER(MyView, onUnhandledEvent)
ATOM_END_EVENT_MAP

MyView::MyView (void)
: _actionMenu(0)
, _rttrigger(0)
, _realtimeCtrl(0)
, _gui(0)
{
	//_scene.enabelHDR (true);
	//_scene.getLight()->setColor4f (ATOM_Vector4f(100.f, 100.f, 100.f, 1.f));
	_scene.getLight()->setDirection (ATOM_Vector3f(0.f, 0.5f, -10.f));
}

void MyView::render (void)
{
	ATOM_RenderDevice *device = ATOM_GetRenderDevice ();
	_gui->render ();
}

bool MyView::initFont (void)
{
	_font = ATOM_GUIFont::createFont (ATOM_GUIFont::default_font_name, 16, ATOM_CC_CP936, 0);
    return true;
}

void MyView::onUnhandledEvent (ATOM_Event *event)
{
	if (_gui)
	{
		_gui->handleEvent (event);
	}
}

void MyView::onWindowClose (ATOM_WindowCloseEvent *event)
{
  ATOM_GetApplication()->postQuitEvent (0);
}

void MyView::onIdle (ATOM_AppIdleEvent *event)
{
	_gui->handleEvent (event);
}

void MyView::onWMEvent (ATOM_AppWMEvent *event)
{
	if (_window && event->hWnd == (HWND)_window->getWindowId ())
	{
		if (event->msg == WM_PAINT)
		{
			//render ();
			//event->eat = false;
			//event->returnValue = 0;
		}
		else if (event->msg == WM_ERASEBKGND)
		{
			event->eat = true;
			event->returnValue = 1;
		}
		else
		{
			event->eat = false;
		}
	}
	else
	{
		event->eat = false;
	}
}

class FogDialogTrigger: public ATOM_EventTrigger
{
public:
	bool enableFog;
	float fogFar;
	float fogNear;
	ATOM_ColorARGB fogColor;
	ATOM_Scene *scene;

public:
	FogDialogTrigger (ATOM_Scene *s)
		: enableFog(s->isFogEnabled())
		, fogFar(s->getFogEnd())
		, fogNear(s->getFogStart())
		, fogColor(s->getFogColor())
		, scene(s)
	{
	}

	void onInit (ATOM_DialogInitEvent *event)
	{
		ATOM_Dialog *dlg = (ATOM_Dialog*)getHost();

		//ATOM_Checkbox *enabled = (ATOM_Checkbox*)dlg->getChildById (ButtonId_EnableFog);
		//enabled->setChecked (enableFog);

		//ATOM_ScrollBar *sliderNear = (ATOM_ScrollBar*)dlg->getChildById (SliderId_Near);
		//sliderNear->setPosition (fogNear);

		//ATOM_ScrollBar *sliderFar = (ATOM_ScrollBar*)dlg->getChildById (SliderId_Far);
		//sliderFar->setPosition (fogFar);
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		ATOM_Dialog *dlg = (ATOM_Dialog*)getHost();

		switch (event->id)
		{
		case ButtonId_Ok:
			{
				dlg->endModal (ButtonId_Ok);
				break;
			}
		case ButtonId_Cancel:
			{
				dlg->endModal (ButtonId_Cancel);
				break;
			}
		case ButtonId_EnableFog:
			{
				ATOM_Checkbox *chbx = (ATOM_Checkbox*)dlg->getChildById(ButtonId_EnableFog);
				scene->enableFog (chbx->getChecked ());
				break;
			}
		case ButtonId_FogColor:
			{
				static COLORREF custColors[16] = { 0 };
				CHOOSECOLOR cc;
				memset (&cc, 0, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = GetActiveWindow ();
				cc.Flags = CC_RGBINIT|CC_FULLOPEN|CC_ANYCOLOR;
				cc.rgbResult = RGB(scene->getFogColor().getByteR(), scene->getFogColor().getByteG(), scene->getFogColor().getByteB());
				cc.lpCustColors = (LPDWORD)custColors;
				if (::ChooseColor (&cc))
				{
					float r = GetRValue(cc.rgbResult) / 255.f;
					float g = GetGValue(cc.rgbResult) / 255.f;
					float b = GetBValue(cc.rgbResult) / 255.f;
					scene->setFogColor (ATOM_ColorARGB(r, g, b, 1));
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	void onScroll (ATOM_ScrollEvent *event)
	{
		ATOM_Dialog *dlg = (ATOM_Dialog*)getHost();

		switch (event->id)
		{
		case SliderId_Near:
			{
				ATOM_ScrollBar *sliderNear = (ATOM_ScrollBar*)dlg->getChildById (SliderId_Near);
				scene->setFogStart (sliderNear->getPosition ());
				break;
			}
		case SliderId_Far:
			{
				ATOM_ScrollBar *sliderFar = (ATOM_ScrollBar *)dlg->getChildById (SliderId_Far);
				scene->setFogEnd (sliderFar->getPosition());
				break;
			}
		default:
			{
				break;
			}
		}
	}

	void onClose (ATOM_TopWindowCloseEvent *event)
	{
		ATOM_Dialog *dlg = (ATOM_Dialog*)getHost();
		dlg->endModal (ButtonId_Cancel);
	}

	ATOM_DECLARE_EVENT_MAP(FogDialogTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(FogDialogTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(FogDialogTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(FogDialogTrigger, ATOM_DialogInitEvent, onInit)
	ATOM_EVENT_HANDLER(FogDialogTrigger, ATOM_ScrollEvent, onScroll)
	ATOM_EVENT_HANDLER(FogDialogTrigger, ATOM_TopWindowCloseEvent, onClose)
ATOM_END_EVENT_MAP

class RealtimeEventTrigger: public ATOM_EventTrigger
{
public:
	MyView *_view;
	float _viewDistance;
	Trackball _trackball;
	ATOM_Vector3f _translation;
	ATOM_STRING _currentModel;
	int mousex, mousey;
	ATOM_AUTOREF(ATOM_Geode) _loadedModel;
	ATOM_AUTOREF(ATOM_Model) _model;
	ModelIO _modelio;
	ATOM_AUTOREF(ATOM_GuiHud) _guiHud;
	float _guiHudPos[2];
	ATOM_Geode::ActionId _currentAction;
	int _currentActionIndex;
	ATOM_STRING _tmpfilename;

public:
	RealtimeEventTrigger (MyView *view): _view(view), _trackball(_view->_window->getWindowWidth(), _view->_window->getWindowHeight())
	{
		_viewDistance = 10.f;
		_currentAction = ATOM_Geode::invalid_actionId;
		_currentActionIndex = -1;
		_translation.set(0, 0, 0);
		char *s = _tempnam (0, "n3mv$");
		_tmpfilename = s;
		free (s);
	}

	void resetCamera (ATOM_Camera *camera, unsigned w, unsigned h)
	{
		camera->setPerspective (1.f, w/h, 1.f, 20000000.f);
		ATOM_Matrix4x4f viewMatrix;
		ATOM_Node *rootNode = _view->_rootNode.get();
		if (!rootNode)
		{
			viewMatrix.makeLookatLH(ATOM_Vector3f(0, -100, 100), ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
		}
		else
		{
			const ATOM_BBox &bbox = rootNode->getBoundingbox ();
			viewMatrix.makeLookatLH(ATOM_Vector3f(0, -5, -_viewDistance-bbox.getExtents().getLength ()), ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			rootNode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(_translation));
		}
		camera->setViewMatrix (viewMatrix);
	}

	void resetView (ATOM_RealtimeCtrl *rc)
	{
		ATOM_Camera *camera = rc->getCamera ();
		float w = rc->getClientRect().w;
		float h = rc->getClientRect().h;
		rc->setPerspective (w/h, 1.f, 20000000.f);

		ATOM_Matrix4x4f viewMatrix;
		ATOM_Node *rootNode = _view->_rootNode.get();
		if (!rootNode)
		{
			viewMatrix.makeLookatLH(ATOM_Vector3f(0, -100, 100), ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
		}
		else
		{
			const ATOM_BBox &bbox = rootNode->getBoundingbox ();
			viewMatrix.makeLookatLH(ATOM_Vector3f(0, -5, -_viewDistance-bbox.getExtents().getLength ()), ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			rootNode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix(_translation));
		}
		camera->setViewMatrix (viewMatrix);
		_trackball.reset ();
		
	}

	bool loadFile (const char *szBuffer, bool replace)
	{
#if 0
				ATOM_HARDREF(ATOM_Geode) geode;
				geode->addComponents ("/zy/zj/G_animation.nm");
#if 0
				geode->addComponents ("/zy/zj/Gp/Gp_mesh_k.nm");
				geode->addComponents ("/zy/zj/Gp/Gp_mesh_s.nm");
				geode->addComponents ("/zy/zj/Gp/Gp_mesh_t.nm");
				geode->addComponents ("/zy/zj/Gp/Gp_mesh_x.nm");
				geode->addComponents ("/zy/zj/Gp/Gp_mesh_y.nm");
#else
				geode->addComponents ("/zy/zj/G_054/G054_mesh_k.nm");
				geode->addComponents ("/zy/zj/G_054/G054_mesh_s.nm");
				geode->addComponents ("/zy/zj/G_054/G054_mesh_t.nm");
				geode->addComponents ("/zy/zj/G_054/G054_mesh_x.nm");
				geode->addComponents ("/zy/zj/G_054/G054_mesh_y.nm");
#endif
				//geode->doAction ("zl");
				geode->setDrawBoundingbox (1);

				_view->_actionMenu->clear ();
				_currentAction = ATOM_Geode::invalid_actionId;
				if (geode->getNumTracks())
				{
					for (unsigned i = 0; i < geode->getNumTracks(); ++i)
					{
						_view->_actionMenu->appendMenuItem (geode->getTrackName (i), MenuId_ActionListStart + i);
					}
				}
				else
				{
					_currentActionIndex = -1;
				}
				_loadedModel = geode.get();
				const ATOM_BBox &bbox = geode->getBoundingbox ();
				_translation = -bbox.getCenter();
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				_view->_rootNode->clearChildren();
				_view->_rootNode->appendChild(geode.get());
				return true;
#else
		long l = strlen (szBuffer);
		const char *p = szBuffer + l - 3;
		ATOM_AUTOREF(ATOM_Geode) geode = _loadedModel; 
		if (!geode)
		{
			geode = ATOM_HARDREF(ATOM_Geode)();
		}
		if (replace)
		{
			geode->removeAllComponents();
			geode->resetActions ();
			if (_currentAction != ATOM_Geode::invalid_actionId)
			{
				geode->removeAction (_currentAction);
			}
		}

		ATOM_Components comp(szBuffer);
		_model = comp.getModel ();
		if (_model)
		{
			if (replace)
			{
				_modelio.load (szBuffer);
			}

			geode->addComponents (comp);

			_view->_actionMenu->clear ();
			_currentAction = ATOM_Geode::invalid_actionId;
			if (geode->getNumTracks())
			{
				for (unsigned i = 0; i < geode->getNumTracks(); ++i)
				{
					_view->_actionMenu->appendMenuItem (geode->getTrackName (i), MenuId_ActionListStart + i);
				}
			}
			else
			{
				_currentActionIndex = -1;
			}

			if (replace || !_loadedModel)
			{
				_loadedModel = geode.get();
				const ATOM_BBox &bbox = geode->getBoundingbox ();
				_translation = -bbox.getCenter();
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				_view->_rootNode->clearChildren();
				_view->_rootNode->appendChild(geode.get());
			}
			return true;
		}
		return false;
#endif
	}

	bool loadPhysicFile (const char *szBuffer)
	{
		FILE *fp = fopen (szBuffer, "rb");
		if (!fp)
		{
			return false;
		}
		fseek (fp, 0, SEEK_END);
		unsigned size = ftell (fp);
		fseek (fp, 0, SEEK_SET);

		ATOM_VECTOR<char> content(size);
		fread (&content[0], 1, size, fp);
		fclose (fp);

		ATOM_HARDREF(ATOM_Geode) geode;
		ATOM_Components comp;
		comp.loadComponentsFromMemory(&content[0], size);

		_model = comp.getModel ();
		if (_model)
		{
			geode->addComponents (comp);
			_loadedModel = geode.get();
			if (_currentActionIndex >= 0)
			{
				_currentAction = _loadedModel->doAction (_loadedModel->getTrackName (_currentActionIndex));
			}

			ATOM_RealtimeCtrl *rc = _view->_realtimeCtrl;
			_view->_rootNode->clearChildren();
			_view->_rootNode->appendChild(geode.get());
			return true;
		}
		return false;
	}

	float randRange (float minVal, float maxVal)
	{
		float f = float(rand())/float(RAND_MAX);
		return minVal + f * (maxVal - minVal);
	}

	void onCommand (ATOM_WidgetCommandEvent *event)
	{
		switch (event->id)
		{
		case MenuId_Load:
			{
				ATOM_FileDlg dlg(0, "/", NULL, NULL, "New3D模型文件(*.nm)|*.nm|New3D场景(*.3sg)|*.3sg|", ::GetActiveWindow());
				if (dlg.doModal () == IDOK) 
				{
					loadFile (dlg.getSelectedFileName(0), true);
					ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
					resetView (rc);
					_currentModel = dlg.getSelectedFileName(0);
				}
				break;
			}
		case MenuId_AddComponent:
			{
				ATOM_FileDlg dlg(0, "/", NULL, NULL, "New3D模型文件(*.nm)|*.nm|New3D场景(*.3sg)|*.3sg|", ::GetActiveWindow());
				if (dlg.doModal () == IDOK) 
				{
					loadFile (dlg.getSelectedFileName(0), false);
					ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
					resetView (rc);
					_currentModel = dlg.getSelectedFileName(0);
				}
				break;
			}
		case MenuId_Reload:
			{
				if (!_currentModel.empty())
				{
					loadFile (_currentModel.c_str(), true);
				}
				break;
			}
		case MenuId_Save:
			{
				if (_loadedModel && _modelio.modified())
				{
					if (!_modelio.save (0))
					{
						::MessageBox (::GetActiveWindow(), "保存失败,请检查是否磁盘已满.", "错误", MB_OK|MB_ICONHAND);
					}
				}
				break;
			}
		case MenuId_SaveAs:
			{
				ATOM_FileDlg dlg(ATOM_FileDlg::MODE_SAVEFILE, "/", "nm", NULL, "New3D模型文件(*.nm)|*.nm|", ::GetActiveWindow());
				if (dlg.doModal () == IDOK)
				{
					if (!_modelio.save (dlg.getSelectedFileName(0)))
					{
						::MessageBox (::GetActiveWindow(), "保存失败,请检查是否磁盘已满.", "错误", MB_OK|MB_ICONHAND);
					}
				}
				break;
			}
		case MenuId_Screenshot:
			{
				ATOM_FileDlg dlg(ATOM_FileDlg::MODE_SAVEFILE, "/", "png", NULL, "PNG文件(*.png)|*.png|", ::GetActiveWindow());
				if (dlg.doModal () == IDOK)
				{
					ATOM_GetRenderDevice ()->screenShot (ATOM_GetRenderDevice()->getCurrentView(), dlg.getSelectedFileName(0));
				}
				break;
			}
		case MenuId_EnablePPL:
			{
				ATOM_RenderSettings::enablePPL (!ATOM_RenderSettings::pplEnabled());
				break;
			}
		case MenuId_DebugDeferredNormals:
			{
				ATOM_RenderSettings::enableDebugDeferredNormals (!ATOM_RenderSettings::isDebugDeferredNormalsEnabled());
				break;
			}
		case MenuId_DumpMemState:
			{
				char filename[256] = { '\0' };
				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = ::GetActiveWindow();
				ofn.lpstrFilter = "文本文件(*.txt)\0*.txt\0";
				ofn.lpstrFile = filename;
				ofn.lpstrDefExt = "txt";
				ofn.nMaxFile = 256;
				ofn.Flags = OFN_OVERWRITEPROMPT;
				if (::GetSaveFileName (&ofn))
				{
					ATOM_DumpMemoryState (filename, 1);
				}
				break;
			}
		case MenuId_TimeScale_32x:
			{
				ATOM_APP->setTimeScale (32.f);
				break;
			}
		case MenuId_TimeScale_16x:
			{
				ATOM_APP->setTimeScale (16.f);
				break;
			}
		case MenuId_TimeScale_8x:
			{
				ATOM_APP->setTimeScale (8.f);
				break;
			}
		case MenuId_TimeScale_4x:
			{
				ATOM_APP->setTimeScale (4.f);
				break;
			}
		case MenuId_TimeScale_2x:
			{
				ATOM_APP->setTimeScale (2.f);
				break;
			}
		case MenuId_TimeScale_1x:
			{
				ATOM_APP->setTimeScale (1.f);
				break;
			}
		case MenuId_TimeScale_1_2x:
			{
				ATOM_APP->setTimeScale (0.5f);
				break;
			}
		case MenuId_TimeScale_1_4x:
			{
				ATOM_APP->setTimeScale (0.25f);
				break;
			}
		case MenuId_TimeScale_1_8x:
			{
				ATOM_APP->setTimeScale (0.125f);
				break;
			}
		case MenuId_TimeScale_1_16x:
			{
				ATOM_APP->setTimeScale (0.0625f);
				break;
			}
		case MenuId_TimeScale_1_32x:
			{
				ATOM_APP->setTimeScale (0.03125f);
				break;
			}
		case MenuId_ToggleWireframe:
			{
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				rc->setWireframeMode (!rc->isWireframeMode());
				break;
			}
		case MenuId_ToggleShadow:
			{
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				ATOM_SDLScene *scene = (ATOM_SDLScene*)rc->getScene();
				ATOM_RenderSettings::enableShadow (!ATOM_RenderSettings::isShadowEnabled());
				break;
			}
		case MenuId_TestTerrain:
			{
				ATOM_HARDREF(ATOM_Terrain) terrain;
				terrain->setTerrainFileName ("/map/100/terrain.xml");
				if (terrain->load (ATOM_GetRenderDevice()))
				{
					terrain->setDrawGrass (0);
					ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
					_view->_rootNode->clearChildren();
					_view->_rootNode->appendChild (terrain.get());
					//_view->_rootNode->appendChild (node.get());
				}
				break;
			}
		case MenuId_TestDeferredShading:
			{
				//static ATOM_DeferredScene deferredScene;
				//static bool init = false;
				//if (!init)
				//{
				//	deferredScene.initialize (512, 512);
				//	init = true;
				//}

				//deferredScene.load ("/map/map0/scene.3sg");
				//ATOM_Matrix4x4f viewMatrix;
				//viewMatrix.makeLookatLH(ATOM_Vector3f(2200, 500, 1200), ATOM_Vector3f(3000.f, 0.f, 2000.f), ATOM_Vector3f(0.f, 1.f, 0.f));
				//deferredScene.getCamera()->setViewMatrix (viewMatrix);
				//deferredScene.getCamera()->setViewport (0, 0, 512, 512);
				//deferredScene.getCamera()->setPerspective (1.57f, 1.f, 1.f, 2000.f);

				//ATOM_RenderDevice *device = ATOM_GetRenderDevice();
				//deferredScene.render (device, true);
				//device->screenShot (device->getCurrentView(), "/deferredTest.png");

				break;
			}
		case MenuId_TestAtmosphere:
			{
				ATOM_HARDREF(ATOM_Atmosphere) atmosphere;
				if (!atmosphere->load (ATOM_GetRenderDevice()))
				{
					::MessageBoxA (::GetActiveWindow(), "初始化大气效果失败！", "Error", MB_OK|MB_ICONHAND);
				}
				else
				{
					atmosphere->setSunIntensity (30.f);
					atmosphere->setExposure (1.5f);
					atmosphere->setRayleighMultiplier (0.0025f);
					atmosphere->setMieMultiplier (0.0046f);
					atmosphere->setG (-0.99f);
					_view->_rootNode->appendChild (atmosphere.get());
				}
				break;
			}
		case MenuId_TestWater:
			{
				ATOM_BBox bbox;
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				//_view->_rootNode->clearChildren();
				ATOM_HARDREF(ATOM_Water) water;
				water->setModelFileName ("/zy/map/effect/shui.water");
				water->load (ATOM_GetRenderDevice());
				water->setO2T (ATOM_Matrix4x4f::getScaleMatrix (ATOM_Vector3f(10.f, 0.f, 10.f)));
				water->setVisibility (100.f);
				water->setExtinction (ATOM_Vector3f(7.f, 30.f, 40.f));
				water->setShoreHardness (0.1f);
				rc->getScene()->getRootNode()->appendChild (water.get());
				gWater = water;
				break;
			}
		case MenuId_TestWaterBug:
			{
				static float s[4] = {20000.f, 10000.f, 5000.f, 2000.f};
				static int n = 0;

				if (gWater)
				{
					float f = s[(n++) % 4];
					gWater->setO2T (ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(f, 1.f, f)));
				}
				break;
			}
		case MenuId_TestActor:
			{
				ATOM_HARDREF(ATOM_Geode) geode;
				geode->addComponents ("/zy/zj/B_animation.nm");
				geode->addComponents ("/B138_mesh_t.nm");
				//geode->addComponents ("/zy/zj/G_054/G054_mesh_s.nm");
				//geode->addComponents ("/zy/zj/G_054/G054_mesh_t.nm");
				//geode->addComponents ("/zy/zj/G_054/G054_mesh_x.nm");
				//geode->addComponents ("/zy/zj/G_054/G054_mesh_y.nm");
				geode->doAction ("zl");
				geode->setDrawBoundingbox (1);
				//for (unsigned i = 0; i < geode->getNumMeshes(); ++i)
				//{
				//	ATOM_InstanceMesh *mesh = geode->getAnimation()->getMesh(i);
				//	ATOM_Vector4f color = mesh->getDiffuseColor ();
				//	color.w = 0.5f;
				//	mesh->setDiffuseColor (color);
				//}
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				const ATOM_BBox &bbox = geode->getBoundingbox ();
				_translation = -bbox.getCenter();
				_view->_rootNode->clearChildren();
				_view->_rootNode->appendChild(geode.get());
				break;
			}
		case MenuId_TestChar:
			{
				ATOM_BBox bbox;
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				_view->_rootNode->clearChildren();
				for (unsigned i = 0; i < 1000; ++i)
				{
					ATOM_HARDREF(ATOM_Geode) geode;
					geode->addComponents ("/zy/guaiwu/23/23_JQX.nm");
					unsigned numActions = geode->getNumTracks ();
					unsigned nTrack = rand() % numActions;
					geode->doAction (geode->getTrackName(nTrack));
					geode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix (ATOM_Vector3f(randRange(-100.f, 100.f), randRange(-30.f, 30.f), randRange(-100.f, 100.f))));
					bbox = geode->getBoundingbox();
					_view->_rootNode->appendChild(geode.get());
				}

				_translation = -bbox.getCenter();
				resetView (rc);
				break;
			}
		case MenuId_TestInstance:
			{
				ATOM_BBox bbox;
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				_view->_rootNode->clearChildren();
				for (unsigned i = 0; i < 2000; ++i)
				{
					ATOM_HARDREF(ATOM_Geode) geode;
					geode->addComponents ("/zy/map/yyf/fw_nf_010.nm");
					geode->setO2T (ATOM_Matrix4x4f::getTranslateMatrix (ATOM_Vector3f(randRange(-100.f, 100.f), randRange(-30.f, 30.f), randRange(-100.f, 100.f))));
					bbox = geode->getBoundingbox();
					_view->_rootNode->appendChild(geode.get());
				}

				_translation = -bbox.getCenter();
				resetView (rc);
				break;
			}
		case MenuId_TestHud:
			{
				_view->_rootNode->clearChildren ();
				{
					ATOM_HARDREF(ATOM_Hud) hud;
					hud->setColor (ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f));
					hud->setScreenSpace (true);
					hud->setPosition (300, 300);
					hud->setWidth (200);
					hud->setHeight (160);

					ATOM_Hud::TextProp textProp;
					textProp.text = "测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试";
					textProp.textColor.setFloats (1.f, 1.f, 0.f, 1.f);
					textProp.fontHandle = 0;
					textProp.cc = ATOM_CC_CP936;
					textProp.charMargin = 2;
					textProp.borderX = 0;
					textProp.borderY = 0;
					textProp.alignment = ATOM_Hud::ALIGN_LEFT|ATOM_Hud::ALIGN_TOP;
					hud->setTextProperties (textProp);

					_view->_rootNode->appendChild (hud.get());
				}
				{
					ATOM_HARDREF(ATOM_Hud) hud;
					hud->setColor (ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f));
					hud->setScreenSpace (true);
					hud->setPosition (600, 300);
					hud->setWidth (200);
					hud->setHeight (160);

					ATOM_Hud::TextProp textProp;
					textProp.text = "测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试测试";
					textProp.textColor.setFloats (1.f, 1.f, 0.f, 1.f);
					textProp.fontHandle = 0;
					textProp.cc = ATOM_CC_CP936;
					textProp.charMargin = 2;
					textProp.borderX = 0;
					textProp.borderY = 0;
					textProp.alignment = ATOM_Hud::ALIGN_LEFT|ATOM_Hud::ALIGN_TOP;
					hud->setTextProperties (textProp);

					_view->_rootNode->appendChild (hud.get());
				}
				break;
			}
		case MenuId_DiscardLoading:
			{
				//ATOM_ContentStream::discardAll ();
				ATOM_SDLScene scene;
				ATOM_HARDREF(ATOM_LightNode) node;
				scene.getRootNode()->appendChild (node.get());
				scene.save ("/xxxxx.3sg");
				break;
			}
		case MenuId_TestMtLoad:
			{
				//GUI_ImageList il;
				//il.load ("/configure/uiimage.xml", true);

				//GUI_Image *image = il.getImage (0x273f);
				//ATOM_ContentStream::waitForInterfaceDone (image->getImage (WST_NORMAL)->GetLoadInterface());

				//break;

				struct Callback
				{
					static void __cdecl callback (ATOM_LoadingRequest *request)
					{
						if (request->stage == LOADINGSTAGE_DISCARD)
						{
							ATOM_LOGGER::log ("request %d discarded\n", int(request->userData));
						}
						else
						{
							ATOM_LOGGER::log ("request %d finished\n", int(request->userData));
						}
						ATOM_DELETE(request);
					}
				};

				static const char *filenames [13] = {
					"/zy/guaiwu/1/1_ls.nm",
					"/zy/guaiwu/2/2_bjg.nm",
					"/zy/map/yyf/fw_hfz_000.nm",
					"/zy/map/yyf/fw_hc_sj_004.nm",
					"/zy/map/yyf/fw_zyd_002_b.nm",
					"/zy/map/yyf/hc_lumian_001.nm",
					"/zy/map/yyf/fw_hc_sc001.nm",
					"/zy/map/yyf/fw_jl_000.nm",
					"/zy/map/yyf/fw_zyd_002_a.nm",
					"/zy/map/yyf/fw_zyd_003_a.nm",
					"/zy/map/yyf/fw_hc_000.nm",
					"/zy/zj/B_animation.nm",
					"/zy/zj/G_animation.nm",
				};

				for (;;)
				{
					for (unsigned i = 0; i < 5000; ++i)
					{
						ATOM_LoadingRequest *request = ATOM_NEW(ATOM_LoadingRequest);
						ATOM_HARDREF(ATOM_Geode) geode;
						geode->setModelFileName (filenames[i%13]);
						geode->getLoadInterface()->mtload (0, i / 10, &Callback::callback, (void*)i);
					}
					ATOM_ContentStream::waitForGroupDone (46);
					ATOM_LOGGER::log ("Waited\n");
					ATOM_ContentStream::discardAll ();
				}

				break;
			}
		case MenuId_ConfigFog:
			{
				ATOM_GUIRenderer *gui = ((ATOM_RealtimeCtrl*)getHost())->getRenderer ();

				ATOM_Dialog *dlgFog = ATOM_NEW(ATOM_Dialog, gui->getDesktop(), ATOM_GUIRect(200, 100, 240, 120));

				ATOM_RealtimeCtrl *rc = ATOM_NEW(ATOM_RealtimeCtrl, dlgFog, dlgFog->getClientRect(), ATOM_Widget::DynamicContent, 38, ATOM_Widget::ShowNormal);
				ATOM_SDLScene *scene = ATOM_NEW(ATOM_SDLScene);
				scene->getCamera()->setPerspective (1.57f, float(rc->getWidgetRect().w)/float(rc->getWidgetRect().h), 1.f, 2000.f);
				scene->getCamera()->lookAt (ATOM_Vector3f(0, 0, -300.f), ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
				ATOM_HARDREF(ATOM_Geode) geode;
				geode->setModelFileName ("/zy/map/yxm/maf001.nm");
				geode->load (ATOM_GetRenderDevice());
				scene->getRootNode()->appendChild (geode.get());
				rc->setScene (scene);
				//ATOM_Checkbox *buttonEnable = ATOM_NEW(ATOM_Checkbox, dlgFog, ATOM_GUIRect(10, 30, 12, 12), ATOM_Widget::Border, ButtonId_EnableFog);
				//ATOM_Label *labelEnableFog = ATOM_NEW(ATOM_Label, dlgFog, ATOM_GUIRect(25, 30, 50, 12), 0, 0, ATOM_Widget::ShowNormal);
				//labelEnableFog->setText ("允许雾效");

				//ATOM_Label *labelStart = ATOM_NEW(ATOM_Label, dlgFog, ATOM_GUIRect(10, 50, 50, 12), 0, 0, ATOM_Widget::ShowNormal);
				//labelStart->setText ("近距离");
				//ATOM_ScrollBar *sliderNear = ATOM_NEW(ATOM_ScrollBar, dlgFog, ATOM_GUIRect(70, 50, 150, 12), 0, SliderId_Near, ATOM_Widget::ShowNormal);
				//sliderNear->setRange (0, 300);

				//ATOM_Label *labelEnd = ATOM_NEW(ATOM_Label, dlgFog, ATOM_GUIRect(10, 70, 50, 12), 0, 0, ATOM_Widget::ShowNormal);
				//labelEnd->setText ("远距离");
				//ATOM_ScrollBar *sliderFar = ATOM_NEW(ATOM_ScrollBar, dlgFog, ATOM_GUIRect(70, 70, 150, 12), 0, SliderId_Far, ATOM_Widget::ShowNormal);
				//sliderFar->setRange (0, 300);

				//ATOM_Button *buttonColor = ATOM_NEW(ATOM_Button, dlgFog, ATOM_GUIRect(10, 90, 60, 18), ATOM_Widget::Border, ButtonId_FogColor);
				//buttonColor->setText ("颜色..");

				//ATOM_Button *buttonOk = ATOM_NEW(ATOM_Button, dlgFog, ATOM_GUIRect(105, 90, 55, 18), ATOM_Widget::Border, ButtonId_Ok);
				//buttonOk->setText ("确定");

				//ATOM_Button *buttonCancel = ATOM_NEW(ATOM_Button, dlgFog, ATOM_GUIRect(165, 90, 55, 18), ATOM_Widget::Border, ButtonId_Cancel);
				//buttonCancel->setText ("取消");

				//ATOM_Scene *scene = ((ATOM_RealtimeCtrl*)getHost())->getScene ();
				FogDialogTrigger trigger(scene);
				dlgFog->setEventTrigger (&trigger);
				if (ButtonId_Cancel == dlgFog->showModal ())
				{
					scene->enableFog (trigger.enableFog);
					scene->setFogStart (trigger.fogNear);
					scene->setFogEnd (trigger.fogFar);
					scene->setFogColor (trigger.fogColor);
				}

				ATOM_DELETE(dlgFog);

				break;
			}
		case MenuId_ChangeColor:
			{
				if (!_currentModel.empty ())
				{
					ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();

					ATOM_AUTOREF(ATOM_Node) node = _view->_rootNode->getChild (0);
					ATOM_Geode *geode = dynamic_cast<ATOM_Geode*>(node.get());
					if (geode)
					{
						static COLORREF custColors[16] = { 0 };
						CHOOSECOLOR cc;
						memset (&cc, 0, sizeof(cc));
						cc.lStructSize = sizeof(cc);
						cc.hwndOwner = GetActiveWindow ();
						cc.Flags = CC_RGBINIT|CC_FULLOPEN|CC_ANYCOLOR;
						cc.rgbResult = RGB(255, 255, 255);
						cc.lpCustColors = (LPDWORD)custColors;
						if (::ChooseColor (&cc))
						{
							float r = GetRValue(cc.rgbResult) / 255.f;
							float g = GetGValue(cc.rgbResult) / 255.f;
							float b = GetBValue(cc.rgbResult) / 255.f;
							for (unsigned m = 0; m < geode->getAnimation()->getNumMeshes(); ++m)
							{
								ATOM_InstanceMesh *mesh = geode->getAnimation()->getMesh (m);
								mesh->setDiffuseColor (ATOM_Vector4f(r, g, b, 1));
							}
						}
					}
				}
				break;
			}
		case MenuId_TestAttachment:
			{
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				_view->_rootNode->clearChildren();
				ATOM_HARDREF(ATOM_Geode) geode;
				geode->setModelFileName ("/zy/npc/n110.nm");
				geode->mtload (ATOM_GetRenderDevice(), false, 0, 0, 0);

				ATOM_HARDREF(ATOM_Geode) geode2;
				geode2->setModelFileName ("/zy/npc/n110_01.nm");
				geode2->load (ATOM_GetRenderDevice());
				geode->attach ("joint5", geode2.get());

				ATOM_HARDREF(ATOM_Geode) geode3;
				geode3->setModelFileName ("/zy/npc/n110_s_003.nm");
				geode3->load (ATOM_GetRenderDevice());
				geode->attach ("joint5", geode3.get());

				ATOM_HARDREF(ATOM_Geode) geode4;
				geode4->setModelFileName ("/zy/texiao/JN/j/j_1105_c_001.nm");
				geode4->load (ATOM_GetRenderDevice());
				geode->attach ("joint5", geode4.get());

				geode->doAction ("xx1");
				_view->_rootNode->appendChild (geode.get());
				ATOM_BBox bbox = geode->getBoundingbox();
				_translation = -bbox.getCenter ();
				resetView (rc);
				break;
			}
		case MenuId_PostEffectClear:
			{
				_view->_scene.setPostEffect (0);
				_view->_scene.enabelHDR (false);
				break;
			}
		case MenuId_PostEffectGray:
			{
				_view->_scene.enabelHDR (false);
				ATOM_BasePostEffect *e = _view->_scene.getPostEffect();
				ATOM_GrayScaleEffect *gs = ATOM_NEW(ATOM_GrayScaleEffect);
				if (!e)
				{
					_view->_scene.setPostEffect (gs);
				}
				else
				{
					e->chain (gs);
				}
				break;
			}
		case MenuId_PostEffectBloom:
			{
				ATOM_BasePostEffect *e = _view->_scene.getPostEffect();
				ATOM_BloomEffect *b = ATOM_NEW(ATOM_BloomEffect);
				if (!e)
				{
					_view->_scene.setPostEffect (b);
				}
				else
				{
					e->chain (b);
				}
				_view->_scene.enabelHDR (true);
				break;
			}
		case MenuId_PostEffectGamma:
			{
				ATOM_BasePostEffect *e = _view->_scene.getPostEffect();
				ATOM_AutoGammaCorrectionEffect *b = ATOM_NEW(ATOM_AutoGammaCorrectionEffect);
				if (!e)
				{
					_view->_scene.setPostEffect (b);
				}
				else
				{
					e->chain (b);
				}
				break;
			}
		case MenuId_ChangeAmbient:
			{
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				ATOM_Vector4f ambientLight = ((ATOM_SDLScene*)rc->getScene())->getAmbientLight();
				COLORREF defaultColor = RGB(ambientLight.x * 255, ambientLight.y * 255, ambientLight.z * 255);
				static COLORREF custColors[16] = { 0 };
				CHOOSECOLOR cc;
				memset (&cc, 0, sizeof(cc));
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = GetActiveWindow ();
				cc.Flags = CC_RGBINIT|CC_FULLOPEN|CC_ANYCOLOR;
				cc.rgbResult = defaultColor;
				cc.lpCustColors = (LPDWORD)custColors;
				if (::ChooseColor (&cc))
				{
					ambientLight.x = GetRValue(cc.rgbResult) / 255.f;
					ambientLight.y = GetGValue(cc.rgbResult) / 255.f;
					ambientLight.z = GetBValue(cc.rgbResult) / 255.f;
					((ATOM_SDLScene*)rc->getScene())->setAmbientLight(ambientLight);
				}
				break;
			}
		case MenuId_EditModel:
			{
				if (_loadedModel)
				{
					if (_modelio.showEditDialog (::GetActiveWindow()))
					{
						if (!_modelio.save (_tmpfilename.c_str()))
						{
							::MessageBox (::GetActiveWindow(), "不能保存临时文件，大概磁盘已满", "错误", MB_OK|MB_ICONHAND);
						}
						else
						{
							loadPhysicFile (_tmpfilename.c_str());
						}
					}
				}
				break;
			}
		case MenuId_ToggleBoundingbox:
			{
				ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
				ATOM_Scene *scene = rc->getScene();
				ATOM_CullVisitor *cullvisitor = scene->getCullVisitor ();
				unsigned flags = cullvisitor->getFlags ();
				if (flags & ATOM_CullVisitor::CULL_DRAW_BOUNDINGBOX)
				{
					flags &= ~ATOM_CullVisitor::CULL_DRAW_BOUNDINGBOX;
				}
				else
				{
					flags |= ATOM_CullVisitor::CULL_DRAW_BOUNDINGBOX;
				}
				cullvisitor->setFlags (flags);
				break;
			}
		case MenuId_Exit:
			{
				ATOM_APP->postQuitEvent (0);
				break;
			}
		case MenuId_TestParticleSystem:
			{
				break;
			}
		default:
			{
				if (event->id >= MenuId_ActionListStart)
				{
					int action = event->id - MenuId_ActionListStart;
					ATOM_ASSERT(_loadedModel && _loadedModel->getNumTracks() > action);
					_loadedModel->removeAction (_currentAction);
					_currentAction = _loadedModel->doAction (_loadedModel->getTrackName (action), 0);
					_currentActionIndex = action;
				}

				break;
			}
		}
	}

	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
	{
		if (_guiHud)
		{
			int x = ATOM_ftol(_guiHud->getWidth() * _guiHudPos[0]);
			int y = ATOM_ftol(_guiHud->getHeight() * _guiHudPos[1]);
			_guiHud->getGuiRenderer()->queueEvent (ATOM_NEW(ATOM_MouseButtonDownEvent, ATOM_GetRenderDevice()->getCurrentView(), x, y, BUTTON_LEFT, event->shiftState), ATOM_APP);
		}
		else
		{
			_trackball.begin (event->x, event->y);
		}
	}

	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event)
	{
		if (_guiHud)
		{
			int x = ATOM_ftol(_guiHud->getWidth() * _guiHudPos[0]);
			int y = ATOM_ftol(_guiHud->getHeight() * _guiHudPos[1]);
			_guiHud->getGuiRenderer()->queueEvent (ATOM_NEW(ATOM_MouseButtonUpEvent, ATOM_GetRenderDevice()->getCurrentView(), x, y, BUTTON_LEFT, event->shiftState), ATOM_APP);
		}
		else
		{
			_trackball.end ();
		}
	}

	void onMouseMove (ATOM_WidgetMouseMoveEvent *event)
	{
		ATOM_Node *node = _view->_rootNode.get();
		mousex = event->x;
		mousey = event->y;

		ATOM_PickVisitor pickV;
		pickV.setFlags (pickV.getFlags() | ATOM_PickVisitor::HAVE_SCREEN_POSITION);
		ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost();
#if 0
		rc->pick (mousex, mousey, pickV);
		if (pickV.getNumPicked ())
		{
			pickV.sortResults ();
			const ATOM_PickVisitor::PickResult &result = pickV.getPickResult(0);
			ATOM_LOGGER::log ("Picked: %s\n", result.node->getClassName());
			if (!strcmp (result.node->getClassName(), ATOM_GuiHud::_classname()))
			{
				_guiHud = result.node;
				_guiHudPos[0] = result.u;
				_guiHudPos[1] = result.v;
				int x = ATOM_ftol(_guiHud->getWidth() * result.u);
				int y = ATOM_ftol(_guiHud->getHeight() * result.v);

				_guiHud->getGuiRenderer()->queueEvent (ATOM_NEW(ATOM_MouseMoveEvent, ATOM_GetRenderDevice()->getCurrentView(), x, y, event->xrel, event->yrel,
					event->leftDown ? KEYSTATE_PRESSED : KEYSTATE_RELEASED, 
					event->middleDown ? KEYSTATE_PRESSED : KEYSTATE_RELEASED,
					event->rightDown ? KEYSTATE_PRESSED : KEYSTATE_RELEASED), ATOM_APP);
			}
			else
			{
				_guiHud = 0;
			}
		}
		else
		{
			ATOM_LOGGER::log ("Nothing picked\n");
			_guiHud = 0;
		}
#endif
		_trackball.move (event->x, event->y);

		if (node)
		{
			ATOM_Matrix4x4f matTrans = ATOM_Matrix4x4f::getTranslateMatrix (_translation);
			ATOM_Matrix4x4f mat = _trackball.getRotationMatrix ();
			node->setO2T (mat >> matTrans);
		}
	}

	void onIdle (ATOM_WidgetIdleEvent *event)
	{
		//ATOM_RealtimeCtrl *w = (ATOM_RealtimeCtrl*)getHost ();

		//ATOM_PickVisitor pickV;
		//pickV.setFlags (pickV.getFlags() | ATOM_PickVisitor::HAVE_SCREEN_POSITION);

		//w->pick (mousex, mousey, pickV);
		//if (pickV.getNumPicked ())
		//{
		//	pickV.sortResults ();
		//	//printf ("Picked: distance = %f\n", pickV.getPickResults().front().distance);
		//}
		//else
		//{
		//	//printf ("Not picked\n");
		//}
	}

	void onMouseWheel (ATOM_WidgetMouseWheelEvent *event)
	{
		ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost ();
		ATOM_Node *rootNode = _view->_rootNode.get();
		if (rootNode)
		{
			const ATOM_BBox &bbox = rootNode->getBoundingbox ();
			ATOM_Matrix4x4f viewMatrix = rc->getCamera()->getViewMatrix();
			ATOM_Vector3f eye(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32);
			float f = eye.getLength();
			float f2 = f - event->delta * 5.f;
			if (f2 == 0.f)
			{
				f2 = 0.1f;
			}
			eye *= (f2 / f);

			viewMatrix.makeLookatLH(eye, ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
			rc->getCamera()->setViewMatrix (viewMatrix);
		}
	}

	void onKeyDown (ATOM_WidgetKeyDownEvent *event)
	{
		if (event->key == KEY_q || event->key == KEY_z)
		{
			ATOM_RealtimeCtrl *rc = (ATOM_RealtimeCtrl*)getHost ();
			ATOM_Node *rootNode = _view->_rootNode.get();
			if (rootNode)
			{
				const ATOM_BBox &bbox = rootNode->getBoundingbox ();
				ATOM_Matrix4x4f viewMatrix = rc->getCamera()->getViewMatrix();
				ATOM_Vector3f eye(viewMatrix.m30, viewMatrix.m31, viewMatrix.m32);
				if (event->key == KEY_q) eye.y += 0.1f;
				if (event->key == KEY_z) eye.y -= 0.1f;

				viewMatrix.makeLookatLH(eye, ATOM_Vector3f(0.f, 0.f, 0.f), ATOM_Vector3f(0.f, 1.f, 0.f));
				rc->getCamera()->setViewMatrix (viewMatrix);
			}
		}
	}

	ATOM_DECLARE_EVENT_MAP(RealtimeEventTrigger, ATOM_EventTrigger)
};

ATOM_BEGIN_EVENT_MAP(RealtimeEventTrigger, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetLButtonUpEvent, onLButtonUp)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetMouseMoveEvent, onMouseMove)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetMouseWheelEvent, onMouseWheel)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetKeyDownEvent, onKeyDown)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetCommandEvent, onCommand)
	ATOM_EVENT_HANDLER(RealtimeEventTrigger, ATOM_WidgetIdleEvent, onIdle)
ATOM_END_EVENT_MAP

MyView::~MyView (void)
{
	ATOM_DELETE(_gui);
	ATOM_DELETE(_rttrigger);
}

void MyView::initGUI (void)
{
  _gui = ATOM_NEW(ATOM_GUIRenderer);
  _gui->getDesktop()->setClearBackground (false);

  _rttrigger = ATOM_NEW(RealtimeEventTrigger, this);

  ATOM_GUIRect viewport;
  viewport.x = 0;
  viewport.y = 0;
  viewport.w = _window->getWindowWidth();
  viewport.h = _window->getWindowHeight();
  _gui->setViewport (viewport);

  ATOM_MenuBar *menubar = ATOM_NEW (ATOM_MenuBar, _gui);
  menubar->appendMenuItem ("文件", 0);
  menubar->appendMenuItem ("视图", 0);
  menubar->appendMenuItem ("动作", 0);
  menubar->appendMenuItem ("测试", 0);
  menubar->appendMenuItem ("帮助", 0);

  ATOM_PopupMenu *submenu;
  submenu = menubar->createSubMenu (0);
  submenu->appendMenuItem ("打开..", MenuId_Load);
  submenu->appendMenuItem ("保存", MenuId_Save);
  submenu->appendMenuItem ("另存为..", MenuId_SaveAs);
  submenu->appendMenuItem ("保存内存日志..", MenuId_DumpMemState);
  submenu->appendMenuItem ("截图..", MenuId_Screenshot);
  submenu->appendMenuItem ("退出", MenuId_Exit);

  submenu = menubar->createSubMenu (1);
  submenu->appendMenuItem ("网格", MenuId_ToggleWireframe);
  submenu->appendMenuItem ("阴影", MenuId_ToggleShadow);
  submenu->appendMenuItem ("改变环境光", MenuId_ChangeAmbient);
  submenu->appendMenuItem ("改变颜色", MenuId_ChangeColor);
  submenu->appendMenuItem ("渲染包围盒", MenuId_ToggleBoundingbox);
  submenu->appendMenuItem ("重新载入", MenuId_Reload);
  submenu->appendMenuItem ("设置雾效..", MenuId_ConfigFog);
  submenu->appendMenuItem ("编辑模型..", MenuId_EditModel);
  submenu->appendMenuItem ("改变时间流逝速度", 0);
  ATOM_PopupMenu *timeScaleMenu = submenu->createSubMenu (8);
  timeScaleMenu->appendMenuItem ("32倍速", MenuId_TimeScale_32x);
  timeScaleMenu->appendMenuItem ("16倍速", MenuId_TimeScale_16x);
  timeScaleMenu->appendMenuItem ("8倍速", MenuId_TimeScale_8x);
  timeScaleMenu->appendMenuItem ("4倍速", MenuId_TimeScale_4x);
  timeScaleMenu->appendMenuItem ("2倍速", MenuId_TimeScale_2x);
  timeScaleMenu->appendMenuItem ("1倍速", MenuId_TimeScale_1x);
  timeScaleMenu->appendMenuItem ("1/2倍速", MenuId_TimeScale_1_2x);
  timeScaleMenu->appendMenuItem ("1/4倍速", MenuId_TimeScale_1_4x);
  timeScaleMenu->appendMenuItem ("1/8倍速", MenuId_TimeScale_1_8x);
  timeScaleMenu->appendMenuItem ("1/16倍速", MenuId_TimeScale_1_16x);
  timeScaleMenu->appendMenuItem ("1/32倍速", MenuId_TimeScale_1_32x);
  submenu->appendMenuItem ("PPL切换", MenuId_EnablePPL);
  submenu->appendMenuItem ("添加组件..", MenuId_AddComponent);
  submenu->appendMenuItem ("查看法线(延迟着色)", MenuId_DebugDeferredNormals);

  _actionMenu = menubar->createSubMenu (2);

  submenu = menubar->createSubMenu (3);
  submenu->appendMenuItem ("测试粒子系统", MenuId_TestParticleSystem);
  submenu->appendMenuItem ("测试骨骼", MenuId_TestChar);
  submenu->appendMenuItem ("测试几何体实例", MenuId_TestInstance);
  submenu->appendMenuItem ("测试装备", MenuId_TestAttachment);
  submenu->appendMenuItem ("测试后处理效果", 0);
  submenu->appendMenuItem ("测试地形", MenuId_TestTerrain);
  submenu->appendMenuItem ("测试水面", MenuId_TestWater);
  submenu->appendMenuItem ("测试水面BUG", MenuId_TestWaterBug);
  submenu->appendMenuItem ("测试角色", MenuId_TestActor);
  submenu->appendMenuItem ("测试HUD", MenuId_TestHud);
  submenu->appendMenuItem ("测试多线程载入", MenuId_TestMtLoad);
  submenu->appendMenuItem ("测试延迟着色", MenuId_TestDeferredShading);
  submenu->appendMenuItem ("测试大气效果", MenuId_TestAtmosphere);
  submenu->appendMenuItem ("清除载入队列", MenuId_DiscardLoading);

  ATOM_PopupMenu *postEffectMenu = submenu->createSubMenu (4);
  postEffectMenu->appendMenuItem ("清除所有", MenuId_PostEffectClear);
  postEffectMenu->appendMenuItem ("灰度", MenuId_PostEffectGray);
  postEffectMenu->appendMenuItem ("泛光(HDR)", MenuId_PostEffectBloom);
  postEffectMenu->appendMenuItem ("Gamma校正)", MenuId_PostEffectGamma);

  submenu = menubar->createSubMenu (4);
  submenu->appendMenuItem ("关于new3d模型查看器", MenuId_About);

  _realtimeCtrl = ATOM_NEW(ATOM_RealtimeCtrl, _gui->getDesktop(), _gui->getDesktop()->getClientRect(), ATOM_Widget::DynamicContent);
  _realtimeCtrl->setMenuBar (menubar);
  _realtimeCtrl->setScene(&_scene);
  _realtimeCtrl->setEventTrigger (_rttrigger);

  ATOM_HARDREF(ATOM_DSky) sky;
  sky->setSphereTextureFileName("/sky/j.png");
  sky->setPlane0TextureFileName("/sky/j1.png");
  sky->setPlane1TextureFileName("/sky/j2.png");
  if (sky->load (ATOM_GetRenderDevice()))
  {
	_realtimeCtrl->getScene()->getRootNode()->appendChild (sky.get());
  }

  ATOM_HARDREF(ATOM_Node) node;
  _realtimeCtrl->getScene()->getRootNode()->appendChild (node.get());
  _rootNode = node;

  _rttrigger->resetView (_realtimeCtrl);
}

void LoadXmlFile(ATOM_TiXmlDocument& doc, const char* pFileName)
{
	ATOM_AutoFile file(pFileName, ATOM_VFS::text|ATOM_VFS::read);
	if(file == NULL)
	{
		int nLen = file->size();
		char* pBuffer = ATOM_NEW_ARRAY(char, nLen+1);
		assert(pBuffer);
		int nRead = file->read(pBuffer, nLen);
		pBuffer[nRead] = 0;
		doc.Parse(pBuffer);
		ATOM_DELETE_ARRAY(pBuffer);

		ATOM_AUTOREF(ATOM_Node) node = ATOM_SceneIO::getInstance()->load(ATOM_GetRenderDevice(), doc.RootElement());
	}
}

class MyApp: public ATOM_Application
{
public:
  MyView view;
  ATOMX_TweakBar *_bar;
  ATOMX_TweakBar *_bar2;
  int _value;
  char *_str;

public:
  MyApp (void)
  {
	  _bar = 0;
	  _value = 0;
	  _str = 0;
  }

  void onIdle (ATOM_AppIdleEvent *event)
  {
    view.handleEvent (event);

	if (ATOM_GetRenderDevice()->beginFrame ())
	{
		view.render ();
		if (_bar)
		{
			ATOMX_TweakBar::draw ();
		}
		ATOM_GetRenderDevice()->endFrame ();
		ATOM_GetRenderDevice()->present ();
	}

	if (_bar)
	{
		_bar->setI ("Int", _bar->getI("Int") + 1);
	}

	ATOM_ContentStream::processRequests ();
	//printf("FPS = %d CLOCK = %d\t\t\r", ATOM_APP->getFrameStamp().FPS, ::GetTickCount ());
  }

  void onAppInit (ATOM_AppInitEvent *event)
  {
	callParentHandler (event);
    if (!event->success)
    {
      return;
    }

    ATOM_LoadPlugin ("ATOM_engine");
    ATOM_LoadPlugin ("ATOM_render");
    ATOM_LoadPlugin ("ATOM_image");

	ATOM_RenderWindowDesc desc;
	desc.width = WINDOWWIDTH;
	desc.height = WINDOWHEIGHT;
	desc.naked = false;
	desc.iconid = 0;
	desc.resizable = true;
	desc.multisample = 0;
	desc.title = "New3d模型查看器";
	desc.windowid = 0;

	view._window = ATOM_InitializeGfx (0, desc, true);
	if (!view._window)
    {
		event->success = false;
		return;
    }

	ATOM_RenderDevice *renderdevice = ATOM_GetRenderDevice();
    renderdevice->setClearColor (view._window.get(), 0, 0, 0.5f, 0);

    if (!view.initFont ())
    {
		ATOMX_TweakBar::terminate ();
		ATOM_DoneGfx ();
		event->success = false;
		return;
    }

    view.initGUI ();

	ATOM_ContentStream::run ();

	ATOM_RenderSettings::enableMultithreadedAnimation (false);
	ATOM_LoadFont ("bug", "/fonts/wqy.ttc");

	if (event->argc == 2)
	{
		view._rttrigger->loadPhysicFile (event->argv[1]);
	}
  }

  void onAppExit (ATOM_AppExitEvent *event)
  {
	ATOM_DELETE(_bar);
	ATOMX_TweakBar::terminate ();
    ATOM_DoneGfx ();
  }

  void onDeviceLost (ATOM_DeviceLostEvent *event)
  {
	  if (_bar)
	  {
		  _bar->handleDeviceLost ();
	  }
  }

  void onDeviceReset (ATOM_DeviceResetEvent *event)
  {
	  unsigned w = ATOM_GetRenderDevice()->getCurrentView()->getWindowWidth ();
	  unsigned h = ATOM_GetRenderDevice()->getCurrentView()->getWindowHeight ();
	  ATOM_GetRenderDevice()->setViewport (0, 0, 0, w, h);
	  view._gui->setViewport (ATOM_GUIRect(0, 0, w, h));
	  view._rttrigger->_trackball.setWindow (w, h);
	  view._realtimeCtrl->resize (view._gui->getDesktop()->getClientRect());
	  if (_bar)
	  {
		  _bar->handleDeviceReset (w, h);
	  }
  }

  void onWMEvent (ATOM_AppWMEvent *event)
  {
	if (event->msg == WMU_RENDERWINDOW_DEVICE_ATTACHED)
	{
		void *device = (void*)event->wParam;

		RECT rc;
		::GetClientRect (event->hWnd, &rc);

#if 1
		if (!_bar && ATOMX_TweakBar::initialize (device, rc.right - rc.left, rc.bottom - rc.top))
		{
			_bar = ATOM_NEW(ATOMX_TweakBar, "Test");
			_bar->setBarColor (0.15f, 0.25f, 0.5f, 1.f);
			_bar->addIntVar ("Int", 0, 26, false, "Group1");
			_bar->addUShortVar ("UShort", 1, 1, false, "Group1");
			_bar->addCharVar ("Char", 2, 'A', false, "Group1");
			_bar->addFloatVar ("Float", 3, 1.f, false, "Group1");
			_bar->addStringVar ("String", 4, "ABCD", false, "Group1");
			_bar->addBoolVar ("Bool", 5, true, false, "Group1");
			_bar->addSeparator ("Separator1", "Group1");
			_bar->addRGBVar ("RGB", 6, 1.f, 0.f, 0.f, false, "Group1");
			_bar->addRGBAVar ("RGBA", 7, 1.f, 0.f, 0.f, 1.f, false, "Group1");
			_bar->addSeparator ("Separator2", "Group1");
			_bar->addQuatVar ("Quat", 8, 0.f, 0.f, 0.f, 1.f, false, "Group1");
			_bar->addDirVar ("Dir", 9, 1.f, 0.f, 0.f, false, "Group1");
			_bar->addEnum ("Enum", 10, 0, false, "Group1", "Enum1", 0, "Enum2", 1, "Enum3", 2, "Enum4", 3, "Enum5", 4, "Enum6", 5, "Enum7", 6, "Enum8", 7, "Enum9", 8, 0);
			_bar->addButton ("Button", 11, "Click Me!", "Group2");
			_bar->addText ("Text", "Look at me!", "Group2");
			_bar->setVarMinMax ("Int", 25, 78);
			_bar->setGroupOpenState ("Group1", false);
			_bar->setBarLabel ("This is a bar demo");
			_bar->setBarIconifiable (false);
			_bar->setBarPosition (300, 300);
		}
#endif
	}
	else
	{
		ATOMX_TweakBar::processWMEvent (event);
	}
  }

  void onTWCommand (ATOMX_TWCommandEvent *event)
  {
	  ATOM_LOGGER::log ("command_id=%d\n", event->id);
  }

  void onTWValueChanged (ATOMX_TWValueChangedEvent *event)
  {
	  switch (event->id)
	  {
	  case 0:
		  ATOM_LOGGER::log ("id=%d, value=%d\n", event->id, event->var.getI ());
		  break;
	  case 1:
		  ATOM_LOGGER::log ("id=%d, value=%d\n", event->id, event->var.getI ());
		  break;
	  case 2:
		  ATOM_LOGGER::log ("id=%d, value=%d\n", event->id, event->var.getI ());
		  break;
	  case 3:
		  ATOM_LOGGER::log ("id=%d, value=%f\n", event->id, event->var.getF ());
		  break;
	  case 4:
		  ATOM_LOGGER::log ("id=%d, value=%s\n", event->id, event->var.getS ());
		  break;
	  case 5:
		  ATOM_LOGGER::log ("id=%d, value=%s\n", event->id, event->var.getB () ? "true" : "false");
		  break;
	  case 6:
		  ATOM_LOGGER::log ("id=%d, value=(%f,%f,%f)\n", event->id, event->var.get3F ()[0], event->var.get3F()[1], event->var.get3F()[2]);
		  break;
	  case 7:
		  ATOM_LOGGER::log ("id=%d, value=(%f,%f,%f,%f)\n", event->id, event->var.get4F ()[0], event->var.get4F()[1], event->var.get4F()[2], event->var.get4F()[3]);
		  break;
	  case 8:
		  ATOM_LOGGER::log ("id=%d, value=(%f,%f,%f,%f)\n", event->id, event->var.get4F ()[0], event->var.get4F()[1], event->var.get4F()[2], event->var.get4F()[3]);
		  break;
	  case 9:
		  ATOM_LOGGER::log ("id=%d, value=(%f,%f,%f)\n", event->id, event->var.get3F ()[0], event->var.get3F()[1], event->var.get3F()[2]);
		  break;
	  case 10:
		  ATOM_LOGGER::log ("id=%d, value=%d\n", event->id, event->var.getI ());
		  break;
	  }
  }

  void onUnhandledEvent (ATOM_Event *event)
  {
    view.handleEvent (event);
  }

  ATOM_DECLARE_EVENT_MAP(MyApp, ATOM_Application)
};

ATOM_BEGIN_EVENT_MAP(MyApp, ATOM_Application)
  ATOM_EVENT_HANDLER(MyApp, ATOM_AppIdleEvent, onIdle)
  ATOM_EVENT_HANDLER(MyApp, ATOM_AppInitEvent, onAppInit)
  ATOM_EVENT_HANDLER(MyApp, ATOM_AppExitEvent, onAppExit)
  ATOM_EVENT_HANDLER(MyApp, ATOM_AppWMEvent, onWMEvent)
  ATOM_EVENT_HANDLER(MyApp, ATOM_DeviceLostEvent, onDeviceLost)
  ATOM_EVENT_HANDLER(MyApp, ATOM_DeviceResetEvent, onDeviceReset)
  ATOM_EVENT_HANDLER(MyApp, ATOMX_TWValueChangedEvent, onTWValueChanged)
  ATOM_EVENT_HANDLER(MyApp, ATOMX_TWCommandEvent, onTWCommand)
  ATOM_UNHANDLED_EVENT_HANDLER(MyApp, onUnhandledEvent)
ATOM_END_EVENT_MAP

class MyThread: public ATOM_Thread
{
public:
	void onIdle (ATOM_ThreadIdleEvent *event)
	{
		static int n = 0;

		if (n++ == 10)
		{
			postQuitEvent (0);
		}
	}

	ATOM_DECLARE_EVENT_MAP(MyThread, ATOM_Thread)
};

ATOM_BEGIN_EVENT_MAP(MyThread, ATOM_Thread)
	ATOM_EVENT_HANDLER(MyThread, ATOM_ThreadIdleEvent, onIdle)
ATOM_END_EVENT_MAP

class MyMiniDumpWriter: public ATOM_MiniDumpWriter
{
protected:
	virtual void endWriteDump (bool succeeded)
	{
		__asm int 3
	}
};

//MyMiniDumpWriter writer;

BOOL WINAPI ConsoleHandler (DWORD event)
{
	switch (event)
	{
	case CTRL_C_EVENT:
		ATOM_LOGGER::log ("Ctrl-C\n");
		break;
	case CTRL_BREAK_EVENT:
		ATOM_LOGGER::log ("Ctrl-Break\n");
		break;
	case CTRL_CLOSE_EVENT:
		ATOM_LOGGER::log ("Ctrl-Close\n");
		break;
	case CTRL_LOGOFF_EVENT:
		ATOM_LOGGER::log ("Ctrl-Logoff\n");
		break;
	case CTRL_SHUTDOWN_EVENT:
		ATOM_LOGGER::log ("Ctrl-Shutdown\n");
		break;
	default:
		ATOM_LOGGER::log ("Unknown event (0x%08X)\n", event);
		break;
	}

	return FALSE;
}

int main(int argc, char *argv [])
{
	ATOM_LOGGER::setLogMethods (ATOM_LOGGER::getLogMethods() | ATOM_LOGGER::CONSOLE);

	MyApp app;

	app.setFrameInterval (20);

	return app.run ();
}
