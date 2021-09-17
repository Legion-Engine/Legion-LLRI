/**
 * @file command_list.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline std::string to_string(command_list_usage usage)
    {
        switch(usage)
        {
            case command_list_usage::Direct:
                return "Direct";
            case command_list_usage::Indirect:
                return "Indirect";
        }

        return "Invalid command_list_usage value";
    }

    inline std::string to_string(command_list_state state)
    {
        switch(state)
        {
            case command_list_state::Empty:
                return "Empty";
            case command_list_state::Recording:
                return "Recording";
            case command_list_state::Ready:
                return "Ready";
        }

        return "Invalid command_list_state value";
    }

    inline result CommandList::begin(const command_list_begin_desc& desc)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (m_state != command_list_state::Empty)
        {
            const std::string msg = std::string("CommandList::begin() returned ErrorInvalidState because CommandList::queryState() returned ") + to_string(m_state) + ". CommandList must be in the command_list_state::Empty state before calling begin().";
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
            return result::ErrorInvalidState;
        }

        if (m_group->m_currentlyRecording)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "CommandList::begin() returned ErrorOccupided because its parent CommandGroup is already recording a CommandList.");
            return result::ErrorOccupied;
        }
        m_group->m_currentlyRecording = this;
#endif
        
#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_begin(desc);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_begin(desc);
#endif
    }

    inline result CommandList::end()
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (m_state != command_list_state::Recording)
        {
            const std::string msg = std::string("CommandList::end() returned ErrorInvalidState because CommandList::queryState() returned ") + to_string(m_state) + ". CommandList must be in the command_list_state::Recording state before calling end().";
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
            return result::ErrorInvalidState;
        }

        m_group->m_currentlyRecording = nullptr;
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_end();
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_end();
#endif
    }

    template<typename Func, typename ...Args>
    result CommandList::record(const command_list_begin_desc& desc, Func&& function, Args&&... args)
    {
        const result r = begin(desc);
        if (r != result::Success)
            return r;

        std::invoke(std::forward<Func>(function), std::forward<Args>(args)...);

        return end();
    }
}
