//
//  Action_FloatLerpTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_FloatLerpTo_h
#define WalaberEngine_Action_FloatLerpTo_h

#include <IAction.h>
#include <Vector2.h>
#include <WalaberMath.h>

namespace Walaber
{

    /*!
    Action to vector interpolate along a curve to a new value
    */
    class Action_FloatLerpTo : public IAction
    {
	public:
		Action_FloatLerpTo( float & operand, const float & targetValue, float timeToLerp ) :
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
            mOperand = _interpolate();

            // If the lerp has run out of time, then it has reached the end
            return mLerpTimeElapsed >= mTimeToLerp;
		}
        
        virtual ~Action_FloatLerpTo()
        {}
        
	protected:
        /*!
        Method to actually perform the interpolation across a curve
        @return the new vector value
        */
		virtual float _interpolate()
		{
            return lerp( mStartValue, mTargetValue, mLerpTimeElapsed / mTimeToLerp );
		}

        float &     mOperand;
        float       mStartValue;
        float       mTargetValue;
        float       mTimeToLerp;
        float       mLerpTimeElapsed;

    protected:
        Action_FloatLerpTo();
    };
    MAKE_SHARED_PTR( Action_FloatLerpTo );
    
}


#endif
