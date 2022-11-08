///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   10.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace sugo
{
/**
 * Interface class for runnable classes.
 */
class IRunnable
{
public:
    virtual ~IRunnable()
    {
    }

    /**
     * Starts the runnable instance.
     * @return true if the instance could be started successfully.
     */
    virtual bool start() = 0;

    /**
     * Stops the runnable instance.
     */
    virtual void stop() = 0;

    /**
     * Indicates if the instance is still running.
     * @return true if the instance is still running.
     */
    virtual bool isRunning() const = 0;
};
}  // namespace sugo
