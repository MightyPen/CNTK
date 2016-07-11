//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define _CRT_SECURE_NO_WARNINGS // "secure" CRT not available on all platforms  --add this at the top of all CPP files that give "function or variable may be unsafe" warnings

#include "SwapOutAction.h"
#include <iostream>
#include <string>

#ifndef CPUONLY
	#include <cuda_runtime.h>
#endif


namespace Microsoft { namespace MSR { namespace CNTK {

using std::cout;
using std::endl;


void SwapOutAction::BeginAction()
{
    size_t cols = m_bufferGPU->GetNumCols();
    size_t rows = m_bufferGPU->GetNumRows();
    size_t bytes = cols*rows*sizeof(float);
    // perform the actual asynchronous copy
    CUDA_CALL(cudaMemcpyAsync(m_bufferCPU, m_bufferGPU->Data(), bytes, cudaMemcpyDefault, m_streamAsync));
}

void SwapOutAction::endAction()
{
    CUDA_CALL(cudaStreamSynchronize(m_streamAsync));
}


void SwapOutAction::allocatePinnedBuffer()
{
    size_t cols = m_bufferGPU->GetNumCols();
    size_t rows = m_bufferGPU->GetNumRows();
    size_t bytes = cols*rows*sizeof(float);

    float *pinnedBuffer;
    //cudaHostAllocPortable preservse the page-lock even across threads
    CUDA_CALL(cudaHostAlloc(&pinnedBuffer, bytes, cudaHostAllocPortable));
    m_bufferCPU = pinnedBuffer;
}

}}}


