//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Adrain Schuur (schuur@de-ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderType.h"

PEGASUS_NAMESPACE_BEGIN
/*
const Uint32 ProviderType::INSTANCE = 0x00000002;
const Uint32 ProviderType::CLASS = 0x00000003;
const Uint32 ProviderType::METHOD = 0x00000004;
const Uint32 ProviderType::ASSOCIATION = 0x00000005;
const Uint32 ProviderType::QUERY = 0x00000006;
const Uint32 ProviderType::INDICATION = 0x00000007;
const Uint32 ProviderType::CONSUMER = 0x00000008;
*/

const Uint32 ProviderType::CLASS = ProviderType_CLASS;

const Uint32 ProviderType::INSTANCE = ProviderType_INSTANCE;

const Uint32 ProviderType::ASSOCIATION = ProviderType_ASSOCIATION;

const Uint32 ProviderType::INDICATION = ProviderType_INDICATION;

const Uint32 ProviderType::METHOD = ProviderType_METHOD;

const Uint32 ProviderType::CONSUMER = ProviderType_CONSUMER;

const Uint32 ProviderType::QUERY = ProviderType_QUERY;

PEGASUS_NAMESPACE_END
