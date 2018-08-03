/**
 * @file frameratecontroller.h
 * @brief Functionality to control the frame rate.
 */
#ifndef _FRAME_RATE_CONTROLLER_H_
#define _FRAME_RATE_CONTROLLER_H_

#include <windows.h>

namespace utils {

    /**
     * @brief Responsible for regulating/controlling the frame rate.
     * @remarks The frame rate will synchronize with the v-sync, to get the actual frame rate, use
     * the extension to decouple the frame rate from the v-sync, but generally you will want that
     * feature up and running.
     * @remarks Note that the frame rate should always be less or equal to the refresh rate.
     * @remarks In locking the frame rate using QueryPerformanceCounter, it is largely preferable
     * to use a combination of sleep and precise looping in order to save up CPU usages and still
     * remains accurate.
     * @remarks We need to re-ask for the frequency every loop when using a high performance
     * counter, this is due to the change in energy consumption and CPU states. to be done later.
     */
    class FrameRateController
    {
    public:
        FrameRateController() {}
        ~FrameRateController() {}

        /**
         * @brief Initializes the frame rate controller. Determines whether resolution counters
         * exists, otherwise rely on default windows timing functions.
         */
        static void Initialize(void)
        {
            // Determining whether or not to use performance counters.
            if (QueryPerformanceFrequency((LARGE_INTEGER *)&ticks_per_second))
                high_resolution_counter_supported = true;
            else
                high_resolution_counter_supported = false;

            // In case high resolution counters are not supported, resort to 'timeBeginPeriod'.
            if (!high_resolution_counter_supported)
                timeBeginPeriod(1);

            // The frame rate is unlocked by default.
            locked = false;
        }

        /// Reset 'timeEndPeriod' in case high resolution counters weren't detected.
        static void Cleanup(void)
        {
            if (!high_resolution_counter_supported)
                timeEndPeriod(1);
        }

        /// Start tracking the time.
        static void Start(void)
        {
            if (high_resolution_counter_supported)
                QueryPerformanceCounter((LARGE_INTEGER *)&at_start);
            else
                t_start = timeGetTime();
        }

        /**
         * @brief Called at the end of the game loop. Optionally blocks till the targeted frame
         * rate is reached.
         */
        static void End(void)
        {
            if (high_resolution_counter_supported) {
                // Get the end time.
                QueryPerformanceCounter((LARGE_INTEGER *)&at_end);
                long long delta = at_end - at_start;
                double seconds = (double)delta/(double)ticks_per_second;

                // Calculate the frame rate.
                current_frame_rate = (int)(1./seconds);

                // Locking the frame rate, we calculate the amount we require the program to sleep.
                if (locked) {
                    if (delta < target_ticks_per_frame) {
                        // Calculating the amount of milliseconds to sleep, minus two.
                        long long remainingticks = target_ticks_per_frame - delta;
                        long long milliseconds = (remainingticks * 1000)/ticks_per_second - 2;

                        if (milliseconds > 0)
                            Sleep((DWORD)milliseconds);

                        // Get the new time, and recalculate delta.
                        QueryPerformanceCounter((LARGE_INTEGER *)&at_end);
                        delta = at_end - at_start;

                        // Extremely high precision (maybe microseconds).
                        while (delta < target_ticks_per_frame) {
                            QueryPerformanceCounter((LARGE_INTEGER *)&at_end);
                            delta = at_end - at_start;
                        }
                    }
                }
            } else {
                t_end = timeGetTime();
                // Possible loss of precision.
                int delta = (int)(t_end - t_start);
                current_frame_rate = (1000/delta);

                // Locking the frame rate.
                if (locked) {
                    while (delta < target_time_per_frame) {
                        t_end = timeGetTime();
                        delta = t_end - t_start;
                    }
                }
            }
        }

        /// Returns actual frame rate.
        static int GetFrameRate(void)
        {
            return current_frame_rate;
        }

        /// Returns the target frame rate, not the actual one.
        static int GetLockedFrameRateTarget(void)
        {
            return target_frame_rate;
        }

        /// Locks the frame rate at a certain target or lower, frames per second.
        static void LockFrameRateAt(int frameratetarget)
        {
            locked = true;
            target_frame_rate = frameratetarget;

            // In case of high resolution counters, we need to find the target ticks per frame.
            if (high_resolution_counter_supported)
                target_ticks_per_frame = ticks_per_second/frameratetarget;
            else
                target_time_per_frame = 1000/frameratetarget;
        }

        /// Unlocks frame rate.
        static void UnlockFrameRate(void)
        {
            locked = false;
        }

    private:
        /// Stores the starting time, specific to the high resolution counters.
        static long long at_start;
        /// Stores the end time, specific to the high resolution counters.
        static long long at_end;
        /// Ticks per second, specific to the high resolution counters.
        static long long ticks_per_second;
        /// The frame rate we are targeting.
        static int target_frame_rate;
        /// Holds the required ticks per second we need to maintain the targeted frame rate.
        static long long target_ticks_per_frame;
        /// Flag that controls whether the frame rate is locked or not.
        static bool locked;
        /// Whether high resolution counters are supported.
        static bool high_resolution_counter_supported;
        /// The current frame rate.
        static int current_frame_rate;
        /// Holds the number of milliseconds at which each frame is expected to run.
        static int target_time_per_frame;
        /// Stores the starting time, 'timeGetTime' specific.
        static DWORD t_start;
        /// Stores the ending time, 'timeGetTime' specific.
        static DWORD t_end;
    };
}

#endif
