//
//  Action_NodeScaleTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_NodeScaleTo_h
#define WalaberEngine_Action_NodeScaleTo_h

#include <IAction.h>
#include <Node.h>

namespace Walaber
{
    /*!
     Action to linear interpolate a node to a new value
     */
    class Action_NodeScaleTo : public IAction
    {
	public:
		Action_NodeScaleTo( Node & operand, const Vector2 & targetValue, float timeToLerp, bool slerp ) :
        mOperand        ( operand ),
        mTimeToLerp     ( timeToLerp ),
        mTargetValue    ( targetValue ),
        mLerpTimeElapsed( 0 ),
        mSlerp          ( slerp )
		{
		}
        
        /*!
         Called the first time the action is executed, before the execute. On the first execution
         frame executeSetup, and execute will both be called.
         @param elapsedTime the time change since the last time execute was called
         */
        virtual void executeSetup(float elapsedTime)
        {
            mStartValue = mOperand.getLocalScale2D();
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
            mOperand.setLocalScale2D( _interpolate() );
            
            // If the lerp has run out of time, then it has reached the end
			return mLerpTimeElapsed >= mTimeToLerp;
		}
        
	protected:
        /*!
         Method to actually perform the linear interpolate
         virtual to be overloaded by other interpolation actions.
         @return the new vector value
         */
		virtual Vector2 _interpolate()
		{
            if(!mSlerp)
            {
                return mStartValue.lerp( mTargetValue, mLerpTimeElapsed / mTimeToLerp );
            }
            else
            {
                return mStartValue.slerp( mTargetValue, mLerpTimeElapsed / mTimeToLerp );
            }
		}
        
        Node &  mOperand;
		Vector2 mStartValue;
		Vector2 mTargetValue;
		float   mTimeToLerp;
		float   mLerpTimeElapsed;
        bool    mSlerp;
        
	private:
		Action_NodeScaleTo();
    };
    MAKE_SHARED_PTR( Action_NodeScaleTo );
}


#endif
