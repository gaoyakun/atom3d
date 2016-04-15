/**	\file editor.h
 *	AS_Editor类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_EDITOR_H
#define __ATOM3D_STUDIO_EDITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_engine.h"

//! 文件菜单ID值
#define AS_MENUITEM_FILE	0
//! 编辑菜单ID值
#define AS_MENUITEM_EDIT	1
//! 观察菜单ID值
#define AS_MENUITEM_VIEW	2
//! 渲染菜单ID值
#define AS_MENUITEM_RENDER	3
//! 工具菜单ID值
#define AS_MENUITEM_TOOL	4
//! 自定义菜单ID值
//! 扩展组件需要添加自定义菜单应当添加在这个ID位置
#define AS_MENUITEM_CUSTOM	5

//! 扩展组件的控件菜单使用的id应当在此ID之后
#define PLUGIN_ID_START	500

//! 自定义属性类型
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

//! 自定义属性结构
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
//! 编辑器主要功能接口
//! 该接口内提供了绝大多数已封装好的编辑功能以及对一些内部控件，摄像机的操作接口
//! \author 高雅昆
//! \ingroup studio
class AS_Editor
{
public:
	//! 析构函数
	virtual ~AS_Editor (void) {}

	//! 获取编辑器内部版本
	//! \return 版本号
	virtual unsigned getVersion (void) const = 0;

	//! 获取渲染窗口指针
	//! \return 渲染窗口指针
	virtual ATOM_RenderWindow *getRenderWindow (void) const = 0;

	//! 获取GUI渲染器指针
	//! \return GUI渲染器指针
	virtual ATOM_GUIRenderer *getGUIRenderer (void) const = 0;

	//! 获取渲染控件指针
	//! \return 渲染控件指针
	virtual ATOM_RealtimeCtrl *getRealtimeCtrl (void) const = 0;

	//! 获取主菜单
	//! \return 主菜单
	virtual ATOM_MenuBar *getMenuBar (void) const = 0;

	//! 获取摄像机接口指针
	//! \return 摄像机接口指针
	virtual AS_CameraModal *getCameraModal (void) const = 0;

	//! 设置摄像机接口指针
	//! \param cameraModal 摄像机接口指针
	virtual void setCameraModal (AS_CameraModal *cameraModal) = 0;

	//! 注册可编辑文件类型
	//! 通过此函数注册的文件类型可通过文件菜单来打开，并调用组件来进行编辑操作
	//! \param plugin 扩展组件实例指针
	//! \param ext 文件扩展名
	//! \param desc 文件类型描述
	//! \param editFlags 标志变量
	//! \return true成功 false失败
	virtual bool registerFileType (AS_Plugin *plugin, const char *ext, const char *desc, unsigned editFlags) = 0;

	//! 注册自定义渲染管理器
	//! \param manager 渲染管理器实例指针
	//! \param name 管理器名称
	//! \return true成功 false失败
	virtual bool registerRenderManager (AS_RenderManager *manager, const char *name) = 0;

	//! 注册资源管理器
	//! 通过此函数注册的资源可在资源面板中进行操作
	//! \param manager 管理器指针
	//! \param name 资源名称
	//! \return true成功 false失败
	virtual bool registerAssetManager (AS_AssetManager *manager, const char *name) = 0;

	//! 创建一个资源
	//! \param name 资源管理器名称, 该名称就是调用registerAssetManager函数的第二个参数
	//! \return 资源实例指针
	//! \sa registerAssetManager
	virtual AS_Asset *createAsset (const char *name) = 0;

	//! 通过文件创建一个资源
	//! \param filename 文件名
	//! \return 资源实例指针
	virtual AS_Asset *createAssetByFileName (const char *filename) = 0;

	//! 令编辑器显示文件打开对话框选择支持的资源文件
	//! \param multi 是否多选
	//! \param save 是否用于保存
	//! \return 用户选择的文件数目
	virtual unsigned getOpenAssetFileNames (bool multi, bool save, const char *title = 0) = 0;

	//! 获取用户选择的文件名
	//! 该函数在getOpenAssetFileNames函数调用后有效
	//! \param index 文件名的索引
	//! \return 文件名
	virtual const char *getOpenedAssetFileName (unsigned index) = 0;

	//! 令编辑器显示文件打开对话框选择图像文件
	//! \param multi 是否多选
	//! \param save 是否用于保存
	//! \return 用户选择的文件数目
	virtual unsigned getOpenImageFileNames (bool multi, bool save, const char *title = 0) = 0;

	//! 获取用户选择的图像文件名
	//! 该函数在getOpenImageFileNames函数调用后有效
	//! \param index 文件名的索引
	//! \return 文件名
	virtual const char *getOpenedImageFileName (unsigned index) = 0;

	//! 令编辑器显示文件打开对话框选择自定义文件
	//! \param ext 默认的扩展名
	//! \param filter 文件扩展名筛选字符串
	//! \param multi 是否多选
	//! \param save 是否用于保存
	//! \return 用户选择的文件数目
	virtual unsigned getOpenFileNames (const char *ext, const char *filter, bool multi, bool save, const char *title = 0) = 0;

	//! 获取用户选择的自定义文件名
	//! 该函数在getOpenFileNames函数调用后有效
	//! \param index 文件名的索引
	//! \return 文件名
	virtual const char *getOpenedFileName (unsigned index) = 0;

	//! 设置当前渲染管理器
	//! \param name
	//! \return true成功 false失败
	virtual bool setRenderManager (const char *name) = 0;

	//! 获取当前的渲染管理器名称
	//! \return 名称，如果为默认的渲染管理器则返回NULL
	virtual const char *getRenderManager (void) = 0;

	//! 抓取鼠标输入
	virtual void setCapture (void) = 0;

	//! 释放鼠标输入
	virtual void releaseCapture (void) = 0;

	//! 显示或隐藏资源管理面板
	//! \param show true显示 false隐藏
	virtual void showAssetEditor (bool show) = 0;

	//! 显示或隐藏场景属性编辑器面板
	//! \param show true显示 false隐藏
	virtual void showScenePropEditor (bool show) = 0;

	//! 设置场景属性编辑器面板的位置
	//! \param x 左上角在渲染控件客户区内的x坐标
	//! \param y 左上角在渲染控件客户区内的y坐标
	//! \param w 面板的宽度
	//! \param h 面板的高度
	virtual void setScenePropEditorPosition (int x, int y, int w, int h) = 0;

	//! 重置场景属性编辑器面板脏标志
	virtual void resetScenePropEditor (void) = 0;

	//! 刷新场景属性编辑面板
	virtual void refreshScenePropEditor (void) = 0;

	//! 设置场景属性编辑器编辑的场景对象
	//! \param scene 要编辑的场景对象
	virtual void setScenePropEditorTarget (ATOM_DeferredScene *scene) = 0;

	//! 设置资源面板的位置
	//! \param x 左上角在渲染控件客户区内的x坐标
	//! \param y 左上角在渲染控件客户区内的y坐标
	//! \param w 面板的宽度
	//! \param h 面板的高度
	virtual void setAssetEditorPosition (int x, int y, int w, int h) = 0;

	//! 打开/关闭色彩曲线编辑器
	//! \param show true打开 false关闭
	//! \param effect 要编辑的色彩曲线
	virtual void showColorGradingEditor (bool show, ATOM_ColorGradingEffect *effect) = 0;

	//! 处理事件
	//! 此接口会自动被调用
	//! \param event 事件
	virtual void handleEvent (ATOM_Event *event) = 0;

	//! 获取鼠标当前位置
	//! \return 鼠标当前在渲染控件客户区的位置
	virtual ATOM_Point2Di getMousePosition (void) const = 0;

	//! 获取当前所编辑的文件名
	//! \return 当前编辑的文档文件名
	virtual const char *getDocumentFileName (void) const = 0;

	//! 设置当前所编辑的文档的修改标志.
	//! 如果当前编辑的文档的修改标志被设定，当该文档需要被关闭的时候会弹出对话框询问是否需要保存文档
	//! \param modified true设定修改标志 false取消修改标志
	virtual void setDocumentModified (bool modified) = 0;

	//! 查询当前编辑文档的修改标志
	//! \return true已修改 false未修改
	virtual bool isDocumentModified (void) const = 0;

	//! 做一个可撤销或重做的编辑动作
	//! \param op 动作
	virtual void doEditOp (AS_Operation *op) = 0;

	//! 做一个属性修改的可撤销动作
	//! \param object 修改对象
	//! \param propName 属性名
	//! \param oldValue 修改前的值
	//! \param newValue 修改后的值
	virtual void doObjectAttribModifyOp (ATOM_Object *object, const char *propName, const ATOM_ScriptVar &oldValue, const ATOM_ScriptVar &newValue) = 0;

	//! 做一个节点变换的可撤销动作
	//! \param node 节点
	//! \param oldMatrix 原来的变换矩阵
	//! \param newMatrix 新的变换矩阵
	virtual void doNodeTransformOp (ATOM_Node *node, const ATOM_Matrix4x4f &oldMatrix, const ATOM_Matrix4x4f &newMatrix) = 0;

	//! 做一个节点创建的可撤销动作
	//! \param nodeChild 创建出的节点
	//! \param nodeParent 该节点的父节点
	virtual void doNodeCreateOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent) = 0;

	//! 做一个节点删除的可撤销动作
	//! \param nodeChild 要删除的节点
	//! \param nodeParent 该节点的父节点
	virtual void doNodeDeleteOp (ATOM_Node *nodeChild, ATOM_Node *nodeParent) = 0;

	//! 撤销队列是否还有可撤销的动作
	//! \return true有 false无
	virtual bool canUndo (void) const = 0;

	//! 执行一个撤销动作
	//! \return true成功 false失败
	virtual bool undo (void) = 0;

	//! 重做队列中是否还有可重做的动作
	//! \return true有 false无
	virtual bool canRedo (void) const = 0;

	//! 执行一个重做的动作
	//! \return true成功 false失败
	virtual bool redo (void) = 0;

	//! 注册一个加速键
	//! 该加速键将会在GUI处理该事件之前被检测，检测到后会产生个ATOM_WidgetCommand事件
	//! \param key 键值
	//! \param keymod 修改键值
	//! \param commandId 命令id
	//! \return 加速键句柄
	virtual AS_AccelKey registerAccelKeyCommand (ATOM_Key key, int keymod, int commandId) = 0;

	//! 注销一个加速键
	//! \param accelKey 加速键句柄
	virtual void unregisterAccelKeyCommand (AS_AccelKey accelKey) = 0;

	//! 设定一个临时节点类型.
	//! 被设定为临时节点类型的节点将不会被保存在场景树里
	//! \param classname 类名
	virtual void markDummyNodeType (const char *classname) = 0;

	//! 查询某个类型是否被注册为临时类型
	//! \param classname 类名
	//! \return true已注册 false未注册
	virtual bool isDummyNodeType (const char *classname) = 0;

	//! 开始进入一个节点的平移编辑状态
	//! \param node 要编辑的节点
	//! \return true成功 false失败
	virtual bool beginEditNodeTranslation (ATOM_Node *node) = 0;

	//! 开始进入一个节点的旋转编辑状态
	//! \param node 要编辑的节点
	//! \return true成功 false失败
	virtual bool beginEditNodeRotation (ATOM_Node *node) = 0;

	//! 开始进入一个节点的缩放编辑状态
	//! \param node 要编辑的节点
	//! \param forceUniformScale  是否强制为等比例缩放
	//! \return true成功 false失败
	virtual bool beginEditNodeScaling (ATOM_Node *node, bool forceUniformScale = false) = 0;

	//! 查询是否处于节点的平移编辑状态
	//! \return true是 false否
	virtual bool isEditingNodeTranslation (void) const = 0;

	//! 查询是否处于节点的旋转编辑状态
	//! \return true是 false否
	virtual bool isEditingNodeRotation (void) const = 0;

	//! 查询是否处于节点的缩放编辑状态
	//! \return true是 false否
	virtual bool isEditingNodeScaling (void) const = 0;

	//! 查询是否处于节点的变换矩阵编辑状态(平移，旋转或缩放)
	//! \return true是 false否
	virtual bool isEditingNodeTransform (void) const = 0;

	//! 结束节点的矩阵编辑状态
	virtual void endEditNodeTransform (void) = 0;

	//! 估算世界空间内的长度换算到屏幕空间的像素数
	//! \param locationWorld 世界空间中距离摄像机的距离
	//! \param distanceWorld 世界空间内的长度
	//! \return 屏幕空间的像素数
	virtual float measureScreenDistance (const ATOM_Vector3f &locationWorld, float distanceWorld) const = 0;

	//! 为某个节点显示或隐藏坐标轴
	//! \param node 节点
	//! \param show true显示 false隐藏
	//! \param minAxisLength 坐标轴的最小长度
	virtual void showNodeAxis (ATOM_Node *node, bool show, float minAxisLength = 100.f) = 0;

	//! 渲染主3D视图
	virtual void renderScene (void) = 0;

	//! 观察某个节点
	//! \param node 要观察的节点
	virtual void lookAtNode (ATOM_Node *node) = 0;

	//! 分配一个剪贴类型句柄
	//! \return 类型句柄，如果该名称已经被使用则返回AS_Clipboard::CONTENT_TYPE_INVALID
	virtual int allocClipboardContentType (const char *name) = 0;

	//! 通过名称获取剪贴类型句柄
	//! \param name 类型的名称
	//! \return 类型句柄,如果名称无效则返回AS_Clipboard::CONTENT_TYPE_INVALID
	virtual int getClipboardContentType (const char *name) const = 0;

	//! 通过剪贴类型句柄获取类型名称
	//! \param type 类型句柄
	//! \return 类型名称，如果是无效句柄则返回NULL
	virtual const char *getClipboardContentName (int type) const = 0;

	//! 复制指定类型数据到内部剪贴板
	//! \param contentType 内容的类型
	//! \param data 数据缓冲区指针
	//! \param dataSize 数据缓冲区长度，单位为字节
	//! \return true成功 false失败
	virtual bool setClipboardData (int contentType, const void *data, unsigned dataSize) = 0;

	//! 复制指定类型名称的数据到内部剪贴板
	//! \param name 类型名称
	//! \param data 数据缓冲区指针
	//! \param dataSize 数据缓冲区长度，单位为字节
	//! \return true成功 false失败
	virtual bool setClipboardDataByName (const char *name, const void *data, unsigned dataSize) = 0;

	//! 获取内部剪贴板内容长度
	//! \return 长度
	virtual unsigned getClipboardDataLength (void) const = 0;

	//! 获取内部剪贴板内容
	//! \param data 缓冲区指针用来存放返回的剪贴板内容，该缓冲区大小必须不小于内容的长度，也可以是NULL
	//! \return 剪贴板内容的类型
	virtual int getClipboardData (void *data) const = 0;

	//! 清空内部剪贴板
	virtual void emptyClipboard (void) = 0;

	// wangjian modified
#if 0
	//! 获取RenderScheme
	virtual ATOM_RenderScheme *getRenderScheme (void) const = 0;
#else
	virtual ATOM_RenderScheme *getRenderScheme (void) = 0;
#endif

	//! 显示改名对话框
	virtual bool showRenameDialog (ATOM_STRING &name) = 0;

	//! 显示新建属性对话框
	virtual bool showNewPropertyDialog (AS_PropertyInfo *info) = 0;
};

#endif // __ATOM3D_STUDIO_EDITOR_H

/*! @} */
