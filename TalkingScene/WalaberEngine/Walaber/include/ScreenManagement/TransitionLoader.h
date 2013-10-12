#ifndef _TRANSITION_LOADER_H_
#define _TRANSITION_LOADER_H_

#include "Transition.h"

#include <vector>
#include <Callback.h>

namespace Walaber
{
	class PropertyList;
	
	class TransitionLoader
	{
	public:
		
		static void transitionFromNameDefault( void* data );
		
        struct TransitionLoaderParameters
        {
            unsigned int inID;
            Transition* outTransition;
            
            TransitionLoaderParameters(unsigned int id):
            inID(id),
            outTransition(0)
            {}
        };
        
		typedef std::vector< CallbackPtr >			TransitionLoaderFunctionList;
		
		static void addTransitionLoader( CallbackPtr slf ) { mTransitionFunctions.push_back(slf); }
		
		static Transition* createTransition( unsigned int n );
		
	private:
		
		static TransitionLoaderFunctionList mTransitionFunctions;
	};
	
}



#endif	// _TRANSITION_LOADER_H_