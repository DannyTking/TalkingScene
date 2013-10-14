//
//  Action_FloatSlerpIn.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_FloatSlerpIn_h
#define WalaberEngine_Action_FloatSlerpIn_h

#include <IAction.h>
#include <Action_FloatLerpTo.h>

namespace Walaber
{

    /*!
    Action to vector interpolate along a curve to a new value
    */
    class Action_FloatSlerpIn : public Action_FloatLerpTo
    {
	public:
		Action_FloatSlerpIn( float & operand, const float & targetValue, float timeToLerp ) :
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
            return slerpin( mStartValue, mTargetValue, mLerpTimeElapsed / mTimeToLerp );
		}
        
    private:
        Action_FloatSlerpIn();
    };
    MAKE_SHARED_PTR( Action_FloatSlerpIn );
    
}


#endif
