/**
* Copyright (c) 2015 Daniel Molkentin <danimo@owncloud.com>. All rights reserved.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*/

#include "FMContextMenuFactory.h"
#include "FMContextMenu.h"
#include <new>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


extern long g_cDllRef;


FMContextMenuFactory::FMContextMenuFactory() : m_cRef(1)
{
    InterlockedIncrement(&g_cDllRef);
}

FMContextMenuFactory::~FMContextMenuFactory()
{
    InterlockedDecrement(&g_cDllRef);
}


// IUnknown methods

IFACEMETHODIMP FMContextMenuFactory::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] =  { QITABENT(FMContextMenuFactory, IClassFactory), { 0 }, };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) FMContextMenuFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) FMContextMenuFactory::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef) {
        delete this;
    }
    return cRef;
}


// IClassFactory methods

IFACEMETHODIMP FMContextMenuFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_NOAGGREGATION;

    // pUnkOuter is used for aggregation. We do not support it in the sample.
    if (pUnkOuter == NULL) {
        hr = E_OUTOFMEMORY;

        // Create the COM component.
        FMContextMenu *pExt = new (std::nothrow) FMContextMenu();
        if (pExt) {
            // Query the specified interface.
            hr = pExt->QueryInterface(riid, ppv);
            pExt->Release();
        }
    }

    return hr;
}

IFACEMETHODIMP FMContextMenuFactory::LockServer(BOOL fLock)
{
    if (fLock)  {
        InterlockedIncrement(&g_cDllRef);
    } else {
        InterlockedDecrement(&g_cDllRef);
    }
    return S_OK;
}
