//
//  Action_NodeScaleByCurve.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_NodeScaleByCurve_h
#define WalaberEngine_Action_NodeScaleByCurve_h

#include <IAction.h>
#include <Node.h>

namespace Walaber
{
    class Action_NodeScaleByCurve : public IAction
    {
    public:
        /*!
        @param operand the node to be operated on
        @param multiplier the multiplier for this wiggle, values between 0 and 1 can be used to
            dampen the wiggle, larger numbers make it more pronounced.
        */
		Action_NodeScaleByCurve( Node & operand, CurvePtr curve, const float multiplier, float duration ) :
            mOperand    ( operand ),
            mDuration   ( duration ),
            mMultiplier ( multiplier ),
            mBounceCurve( curve ),
            mTimeElapsed( 0 )
		{
		}

        /*!
        @param elapsedTime the time change since the last time execute was called
        @return true when all actions have been executed
        */
		virtual bool execute( float elapsedTime )
		{
            mTimeElapsed += elapsedTime;
            
            // Do some awesome wiggling
            Vector2 oldScale = mOperand.getLocalScale2D();
            Vector2 newScale;
            
            // The curve starts at one - we just want the offset from one
            newScale.X = mBounceCurve->evaluate( getInterpVal( mTimeElapsed, 0, mDuration ) ) - 1.0f;
            // The y is opposite x (conservation of matter!  When x increases, y decreases the same amount)
            newScale.Y = newScale.X * -1.0f;
            
            // Curve may too intense for our liking
            newScale *= mMultiplier;
            // And finally the curve is moved back to one
            newScale += Vector2::One;
            
            // Finally, apply it to teh widget
            mOperand.applyScaleOffset2D(newScale - oldScale);

            return mTimeElapsed >= mDuration;
		}

	protected:
        Node &  mOperand;
		float   mMultiplier;
		float   mDuration;
		float   mTimeElapsed;
        CurvePtr mBounceCurve;

        private:
            Action_NodeScaleByCurve();
    };
}


#endif
