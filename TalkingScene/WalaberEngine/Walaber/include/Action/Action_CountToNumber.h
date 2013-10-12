//
//  Action_CountToNumber.h
//  MickeyGame
//
//  Created by Christopher Gulliver on 02/08/13.
//  Copyright 2013 Disney Mobile. All rights reserved.
//

#ifndef _WALABER_ACTION_COUNT_TO_NUMBER_H_
#define _WALABER_ACTION_COUNT_TO_NUMBER_H_

// System Includes

// Walaber Includes
#include "IAction.h"
#include <Callback.h>

namespace Walaber
{
    /*!
    A templated countdown timer, useful for Int, Float, and Double. Can be used to Count up, or down
    to the target value at the specified increment, with the specifed step time. Every time the
    value changes the increment callback will be called
    */
    template< typename T >
    class Action_CountToNumber : public IAction
    {
    public:
        struct CountCallbackParameters
        {
            T currentNumber;
        };

        /*!
        Count the specified a number to the ending number incrementing every stepDelaySec time and
        Call the, invoking the callback every increment/decrement. The callback will be passed the
        CountCallbackParameters parameter, which contains the current value of the count.
        @param startNumber the beginning number to increment/decrement from
        @param endNumber the target value to reach, at the end of execution the value is guaranteed
                            to be this value.
        @param stepValue The amount to increment/decrement
        @param stepDelaySec The time between the increments/decrements
        @param stepCallback A callback that will be called after each increment/decrements
        @see CountCallbackParameters
        */
        Action_CountToNumber( const T startNumber, const T endNumber, const T stepValue,
                              const float stepDelaySec, CallbackPtr stepCallback ) :
            mCurrentNumber( startNumber ),
            mEndNumber( endNumber ),
            mStepIncrement( stepValue ),
            mTimeElapsed( 0 ),
            mStepDelaySec( stepDelaySec ),
            mCallback( stepCallback )
        {
            // Test to see if this is a count down, if it is, change the increment to be negative
            if ( startNumber > endNumber && mStepIncrement > 0 )
            {
                mStepIncrement *= -1;
            }
        }

        /*!
        Increment the number every frame and invoke the callback
        @param elapsedTime the time change since the last time execute was called
        @return true if the action has completed execution
        */
        virtual bool execute( float elapsedTime )
        {
            mTimeElapsed += elapsedTime;
            
            if ( mTimeElapsed >= mStepDelaySec )
            {
                mTimeElapsed = 0;

                // Do the actual increment/decrement
                mCurrentNumber += mStepIncrement;

                // Clamp to make sure we do not go over our end value 
                if ( mStepIncrement > 0 )
                {
                    if ( mCurrentNumber > mEndNumber )
                    {
                        mCurrentNumber = mEndNumber;
                    }
                }
                else
                {
                    if ( mCurrentNumber < mEndNumber )
                    {
                        mCurrentNumber = mEndNumber;
                    }
                }

                // Invoke the callback to inform the caller that an increment has been made
                if ( mCallback )
                {
                    CountCallbackParameters params;
                    params.currentNumber = mCurrentNumber;
                
                    mCallback->invoke( &params );
                }

                if ( mCurrentNumber == mEndNumber )
                {
                    // Execution has finished, we have reached our end value
                    return true;
                }
            }

            // Continue executing
            return false;
        }

    private:
        T           mCurrentNumber; ///< The current value of the count
        T           mEndNumber;     ///< The target number to count to
        T           mStepIncrement; ///< The amount to increment every stepDelaySec
        float       mTimeElapsed;   ///< The time that has elapsed between steps
        float       mStepDelaySec;  ///< The time to wait between increments
        CallbackPtr mCallback;      ///< Callback to call whenever the value changed
    };

}

#endif // _WALABER_ACTION_COUNT_TO_NUMBER_H_