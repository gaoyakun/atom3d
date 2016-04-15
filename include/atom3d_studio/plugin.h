/**	\file plugin.h
 *	AS_Plugin类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup studio
 *	@{
 */

#ifndef __ATOM3D_STUDIO_PLUGIN_H
#define __ATOM3D_STUDIO_PLUGIN_H

#if _MSC_VER > 1000
# pragma once
#endif

#define AS_FILETYPE_CANLOADASNODE	(1<<0)
#define AS_FILETYPE_CANEDIT			(1<<1)

class AS_Editor;

//! \class AS_Plugin
//! 用户扩展组件接口
//! \author 高雅昆
//! \ingroup studio
class AS_Plugin
{
public:
	//! 析构函数 
	virtual ~AS_Plugin (void) {}

	//! 查询组件的版本号
	//! \return 版本号
	virtual unsigned getVersion (void) const = 0;

	//! 查询组件的名称
	//! \return 组件名称
	virtual const char *getName (void) const = 0;

	//! 删除组件
	virtual void deleteMe (void) = 0;

	//! 初始化组件
	//! 当组件DLL被加载后此函数会被调用
	//! \param editor 编辑器功能接口指针，如果需要的话应当保留此指针以备使用
	virtual bool initPlugin (AS_Editor *editor) = 0;

	//! 卸载组件
	//! 在这里做组件卸载的清理工作
	virtual void donePlugin (void) = 0;

	//! 开始编辑新的文档
	//! \param filename 文件名，如果为NULL则表示是新建的文档尚未保存，否则为通过文件菜单打开的文档。
	//! \return true成功 false失败
	virtual bool beginEdit (const char *filename) = 0;

	//! 结束当前文档的编辑
	//! 当用户选择文件菜单中的关闭或新建或打开新的文档时调用此接口
	virtual void endEdit (void) = 0;

	//! 保存文档
	//! \param filename 文件名
	//! \return true成功 false失败
	virtual bool saveFile (const char *filename) = 0;

	//! 每帧更新时被调用
	virtual void frameUpdate (void) = 0;

	//! 处理事件
	//! 大多数系统事件都会分发到这里
	//! \param event 事件
	virtual void handleEvent (ATOM_Event *event) = 0;

	//! 处理在几何变换编辑模式下节点变换事件
	//! \param node 节点
	virtual void handleTransformEdited (ATOM_Node *node) = 0;

	//! 处理场景参数变化
	virtual void handleScenePropChanged (void) = 0;

	//! 查询使用该组件需要的最小窗口宽度
	//! \return 窗口宽度
	virtual unsigned getMinWindowWidth (void) const = 0;

	//! 查询使用该组件需要的最小窗口高度
	//! \return 窗口高度
	virtual unsigned getMinWindowHeight (void) const = 0;

	//! 查询文档是否有修改过.
	//! 当主框架被要求关闭当前文档(新文档被创建，或程序要退出时)会调用此接口查询文档是否改变，如改变会弹出是否保存的对话框，如果使用了AS_Editor::setDocumentModified()接口的话，可调用AS_Editor::isDocumentModified()函数来查询
	//! \return true修改过 false未修改
	virtual bool isDocumentModified (void) = 0;

	//! 渲染管线发生改变
	//! \param newScheme 新渲染管线
	virtual void changeRenderScheme (void) = 0;
};

#endif // __ATOM3D_STUDIO_PLUGIN_H

/*! @} */
