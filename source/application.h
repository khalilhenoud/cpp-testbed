/**
 * @file application.h
 * @brief Application driver class.
 */
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <cstdio>

namespace core {

    /**
     * @brief Everything is controlled by this class, inherit and instantiate.
     */
    class Application
    {
    public:
        /// Saves the current instance to a static class reference.
        Application()
        {
            instance = this;
        }

        /// Reset the static class reference if it was this one.
        virtual ~Application()
        {
            if (instance == this)
                instance = NULL;
        }

        // Initialization, cleanup and update functions.
        virtual void Initialize(void) = 0;
        virtual void Cleanup(void) = 0;
        virtual void Update(void) = 0;

        /**
         * @brief Gets the current application reference.
         * @remarks No checks are done for the reference validity.
         */
        static Application *GetCurrentApplication(void)
        {
            return Application::instance;
        }

    private:
        /// The current active instance.
        static Application *instance;
    };
}

#endif
