/*
** Copyright (c) 2021 LunarG, Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to
** deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
** sell copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
*/

#include "encode/custom_dx12_api_call_encoders.h"

#include "encode/custom_dx12_struct_encoders.h"
#include "encode/d3d12_capture_manager.h"
#include "encode/parameter_encoder.h"
#include "encode/struct_pointer_encoder.h"
#include "format/api_call_id.h"
#include "util/defines.h"

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(encode)

void Encode_ID3D12Device_CheckFeatureSupport(format::HandleId wrapper_id,
                                             HRESULT          result,
                                             D3D12_FEATURE    Feature,
                                             void*            pFeatureSupportData,
                                             UINT             FeatureSupportDataSize)
{
    auto encoder = D3D12CaptureManager::Get()->BeginMethodCallCapture(
        format::ApiCallId::ApiCall_ID3D12Device_CheckFeatureSupport, wrapper_id);
    if (encoder)
    {
        encoder->EncodeEnumValue(Feature);
        EncodeD3D12FeatureStruct(encoder, pFeatureSupportData, Feature);
        encoder->EncodeUInt32Value(FeatureSupportDataSize);
        encoder->EncodeInt32Value(result);
        D3D12CaptureManager::Get()->EndMethodCallCapture(encoder);
    }
}

GFXRECON_END_NAMESPACE(encode)
GFXRECON_END_NAMESPACE(gfxrecon)
