//
//  Action_IntLerpTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_IntLerpTo_h
#define WalaberEngine_Action_IntLerpTo_h

#include <IAction.h>

namespace Walaber
{

    /*!
    Action to vector interpolate an integer to a target value
    */
    class Action_IntLerpTo : public IAction
    {
	public:
		Action_IntLerpTo( int & operand, const int & targetValue, float timeToLerp ) :
            mOperand    ( operand ),
            mStartValue ( 0 ),
            mTargetValue( targetValue ),
            mTimeToLerp ( timeToLerp ),
            mLerpTimeElapsed( 0 )
		{
		}
        
        /*!
        Called the first time the action is executed, before the execute. On the first execution
        frame executeSetup, and execute will both be called.
        @param elapsedTime the time change since the last time execute was called
        */
        virtual void executeSetup(float elapsedTime)
        {
            mStartValue = mOperand;
        };
        
        /*!
        @param elapsedTime the time change since the last time execute was called
        @return true when all actions have been executed
        */
		virtual bool execute( float elapsedTime )
		{
            // Add the time that has elapsed since the last execute
            mLerpTimeElapsed += elapsedTime;

            // Clamp time to ensure it does not exceed the time we're supposed to lerp
            mLerpTimeElapsed = minf( mLerpTimeElapsed, mTimeToLerp );

            // Apply the actual interp value
            mOperand = static_cast<int>( _interpolate() );

            // If the lerp has run out of time, then it has reached the end
            return mLerpTimeElapsed >= mTimeToLerp;
		}
        
	protected:
        /*!
        Method to actually perform the interpolation across a curve
        @return the new vector value
        */
		virtual float _interpolate()
		{
            return lerp( mStartValue, mTargetValue, mLerpTimeElapsed / mTimeToLerp );
		}

        int &       mOperand;
        int         mStartValue;
        int         mTargetValue;
        float       mTimeToLerp;
        float       mLerpTimeElapsed;      

    private:
        Action_IntLerpTo();
    };
    MAKE_SHARED_PTR( Action_IntLerpTo );
    
}


#endif
