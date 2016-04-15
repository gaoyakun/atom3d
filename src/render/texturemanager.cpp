#include "stdafx.h"
#include "texturemanager.h"
#include "d3d/texture_d3d.h"

typedef USHORT (WINAPI *CaptureStackBackTraceFunc)(ULONG,ULONG,PVOID*,PULONG);
static HMODULE hNtDll = ::GetModuleHandle ("NTDLL.DLL");
static CaptureStackBackTraceFunc pfnCaptureStackBackTrace = (CaptureStackBackTraceFunc)::GetProcAddress(hNtDll, "RtlCaptureStackBackTrace");
static int TextureAllocationId = 0;

ATOM_TextureManager::ATOM_TextureManager (ATOM_RenderDevice *device) {
  ATOM_ASSERT (device);
  _M_device = device;
  _M_nodes = 0;
  _M_num_textures = 0;
}

ATOM_TextureManager::~ATOM_TextureManager() {
  // Does nothing
  return;
}

ATOM_TextureNode *ATOM_TextureManager::newTextureNode (ATOM_Texture *texture, const char *filename)
{
	ATOM_TextureNode *node = ATOM_NEW(ATOM_TextureNode);
	node->texture = texture;
	texture->setNode (node);
	if (filename)
	{
		strcpy (node->filename, filename);
	}
	else
	{
		node->filename[0] = '\0';
	}
	node->numFrames = pfnCaptureStackBackTrace ? pfnCaptureStackBackTrace(0, 16, node->callstack, 0) : 0;
	node->prev = 0;
	node->next = 0;
	node->allocId = ++TextureAllocationId;
	node->manager = this;

	return node;
}

void ATOM_TextureManager::freeTextureNode (ATOM_TextureNode *node)
{
	ATOM_DELETE(node);
}

void ATOM_TextureManager::attachNode (ATOM_TextureNode *node)
{
	node->prev = 0;
	node->next = _M_nodes;

	if (_M_nodes)
	{
		_M_nodes->prev = node;
	}

	_M_nodes = node;
	_M_num_textures++;

	validateNodeList ();
}

void ATOM_TextureManager::detachNode (ATOM_TextureNode *node)
{
	if (node->prev)
	{
		node->prev->next = node->next;
	}
	else
	{
		_M_nodes = node->next;
	}

	if (node->next)
	{
		node->next->prev = node->prev;
	}

	node->prev = 0;
	node->next = 0;
	_M_num_textures--;

	validateNodeList ();
}

void ATOM_TextureManager::stepFront (ATOM_TextureNode *node)
{
	ATOM_TextureNode *prev = node->prev;
	ATOM_TextureNode *next = node->next;

	if (prev)
	{
		node->prev = prev->prev;
		node->next = prev;

		if (node->prev)
		{
			node->prev->next = node;
		}

		if (next)
		{
			next->prev = prev;
		}

		prev->prev = node;
		prev->next = next;
	}
	validateNodeList ();
}

void ATOM_TextureManager::stepBack (ATOM_TextureNode *node)
{
	if (node->next)
	{
		stepFront (node->next);
	}
	validateNodeList ();
}

void ATOM_TextureManager::setFront (ATOM_TextureNode *node)
{
	detachNode (node);
	attachNode (node);
	validateNodeList ();
}

ATOM_AUTOREF(ATOM_Texture) ATOM_TextureManager::AllocateTexture (unsigned flags, const char* name)
{
	ATOM_AUTOREF(ATOM_Texture) texture = doAllocateTexture (flags, name);
	if (texture)
	{
		texture->setRenderDevice (_M_device);
		ATOM_TextureNode *node = newTextureNode (texture.get(), 0);
		attachNode (node);
		texture->setNode (node);
	}
	return texture;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_TextureManager::AllocateTexture (const void* image, int w, int h, ATOM_PixelFormat format, unsigned flags, const char* name)
{
	ATOM_AUTOREF(ATOM_Texture) texture = doAllocateTexture (image, w, h, format, flags, name);
	if (texture)
	{
		texture->setRenderDevice (_M_device);
		ATOM_TextureNode *node = newTextureNode (texture.get(), 0);
		attachNode (node);
		texture->setNode (node);
	}
	return texture;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_TextureManager::AllocateVolumeTexture (const void* image, int w, int h, int d, ATOM_PixelFormat format, unsigned flags, const char* name)
{
	ATOM_AUTOREF(ATOM_Texture) texture = doAllocateVolumeTexture (image, w, h, d, format, flags, name);
	if (texture)
	{
		texture->setRenderDevice (_M_device);
		ATOM_TextureNode *node = newTextureNode (texture.get(), 0);
		attachNode (node);
		texture->setNode (node);
	}
	return texture;
}

ATOM_AUTOREF(ATOM_Texture) ATOM_TextureManager::AllocateCubeTexture (const void* image[6], int size, ATOM_PixelFormat format, unsigned flags, const char* name)
{
	ATOM_AUTOREF(ATOM_Texture) texture = doAllocateCubeTexture (image, size, format, flags, name);
	if (texture)
	{
		texture->setRenderDevice (_M_device);
		ATOM_TextureNode *node = newTextureNode (texture.get(), 0);
		attachNode (node);
		texture->setNode (node);
	}
	return texture;
}
//--- wangjian added for DX9 DEPTH STENCIL FORMAT ---//
// * �������� bool bUseDX9DSFormat
// * ����ĳЩ�������ʱ ��Ҫ����DX9 DEPTH STENCIL FORMAT���͵�DEPTH TEXTURE.
// * ��������shadow mapping��Ӳ��PCF.
ATOM_AUTOREF(ATOM_Texture) ATOM_TextureManager::AllocateDepthTexture (int w, int h, bool bUseDX9DSFormat)
{
	//--- wangjian modified ---//
	ATOM_AUTOREF(ATOM_Texture) texture = doAllocateDepthTexture (w, h, bUseDX9DSFormat);
	if (texture)
	{
		texture->setRenderDevice (_M_device);
		ATOM_TextureNode *node = newTextureNode (texture.get(), 0);
		attachNode (node);
		texture->setNode (node);
	}
	return texture;
}
//---------------------------------------------------//
//--- wangjian MODIFIED ---//
// �첽���� �� ����첽�������ȼ�
ATOM_AUTOREF(ATOM_Texture) ATOM_TextureManager::AllocateTexture (	const char *filename, 
																	unsigned width, 
																	unsigned height, 
																	bool asResource, 
																	ATOM_PixelFormat format,
																	int loadPriority	)
{
	char buffer[ATOM_VFS::max_filename_length];
	if (!filename)
	{
		return 0;
	}
	if (!ATOM_CompletePath (filename, buffer))
	{
		return 0;
	}

	ATOM_AUTOREF(ATOM_Texture) texture = asResource ? ATOM_LookupObject (ATOM_D3DTexture::_classname(), buffer) : 0;

	// ����������� ����
	if ( !texture )
	{
		//--- wangjian MODIFIED for multithreaded ---//
		// �첽���� 
		texture = doAllocateTexture (buffer, width, height, asResource, format, loadPriority);
		//-------------------------------------------//
		if (texture)
		{
			texture->setRenderDevice (_M_device);
			ATOM_TextureNode *node = newTextureNode (texture.get(), buffer);
			attachNode (node);
			texture->setNode (node);
		}
	}
	// ����������
	else
	{
		if( texture->getAsyncLoader()->priority != ATOM_LoadPriority_IMMEDIATE )
		{
			if( loadPriority > texture->getAsyncLoader()->priority )
				texture->getAsyncLoader()->priority = loadPriority;

			// ���������ض��󱻷��� ����
			if( texture->getAsyncLoader()->abandoned )
				ATOM_AsyncLoader::ReProcessLoadObject( texture->getAsyncLoader() );
		}
	}

	////--- wangjian added ---//
	//// �첽����
	//// ���������Ҫ����һ�¸������Ƿ�ͬ�������첽�����ж�
	//// �����ͬ�����أ����ж�һ�¸�����ǰ�Ƿ����첽�����У�
	//// ������ڣ���ô��Ȼֱ�Ӽ��أ����Ҽ�����֮������첽������ɱ�ǣ���ֹ�ٴ����������ٴ�ֱ�Ӽ���.
	//// ������첽���أ��򲻹ܡ�
	//bool bStillDoAlloc = false;
	//if( !bMT && 
	//	texture && 
	//	( texture->getAsyncLoader()->GetLoadStage() > ATOM_AsyncLoader::ATOM_ASYNCLOAD_NOTLOADSTARTED && !texture->getAsyncLoader()->IsLoadAllFinished() ) )
	//{
	//	bStillDoAlloc = true;
	//}
	////----------------------//

	////--- wangjian modified ---//
	//if ( !texture || bStillDoAlloc )
	//{
	//	// �����������
	//	if( !texture )
	//	{
	//		//--- wangjian MODIFIED for multithreaded ---//
	//		texture = doAllocateTexture (buffer, width, height, asResource, format, bMT);
	//		//-------------------------------------------//
	//		if (texture)
	//		{
	//			texture->setRenderDevice (_M_device);
	//			ATOM_TextureNode *node = newTextureNode (texture.get(), buffer);
	//			attachNode (node);
	//			texture->setNode (node);
	//		}
	//	}
	//	// ������� ֱ�Ӽ��������ļ�
	//	else
	//	{
	//		// ��Ǹ������첽�������
	//		//����Ȼ�����첽���صģ����ǿ��ܳ�����ֱ�Ӽ���֮ǰ��������Դ�������첽���ز�����
	//		texture->getAsyncLoader()->SetLoadStage(ATOM_AsyncLoader::ATOM_ASYNCLOAD_ALLFINISHED);

	//		if( !texture->loadTexImageFromFile(buffer,width,height,ATOM_PIXEL_FORMAT_UNKNOWN) )
	//		{
	//			ATOM_LOGGER::error ("%s(%s) texture direct loading failed.\n", __FUNCTION__, buffer);
	//			return 0;
	//		}

	//		// �����ڴ�
	//		//texture->getAsyncLoader()->GarbageCollect();
	//	}
	//}
	//-------------------------------------------------//

	return texture;
}

unsigned ATOM_TextureManager::GetNumTextures (void) const
{
	return _M_num_textures;
}

void ATOM_TextureManager::validateNodeList (void)
{
	return;

	ATOM_TextureNode *node = _M_nodes;
	ATOM_TextureNode *prev = 0;
	unsigned num = 0;

	while (node)
	{
		++num;

		if (node->prev != prev)
		{
			ATOM_LOGGER::error ("ERR: TextureManager node validation failed!\n");
			ATOM_ASSERT(0);
			return;
		}
		prev = node;
		node = node->next;
	}

	if (num != _M_num_textures)
	{
		ATOM_LOGGER::error ("ERR: TextureManager node validation failed!\n");
		ATOM_ASSERT(0);
	}
}

bool ATOM_TextureManager::DumpTextureStats (const char *filename)
{
	FILE *fp = fopen (filename , "wt");
	if (fp)
	{
		fprintf (fp, "Total textures: %d\n", _M_num_textures);
		fprintf (fp, "\n\n");

		ATOM_TextureNode *node = _M_nodes;
		ATOM_CallStack callstack(0, ATOM_CallStack::SYM_BUILDPATH);

		while (node)
		{
			fprintf (fp, "======================================================================================\n");
			fprintf (fp, "allocid: %d\n", node->allocId);
			fprintf (fp, "image: %s\n", node->filename);
			fprintf (fp, "creation callstack:\n");
			for (unsigned i = 0; i < node->numFrames; ++i)
			{
				ATOM_CallStack::StackEntry e;
				e.offset = node->callstack[i];
				callstack.resolveSymbols (e);
				fprintf (fp, "\t%s(%s@%d)\n", e.und_name, e.line_filename, e.line_number);
			}
			node = node->next;
		}
		fclose (fp);
		return true;
	}
	return false;
}

