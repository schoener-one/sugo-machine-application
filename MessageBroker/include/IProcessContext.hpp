///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   26.09.2022
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRunnable.hpp"
#include "Thread.hpp"

namespace sugo::message
{
/**
 * Interface class representing the process context.
 */
class IProcessContext : public IRunnable
{
public:
    /**
     * @brief Set the process runner function to be used on start from this context.
     * Must not be set during running!
     *
     * @param process     Process to be used.
     * @param stopProcess Process to be called in order to stop the process.
     * @return true       If the process has not yet started and could be set.
     * @return false      If the process has already started.
     */
    virtual bool setProcessRunner(Thread::Runnable process, Thread::Runnable stopProcess) = 0;

    /**
     * Waits until process thread has finished.
     */
    virtual void waitUntilFinished() = 0;
};

}  // namespace sugo::message
