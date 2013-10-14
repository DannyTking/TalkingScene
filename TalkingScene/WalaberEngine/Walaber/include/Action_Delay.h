//
//  Action_Delay.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_Delay_h
#define WalaberEngine_Action_Delay_h

#include <IAction.h>

namespace Walaber
{
    /*! An action that waits / delays for a specific amount of time before finishing execution
    */
    class Action_Delay : public IAction
    {
    public:
        Action_Delay( float delayTime ) :
            mTimeElapsed(0),
            mTimeToDelay(delayTime)
        {
            // Verify that the delay is not negative
            assert( mTimeToDelay >= 0.0f ); // Why you trying have negative delay?
        }

        /*!
        Execute until the delay time has expired
        @param elapsedTime the time change since the last time execute was called
        @return true when the delay time has expired
        */
        virtual bool execute(float elapsedTime )
        {
            mTimeElapsed += elapsedTime;
            return mTimeElapsed >= mTimeToDelay;
        }

    protected:
        Action_Delay();
        
        float mTimeElapsed;
        float mTimeToDelay;
    };
    MAKE_SHARED_PTR( Action_Delay );

}

#endif
