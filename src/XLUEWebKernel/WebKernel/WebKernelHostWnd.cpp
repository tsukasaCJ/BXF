/********************************************************************
/* Copyright (c) 2013 The BOLT UIEngine. All rights reserved.
/* Use of this source code is governed by a BOLT license that can be
/* found in the LICENSE file.
********************************************************************/ 
#include "stdafx.h"
#include "./WebKernelHostWnd.h"

WebKernelHostWnd::WebKernelHostWnd(void)
{
}

WebKernelHostWnd::~WebKernelHostWnd(void)
{
}

LRESULT WebKernelHostWnd::OnEraseBkgnd( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	return 1;
}