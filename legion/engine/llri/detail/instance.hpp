/**
 * @file instance.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>
#include <vector>
#include <map>

namespace LLRI_NAMESPACE
{
    enum struct result;
    struct instance_extension;
    class Instance;
    class Adapter;

    struct device_desc;
    class Device;

    /**
     * @brief Describes the severity of a callback message.
     * @note validation_callback_severity is meant to be used for message filtering, and has no binding impact on the implementation's behaviour.
    */
    enum struct validation_callback_severity
    {
        /**
         * @brief Extra, often excessive information about API calls, diagnostics, support, etc.
        */
        Verbose,
        /**
         * @brief Information about the implementation, operations, or resource details.
        */
        Info,
        /**
         * @brief A potential issue in the application. The issue may not be harmful, but could still lead to performance drops or unexpected behaviour.
        */
        Warning,
        /**
         * @brief Invalid (possibly fatal) API usage was detected.
        */
        Error,
        /**
         * @brief Data/memory corruption occurred.
        */
        Corruption,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Corruption
    };

    /**
     * @brief Converts a validation_callback_severity to a string.
     * @return The enum value as a string, or "Unknown validation_callback_severity value" if the value was not recognized as an enum member.
     *
    */
    constexpr const char* to_string(const validation_callback_severity& severity);

    /**
     * @brief Describes the source of the validation callback message.
    */
    enum struct validation_callback_source
    {
        /**
         * @brief The message came from the LLRI API.
         * API validation does basic parameter checks to make sure that the API doesn't crash.
         *
         * @note This value never occurs if LLRI_DISABLE_VALIDATION is defined.
        */
        Validation,
        /**
         * @brief The message came from the implementation.
         * Implementation validation needs to be enabled through driver_validation_ext and/or gpu_validation_ext for this kind of message to appear.
         *
         * @note This value never occurs if LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING is defined.
        */
        Implementation,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Implementation
    };

    /**
     * @brief Converts a validation_callback_source to a string.
     * @return The enum value as a string, or "Unknown validation_callback_source value" if the value was not recognized as an enum member.
    */
    constexpr const char* to_string(const validation_callback_source& source);

    /**
     * @brief The validation callback function.
     * The callback passes numerous parameters which help classify the message's severity and source. It also passes the userData pointer that was initially passed in validation_callback_desc.
    */
    using validation_callback = void(
        const validation_callback_severity& severity,
        const validation_callback_source& source,
        const char* message,
        void* userData
        );

    /**
     * @brief The validation callback allows the user to subscribe to validation messages so that they can write the message into their own logging system.
     *
     * The callback contains contextual information about the message, like for example its severity.
     * The callback may be used for both API validation and implementation validation, each message's source is indicated through the validation_callback_source enum.
     *
     * @note Implementation messages only occur if driver_validation_ext and/or gpu_validation_ext are enabled. If no callback is set, some implementations might still output messages (Vulkan tends to print to stdout, whereas DirectX tends to print to the "Output" window in Visual Studio).
    */
    struct validation_callback_desc
    {
        /**
         * @brief The callback, the function passed must conform to the validation_callback definition.
         * You **may** set this value to nullptr, in which case no validation messages will be sent.
        */
        validation_callback* callback;
        /**
         * @brief Optional user data pointer. Not used by LLRI but it's passed around and sent along the callback.
        */
        void* userData;

#ifndef DOXY_EXCLUDE
        /**
         * @brief Convenience operator used internally to call the callback.
        */
        void operator ()(const validation_callback_severity& severity, const validation_callback_source& source, const char* message) const { callback(severity, source, message, userData); }
#endif
    };

    /**
     * @brief Instance description to be used in llri::createInstance().
    */
    struct instance_desc
    {
        /**
         * @brief The number of instance extensions in the instance_desc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The instance extensions, if instance_desc::numExtensions > 0, then this has to be a valid pointer to an array of instance_extension.
         * If numExtensions == 0, then this pointer may be nullptr.
        */
        instance_extension* extensions;
        /**
         * @brief Sets the name of the application in the implementation if applicable.
         * @note This parameter is not guaranteed to be used but is known to at least apply to Vulkan.
        */
        const char* applicationName;
        /**
         * @brief Describes the optional validation callback. callbackDesc.callback can be nullptr in which case no callbacks will be sent.
        */
        validation_callback_desc callbackDesc;
    };

    /**
     * @brief Internal functions, don't use outside of the API.
    */
    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableImplementationMessagePolling);
        void impl_destroyInstance(Instance* instance);

        using messenger_type = void;
        /**
         * @brief Polls API messages, only called if LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING is not defined.
         * Used internally only
         * @param validation The validation function / userdata
         * @param messenger This value may differ depending on the function that is calling it, the most relevant messenger will be picked.
        */
        void impl_pollAPIMessages(const validation_callback_desc& validation, messenger_type* messenger);
    }

    /**
     * @brief Create an llri Instance.
     * Like with all API objects, the user is responsible for destroying the instance again using destroyInstance().
     * @param desc The description of the instance.
     * @param instance Must be a valid pointer to an Instance variable.
     *
     * @return Success upon correct execution of the operation.
     * @return ErrorInvalidUsage if the instance is nullptr, or if desc.numExtensions > 0 and desc.extensions is nullptr.
     * @return ErrorExtensionNotSupported if any of the extensions fail to be created.
     * @return Otherwise it may also return: ErrorExtensionNotSupported, ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed, ErrorIncompatibleDriver.
    */
    result createInstance(const instance_desc& desc, Instance** instance);

    /**
     * @brief Destroys the given instance and its directly related internal resources (debug info etc).
     * All resources created through the instance must be destroyed PRIOR to calling this function.
     *
     * @param instance The instance to destroy. This value has to be nullptr or a valid instance pointer.
    */
    void destroyInstance(Instance* instance);

    /**
     * @brief Instance is the center of the application and is used to create most other API objects.
     * Usually only a single instance of Instance exists within an application, but if so desired, multiple Instance instances are supported.
    */
    class Instance
    {
        friend result detail::impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableImplementationMessagePolling);
        friend void detail::impl_destroyInstance(Instance* instance);

        friend result llri::createInstance(const instance_desc& desc, Instance** instance);
        friend void llri::destroyInstance(Instance* instance);

    public:
        /**
         * @brief Get a vector of available adapters.
         * Using this function, you can select one or more adapters for llri::Device creation.
         * The adapters returned by this function are individual adapters and are listed separately regardless of SLI/Crossfire/Multi-GPU configuration. //TODO: SLI/Crossfire support
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if adapters is nullptr.
         * @return Otherwise it may return: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed.
        */
        result enumerateAdapters(std::vector<Adapter*>* adapters);

        /**
         * @brief Creates a virtual LLRI device. Device represents one or multiple adapters and enables you to allocate memory, create resources, or send commands to the adapter.
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if the instance is nullptr, if device is nullptr, if desc.adapter is nullptr, or if desc.numExtensions is more than 0 and desc.extensions is nullptr.
         * @return ErrorDeviceLost if the adapter was lost.
        */
        result createDevice(const device_desc& desc, Device** device) const;

        /**
         * @brief Destroy the LLRI device. This does not delete the resources allocated through the device, that responsibility remains with the user.
        */
        void destroyDevice(Device* device) const;

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Instance() = default;
        ~Instance() = default;

        void* m_ptr = nullptr;
        void* m_debugAPI = nullptr;
        void* m_debugGPU = nullptr;

        validation_callback_desc m_validationCallback;
        bool m_shouldConstructValidationCallbackMessenger;
        void* m_validationCallbackMessenger = nullptr; //Allows API to store their callback messenger if needed

        std::map<void*, Adapter*> m_cachedAdapters;

        result impl_enumerateAdapters(std::vector<Adapter*>* adapters);
        result impl_createDevice(const device_desc& desc, Device** device) const;
        void impl_destroyDevice(Device* device) const;
    };
}
