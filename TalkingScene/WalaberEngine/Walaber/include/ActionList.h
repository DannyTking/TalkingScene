//
//  ActionList.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_ActionList_h
#define WalaberEngine_ActionList_h

// Standard Library Includes
#include <string>
#include <vector>
#include <list>

// Walaber includes
#include <IAction.h>

namespace Walaber
{
    typedef std::list< IActionPtr > ActionListContainer;

    class ActionList : public IAction
    {
    public:
        /*!
        Iterate through all contained actions and calculate the size (number of actions in this list)
        */
        virtual int calculateActionUnits() const
        {
            int unitCount = 0;
            
            ActionListContainer::const_iterator actionIterator    = mActions.begin();
            ActionListContainer::const_iterator actionIteratorEnd = mActions.end();

            for ( ; actionIterator != actionIteratorEnd; ++actionIterator )
            {
                IActionPtr action( *actionIterator );

                unitCount += action->calculateActionUnits();
            }

            return unitCount;
        }

        /*!
        Add an action to this list, once an action has been added to an action list it
        now belongs to this object, and will be deleted when it finishes execution.
        @param action the action to be added
        */
        void addAction( IActionPtr action )
        {
            mActions.push_back( action );
        }

        /*!
        Add the specified action to this list. Once an action has been added to an action list it is
        wrapped in a shared pointer and will be deleted when it finishes execution.
        @param trackID the track to add this action to, usually an enum identifier
        @param action a pointer to the action, this is wrapped in a shared pointer, and deleted when necessary
        */
        template< typename T >
        void addAction( T * action )
        {
            addAction( CreateActionPtr( action ) );
        }

        /*!
        Remove all actions and delete them as necessary
        */
        void clearActions()
        {
            // Delete all actions
            mActions.clear();
        }

        /*!
        Destructor
        */
        virtual ~ActionList()
        {
            clearActions();
        }
        
    protected:
        ActionList()
        {
        }

        ActionListContainer mActions;
    };
    MAKE_SHARED_PTR( ActionList );


    /*!
    A sequence, or chained list, of actions to be executed in order. An action can only be executed
    when all previous actions in this sequence have been completed. Once an action has been added
    to an action sequence it now belongs to this action, and will be deleted when it finishes execution.
    @see ActionConcurrent
    @see ActionList
    */
    class ActionSequence : public ActionList
    {
    public:
        ActionSequence()
        {
        }

        /*!
        Executes actions in sequential order, until an acion returns false
        @param elapsedTime the time change since the last time execute was called
        */
        virtual bool execute(float elapsedTime)
        {
            float adjustedElapsedTime = elapsedTime;
            
            // Execute
            if ( !mActions.empty() )
            {
                IActionPtr action( *mActions.begin() );

                bool actionCompleted = action->performExecute( adjustedElapsedTime );

                if ( actionCompleted )
                {
                    // An action's execute may have cleared the sequence
                    // make sure there is something to clear
                    if ( mActions.begin() != mActions.end() )
                    {
                        mActions.pop_front();
                    }
                }
            }

            // The sequence has finished when there is nothing left in it to execute
            return (mActions.size() == 0);
        }
    };
    MAKE_SHARED_PTR( ActionSequence );

    /*!
    A concurrent, or parrallel, list of actions to be executed. They are executed in order, however
    all actions in the list are executed every "execute". Once an action has been added it
    now belongs to this action, and will be deleted when it finishes execution. The term "concurrent"
    is used to describe that multiple actions will execute in a single frame, it has nothing to do
    with threads.
    @see ActionSequence
    */
    class ActionConcurrent : public ActionList
    {
    public:
        ActionConcurrent()
        {
        }
    
        /*!
        Executes all actions in the action list, this gives the illusion of
        concurrency. Actions are removed and deleted as they complete.
        @param elapsedTime the time change since the last time execute was called
        @return true when all actions have been executed
        */
        virtual bool execute(float elapsedTime)
        {
            ActionListContainer::iterator actionIterator = mActions.begin();
            while ( actionIterator != mActions.end() )
            {
                IActionPtr action( *actionIterator );
                bool actionCompleted = action->performExecute( elapsedTime );

                if ( actionCompleted )
                {
                    // An action's execute may have cleared the sequence
                    // make sure there is something to clear
                    if ( mActions.begin() != mActions.end() )
                    {
                        actionIterator = mActions.erase( actionIterator );
                    }
                }
                else
                {
                    ++actionIterator;
                }
            }

            // The concurrent action has finished when there is nothing left in it to execute
            return (mActions.size() == 0);
        }
    };
    MAKE_SHARED_PTR( ActionConcurrent );
    
}

#endif
