//
//  Action_FloatSlerpOut.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_FloatSlerpOut_h
#define WalaberEngine_Action_FloatSlerpOut_h

#include <IAction.h>
#include <Action_FloatLerpTo.h>

namespace Walaber
{

    /*!
    Action to vector interpolate along a curve to a new value
    */
    class Action_FloatSlerpOut : public Action_FloatLerpTo
    {
	public:
		Action_FloatSlerpOut( float & operand, const float & targetValue, float timeToLerp ) :
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
            return slerpout( mStartValue, mTargetValue, mLerpTimeElapsed / mTimeToLerp );
		}
        
    private:
        Action_FloatSlerpOut();
    };    
    MAKE_SHARED_PTR( Action_FloatSlerpOut );

}


#endif
