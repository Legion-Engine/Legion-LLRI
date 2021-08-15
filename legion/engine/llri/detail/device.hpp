/**
 * @file device.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
//detail includes should be kept to a minimum but
//are allowed as long as dependencies are upwards (e.g. device may include adapter but not vice versa)
#include <llri/detail/adapter.hpp>

namespace LLRI_NAMESPACE
{
    struct adapter_extension;
    struct queue_desc;
    enum struct queue_type : uint8_t;
    class Queue;

    /**
     * @brief Device description to be used in Instance::createDevice().
    */
    struct device_desc
    {
        /**
         * @brief The adapter to create the instance for.
        */
        Adapter* adapter;
        /**
         * @brief The enabled adapter features.
         * It is **recommended** to only enable features that will be used because unused enabled features might disable driver optimizations.
        */
        adapter_features features;

        /**
         * @brief The number of device extensions in the device_desc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The adapter extensions, if device_desc::numExtensions > 0, then this **must** be a valid pointer to an array of adapter_extension.
         * If device_desc::numExtensions == 0, then this pointer **may** be nullptr.
        */
        adapter_extension* extensions;

        /**
         * @brief The number of queues that are in the device_desc::queues array. Device **can not** be created without queues, thus this value **must** be at least 1 or higher.
        */
        uint32_t numQueues;
        /**
         * @brief An array of device queue descriptions, which is used to create the queues upon device creation. This value **must** be a valid pointer to an array of queue_desc structures, with a size of at least device_desc::numQueues.
        */
        queue_desc* queues;
    };

    /**
     * @brief A Device is a virtual representation of an Adapter and can create/destroy/allocate/query resources for the said Adapter.
     */
    class Device
    {
        friend Instance;
    public:
        /**
         * @brief Query a created Queue by type and index.
         *
         * All queues are created upon device creation, and stored for quick access through queryQueue(). Queues are thus owned by the Device, the user **may** query the created queues for use, but the user never obtains ownership over the queue.
         *
         * Queues are stored contiguously (but separated by type) in the order of device_desc::queues. Thus if device_desc::queues contained [Graphics, Compute, Graphics, Transfer, Graphics], the graphics queues for that array could be accessed with index 0, 1, 2, and not by their direct index in the array.
         *
         * @param type The type of Queue. This value must be a valid queue_type value, and at least one of this queue type must have been requested during device creation.
         * @param index The Queue array index. Queues are stored per type so this index must be from 0 to n-1 where n is the number of requested queues of this particular type.
         * @param queue A pointer to the resulting queue variable.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage If type is not a valid enum value
         * @return ErrorInvalidUsage If index is more than the number of queues created of the given type
         * @return ErrorInvalidUsage If queue is nullptr.
         *
         * @note (Device nodes) Queues are shared across device nodes. The API selects nodes (Adapters) to execute the commands on based on command list parameters.
        */
        result queryQueue(queue_type type, uint8_t index, Queue** queue);

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Device() = default;
        ~Device() = default;

        void* m_ptr = nullptr;
        void* m_functionTable = nullptr;

        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;

        std::vector<Queue*> m_graphicsQueues;
        std::vector<Queue*> m_computeQueues;
        std::vector<Queue*> m_transferQueues;
    };
}
