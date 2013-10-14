//
//  Action_VectorMoveTo.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_VectorMoveTo_h
#define WalaberEngine_Action_VectorMoveTo_h

#include <IAction.h>
#include <Vector2.h>
#include <WalaberMath.h>

namespace Walaber
{

    /*!
    Action to assign the operand vector to the target vector
    */
    class Action_VectorMoveTo : public IAction
    {
	public:
		Action_VectorMoveTo( Vector2 & operand, const Vector2 & targetValue ) :
            mOperand( operand ),
            mTargetValue( targetValue )
		{
		}

        /*!
        Set the operand position to the target value specified in the constructor
        @param elapsedTime the time change since the last time execute was called
        @return true when all actions have been executed
        */
		virtual bool execute(float elapsedTime)
		{
            mOperand = mTargetValue;
			return true;
		}

	private:
		Action_VectorMoveTo();

        Vector2 &   mOperand;
		Vector2     mTargetValue;
    };
    MAKE_SHARED_PTR( Action_VectorMoveTo );

}
#endif
