//
//  IAction.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_IAction_h
#define WalaberEngine_IAction_h

#include <macros.h>
#include <Utilities.h>

namespace Walaber
{
    /*!
    @class An interface class to define the interface for an action
    Actions perform an amount of work each update through the execute method.
    */
    class IAction : Utilities::noncopyable
    {
    public:
        /*!
        perform the execute for the action, this method calls executeSetup and execute on the 
        children classes
        @param elapsedTime the time change in seconds since the last time execute was called
        @return true if the action has completed execution
        */
        bool performExecute( float elapsedTime )
        {
            // Setup if it has not yet been setup
            if ( !hasSetup() )
            {
                executeSetup( elapsedTime );
                setHasSetup();
            }

            // Perform the actual executation, now that the setup has been called
            return execute( elapsedTime );
        }

        /*!
        How many action units are part of this action
        @return the number of action units
        */
        virtual int calculateActionUnits() const
        {
            return 1;
        }

        /*!
        Virtual destructor
        */
        virtual ~IAction() {}

    protected:
        /*!
        Called the first time the action is executed, before the execute. On the first execution
        frame executeSetup, and execute will both be called.
        @param elapsedTime the time change in seconds since the last time execute was called
        */
        virtual void executeSetup(float elapsedTime) {};

        /*!
        Perform an amount of work for this action, this method will be called each update
        until it returns true. Implement this method in each child action.
        @param elapsedTime the time change since the last time execute was called
        @return true if the action has completed execution
        */
        virtual bool execute(float elapsedTime) = 0;

        /*!
        Set that the action has been setup
        */
        void setHasSetup()
        {
            mHasSetup = true;
        }

        /*!
        If the action has been setup or not, if it has not executeSetup should be called before
        execute
        @return true if the action has been setup
        */
        bool hasSetup()
        {
            return mHasSetup;
        }
        
        /*!
        A protected constructor, to ensure that with the help of the 
        */
        IAction() :
            mHasSetup( false )
        {}

    private:
        bool mHasSetup;
    };
    MAKE_SHARED_PTR( IAction );

    /*!
    A helper function for creating shared action ptrs
    @param
    @param 
    @return CallbackPtr representing a shared pointer to the action
    */
    template< typename T >
    IActionPtr CreateActionPtr( T * action )
    {
        assert( action != NULL );
        return SharedPtr< T >( action );
    }
}

#endif
