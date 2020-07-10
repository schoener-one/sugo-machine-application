///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   10.07.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef IRUNNABLE_HPP_
#define IRUNNABLE_HPP_

namespace moco
{
/**
 * Interface class for runnable classes.
 */
class IRunnable
{
public:
    virtual ~IRunnable() {}

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

protected:
    IRunnable() {}
};
}  // namespace moco

#endif  // IRUNNABLE_HPP_
