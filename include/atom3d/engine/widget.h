/**	\file widget.h
 *	�ؼ�����.
 *
 *	\author ������
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

/** GUI�����ؼ���
\remarks
	ATOM_Widget������GUIϵͳ������GUI���ڵĻ���
\par
	ATOM_Widget�����ṩ������GUI���ڵĻ������ܣ����νṹ��λ�ô�С���ƣ�����֧�ֵȵȡ�
	�������GUIϵͳ�У����е�GUI���ڶ�������ATOM_Desktop�ؼ�Ϊ������һ����״�ṹ�С�ÿ��
	���ڵ����Ϳɷ�Ϊ�ؼ�(Control)�Ͷ��㴰��(TopWindow)���֡�ÿ�����ڵ���Ⱦ���̴��Ϊ��
	��Ⱦ��������-->��Ⱦ���пؼ��Ӵ���-->��Ⱦ���ж����Ӵ��ڡ�Ҳ����˵���������������棬
	Ȼ���ǿؼ����Ӵ��ڣ�Ȼ���Ƕ����Ӵ����������档����Ҫ���ƴ��ڵ�ǰ�������Ҫ���ݴ�
	��������ƴ��ڲ�νṹ��
	ÿ�����ڴ�����ʱ������趨����Id��(�����ظ�)����GUI�¼������п�����Id������ѯ��Ӧ��
	���ڡ�
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
	//! ���ڵ���ʾ״̬ 
	//! \see ATOM_Widget::show ATOM_Widget::getShowState
	enum ShowState
	{
		//! ����
		Hide,
		//! ������ʾ
		ShowNormal
	};

	//! �����������
	//! \see ATOM_Widget::setFlags ATOM_Widget::getFlags
	enum
	{
		//! ����
		Disabled = (1<<0),
		//! ʹ�ô��ڳ�Ϊ�������ڲ㼶����㴰��
		TopMost = (1<<1),
		//! �����ڲ�ʹ��
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

	//! ���ڷ�񣬿����ڹ���ʱ�趨��Ҳ���޸�
	//! \see ATOM_Widget::ATOM_Widget ATOM_Widget::setStyle ATOM_Widget::getStyle
	enum
	{
		//! ���ھ��б߿�
		//! \see ATOM_Widget::BorderMode ATOM_Widget::setBorderMode ATOM_Widget::getBorderMode
		Border = (1<<0),
		//! ���ھ��б�����
		TitleBar = (1<<1),
		//! ���ھ��йرհ�ť
		CloseButton = (1<<2),
		//! ����Ϊ�ؼ�(��������ô˱�־��Ϊ���㴰��)
		Control = (1<<3),
		//! ���ᱻ�����ڱ߽�������
		NoClip = (1<<4),
		//! �ô����ڲ�Ϊ��̬����
		DynamicContent = (1<<5),
		//! �ô��ڲ����ܽ���
		NoFocus = (1<<6),
		//! �ô��ھ�����ֱ������
		VScroll = (1<<7),
		//! �ô��ھ���ˮƽ������
		HScroll = (1<<8),
		//! �������ڹ�����ʱ���Լ����������
		NonScrollable = (1<<9),
		//! �ü��Ӵ��ڵ��Լ��Ŀͻ�����
		ClipChildren = (1<<10),
		//! �������ڴ������(�����ô˱����Ϊ�ұ�)
		LeftScroll = (1<<11),
		//! ˮƽ�������ڴ����ϱ�(�����ô˱����Ϊ�±�)
		TopScroll = (1<<12),
		//! ��Ӧ�ø����ڵĲ���
		NonLayoutable = (1<<13),
		//! ˽�пؼ�
		Private = (1<<20),
		//! ����ʽ��������
		Popup = (1<<21),
		//! ���и��Ǳ���
		Overlay = (1<<22)
	};

	//! ���ڱ߿�ģʽ
	//! \see ATOM_Widget::setBorderMode ATOM_Widget::getBorderMode
	enum BorderMode
	{
		//! ƽ̹
		Flat,
		//! ͹��
		Raise,
		//! ����
		Drop
	};

	//! �϶�ģʽ
	//! \see ATOM_Widget::setDragMode ATOM_Widget::getDragMode
	enum
	{
		//! �������϶�
		NoDrag = 0,
		//! �������϶�
		TitleBarDrag = (1<<0),
		//! �ͻ����϶�
		ClientDrag = (1<<1)
	};

public:
	//! Ĭ�Ϲ��캯��
	ATOM_Widget(void);

	//! ���캯��
	//! \param parent ������
	//! \param rect ���ڴ�С
	//! \param style ���ڷ��
	//! \param id ����Id
	//! \param showState ������ʾ״̬
	ATOM_Widget (ATOM_Widget *parent, const ATOM_Rect2Di &rect, unsigned style, int id = ATOM_Widget::AnyId, ATOM_Widget::ShowState showState = ATOM_Widget::Hide);

	//! ��������
	//! \note �����ڱ�ɾ��ʱ����������ĸ������б�ժ����
	virtual ~ATOM_Widget (void);

public:
	//! ��ѯ�����ǲ��ǲ˵��ؼ�
	//! \return true �� false ����
	virtual bool isMenu (void) const;

	//! ��ѯ�����ǲ��Ƕ��㴰��
	//! \return true �� false ����
	virtual bool isTopWindow (void) const;

	//! ����ѡ��ѯ
	//! \param x ����������ͻ��˵�x����
	//! \param y ����������ͻ��˵�y����
	//! \param result ��ѯ���
	virtual void hitTest (int x, int y, ATOM_GUIHitTestResult *result, bool fullTest = false);

	//! ��ȡ���������
	//! \return ����
	virtual ATOM_WidgetType getType (void) const;

	//! ��Ⱦ
	virtual void draw (void);

	//! �Ƶ�ĳ��λ��
	//! param x Ŀ����x����(����ڸ����ڿͻ���)
	//! param y Ŀ����y����(����ڸ����ڿͻ���)
	virtual void moveTo (int x, int y);

	//! �޸���ʾ״̬
	//! \param showState ��ʾ״̬
	virtual void show (ATOM_Widget::ShowState showState);

	//! �������˵�
	//! \param menubar ���˵�
	virtual void setMenuBar (ATOM_MenuBar *menubar);

	//! ��ȡ��ǰ���˵�
	//! \return ���˵�
	virtual ATOM_MenuBar *getMenuBar (void) const;

	//! ��ѯ�����Ƿ�֧��IME����
	//! \return true ֧�� false ��֧��
	virtual bool enableIME (void) const;

	//! ��������
	virtual void setFont (ATOM_GUIFont::handle font);

	//! ȡ������
	virtual ATOM_GUIFont::handle getFont (void) const;

	//! ����������ɫ
	virtual void setFontColor (const ATOM_ColorARGB& clr);

	//! ȡ��������ɫ
	virtual ATOM_ColorARGB getFontColor() const;

	//! ȡ���Լ����������
	virtual ATOM_GUIFont::handle getExactFont (void) const;

	//! �����Ƿ��Ϸ�Դ
	virtual void enableDrag (bool enable);

	//! ��ѯ�Ƿ��Ϸ�Դ
	virtual bool isDragEnabled (void) const;

	//! �����Ƿ��Ϸ�Ŀ��
	virtual void enableDrop (bool enable);

	//! ��ѯ�Ƿ��Ϸ�Ŀ��
	virtual bool isDropEnabled (void) const;

	//! ���ý���
	virtual void setFocus (void);

	//! ���ø�����
	virtual bool reparent (ATOM_Widget *newParent);

	//! �����Ӽ������б�
	virtual ATOM_Widget *loadHierarchy (const char *filename);

	//! ͨ��Ԥ���ص�XML�����Ӽ�����
	virtual ATOM_Widget *loadHierarchyFromPreloadedParameters (ATOM_WidgetPreloadedParameters *params);

	//! ������ʴ�С
	virtual ATOM_Size2Di calcWidgetSize (void) const;

public:
	//! ��ȡGUI������ָ��
	//! \return GUI������ָ��
	ATOM_GUIRenderer *getRenderer (void) const;

	//! ����Layout����
	void setLayoutType (ATOM_WidgetLayout::Type type);

	//! ��ȡlayout����
	ATOM_WidgetLayout::Type getLayoutType (void) const;

	//! ��ȡLayoutָ��
	ATOM_WidgetLayout *getLayout (void) const;

	//! ��ȡLayout��Ԫ
	ATOM_WidgetLayout::Unit *getLayoutUnit (void) const;

	//! ǰ�Ʋ���Ԫ�ش���
	bool moveLayoutUnitUp (void);

	//! ���Ʋ���Ԫ�ش���
	bool moveLayoutUnitDown (void);

	//! ����ͼ���б�
	//! \param imagelist ͼ���б�
	void setImageList (ATOM_GUIImageList *imagelist);

	//! ��ȡͼ���б�
	//! \return ͼ���б�
	ATOM_GUIImageList *getImageList (void) const;

	//! ���ÿͻ���ͼ���б�Id
	//! \param id ͼ���б�Id
	void setClientImageId (int id);

	//! ��ȡ�ͻ���ͼ���б�Id
	//! \return �ͻ���ͼ���б�Id
	int getClientImageId (void) const;

	//! ���ù��������
	//! \param w ���
	virtual void setscrollBarWidth (int w);

	//! ��ȡ���������
	//! \return ���
	int getscrollBarWidth (void) const;

	//! ��ȡ������
	//! \return ������
	ATOM_Widget *getParent (void) const;

	//! ��ȡ����Ĵ���Id
	//! \return ����Id
	int getId (void) const;

	//! ��������Ĵ���Id
	//! \param id ����Id
	void setId (int id);

	//! ��ȡ���ִ�Сλ��
	ATOM_Rect2Di getLayoutRect (void) const;

	//! ��ȡ���ڴ�Сλ��
	//! \return ���ڵĴ�Сλ��
	const ATOM_Rect2Di &getWidgetRect (void) const;

	//! ��ȡ�ͻ�����Сλ��
	//! \return �ͻ�����Сλ��
	const ATOM_Rect2Di &getClientRect (void) const;

	//! ��ȡ�Ӵ�����Ŀ
	//! \return �Ӵ�����Ŀ
	unsigned getNumChildren (void) const;

	//! ��ȡָ���������Ӵ���
	//! \return �Ӵ���ָ��
	ATOM_Widget *getChild (unsigned index) const;

	//! ��ȡ��һ�������Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getFirstChild (void) const;

	//! ��ȡ���һ���������Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getLastChild (void) const;

	//! ��ȡ��һ���ؼ��Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getFirstControl (void) const;

	//! ��ȡ���һ���ؼ��Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getLastControl (void) const;

	//! ��ȡ��һ��ͬ��ε������Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getNextSibling (void) const;

	//! ��ȡǰһ��ͬ��ε������Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getPrevSibling (void) const;

	//! ��ȡ����IdΪָ��ֵ���Ӵ���,�÷���ֻ����Լ���ֱ���Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getChildById (int id) const;

	//! ��ȡ����IdΪָ��ֵ���Ӵ���,�÷����Թ�����ȵ�ģʽ�����Ӵ����µ��Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getChildByIdRecursive (int id, bool hierichyOnly = true) const;

	//! ��ȡ��������Ϊָ���ַ������Ӵ���,�÷���ֻ����Լ���ֱ���Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getChildByName (const char *name) const;

	//! ��ȡ��������Ϊָ���ַ������Ӵ���,�÷����Թ�����ȵ�ģʽ�����Ӵ����µ��Ӵ���
	//! \return �Ӵ���ָ�룬���û�з���NULL
	ATOM_Widget *getChildByNameRecursive (const char *name, bool hierichyOnly = true) const;

	//! ɾ���Ӵ���
	//! \param widget
	//! \note ���Ӵ��ڵ������Ӵ���Ҳ�����Զ�ɾ��
	void deleteChild (ATOM_Widget *widget);

	//! ɾ�������Ӵ���
	void clearChildren ();

	//
	ATOM_Point2Di *clientToScreen (ATOM_Point2Di *point) const;
	ATOM_Point2Di *screenToClient (ATOM_Point2Di *point) const;
	ATOM_Point2Di *clientToViewport (ATOM_Point2Di *point) const;
	ATOM_Point2Di *viewportToClient (ATOM_Point2Di *point) const;
	ATOM_Point2Di *clientToGUI (ATOM_Point2Di *point) const;
	ATOM_Point2Di *GUIToClient (ATOM_Point2Di *point) const;

	//! ���ô������
	//! \param state ���
	void setFlags (unsigned state);

	//! ��ȡ�������
	//! \return ���
	unsigned getFlags (void) const;

	//! ���ô��ڷ��
	//! \param style ���
	void setStyle (unsigned style);

	//! ��ȡ���ڷ��
	//! \return ���
	unsigned getStyle (void) const;

	//! ���ñ߿�ģʽ
	//! \param mode �߿�ģʽ
	void setBorderMode (ATOM_Widget::BorderMode mode);

	//! ��ȡ�߿�ģʽ
	//! \return �߿�ģʽ
	ATOM_Widget::BorderMode getBorderMode (void) const;

	//! �����϶�ģʽ
	//! \param mode �϶�ģʽ
	void setDragMode (unsigned mode);

	//! ��ȡ�϶�ģʽ
	//! \return �϶�ģʽ
	unsigned getDragMode (void) const;

	//! �Ƿ�ؼ�����
	//! return true �� false ����
	bool isControl (void) const;

	//! �Ƿ�˽�пؼ�
	//! return true �� false ����
	bool isPrivate (void) const;

	//! ǿ���ػ�
	void invalidate (void);

	//! ��ʾ�����˵�
	//! param x	�����˵���x����(���������ͻ���)
	//! param y �����˵���y����(���������ͻ���)
	//! param popupMenu �����˵�
	void trackPopupMenu (int x, int y, ATOM_PopupMenu *popupMenu);

	//! ���ù�������(�������Popup����,�������������غ��Լ�Ҳ������)
	//! param widget ��������
	void setPopupRelation (ATOM_Widget *widget);

	//! ��ȡ��������
	//! return ��������
	ATOM_Widget *getPopupRelation (void) const;

	//! �������
	void setCapture (void);

	//! ����������
	void releaseCapture (void);

	//! �Ƿ��Ѳ������
	//! \return true �� false ��
	bool isCaptured (void) const;

	//! ��ȡ������ʾ״̬
	//! \return ��ʾ״̬
	ATOM_Widget::ShowState getShowState (void) const;

	//! ����/��ֹ
	//! \param enable true ����� false ���ô���
	void enable (bool enable);

	//! �����Ƿ񼤻�
	//! \return false ���ڱ����� true ���ڱ�����
	bool isEnabled (void) const;

	//! ���ô����Ƿ�������ѡ
	//! \param pickable true ���� false ������
	void setPickable (bool pickable);

	//! ��ѯ�����Ƿ�������ѡ
	//! \return pickable true ���� false ������
	bool isPickable (void) const;

	//! ��ѯ����Ƿ��ڱ�������
	//! \return true �� false ����
	bool isMouseHover (void) const;

	//! ��ѯ�ô����Ƿ�֧�ֿ��д���
	//! \return true ֧�� false ��֧��
	bool handleIdleEvent (void);

	//! ��ȡ���ڵĵ�ǰ״̬
	//! \return ��ǰ״̬
	ATOM_WidgetState getWidgetState (void) const;

	//! �������״̬
	//! \param x ���x����
	//! \param y ���y����
	//! \param _leftDown �������Ƿ���
	//! \param _middleDown ����м��Ƿ���
	//! \param _rightDown ����Ҽ��Ƿ���
	//! \note �÷��������ڲ�ʹ��
	void updateMouseState (int x, int y, bool _leftDown, bool _middleDown, bool _rightDown);

	//! ���²���
	void updateLayout (void);

	//! ����ָ��Id����Ч
	//! \param id ��Ч��Id
	void playSound(int id);

	//! ���ô��ڵĻ�����С
	//! �����ڻ�����С���ڴ���ʵ�ʴ�С��ʱ������ù���������setScrollValue������������ͼ
	//! \param size ������С
	//! \see getCanvasSize setScrollValue getScrollValue
	void setCanvasSize (const ATOM_Size2Di &size);

	//! ��ȡ��ǰ���ڵĻ�����С
	//! \return ������С
	const ATOM_Size2Di &getCanvasSize (void) const;

	//! ���ù���λ��
	//! \param val ����λ��
	void setScrollValue (const ATOM_Point2Di &val);

	//! ��ȡ����λ��
	//! \return ����λ��
	const ATOM_Point2Di &getScrollValue (void) const;

	//! ��ȡ��ֱ������
	//! \return ������
	ATOM_ScrollBar *getVerticalScrollBar (void) const;

	//! ��ȡˮƽ������
	//! \return ������
	ATOM_ScrollBar *getHorizontalScrollBar (void) const;

	//! ������
	void setLayoutable (bool layoutable);

	//! ��ѯ�Ƿ�������
	bool isLayoutable (void) const;

	//! �Ƿ񵯳�ʽ����
	bool isPopup (void) const;

	//! ��ȡ���Լ���
	ATOM_UserAttributes *getAttributes (void) const;

	//! �ƶ��ô���һ������
	//! \param x x����ľ���(����)
	//! \param y y����ľ���(����)
	void moveBy (int x, int y);

	//! �ı䴰�ڴ�Сλ��
	//! \param rect ���ڵĴ�Сλ��(����ڸ�����)
	void resize (const ATOM_Rect2Di &rect);

	//! �Ѹô����ᵽ��ǰ
	void bringToFront (void);

	//! ժ��ĳ���Ӵ���
	//! \param widget �Ӵ���
	//! \note �����ڲ�ʹ��
	void detach (ATOM_Widget *widget);

	//! ���ĳ������Ϊ�Ӵ���
	//! \param widget Ҫ��ӵ��Ӵ���
	//! \note �����ڲ�ʹ��
	bool attach (ATOM_Widget *widget);

	//! ��ȡָ����ͼ��
	//! \param imageId ͼ���ʶ
	//! \return GUIͼ��ָ���NULL
	ATOM_GUIImage *getValidImage (int imageId);

	//! ��ȡ�����ڻ���
	//! \return ����ָ��
	//! \see ATOM_Widget::freeCanvas
	ATOM_GUICanvas *getWidgetCanvas (void);

	//! ��ȡ�ͻ�������
	//! \return ����ָ��
	//! \see ATOM_Widget::freeCanvas
	ATOM_GUICanvas *getClientCanvas (void);

	//! �ͷ�ͨ��getWidgetCanvas()��getClientCanvas()����Ļ�������
	//! \param canvas �ѷ���Ļ���ָ��
	void freeCanvas (ATOM_GUICanvas *canvas);

	//! �����û�����
	void setUserData(unsigned long long data);

	//! ȡ���û�����
	unsigned long long getUserData() const;

	//! ���ÿؼ�����
	void setWidgetName (const char *name);

	//! ��ȡ�ؼ�����
	const char *getWidgetName (void) const;

	//! �������������ɫ
	void setFontOutLineColor(const ATOM_ColorARGB& clr);

	//! ��ȡ���������ɫ
	ATOM_ColorARGB getFontOutLineColor();

	//! �����ⲿ���뱳��ͼƬ
	void setExClientImage(ATOM_GUIImage * image);
	//! �Ƿ������ⲿ����ͼƬ
	bool isUserExClientImage();
	//! ��ȡ��ֱ��������ͼƬ
	//! \param backImageId ����ͼ��
	//! \param sliderImageId ������ͼ��
	//! \param upButtonImageId �ϰ�ťͼ��
	//! \param downButtonImageId �°�ťͼ��
	virtual void setVerticalSCrollBarImageId(int backImageId, int sliderImageId, int upButtonImageId, int downButtonImageId );

	//! ���ô�ֱ������ĳ���
	virtual void setScrollBarSliderLength(int len);

	//! ���ô�ֱ��������ť�ĳ���
	virtual void setScrollBarButtonLength(int len);

	//! ����Ͷ����������Ϣ
	void setWheelDeliver(bool wheelDeliver);

	//! �Զ�������С
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
	// �첽�������
public:
	enum
	{
		WIDGET_LOAD_PRIORITY_BASE = 5000,		// WIDGET�ļ������ȼ����Ϊ5000���ǳ��ߣ�
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
