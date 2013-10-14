//
//  Action_FloatSlerpTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_FloatSlerpTo_h
#define WalaberEngine_Action_FloatSlerpTo_h

#include <IAction.h>
#include <Action_FloatLerpTo.h>

namespace Walaber
{


    /*!
    Action to vector interpolate along a curve to a new value
    */
    class Action_FloatSlerpTo : public Action_FloatLerpTo
    {
	public:
		Action_FloatSlerpTo( float & operand, const float & targetValue, float timeToLerp ) :
            Action_FloatLerpTo( operand, targetValue, timeToLerp )
		{
		}
        
	protected:
        /*!
        Method to actually perform the interpolation across a curve
        @return the new vector value
        */
		virtual float _interpolate()
		{
            return slerp( mStartValue, mTargetValue, mLerpTimeElapsed / mTimeToLerp );
		}
        
    private:
        Action_FloatSlerpTo();
    };
    MAKE_SHARED_PTR( Action_FloatSlerpTo );
    
}


#endif
