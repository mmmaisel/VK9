//========= Copyright Valve Corporation, All rights reserved. ============//
//                       TOGL CODE LICENSE
//
//  Copyright 2011-2014 Valve Corporation
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
// dxabstract.h
//
//==================================================================================================

/*
 * Code from ToGL has been modified to fit the design.
 */
 
#include "COpenGLBaseTexture9.h"
#include "COpenGLDevice9.h"

COpenGLBaseTexture9::COpenGLBaseTexture9()
{
	m_refcount[0] = 1;
	m_refcount[1] = 0;
	m_mark = (IUNKNOWN_ALLOC_SPEW_MARK_ALL != 0);	// either all are marked, or only the ones that have SetMark(true) called on them
}

COpenGLBaseTexture9::~COpenGLBaseTexture9()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );
	GLMPRINTF(( ">-A- ~IDirect3DBaseTexture9" ));

	if (m_device)
	{
		Assert( m_device->m_ObjectStats.m_nTotalTextures >= 1 );
		m_device->m_ObjectStats.m_nTotalTextures--;

		GLMPRINTF(( "-A- ~IDirect3DBaseTexture9 taking normal delete path on %08x, device is %08x ", this, m_device ));
		m_device->ReleasedTexture( this );
		
		if (m_tex)
		{
			GLMPRINTF(("-A- ~IDirect3DBaseTexture9 deleted '%s' @ %08x (GLM %08x) %s",m_tex->m_layout->m_layoutSummary, this, m_tex, m_tex->m_debugLabel ? m_tex->m_debugLabel : "" ));

			m_device->ReleasedCGLMTex( m_tex );

			m_tex->m_ctx->DelTex( m_tex );
			m_tex = NULL;
		}
		else
		{
			GLMPRINTF(( "-A- ~IDirect3DBaseTexture9 : whoops, no tex to delete here ?" ));
		}		
		m_device = NULL;	// ** THIS ** is the only place to scrub this.  Don't do it in the subclass destructors.
	}
	else
	{
		GLMPRINTF(( "-A- ~IDirect3DBaseTexture9 taking strange delete path on %08x, device is %08x ", this, m_device ));
	}

	GLMPRINTF(( "<-A- ~IDirect3DBaseTexture9" ));	
}

ULONG STDMETHODCALLTYPE COpenGLBaseTexture9::AddRef(void)
{
	this->AddRef(0);
}

HRESULT STDMETHODCALLTYPE COpenGLBaseTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	
}

ULONG STDMETHODCALLTYPE COpenGLBaseTexture9::Release(void)
{
	this->Release(0);
}

ULONG STDMETHODCALLTYPE COpenGLBaseTexture9::AddRef(int which, char *comment)
{
	Assert( which >= 0 );
	Assert( which < 2 );
	m_refcount[which]++;
		
	#if IUNKNOWN_ALLOC_SPEW
		if (m_mark)
		{
			GLMPRINTF(("-A- IUAddRef  (%08x,%d) refc -> (%d,%d) [%s]",this,which,m_refcount[0],m_refcount[1],comment?comment:"..."))	;
			if (!comment)
			{
				GLMPRINTF((""))	;	// place to hang a breakpoint
			}
		}
	#endif	

	return m_refcount[0];
}

ULONG STDMETHODCALLTYPE	COpenGLBaseTexture9::Release(int which, char *comment)
{
	Assert( which >= 0 );
	Assert( which < 2 );
		
	//int oldrefcs[2] = { m_refcount[0], m_refcount[1] };
	bool deleting = false;
		
	m_refcount[which]--;
	if ( (!m_refcount[0]) && (!m_refcount[1]) )
	{
		deleting = true;
	}
		
	#if IUNKNOWN_ALLOC_SPEW
		if (m_mark)
		{
			GLMPRINTF(("-A- IURelease (%08x,%d) refc -> (%d,%d) [%s] %s",this,which,m_refcount[0],m_refcount[1],comment?comment:"...",deleting?"->DELETING":""));
			if (!comment)
			{
				GLMPRINTF((""))	;	// place to hang a breakpoint
			}
		}
	#endif

	if (deleting)
	{
		if (m_mark)
		{
			GLMPRINTF((""))	;		// place to hang a breakpoint
		}
		delete this;
		return 0;
	}
	else
	{
		return m_refcount[0];
	}
}

HRESULT STDMETHODCALLTYPE COpenGLBaseTexture9::FreePrivateData(REFGUID refguid)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLBaseTexture9::GetPriority()
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLBaseTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	return E_NOTIMPL;
}

//D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLBaseTexture9::GetType()
//{
//	return D3DRTYPE_SURFACE;
//}

void STDMETHODCALLTYPE COpenGLBaseTexture9::PreLoad()
{
	return; 
}

DWORD STDMETHODCALLTYPE COpenGLBaseTexture9::SetPriority(DWORD PriorityNew)
{
	return 1;
}

HRESULT STDMETHODCALLTYPE COpenGLBaseTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE COpenGLBaseTexture9::GenerateMipSubLevels()
{
	return; //TODO: implement GenerateMipSubLevels
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE COpenGLBaseTexture9::GetAutoGenFilterType()
{
	return D3DTEXF_NONE; //TODO: implement GetAutoGenFilterType
}

DWORD STDMETHODCALLTYPE COpenGLBaseTexture9::GetLOD()
{
	return 0; //TODO: implement GetLOD
}


DWORD STDMETHODCALLTYPE COpenGLBaseTexture9::GetLevelCount()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_tex->m_layout->m_mipCount;	
}


HRESULT STDMETHODCALLTYPE COpenGLBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE COpenGLBaseTexture9::SetLOD(DWORD LODNew)
{
	return 0; //TODO: implement SetLOD
}

D3DRESOURCETYPE STDMETHODCALLTYPE COpenGLBaseTexture9::GetType()
{
	GL_BATCH_PERF_CALL_TIMER;
	GL_PUBLIC_ENTRYPOINT_CHECKS( m_device );

	return m_restype;	//D3DRTYPE_TEXTURE;	
}