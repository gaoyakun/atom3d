/**	\file plugin.h
 *	AS_Plugin�������.
 *
 *	\author ������
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
//! �û���չ����ӿ�
//! \author ������
//! \ingroup studio
class AS_Plugin
{
public:
	//! �������� 
	virtual ~AS_Plugin (void) {}

	//! ��ѯ����İ汾��
	//! \return �汾��
	virtual unsigned getVersion (void) const = 0;

	//! ��ѯ���������
	//! \return �������
	virtual const char *getName (void) const = 0;

	//! ɾ�����
	virtual void deleteMe (void) = 0;

	//! ��ʼ�����
	//! �����DLL�����غ�˺����ᱻ����
	//! \param editor �༭�����ܽӿ�ָ�룬�����Ҫ�Ļ�Ӧ��������ָ���Ա�ʹ��
	virtual bool initPlugin (AS_Editor *editor) = 0;

	//! ж�����
	//! �����������ж�ص�������
	virtual void donePlugin (void) = 0;

	//! ��ʼ�༭�µ��ĵ�
	//! \param filename �ļ��������ΪNULL���ʾ���½����ĵ���δ���棬����Ϊͨ���ļ��˵��򿪵��ĵ���
	//! \return true�ɹ� falseʧ��
	virtual bool beginEdit (const char *filename) = 0;

	//! ������ǰ�ĵ��ı༭
	//! ���û�ѡ���ļ��˵��еĹرջ��½�����µ��ĵ�ʱ���ô˽ӿ�
	virtual void endEdit (void) = 0;

	//! �����ĵ�
	//! \param filename �ļ���
	//! \return true�ɹ� falseʧ��
	virtual bool saveFile (const char *filename) = 0;

	//! ÿ֡����ʱ������
	virtual void frameUpdate (void) = 0;

	//! �����¼�
	//! �����ϵͳ�¼�����ַ�������
	//! \param event �¼�
	virtual void handleEvent (ATOM_Event *event) = 0;

	//! �����ڼ��α任�༭ģʽ�½ڵ�任�¼�
	//! \param node �ڵ�
	virtual void handleTransformEdited (ATOM_Node *node) = 0;

	//! �����������仯
	virtual void handleScenePropChanged (void) = 0;

	//! ��ѯʹ�ø������Ҫ����С���ڿ��
	//! \return ���ڿ��
	virtual unsigned getMinWindowWidth (void) const = 0;

	//! ��ѯʹ�ø������Ҫ����С���ڸ߶�
	//! \return ���ڸ߶�
	virtual unsigned getMinWindowHeight (void) const = 0;

	//! ��ѯ�ĵ��Ƿ����޸Ĺ�.
	//! ������ܱ�Ҫ��رյ�ǰ�ĵ�(���ĵ��������������Ҫ�˳�ʱ)����ô˽ӿڲ�ѯ�ĵ��Ƿ�ı䣬��ı�ᵯ���Ƿ񱣴�ĶԻ������ʹ����AS_Editor::setDocumentModified()�ӿڵĻ����ɵ���AS_Editor::isDocumentModified()��������ѯ
	//! \return true�޸Ĺ� falseδ�޸�
	virtual bool isDocumentModified (void) = 0;

	//! ��Ⱦ���߷����ı�
	//! \param newScheme ����Ⱦ����
	virtual void changeRenderScheme (void) = 0;
};

#endif // __ATOM3D_STUDIO_PLUGIN_H

/*! @} */
