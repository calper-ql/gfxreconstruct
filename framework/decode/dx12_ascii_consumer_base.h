/*
** Copyright (c) 2021 LunarG, Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
*/

#ifndef GFXRECON_DECODE_DX12_ASCII_CONSUMER_BASE_H
#define GFXRECON_DECODE_DX12_ASCII_CONSUMER_BASE_H

#include "generated/generated_dx12_consumer.h"
#include "util/to_string.h"

#include <cctype>

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(decode)

class Dx12AsciiConsumerBase : public Dx12Consumer
{
  public:
    Dx12AsciiConsumerBase();
    virtual ~Dx12AsciiConsumerBase();

    void Initialize(const std::string& filename, FILE* file);

    void Destroy();

    bool IsValid() const { return (m_file != nullptr); }

    const std::string& GetFilename() const { return m_filename; }

    void Process_ID3D12Device_CheckFeatureSupport(format::HandleId object_id,
                                                  HRESULT          original_result,
                                                  D3D12_FEATURE    feature,
                                                  const void*      capture_feature_data,
                                                  void*            replay_feature_data,
                                                  UINT             feature_data_size) override;

    void Process_IDXGIFactory5_CheckFeatureSupport(format::HandleId object_id,
                                                   HRESULT          original_result,
                                                   DXGI_FEATURE     feature,
                                                   const void*      capture_feature_data,
                                                   void*            replay_feature_data,
                                                   UINT             feature_data_size) override;

    void OverridePresent(format::HandleId object_id, HRESULT return_value, UINT SyncInterval, UINT Flags);

    void OverridePresent1(format::HandleId                                       object_id,
                          HRESULT                                                return_value,
                          UINT                                                   SyncInterval,
                          UINT                                                   PresentFlags,
                          StructPointerDecoder<Decoded_DXGI_PRESENT_PARAMETERS>* pPresentParameters);

    void DumpPresentWithFrameNumber(format::HandleId                                       object_id,
                                    HRESULT                                                return_value,
                                    UINT                                                   SyncInterval,
                                    UINT                                                   PresentFlags,
                                    StructPointerDecoder<Decoded_DXGI_PRESENT_PARAMETERS>* pPresentParameters);

  protected:
    FILE*    GetFile() const { return m_file; }
    uint32_t current_frame_number_;

    struct WriteApiCallToFileInfo
    {
        const char*      pObjectTypeName{};
        format::HandleId handleId{};
        const char*      pFunctionName{};
        const char*      pReturnValue{};
    };

    // clang-format off
    template <typename ToStringFunctionType>
    inline void WriteApiCallToFile(const WriteApiCallToFileInfo& writeApiCallToFileInfo, util::ToStringFlags toStringFlags, uint32_t& tabCount, uint32_t tabSize, ToStringFunctionType toStringFunction)
    {
        using namespace util;
        fprintf(m_file, "%s\n", (m_apiCallCount ? "," : ""));
        fprintf(m_file, "%s", ObjectToString(toStringFlags, tabCount, tabSize,
            [&](std::stringstream& strStrm)
            {
                FieldToString(strStrm, true, "index", toStringFlags, tabCount, tabSize, ToString(m_apiCallCount++, toStringFlags, tabCount, tabSize));
                if (writeApiCallToFileInfo.pObjectTypeName) {
                    FieldToString(strStrm, false, "object", toStringFlags, tabCount, tabSize,
                        ObjectToString(toStringFlags, tabCount, tabSize,
                            [&](std::stringstream& objectStrStrm)
                            {
                                FieldToString(objectStrStrm, true, "type", toStringFlags, tabCount, tabSize, '"' + std::string(writeApiCallToFileInfo.pObjectTypeName) + '"');
                                FieldToString(objectStrStrm, false, "handle", toStringFlags, tabCount, tabSize, HandleIdToString(writeApiCallToFileInfo.handleId));
                            }
                        )
                    );
                }
                assert(writeApiCallToFileInfo.pFunctionName);
                if (writeApiCallToFileInfo.pFunctionName)
                {
                    FieldToString(strStrm, false, "function", toStringFlags, tabCount, tabSize, '"' + std::string(writeApiCallToFileInfo.pFunctionName) + '"');
                }
                if (writeApiCallToFileInfo.pReturnValue)
                {
                    FieldToString(strStrm, false, "return", toStringFlags, tabCount, tabSize, std::string(writeApiCallToFileInfo.pReturnValue));
                }
                const auto& parametersStr = ObjectToString(toStringFlags, tabCount, tabSize, toStringFunction);
                for (auto c : parametersStr)
                {
                    if (c != '{' && !isspace(static_cast<int>(c)) && c != '}')
                    {
                        FieldToString(strStrm, false, "parameters", toStringFlags, tabCount, tabSize, parametersStr);
                        break;
                    }
                }
                return strStrm.str();
            }
        ).c_str());
    }
    // clang-format on

  private:
    FILE*       m_file;
    std::string m_filename;
    uint64_t    m_apiCallCount{ 0 };
};

GFXRECON_END_NAMESPACE(decode)
GFXRECON_END_NAMESPACE(gfxrecon)

#endif // GFXRECON_DECODE_DX12_ASCII_CONSUMER_BASE_H
