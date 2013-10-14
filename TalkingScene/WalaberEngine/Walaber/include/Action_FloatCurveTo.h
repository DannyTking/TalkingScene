//
//  Action_FloatCurveTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_FloatCurveTo_h
#define WalaberEngine_Action_FloatCurveTo_h

#include <IAction.h>
#include <Action_FloatLerpTo.h>
#include <Curve.h>
#include <CurveManager.h>

namespace Walaber
{

    /*!
    Action to vector interpolate along a curve to a new value
    */
    class Action_FloatCurveTo : public Action_FloatLerpTo
    {
	public:
		Action_FloatCurveTo( float & operand, const float & targetValue, float timeToLerp, Curve & curve ) :
            Action_FloatLerpTo( operand, targetValue, timeToLerp ),
            mCurve           ( curve )
		{
		}
        
	protected:
        /*!
        Method to actually perform the interpolation across a curve
        @return the new vector value
        */
		virtual float _interpolate()
		{
            float timePercentCompleted  = ( mLerpTimeElapsed / mTimeToLerp ) * mCurve.getDuration();
            return mCurve.evaluate( timePercentCompleted );
		}
        
	private:
		Curve & mCurve;
    };
    MAKE_SHARED_PTR( Action_FloatCurveTo );
    
}


#endif
