#include "TransitionLoader.h"

#include "Transition.h"

#include "Transition_Wipe.h"
#include "Transition_Fade.h"
#include "Transition_Block.h"
#include "Transition_Slide.h"
#include "Transition_Iris.h"
#include "Transition_Widget_Slide.h"

#include <PropertyList.h>

namespace Walaber
{

	TransitionLoader::TransitionLoaderFunctionList TransitionLoader::mTransitionFunctions(1, CallbackPtr(new Callback(&TransitionLoader::transitionFromNameDefault)));
	
	
	Transition* TransitionLoader::createTransition( unsigned int n )
	{
		Transition* ret = NULL;
		
		for (TransitionLoaderFunctionList::iterator it = mTransitionFunctions.begin(); it != mTransitionFunctions.end(); it++)
		{
			TransitionLoaderParameters params(n);
            (*it)->invoke( &params );
			
            ret = params.outTransition;
            
			if (ret != NULL)
				return ret;
		}
		
		return ret;
	}
	
	

	void TransitionLoader::transitionFromNameDefault( void* data )
	{
        TransitionLoaderParameters* params = (TransitionLoaderParameters*)data;
		params->outTransition = 0;
		
		switch (params->inID)
		{
			case Transition::TT_Wipe:
				params->outTransition = new Transition_Wipe();
				break;
				
			case Transition::TT_Fade:
				params->outTransition = new Transition_Fade();
				break;
				
			case Transition::TT_Block:
				params->outTransition = new Transition_Block();
				break;
				
			case Transition::TT_Slide:
				params->outTransition = new Transition_Slide();
				break;

			case Transition::TT_Iris:
				params->outTransition = new Transition_Iris();
				break;
				
			case Transition::TT_WidgetSlide:
				params->outTransition = new Transition_Widget_Slide();
				break;
		}
	}
}