/**
 * @file adapter.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        result mapHRESULT(HRESULT value);
    }

    result Adapter::impl_queryInfo(adapter_info* info) const
    {
        DXGI_ADAPTER_DESC1 desc;
        static_cast<IDXGIAdapter1*>(m_ptr)->GetDesc1(&desc);

        adapter_info result;
        result.vendorId = desc.VendorId;
        result.adapterId = desc.DeviceId;
        const auto description = std::string(desc.Description, desc.Description + 128);
        result.adapterName = description.substr(0, description.find_last_not_of(' '));

        if (desc.Flags == DXGI_ADAPTER_FLAG_REMOTE)
            result.adapterType = adapter_type::Virtual;
        else if (desc.DedicatedVideoMemory > 256000000) //safely assume that all GPUS with less than 256MB of VRAM are iGPUs
            result.adapterType = adapter_type::Discrete;
        else // video memory == 0 
            result.adapterType = adapter_type::Integrated;

        *info = result;
        return result::Success;
    }

    result Adapter::impl_queryFeatures(adapter_features* features) const
    {
        HRESULT level12_0 = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        if (level12_0 == E_FAIL)
            return result::ErrorIncompatibleDriver;
        else if (FAILED(level12_0)) //no matter what reason, LEVEL_12_0 should always be supported for DX12
            return internal::mapHRESULT(level12_0);

        //Level 1 and 2 can more easily grant us guarantees about feature support
        HRESULT level12_1 = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        HRESULT level12_2 = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);

        adapter_features output;

        //Set all the information in a structured way here

        *features = output;
        return result::Success;
    }

    result Adapter::impl_queryExtensionSupport(adapter_extension_type type, bool* supported) const
    {
        *supported = false;

        switch (type)
        {
        default:
            break;
        }

        return result::Success;
    }
}
