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
 
#ifndef COPENGLSURFACE9_H
#define COPENGLSURFACE9_H

#include "d3d9.h" // Base class: IDirect3DSurface9
#include "COpenGLResource9.h"

class COpenGLSurface9 : public IDirect3DSurface9
{
public:
	COpenGLSurface9();
	~COpenGLSurface9();

	int	m_refcount[2];
	bool m_mark;

	COpenGLDevice9	*m_device;		// parent device
	D3DRESOURCETYPE		m_restype;

	D3DSURFACE_DESC			m_desc;
	CGLMTex					*m_tex;
	int						m_face;
	int						m_mip;

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DResource9
	virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
	virtual DWORD STDMETHODCALLTYPE GetPriority();
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual D3DRESOURCETYPE STDMETHODCALLTYPE GetType();
	virtual void STDMETHODCALLTYPE PreLoad();
	virtual DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);

	//IDirect3DSurface9
	virtual HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer);
	virtual HRESULT STDMETHODCALLTYPE GetDC(HDC* phdc);
	virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DSURFACE_DESC* pDesc);
	virtual HRESULT STDMETHODCALLTYPE LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hdc);
	virtual HRESULT STDMETHODCALLTYPE UnlockRect();

	ULONG STDMETHODCALLTYPE AddRef( int which, char *comment = NULL );
	ULONG STDMETHODCALLTYPE	Release( int which, char *comment = NULL );
};

#endif // COPENGLSURFACE9_H
