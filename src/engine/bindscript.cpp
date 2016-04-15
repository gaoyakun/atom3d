#include "StdAfx.h"

extern void __dbghlp_bind_script (ATOM_Script *script);
extern void __engine_bind_script (ATOM_Script *script);
extern void __kernel_bind_script (ATOM_Script *script);
extern void __render_bind_script (ATOM_Script *script);
extern void __misc_bind_script (ATOM_Script *script);
extern void __system_bind_script (ATOM_Script *script);

ATOM_ENGINE_API void ATOM_BindToScript (ATOM_Script *script)
{
	__system_bind_script (script);
	__misc_bind_script (script);
	__dbghlp_bind_script (script);
	__kernel_bind_script (script);
	__render_bind_script (script);
	__engine_bind_script (script);
}

ATOM_ENGINE_API void ATOM_BindSystemToScript (ATOM_Script *script)
{
	__system_bind_script (script);
}

ATOM_ENGINE_API void ATOM_BindMiscToScript (ATOM_Script *script)
{
	__misc_bind_script (script);
}

ATOM_ENGINE_API void ATOM_BindDbgHlpToScript (ATOM_Script *script)
{
	__dbghlp_bind_script (script);
}

ATOM_ENGINE_API void ATOM_BindKernelToScript (ATOM_Script *script)
{
	__kernel_bind_script (script);
}

ATOM_ENGINE_API void ATOM_BindRenderToScript (ATOM_Script *script)
{
	__render_bind_script (script);
}

ATOM_ENGINE_API void ATOM_BindEngineToScript (ATOM_Script *script)
{
	__engine_bind_script (script);
}

