#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    constexpr inline const char* to_string(const adapter_type& type)
    {
        switch (type)
        {
        case adapter_type::Other:
            return "Other";
        case adapter_type::Integrated:
            return "Integrated";
        case adapter_type::Discrete:
            return "Discrete";
        case adapter_type::Virtual:
            return "Virtual";
        }

        return "Invalid adapter_type value";
    }

    inline result Adapter::queryInfo(adapter_info* info) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (info == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryInfo() returned ErrorInvalidUsage because the passed info parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryInfo() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryInfo(info);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryInfo(info);
#endif
    }

    inline result Adapter::queryFeatures(adapter_features* features) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (features == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryFeatures() returned ErrorInvalidUsage because the passed features parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryFeatures() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryFeatures(features);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryFeatures(features);
#endif
    }

    inline result Adapter::queryExtensionSupport(const adapter_extension_type& type, bool* supported) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (supported == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryExtensionSupport() returned ErrorInvalidUsage because the passed supported parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryExtensionSupport() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryExtensionSupport(type, supported);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryExtensionSupport(type, supported);
#endif
    }
}
