//
//  Action_VectorSlerpTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_VectorSlerpTo_h
#define WalaberEngine_Action_VectorSlerpTo_h

#include <IAction.h>
#include <Action_VectorLerpTo.h>

namespace Walaber
{

    /*!
    Action to spherical interpolate a vector to a new value
    */
    class Action_VectorSlerpTo : public Action_VectorLerpTo
    {
	public:
		Action_VectorSlerpTo( Vector2 & operand, const Vector2 & targetValue, float timeToLerp ) :
            Action_VectorLerpTo( operand, targetValue, timeToLerp )
		{
		}
        
	protected:
        /*!
        Method to actually perform the spherical interpolate
        @return the new vector value
        */
		virtual Vector2 _interpolate()
		{
			return mStartValue.slerp( mTargetValue, mLerpTimeElapsed / mTimeToLerp );
        }
    private:
        Action_VectorSlerpTo();
    };
    MAKE_SHARED_PTR( Action_VectorSlerpTo );
        
}


#endif
