/**	\file editor.h
 *	AS_Editor�������.
 *
 *	\author ������
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_EDITOR_H
#define __ATOM3D_STUDIO_EDITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_engine.h"

//! �ļ��˵�IDֵ
#define AS_MENUITEM_FILE	0
//! �༭�˵�IDֵ
#define AS_MENUITEM_EDIT	1
//! �۲�˵�IDֵ
#define AS_MENUITEM_VIEW	2
//! ��Ⱦ�˵�IDֵ
#define AS_MENUITEM_RENDER	3
//! ���߲˵�IDֵ
#define AS_MENUITEM_TOOL	4
//! �Զ���˵�IDֵ
//! ��չ�����Ҫ����Զ���˵�Ӧ����������IDλ��
#define AS_MENUITEM_CUSTOM	5

//! ��չ����Ŀؼ��˵�ʹ�õ�idӦ���ڴ�ID֮��
#define PLUGIN_ID_START	500

//! �Զ�����������
enum AS_PropertyType
{
	AS_PROP_TYPE_NONE = -1,
	AS_PROP_TYPE_INT = 0,
	AS_PROP_TYPE_FLOAT = 1,
	AS_PROP_TYPE_STRING = 2,
	AS_PROP_TYPE_VECTOR = 3,
	AS_PROP_TYPE_COLOR4F = 4,
	AS_PROP_TYPE_SWITCH = 5
};

//! �Զ������Խṹ
struct AS_PropertyInfo
{
	ATOM_STRING name;
	AS_PropertyType type;
};

class AS_CameraModal;
class AS_Plugin;
class AS_AssetManager;
class AS_RenderManager;
class AS_Asset;
class AS_Operation;

typedef void * AS_AccelKey;

//! \class AS_Editor
//! �༭����Ҫ���ܽӿ�
//! �ýӿ����ṩ�˾�������ѷ�װ�õı༭�����Լ���һЩ�ڲ��ؼ���������Ĳ����ӿ�
//! \author ������
//! \ingroup studio
class AS_Editor
{
public:
	//! ��������
	virtual ~AS_Editor (void) {}

	//! ��ȡ�༭���ڲ��汾
	//! \return �汾��
	virtual unsigned getVersion (void) const = 0;

	//! ��ȡ��Ⱦ����ָ��
	//! \return ��Ⱦ����ָ��
	virtual ATOM_RenderWindow *getRenderWindow (void) const = 0;

	//! ��ȡGUI��Ⱦ��ָ��
	//! \return GUI��Ⱦ��ָ��
	virtual ATOM_GUIRenderer *getGUIRenderer (void) const = 0;

	//! ��ȡ��Ⱦ�ؼ�ָ��
	//! \return ��Ⱦ�ؼ�ָ��
	virtual ATOM_RealtimeCtrl *getRealtimeCtrl (void) const = 0;

	//! ��ȡ���˵�
	//! \return ���˵�
	virtual ATOM_MenuBar *getMenuBar (void) const = 0;

	//! ��ȡ������ӿ�ָ��
	//! \return ������ӿ�ָ��
	virtual AS_CameraModal *getCameraModal (void) const = 0;

	//! ����������ӿ�ָ��
	//! \param cameraModal ������ӿ�ָ��
	virtual void setCameraModal (AS_CameraModal *cameraModal) = 0;

	//! ע��ɱ༭�ļ�����
	//! ͨ���˺���ע����ļ����Ϳ�ͨ���ļ��˵����򿪣���������������б༭����
	//! \param plugin ��չ���ʵ��ָ��
	//! \param ext �ļ���չ��
	//! \param desc �ļ���������
	//! \param editFlags ��־����
	//! \return true�ɹ� falseʧ��
	virtual bool registerFileType (AS_Plugin *plugin, const char *ext, const char *desc, unsigned editFlags) = 0;

	//! ע���Զ�����Ⱦ������
	//! \param manager ��Ⱦ������ʵ��ָ��
	//! \param name ����������
	//! \return true�ɹ� falseʧ��
	virtual bool registerRenderManager (AS_RenderManager *manager, const char *name) = 0;

	//! ע����Դ������
	//! ͨ���˺���ע�����Դ������Դ����н��в���
	//! \param manager ������ָ��
	//! \param name ��Դ����
	//! \return true�ɹ� falseʧ��
	virtual bool registerAssetManager (AS_AssetManager *manager, const char *name) = 0;

	//! ����һ����Դ
	//! \param name ��Դ����������, �����ƾ��ǵ���registerAssetManager�����ĵڶ�������
	//! \return ��Դʵ��ָ��
	//! \sa registerAssetManager
	virtual AS_Asset *createAsset (const char *name) = 0;

	//! ͨ���ļ�����һ����Դ
	//! \param filename �ļ���
	//! \return ��Դʵ��ָ��
	virtual AS_Asset *createAssetByFileName (const char *filename) = 0;

	//! ��༭����ʾ�ļ��򿪶Ի���ѡ��֧�ֵ���Դ�ļ�
	//! \param multi �Ƿ��ѡ
	//! \param save �Ƿ����ڱ���
	//! \return �û�ѡ����ļ���Ŀ
	virtual unsigned getOpenAssetFileNames (bool multi, bool save, const char *title = 0) = 0;

	//! ��ȡ�û�ѡ����ļ���
	//! �ú�����getOpenAssetFileNames�������ú���Ч
	//! \param index �ļ���������
	//! \return �ļ���
	virtual const char *getOpenedAssetFileName (unsigned index) = 0;

	//! ��༭����ʾ�ļ��򿪶Ի���ѡ��ͼ���ļ�
	//! \param multi �Ƿ��ѡ
	//! \param save �Ƿ����ڱ���
	//! \return �û�ѡ����ļ���Ŀ
	virtual unsigned getOpenImageFileNames (bool multi, bool save, const char *title = 0) = 0;

	//! ��ȡ�û�ѡ���ͼ���ļ���
	//! �ú�����getOpenImageFileNames�������ú���Ч
	//! \param index �ļ���������
	//! \return �ļ���
	virtual const char *getOpenedImageFileName (unsigned index) = 0;

	//! ��༭����ʾ�ļ��򿪶Ի���ѡ���Զ����ļ�
	//! \param ext Ĭ�ϵ���չ��
	//! \param filter �ļ���չ��ɸѡ�ַ���
	//! \param multi �Ƿ��ѡ
	//! \param save �Ƿ����ڱ���
	//! \return �û�ѡ����ļ���Ŀ
	virtual unsigned getOpenFileNames (const char *ext, const char *filter, bool multi, bool save, const char *title = 0) = 0;

	//! ��ȡ�û�ѡ����Զ����ļ���
	//! �ú�����getOpenFileNames�������ú���Ч
	//! \param index �ļ���������
	//! \return �ļ���
	virtual const char *getOpenedFileName (unsigned index) = 0;

	//! ���õ�ǰ��Ⱦ������
	//! \param name
	//! \return true�ɹ� falseʧ��
	virtual bool setRenderManager (const char *name) = 0;

	//! ��ȡ��ǰ����Ⱦ����������
	//! \return ���ƣ����ΪĬ�ϵ���Ⱦ�������򷵻�NULL
	virtual const char *getRenderManager (void) = 0;

	//! ץȡ�������
	virtual void setCapture (void) = 0;

	//! �ͷ��������
	virtual void releaseCapture (void) = 0;

	//! ��ʾ��������Դ�������
	//! \param show true��ʾ false����
	virtual void showAssetEditor (bool show) = 0;

	//! ��ʾ�����س������Ա༭�����
	//! \param show true��ʾ false����
	virtual void showScenePropEditor (bool show) = 0;

	//! ���ó������Ա༭������λ��
	//! \param x ���Ͻ�����Ⱦ�ؼ��ͻ����ڵ�x����
	//! \param y ���Ͻ�����Ⱦ�ؼ��ͻ����ڵ�y����
	//! \param w ���Ŀ��
	//! \param h ���ĸ߶�
	virtual void setScenePropEditorPosition (int x, int y, int w, int h) = 0;

	//! ���ó������Ա༭��������־
	virtual void resetScenePropEditor (void) = 0;

	//! ˢ�³������Ա༭���
	virtual void refreshScenePropEditor (void) = 0;

	//! ���ó������Ա༭���༭�ĳ�������
	//! \param scene Ҫ�༭�ĳ�������
	virtual void setScenePropEditorTarget (ATOM_DeferredScene *scene) = 0;

	//! ������Դ����λ��
	//! \param x ���Ͻ�����Ⱦ�ؼ��ͻ����ڵ�x����
	//! \param y ���Ͻ�����Ⱦ�ؼ��ͻ����ڵ�y����
	//! \param w ���Ŀ��
	//! \param h ���ĸ߶�
	virtual void setAssetEditorPosition (int x, int y, int w, int h) = 0;

	//! ��/�ر�ɫ�����߱༭��
	//! \param show true�� false�ر�
	//! \param effect Ҫ�༭��ɫ������
	virtual void showColorGradingEditor (bool show, ATOM_ColorGradingEffect *effect) = 0;

	//! �����¼�
	//! �˽ӿڻ��Զ�������
	//! \param event �¼�
	virtual void handleEvent (ATOM_Event *event) = 0;

	//! ��ȡ��굱ǰλ��
	//! \return ��굱ǰ����Ⱦ�ؼ��ͻ�����λ��
	virtual ATOM_Point2Di getMousePosition (void) const = 0;

	//! ��ȡ��ǰ���༭���ļ���
	//! \return ��ǰ�༭���ĵ��ļ���
	virtual const char *getDocumentFileName (void) const = 0;

	//! ���õ�ǰ���༭���ĵ����޸ı�־.
	//! �����ǰ�༭���ĵ����޸ı�־���趨�������ĵ���Ҫ���رյ�ʱ��ᵯ���Ի���ѯ���Ƿ���Ҫ�����ĵ�
	//! \param modified true�趨�޸ı�־ falseȡ���޸ı�־
	virtual void setDocumentModified (bool modified) = 0;

	//! ��ѯ��ǰ�༭�ĵ����޸ı�־
	//! \return true���޸� falseδ�޸�
	virtual bool isDocumentModified (void) const = 0;

	//! ��һ���ɳ����������ı༭����
	//! \param op ����
	virtual void doEditOp (AS_Operation *op) = 0;

	//! ��һ�������޸ĵĿɳ�������
	//! \param object �޸Ķ���
	//! \param propName ������
	//! \param oldValue �޸�ǰ��ֵ
	//! \param newValue �޸ĺ��ֵ
	virtual void doObjectAttribModifyOp (ATOM_Object *object, const char *propName, const ATOM_ScriptVar &oldValue, const ATOM_ScriptVar &newValue) = 0;

	//! ��һ���ڵ�任�Ŀɳ�������
	//! \param node �ڵ�
	//! \param oldMatrix ԭ���ı任����
	//! \param newMatrix �µı任����
	virtual void doNodeTransformOp (ATOM_Node *node, const ATOM_Matrix4x4f &oldMatrix, const ATOM_Matrix4x4f &newMatrix) = 0;

	//! ��һ���ڵ㴴���Ŀɳ�������
	//! \param nodeChild �������Ľڵ�
	//! \param nodeParent �ýڵ�ĸ��ڵ�
	virtual void doNodeCreateOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent) = 0;

	//! ��һ���ڵ�ɾ���Ŀɳ�������
	//! \param nodeChild Ҫɾ���Ľڵ�
	//! \param nodeParent �ýڵ�ĸ��ڵ�
	virtual void doNodeDeleteOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent) = 0;

	//! ���������Ƿ��пɳ����Ķ���
	//! \return true�� false��
	virtual bool canUndo (void) const = 0;

	//! ִ��һ����������
	//! \return true�ɹ� falseʧ��
	virtual bool undo (void) = 0;

	//! �����������Ƿ��п������Ķ���
	//! \return true�� false��
	virtual bool canRedo (void) const = 0;

	//! ִ��һ�������Ķ���
	//! \return true�ɹ� falseʧ��
	virtual bool redo (void) = 0;

	//! ע��һ�����ټ�
	//! �ü��ټ�������GUI������¼�֮ǰ����⣬��⵽��������ATOM_WidgetCommand�¼�
	//! \param key ��ֵ
	//! \param keymod �޸ļ�ֵ
	//! \param commandId ����id
	//! \return ���ټ����
	virtual AS_AccelKey registerAccelKeyCommand (ATOM_Key key, int keymod, int commandId) = 0;

	//! ע��һ�����ټ�
	//! \param accelKey ���ټ����
	virtual void unregisterAccelKeyCommand (AS_AccelKey accelKey) = 0;

	//! �趨һ����ʱ�ڵ�����.
	//! ���趨Ϊ��ʱ�ڵ����͵Ľڵ㽫���ᱻ�����ڳ�������
	//! \param classname ����
	virtual void markDummyNodeType (const char *classname) = 0;

	//! ��ѯĳ�������Ƿ�ע��Ϊ��ʱ����
	//! \param classname ����
	//! \return true��ע�� falseδע��
	virtual bool isDummyNodeType (const char *classname) = 0;

	//! ��ʼ����һ���ڵ��ƽ�Ʊ༭״̬
	//! \param node Ҫ�༭�Ľڵ�
	//! \return true�ɹ� falseʧ��
	virtual bool beginEditNodeTranslation (ATOM_Node *node) = 0;

	//! ��ʼ����һ���ڵ����ת�༭״̬
	//! \param node Ҫ�༭�Ľڵ�
	//! \return true�ɹ� falseʧ��
	virtual bool beginEditNodeRotation (ATOM_Node *node) = 0;

	//! ��ʼ����һ���ڵ�����ű༭״̬
	//! \param node Ҫ�༭�Ľڵ�
	//! \param forceUniformScale  �Ƿ�ǿ��Ϊ�ȱ�������
	//! \return true�ɹ� falseʧ��
	virtual bool beginEditNodeScaling (ATOM_Node *node, bool forceUniformScale = false) = 0;

	//! ��ѯ�Ƿ��ڽڵ��ƽ�Ʊ༭״̬
	//! \return true�� false��
	virtual bool isEditingNodeTranslation (void) const = 0;

	//! ��ѯ�Ƿ��ڽڵ����ת�༭״̬
	//! \return true�� false��
	virtual bool isEditingNodeRotation (void) const = 0;

	//! ��ѯ�Ƿ��ڽڵ�����ű༭״̬
	//! \return true�� false��
	virtual bool isEditingNodeScaling (void) const = 0;

	//! ��ѯ�Ƿ��ڽڵ�ı任����༭״̬(ƽ�ƣ���ת������)
	//! \return true�� false��
	virtual bool isEditingNodeTransform (void) const = 0;

	//! �����ڵ�ľ���༭״̬
	virtual void endEditNodeTransform (void) = 0;

	//! ��������ռ��ڵĳ��Ȼ��㵽��Ļ�ռ��������
	//! \param locationWorld ����ռ��о���������ľ���
	//! \param distanceWorld ����ռ��ڵĳ���
	//! \return ��Ļ�ռ��������
	virtual float measureScreenDistance (const ATOM_Vector3f &locationWorld, float distanceWorld) const = 0;

	//! Ϊĳ���ڵ���ʾ������������
	//! \param node �ڵ�
	//! \param show true��ʾ false����
	//! \param minAxisLength ���������С����
	virtual void showNodeAxis (ATOM_Node *node, bool show, float minAxisLength = 100.f) = 0;

	//! ��Ⱦ��3D��ͼ
	virtual void renderScene (void) = 0;

	//! �۲�ĳ���ڵ�
	//! \param node Ҫ�۲�Ľڵ�
	virtual void lookAtNode (ATOM_Node *node) = 0;

	//! ����һ���������;��
	//! \return ���;��������������Ѿ���ʹ���򷵻�AS_Clipboard::CONTENT_TYPE_INVALID
	virtual int allocClipboardContentType (const char *name) = 0;

	//! ͨ�����ƻ�ȡ�������;��
	//! \param name ���͵�����
	//! \return ���;��,���������Ч�򷵻�AS_Clipboard::CONTENT_TYPE_INVALID
	virtual int getClipboardContentType (const char *name) const = 0;

	//! ͨ���������;����ȡ��������
	//! \param type ���;��
	//! \return �������ƣ��������Ч����򷵻�NULL
	virtual const char *getClipboardContentName (int type) const = 0;

	//! ����ָ���������ݵ��ڲ�������
	//! \param contentType ���ݵ�����
	//! \param data ���ݻ�����ָ��
	//! \param dataSize ���ݻ��������ȣ���λΪ�ֽ�
	//! \return true�ɹ� falseʧ��
	virtual bool setClipboardData (int contentType, const void *data, unsigned dataSize) = 0;

	//! ����ָ���������Ƶ����ݵ��ڲ�������
	//! \param name ��������
	//! \param data ���ݻ�����ָ��
	//! \param dataSize ���ݻ��������ȣ���λΪ�ֽ�
	//! \return true�ɹ� falseʧ��
	virtual bool setClipboardDataByName (const char *name, const void *data, unsigned dataSize) = 0;

	//! ��ȡ�ڲ����������ݳ���
	//! \return ����
	virtual unsigned getClipboardDataLength (void) const = 0;

	//! ��ȡ�ڲ�����������
	//! \param data ������ָ��������ŷ��صļ��������ݣ��û�������С���벻С�����ݵĳ��ȣ�Ҳ������NULL
	//! \return ���������ݵ�����
	virtual int getClipboardData (void *data) const = 0;

	//! ����ڲ�������
	virtual void emptyClipboard (void) = 0;

	// wangjian modified
#if 0
	//! ��ȡRenderScheme
	virtual ATOM_RenderScheme *getRenderScheme (void) const = 0;
#else
	virtual ATOM_RenderScheme *getRenderScheme (void) = 0;
#endif

	//! ��ʾ�����Ի���
	virtual bool showRenameDialog (ATOM_STRING &name) = 0;

	//! ��ʾ�½����ԶԻ���
	virtual bool showNewPropertyDialog (AS_PropertyInfo *info) = 0;
};

#endif // __ATOM3D_STUDIO_EDITOR_H

/*! @} */
