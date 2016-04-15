#ifndef ES_PLUGIN_H
#define ES_PLUGIN_H


enum ColorMode
{
  ColorMode_Add,
  ColorMode_Add_Minus_Half,
  ColorMode_Mulitply
};

enum PlugInType
{
  PlugIn_Export,  //export something
  PlugIn_Import,  //import something to the current terrain format 
  PlugIn_Generate  //generate something pretty (alter heightmap or texture maps)
};

//Map data accessible to the plugin
//Note that you cannnot modify the map size or dimensions of the textures
//A plugin must operate on the size and parameters of the current map
struct MapData
{
  //r = read only
  //w = write  
  //r/w = read/write

  int     m_heightMapSize;  //r
  float*  m_pHeightMap;     //r/w
  float   m_heightMapScale; //r/w the maximum possible height of the map
  char*   m_mapName;    //r 
  float   m_maxHeight;  //r height of the heighest vertex
  float   m_minHeight;  //r height of the lowest vertex
  bool    m_bRecalculateLighting; //w for import/generate only (recalculates lighting after modifying the heightmap)

  bool    m_bWater;      //r/w
  float   m_waterHeight; //r/w
  Color   m_waterColor;  //r/w

  int     m_lightMapSize; //r
  bool    m_bRGBLightMap; //r
  BYTE*   m_pLightMap; //r/w 1bpp or 3bpp (if m_bRGBLightMap is true)
  int     m_detailMapSize; //r
  BYTE*   m_pDetailMap; //r/w 4bpp (rgba channels indicating ratio value of each detail texture)
  BYTE*   m_pDetailMap2; //r/w 4bbp (null if only 4 detaill textures present)
  int     m_colorMapSize; //r
  BYTE*   m_pColorMap;//r/w 3bpp
  ColorMode m_colorMode;//r/w

  int     m_numDetailTextures; //r 4 or 8
  Image*  m_pDetailTextures[8]; //r
  float   m_detailTextureScale[8]; //r

  Color   m_ambient; //r/w
  Color   m_light; //r/w
  float   m_lightDirection[3]; //r/w xyz

  float   m_fogNear; //r/w
  float   m_fogFar;  //r/w
  Color   m_fogColor; //r/w
  Color   m_backgroundColor; //r/w

  float * m_pNormals; //r xyz vertex normals of heightmap (only if requested)
};

// diffrent types of UI controls
enum UIType
{
  UI_LABEL,
  UI_FLOAT,
  UI_INT,
  UI_CHECK_BOX,
  UI_LIST_BOX,
};


typedef void (*UICallback) ();

// UI control type List Box
struct UIListBox
{
  int m_currentIndex;
  int m_fields;
  char **m_sFields; // [m_fields][] array of strings

  UIListBox()
  {
     m_currentIndex = 0;
     m_fields = 0;
     m_sFields = NULL;
  }

  ~UIListBox()
  {
    if(m_sFields)
    {
      for(int i=0; i<m_fields; ++i)
        delete [] m_sFields[i];

      delete [] m_sFields;
    }
  }
};

// a UI Control if using UI in your plugin
struct UIControl
{  
  char        m_sName[64];
  UIType      m_type;
  bool        m_bEnabled;
  void*       m_pValue;
  float       m_minValue;
  float       m_maxValue;
  UICallback  m_callback; //for checkboxes/buttons (you can turn off other checkboxes by changing their bool values)
  DWORD       m_uiHandle; //do not touch this! (you will be sorry!)

  UIControl(char* sName, UIType type, void* pValue)
  {
    strcpy(m_sName,sName);
    m_type = type;
    m_pValue = pValue;
    m_bEnabled = true;
    m_minValue = 0;
    m_maxValue = 1;
    m_uiHandle = 0;
    m_callback = NULL;
  }
};

// mask bits for requesting data
#define DATA_REQUEST_NONE        0
#define DATA_REQUEST_HEIGHT      1
#define DATA_REQUEST_DETAILMAP   2
#define DATA_REQUEST_COLORMAP    4
#define DATA_REQUEST_LIGHTMAP    8
#define DATA_REQUEST_NORMALS     16

// mask bits for modifying data
#define DATA_MODIFY_NONE        0
#define DATA_MODIFY_HEIGHT      1
#define DATA_MODIFY_DETAILMAP   2
#define DATA_MODIFY_COLORMAP    4
#define DATA_MODIFY_LIGHTMAP    8


struct ESPlugIn
{
  char              m_sName[64]; //name please?
  PlugInType        m_type;   // choose your plugin type
  bool              m_bUseUI; //use a ui (it's optional)
  bool              m_bStickyUI; //keep plugin UI after running plugin
  char              m_fileDescription[64]; //for import/export file dialog
  char              m_filePath[MAX_PATH];  //import/export filename
  UIControl*        m_controls[20]; //max 20 ui controls in the dialog
  int               m_numControls;
  UINT              m_dataRequest; 
  UINT              m_dataModify; //only applicable for import and generate plugins
  MapData*          m_mapData; //map data, gets passed core data on plugin Run
};

typedef bool (*PFUNCES)(ESPlugIn& out_plugIn);

extern "C" ES_PLUGIN  float Version();
extern "C" ES_PLUGIN  bool Init(ESPlugIn& out_plugIn); //initilize plugin and ui 
extern "C" ES_PLUGIN  bool Run(ESPlugIn& in_plugIn); //run the plugin, pray it doesn't crash
extern "C" ES_PLUGIN  bool Die(ESPlugIn& in_plugIn); //deallocate your loved ones


#endif