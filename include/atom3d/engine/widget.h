/**	\file widget.h
 *	控件基类.
 *
 *	\author 高雅坤
 *	\addtogroup engine
 *	@{
 */

#ifndef __ATOM3D_ENGINE_WIDGET_H
#define __ATOM3D_ENGINE_WIDGET_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "../ATOM_render.h"

#include "basedefs.h"
#include "gui_misc.h"
#include "gui_events.h"
#include "gui_font.h"
#include "widgetlayout.h"
#include "imagelist.h"

#undef DEBUG_WIDGET

#if !defined(NDEBUG)&&defined(DEBUG_WIDGET)
#	define USE_WIDGET_DEBUG_INFO
#else
#	undef USE_WIDGET_DEBUG_INFO
#endif

class ATOM_GUIRenderer;
class ATOM_GUIImage;
class ATOM_GUICanvas;
class ATOM_WidgetLayout;
class ATOM_PopupMenu;
class ATOM_ScrollBar;
class ATOM_Button;
class ATOM_MenuBar;
class ATOM_WidgetPreloadedParameters;

class ATOM_ENGINE_API ATOM_WidgetBase
{
public:
	virtual ~ATOM_WidgetBase (void);

	ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOM_WidgetBase)
};

/** GUI基础控件类
\remarks
	ATOM_Widget是引擎GUI系统的所有GUI窗口的基类
\par
	ATOM_Widget类中提供了所有GUI窗口的基础功能，如层次结构，位置大小控制，滚动支持等等。
	在引擎的GUI系统中，所有的GUI窗口都处于以ATOM_Desktop控件为树根的一个树状结构中。每个
	窗口的类型可分为控件(Control)和顶层窗口(TopWindow)两种。每个窗口的渲染流程大概为：
	渲染窗口自身-->渲染所有控件子窗口-->渲染所有顶层子窗口。也就是说，父窗口在最下面，
	然后是控件类子窗口，然后是顶层子窗口在最上面。所以要控制窗口的前后次序，需要根据此
	规则来设计窗口层次结构。
	每个窗口创建的时候必须设定它的Id号(允许重复)，在GUI事件处理中可以用Id号来查询对应的
	窗口。
*/
class ATOM_ENGINE_API ATOM_Widget: public ATOM_WidgetBase
{
	friend class ATOM_GUIRenderer;

public:
	static const int AnyId = -1;
	static const int CloseButtonId = -2;
	static const int VScrollBarId = -3;
	static const int HScrollBarId = -4;
	static int fontSize;
	//! 窗口的显示状态 
	//! \see ATOM_Widget::show ATOM_Widget::getShowState
	enum ShowState
	{
		//! 隐藏
		Hide,
		//! 正常显示
		ShowNormal
	};

	//! 窗口特性旗标
	//! \see ATOM_Widget::setFlags ATOM_Widget::getFlags
	enum
	{
		//! 禁用
		Disabled = (1<<0),
		//! 使该窗口成为自身所在层级的最顶层窗口
		TopMost = (1<<1),
		//! 仅供内部使用
		TemporalDisabled = (1<<2)
	};

	enum	// align enumerations
	{
		AlignX_Left = (1<<0),
		AlignX_Middle = (1<<1),
		AlignX_Right = (1<<2),
		AlignY_Top = (1<<3),
		AlignY_Middle = (1<<4),
		AlignY_Bottom = (1<<5),
	};

	//! 窗口风格，可以在构造时设定，也可修改
	//! \see ATOM_Widget::ATOM_Widget ATOM_Widget::setStyle ATOM_Widget::getStyle
	enum
	{
		//! 窗口具有边框
		//! \see ATOM_Widget::BorderMode ATOM_Widget::setBorderMode ATOM_Widget::getBorderMode
		Border = (1<<0),
		//! 窗口具有标题栏
		TitleBar = (1<<1),
		//! 窗口具有关闭按钮
		CloseButton = (1<<2),
		//! 窗口为控件(如果不设置此标志则为顶层窗口)
		Control = (1<<3),
		//! 不会被父窗口边界所剪裁
		NoClip = (1<<4),
		//! 该窗口内部为动态内容
		DynamicContent = (1<<5),
		//! 该窗口不接受焦点
		NoFocus = (1<<6),
		//! 该窗口具有竖直滚动条
		VScroll = (1<<7),
		//! 该窗口具有水平滚动条
		HScroll = (1<<8),
		//! 当父窗口滚动的时候自己不跟随滚动
		NonScrollable = (1<<9),
		//! 裁剪子窗口到自己的客户区内
		ClipChildren = (1<<10),
		//! 滚动条在窗口左边(不设置此标记则为右边)
		LeftScroll = (1<<11),
		//! 水平滚动条在窗口上边(不设置此标记则为下边)
		TopScroll = (1<<12),
		//! 不应用父窗口的布局
		NonLayoutable = (1<<13),
		//! 私有控件
		Private = (1<<20),
		//! 弹出式浮动窗口
		Popup = (1<<21),
		//! 具有覆盖表面
		Overlay = (1<<22)
	};

	//! 窗口边框模式
	//! \see ATOM_Widget::setBorderMode ATOM_Widget::getBorderMode
	enum BorderMode
	{
		//! 平坦
		Flat,
		//! 凸起
		Raise,
		//! 凹下
		Drop
	};

	//! 拖动模式
	//! \see ATOM_Widget::setDragMode ATOM_Widget::getDragMode
	enum
	{
		//! 不允许拖动
		NoDrag = 0,
		//! 标题栏拖动
		TitleBarDrag = (1<<0),
		//! 客户区拖动
		ClientDrag = (1<<1)
	};

public:
	//! 默认构造函数
	ATOM_Widget(void);

	//! 构造函数
	//! \param parent 父窗口
	//! \param rect 窗口大小
	//! \param style 窗口风格
	//! \param id 窗口Id
	//! \param showState 窗口显示状态
	ATOM_Widget (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id = ATOM_Widget::AnyId, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);

	//! 析构函数
	//! \note 当窗口被删除时它将会从它的父窗口中被摘下来
	virtual ~ATOM_Widget (void);

public:
	//! 查询自身是不是菜单控件
	//! \return true 是 false 不是
	virtual bool isMenu (void) const;

	//! 查询自身是不是顶层窗口
	//! \return true 是 false 不是
	virtual bool isTopWindow (void) const;

	//! 鼠标点选查询
	//! \param x 鼠标相对自身客户端的x坐标
	//! \param y 鼠标相对自身客户端的y坐标
	//! \param result 查询结果
	virtual void hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest = false);

	//! 获取自身的类型
	//! \return 类型
	virtual ATOM_WidgetType getType (void) const;

	//! 渲染
	virtual void draw (void);

	//! 移到某个位置
	//! param x 目标点的x坐标(相对于父窗口客户区)
	//! param y 目标点的y坐标(相对于父窗口客户区)
	virtual void moveTo (int x, int y);

	//! 修改显示状态
	//! \param showState 显示状态
	virtual void show (ATOM_Widget::ShowState showState);

	//! 设置主菜单
	//! \param menubar 主菜单
	virtual void setMenuBar (ATOM_MenuBar *menubar);

	//! 获取当前主菜单
	//! \return 主菜单
	virtual ATOM_MenuBar *getMenuBar (void) const;

	//! 查询自身是否支持IME输入
	//! \return true 支持 false 不支持
	virtual bool enableIME (void) const;

	//! 设置字体
	virtual void setFont (ATOM_GUIFont::handle font);

	//! 取得字体
	virtual ATOM_GUIFont::handle getFont (void) const;

	//! 设置字体颜色
	virtual void setFontColor (const ATOM_ColorARGB& clr);

	//! 取得字体颜色
	virtual ATOM_ColorARGB getFontColor() const;

	//! 取得自己本身的字体
	virtual ATOM_GUIFont::handle getExactFont (void) const;

	//! 设置是否拖放源
	virtual void enableDrag (bool enable);

	//! 查询是否拖放源
	virtual bool isDragEnabled (void) const;

	//! 设置是否拖放目标
	virtual void enableDrop (bool enable);

	//! 查询是否拖放目标
	virtual bool isDropEnabled (void) const;

	//! 设置焦点
	virtual void setFocus (void);

	//! 设置父窗口
	virtual bool reparent (ATOM_Widget *newParent);

	//! 载入子级窗口列表
	virtual ATOM_Widget *loadHierarchy (const char *filename);

	//! 通过预加载的XML载入子级窗口
	virtual ATOM_Widget *loadHierarchyFromPreloadedParameters (ATOM_WidgetPreloadedParameters *params);

	//! 计算合适大小
	virtual ATOM_Size2Di calcWidgetSize (void) const;

public:
	//! 获取GUI管理器指针
	//! \return GUI管理器指针
	ATOM_GUIRenderer *getRenderer (void) const;

	//! 设置Layout类型
	void setLayoutType (ATOM_WidgetLayout::Type type);

	//! 获取layout类型
	ATOM_WidgetLayout::Type getLayoutType (void) const;

	//! 获取Layout指针
	ATOM_WidgetLayout *getLayout (void) const;

	//! 获取Layout单元
	ATOM_WidgetLayout::Unit *getLayoutUnit (void) const;

	//! 前移布局元素次序
	bool moveLayoutUnitUp (void);

	//! 后移布局元素次序
	bool moveLayoutUnitDown (void);

	//! 设置图像列表
	//! \param imagelist 图像列表
	void setImageList (ATOM_GUIImageList *imagelist);

	//! 获取图像列表
	//! \return 图像列表
	ATOM_GUIImageList *getImageList (void) const;

	//! 设置客户区图像列表Id
	//! \param id 图像列表Id
	void setClientImageId (int id);

	//! 获取客户区图像列表Id
	//! \return 客户区图像列表Id
	int getClientImageId (void) const;

	//! 设置滚动条宽度
	//! \param w 宽度
	virtual void setscrollBarWidth (int w);

	//! 获取滚动条宽度
	//! \return 宽度
	int getscrollBarWidth (void) const;

	//! 获取父窗口
	//! \return 父窗口
	ATOM_Widget *getParent (void) const;

	//! 获取自身的窗口Id
	//! \return 窗口Id
	int getId (void) const;

	//! 设置自身的窗口Id
	//! \param id 窗口Id
	void setId (int id);

	//! 获取布局大小位置
	ATOM_Rect2Di getLayoutRect (void) const;

	//! 获取窗口大小位置
	//! \return 窗口的大小位置
	const ATOM_Rect2Di &getWidgetRect (void) const;

	//! 获取客户区大小位置
	//! \return 客户区大小位置
	const ATOM_Rect2Di &getClientRect (void) const;

	//! 获取子窗口数目
	//! \return 子窗口数目
	unsigned getNumChildren (void) const;

	//! 获取指定索引的子窗口
	//! \return 子窗口指针
	ATOM_Widget *getChild (unsigned index) const;

	//! 获取第一个顶层子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getFirstChild (void) const;

	//! 获取最后一个个顶层子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getLastChild (void) const;

	//! 获取第一个控件子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getFirstControl (void) const;

	//! 获取最后一个控件子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getLastControl (void) const;

	//! 获取下一个同层次的相邻子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getNextSibling (void) const;

	//! 获取前一个同层次的相邻子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getPrevSibling (void) const;

	//! 获取窗口Id为指定值的子窗口,该方法只检查自己的直接子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getChildById (int id) const;

	//! 获取窗口Id为指定值的子窗口,该方法以广度优先的模式查找子窗口下的子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getChildByIdRecursive (int id, bool hierichyOnly = true) const;

	//! 获取窗口名称为指定字符串的子窗口,该方法只检查自己的直接子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getChildByName (const char *name) const;

	//! 获取窗口名称为指定字符串的子窗口,该方法以广度优先的模式查找子窗口下的子窗口
	//! \return 子窗口指针，如果没有返回NULL
	ATOM_Widget *getChildByNameRecursive (const char *name, bool hierichyOnly = true) const;

	//! 删除子窗口
	//! \param widget
	//! \note 该子窗口的所有子窗口也将被自动删除
	void deleteChild (ATOM_Widget *widget);

	//! 删除所有子窗口
	void clearChildren ();

	//
	ATOM_Point2Di *clientToScreen (ATOM_Point2Di *point) const;
	ATOM_Point2Di *screenToClient (ATOM_Point2Di *point) const;
	ATOM_Point2Di *clientToViewport (ATOM_Point2Di *point) const;
	ATOM_Point2Di *viewportToClient (ATOM_Point2Di *point) const;
	ATOM_Point2Di *clientToGUI (ATOM_Point2Di *point) const;
	ATOM_Point2Di *GUIToClient (ATOM_Point2Di *point) const;

	//! 设置窗口旗标
	//! \param state 旗标
	void setFlags (unsigned state);

	//! 获取窗口旗标
	//! \return 旗标
	unsigned getFlags (void) const;

	//! 设置窗口风格
	//! \param style 风格
	void setStyle (unsigned style);

	//! 获取窗口风格
	//! \return 风格
	unsigned getStyle (void) const;

	//! 设置边框模式
	//! \param mode 边框模式
	void setBorderMode (ATOM_Widget::BorderMode mode);

	//! 获取边框模式
	//! \return 边框模式
	ATOM_Widget::BorderMode getBorderMode (void) const;

	//! 设置拖动模式
	//! \param mode 拖动模式
	void setDragMode (unsigned mode);

	//! 获取拖动模式
	//! \return 拖动模式
	unsigned getDragMode (void) const;

	//! 是否控件窗口
	//! return true 是 false 不是
	bool isControl (void) const;

	//! 是否私有控件
	//! return true 是 false 不是
	bool isPrivate (void) const;

	//! 强制重绘
	void invalidate (void);

	//! 显示弹出菜单
	//! param x	弹出菜单的x坐标(相对于自身客户区)
	//! param y 弹出菜单的y坐标(相对于自身客户区)
	//! param popupMenu 弹出菜单
	void trackPopupMenu (int x, int y, ATOM_PopupMenu *popupMenu);

	//! 设置关联窗口(如果具有Popup属性,当关联窗口隐藏后自己也将隐藏)
	//! param widget 关联窗口
	void setPopupRelation (ATOM_Widget *widget);

	//! 获取关联窗口
	//! return 关联窗口
	ATOM_Widget *getPopupRelation (void) const;

	//! 捕获鼠标
	void setCapture (void);

	//! 解除捕获鼠标
	void releaseCapture (void);

	//! 是否已捕获鼠标
	//! \return true 是 false 否
	bool isCaptured (void) const;

	//! 获取窗口显示状态
	//! \return 显示状态
	ATOM_Widget::ShowState getShowState (void) const;

	//! 激活/禁止
	//! \param enable true 激活窗口 false 禁用窗口
	void enable (bool enable);

	//! 窗口是否激活
	//! \return false 窗口被禁用 true 窗口被激活
	bool isEnabled (void) const;

	//! 设置窗口是否允许被点选
	//! \param pickable true 允许 false 不允许
	void setPickable (bool pickable);

	//! 查询窗口是否允许被点选
	//! \return pickable true 允许 false 不允许
	bool isPickable (void) const;

	//! 查询鼠标是否浮于本窗口内
	//! \return true 是 false 不是
	bool isMouseHover (void) const;

	//! 查询该窗口是否支持空闲处理
	//! \return true 支持 false 不支持
	bool handleIdleEvent (void);

	//! 获取窗口的当前状态
	//! \return 当前状态
	ATOM_WidgetState getWidgetState (void) const;

	//! 更新鼠标状态
	//! \param x 鼠标x坐标
	//! \param y 鼠标y坐标
	//! \param _leftDown 鼠标左键是否按下
	//! \param _middleDown 鼠标中键是否按下
	//! \param _rightDown 鼠标右键是否按下
	//! \note 该方法仅供内部使用
	void updateMouseState (int x, int y, bool _leftDown, bool _middleDown, bool _rightDown);

	//! 更新布局
	void updateLayout (void);

	//! 播放指定Id的音效
	//! \param id 音效的Id
	void playSound(int id);

	//! 设置窗口的画布大小
	//! 当窗口画布大小大于窗口实际大小的时候可利用滚动条，或setScrollValue方法来滚动视图
	//! \param size 画布大小
	//! \see getCanvasSize setScrollValue getScrollValue
	void setCanvasSize (const ATOM_Size2Di &size);

	//! 获取当前窗口的画布大小
	//! \return 画布大小
	const ATOM_Size2Di &getCanvasSize (void) const;

	//! 设置滚动位置
	//! \param val 滚动位置
	void setScrollValue (const ATOM_Point2Di &val);

	//! 获取滚动位置
	//! \return 滚动位置
	const ATOM_Point2Di &getScrollValue (void) const;

	//! 获取竖直滚动条
	//! \return 滚动条
	ATOM_ScrollBar *getVerticalScrollBar (void) const;

	//! 获取水平滚动条
	//! \return 滚动条
	ATOM_ScrollBar *getHorizontalScrollBar (void) const;

	//! 允许布局
	void setLayoutable (bool layoutable);

	//! 查询是否允许布局
	bool isLayoutable (void) const;

	//! 是否弹出式窗口
	bool isPopup (void) const;

	//! 获取属性集合
	ATOM_UserAttributes *getAttributes (void) const;

	//! 移动该窗口一定距离
	//! \param x x方向的距离(像素)
	//! \param y y方向的距离(像素)
	void moveBy (int x, int y);

	//! 改变窗口大小位置
	//! \param rect 窗口的大小位置(相对于父窗口)
	void resize (const ATOM_Rect2Di &rect);

	//! 把该窗口提到最前
	void bringToFront (void);

	//! 摘除某个子窗口
	//! \param widget 子窗口
	//! \note 仅供内部使用
	void detach (ATOM_Widget *widget);

	//! 添加某个窗口为子窗口
	//! \param widget 要添加的子窗口
	//! \note 仅供内部使用
	bool attach (ATOM_Widget *widget);

	//! 获取指定的图像
	//! \param imageId 图像标识
	//! \return GUI图像指针或NULL
	ATOM_GUIImage *getValidImage (int imageId);

	//! 获取整窗口画布
	//! \return 画布指针
	//! \see ATOM_Widget::freeCanvas
	ATOM_GUICanvas *getWidgetCanvas (void);

	//! 获取客户区画布
	//! \return 画布指针
	//! \see ATOM_Widget::freeCanvas
	ATOM_GUICanvas *getClientCanvas (void);

	//! 释放通过getWidgetCanvas()或getClientCanvas()分配的画布对象
	//! \param canvas 已分配的画布指针
	void freeCanvas (ATOM_GUICanvas *canvas);

	//! 设置用户数据
	void setUserData(unsigned long long data);

	//! 取得用户数据
	unsigned long long getUserData() const;

	//! 设置控件名称
	void setWidgetName (const char *name);

	//! 获取控件名称
	const char *getWidgetName (void) const;

	//! 设置字体描边颜色
	void setFontOutLineColor(const ATOM_ColorARGB& clr);

	//! 获取字体描边颜色
	ATOM_ColorARGB getFontOutLineColor();

	//! 设置外部传入背景图片
	void setExClientImage(ATOM_GUIImage * image);
	//! 是否用了外部背景图片
	bool isUserExClientImage();
	//! 获取垂直滚动条的图片
	//! \param backImageId 背景图像
	//! \param sliderImageId 滚动块图像
	//! \param upButtonImageId 上按钮图像
	//! \param downButtonImageId 下按钮图像
	virtual void setVerticalSCrollBarImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId );

	//! 设置垂直滚动块的长度
	virtual void setScrollBarSliderLength(int len);

	//! 设置垂直滚动条按钮的长度
	virtual void setScrollBarButtonLength(int len);

	//! 允许投递鼠标滚轮消息
	void setWheelDeliver(bool wheelDeliver);

	//! 自动调整大小
	void compactLayout (void);

protected:
	virtual void drawSelf (void);
	virtual void drawClient (ATOM_GUICanvas *canvas, ATOM_WidgetState state);
	virtual void drawFrame (ATOM_GUICanvas *canvas, ATOM_WidgetState state);
	virtual void invalidateDisplay (void);
	virtual bool supportIME (void) const;

protected:
	void drawControls (void);
	void drawChildren (void);
	void drawOverlay (void);
	void attachControl (ATOM_Widget *widget);
	void attachNonControl (ATOM_Widget *widget);
	void detachControl (ATOM_Widget *widget);
	void detachNonControl (ATOM_Widget *widget);
	void bringChildToFront (ATOM_Widget *widget);
	void temporalEnable (void);
	void temporalDisable (void);
	void setMouseHover (bool hover);
	void notifyDynamicCounter (int change);
	void calcLayout (void);
	void scrollChildren (int x, int y);
	const ATOM_GUIImageList *getValidImageList (void);
	void invalidateLayout (void);
	void refreshLayout (void);
	void resetLayout (ATOM_Widget *parent, ATOM_WidgetLayout *newLayout);
	//--- wangjian modified ---//
	ATOM_Widget *loadHierarchyR (ATOM_Widget *widget, ATOM_Widget *root, ATOM_TiXmlElement *xml, int loadPriority);
	//-------------------------//
	ATOM_Widget *loadHierarchyFromPreloadedParametersR (ATOM_Widget *widget, ATOM_Widget *root, ATOM_WidgetPreloadedParameters *params, int loadPriority);
	void compactLayoutR (ATOM_Widget *widget);

protected:
	ATOM_GUIRenderer *_guiRenderer;
	ATOM_WidgetLayout *_layout;
	ATOM_WidgetLayout::Unit *_layoutUnit;
	int _widgetId;
	ATOM_Rect2Di _widgetRect;
	ATOM_Rect2Di _clientRect;
	ATOM_Rect2Di _titleBarRect;
	ATOM_Widget *_parent;
	ATOM_Widget *_next;
	ATOM_Widget *_prev;
	ATOM_Widget *_firstChild;
	ATOM_Widget *_lastChild;
	ATOM_Widget *_firstControl;
	ATOM_Widget *_lastControl;
	ATOM_Widget *_hierichyRoot;
	ATOM_Widget *_popupRelation;
	ShowState _showState;
	unsigned _stateFlags;
	unsigned _creationStyle;
	BorderMode _borderMode;
	unsigned _dragMode;
	int _clientImageId;
	ATOM_WidgetState _lastClientImageState;
	int _dynamicCounter;

	ATOM_Button *_closeButton;
	ATOM_MenuBar *_menuBar;
	ATOM_ScrollBar *_verticalScrollBar;
	ATOM_ScrollBar *_horizontalScrollBar;
	ATOM_GUIImage * _clientImage;
	bool _isExClientImage;
	ATOM_AUTOPTR(ATOM_GUIImageList) _imagelist;
	bool _mouseHover;
	int _handleIdleEvent;

	int _scrollBarWidth;
	ATOM_Size2Di _canvasSize;
	ATOM_Point2Di _scrollOffset;

	bool _pickable;
	bool _enableDrag;
	bool _enableDrop;
	bool _enableWheelDeliver;
	ATOM_GUIFont::handle _widgetFont;
	ATOM_ColorARGB _widgetFontColor;
	ATOM_ColorARGB _widgetFontOutLineColor;

	unsigned long long _userData;
	ATOM_STRING _name;

	ATOM_UserAttributes *_attributes;

public:
	static bool isValidWidget (ATOM_Widget *widget);
	static bool validateWidget (ATOM_Widget *widget);

#if defined(USE_WIDGET_DEBUG_INFO)
	struct WidgetDebugInfo
	{
	ATOM_WidgetType type;
	ATOM_Widget *parent;
	};
	static const char *getWidgetTypeString (ATOM_WidgetType type);
	static void registerWidgetDebugInfo (ATOM_Widget *widget);
	static const ATOM_Widget::WidgetDebugInfo *getWidgetDebugInfo (ATOM_Widget *widget);
#endif

private:
	static ATOM_HASHSET<ATOM_Widget*> _widgetSet;

public:
	void onHitTest (ATOM_WidgetHitTestEvent *event);
	void onResize (ATOM_WidgetResizeEvent *event);
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onPaintFrame (ATOM_WidgetDrawFrameEvent *event);
	void onScroll (ATOM_ScrollEvent *event);
	void onMouseWheel (ATOM_WidgetMouseWheelEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onLButtonUp (ATOM_WidgetLButtonUpEvent *event);

public:
	virtual void setEventTrigger (ATOM_EventTrigger *trigger);

	ATOM_DECLARE_EVENT_MAP(ATOM_Widget, ATOM_WidgetBase)







	//--- wangjian added for test ---//
	// 异步加载相关
public:
	enum
	{
		WIDGET_LOAD_PRIORITY_BASE = 5000,		// WIDGET的加载优先级最低为5000（非常高）
	};
	void					setAsyncLoad(int loadPriority=WIDGET_LOAD_PRIORITY_BASE);
	int						getAsyncLoad(void) const;
	bool					isAsyncLoad() const;
	void					setWaiting(bool bWaiting=false);
	bool					isWaiting();
	UINT8					initWaiting();
	static void				setWaitingImage(int imageId);

	void					enableCheckWaiting(UINT8 enable );
	UINT8					isCheckWaitingEnabled(void) const;

protected:
	enum 
	{
		WIDGET_LOAD_NOWAITING = 0,
		WIDGET_LOAD_WAITING = 1,
		WIDGET_LOAD_REWAITING = 2,
		WIDGET_LOAD_TOTAL,
	};
	enum
	{
		WAITING_MODE_NOWAITING = 0,
		WAITING_MODE_SHOWNONE = 1,
		WAITING_MODE_SHOWWAITINGUI = 2,
	};

	void					submitLoadRequest();
	bool					checkWaiting();
	void					reWaiting();
	void					drawWaitingImage();
	int						_loadPriority;
	UINT8					_bWaiting;

	UINT8					_WaitingMode;

	UINT8					_bLoadRequestSubmitted;

	UINT8					_bCheckWaiting;

	static ATOM_WeakPtrT<ATOM_GUIImage>	_imageWaiting;
	static int				_imageWaitingId;
	//-------------------------------//
};

#endif // __ATOM3D_ENGINE_WIDGET_H
/*! @} */
