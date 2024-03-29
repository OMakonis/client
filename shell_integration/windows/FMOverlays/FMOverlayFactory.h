/**
 * Copyright (c) 2000-2013 Liferay, Inc. All rights reserved.
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

#pragma once

#include <unknwn.h>

enum State {
    State_Error = 0,
    State_OK, State_OKShared,
    State_Sync, 
    State_Warning
};

class FMOverlayFactory : public IClassFactory
{
public:
    FMOverlayFactory(int state);

    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) Release();

protected:
    ~FMOverlayFactory();

private:
    long _referenceCount;
    int _state;
};

