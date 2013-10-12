//
//  Action.h
//  MickeyGame
//
//  Created by Christopher Gulliver on 05/28/13.
//  Copyright 2013 Disney Mobile. All rights reserved.
//

#ifndef WalaberEngine_ActionTimeline_h
#define WalaberEngine_ActionTimeline_h

#include <IAction.h>
#include <ActionList.h>

namespace Walaber
{

    /*! An action timeline, it is esentially a series of sequences wrapped into a concurrent action.
    This means that each track executes independnet of one another, and will complete as quickly as
    they can. In order to organize multiple actions between different tracks, one could use the delay
    action to pad actions to execute at the same time, or one could add a concurrent action to a track
    that contains the actions that should be executed at the same time.
    */
    class ActionTimeline : public IAction
    {
    public:
        /*!
        Constructor
        @param numberOfTracks the initial number of tracks to support for this timeline
        @param completeWhenEmpty if the timeline should return true from execute when it is empty,
        i.e. all actions have been completed. This parameter should only ever be true when the timeline
        is being used to execute actions as a manager, but not when used to organize actions as part of
        a sequence, concurrent, or another timeline. 
        */
        ActionTimeline( int numberOfTracks )
        {
            _createAllTracks( numberOfTracks );
        }

        /*!
        Virtual destructor
        */
        virtual ~ActionTimeline()
        {
            clearActions();
        }

        /*!
        Add the specified action to the specified track. All tracks run concurrently. Once an action
        has been added to an action list it is wrapped in a shared pointer and will be deleted when it
        finishes execution.
        @param trackID the track to add this action to, usually an enum identifier
        @param action a pointer to the action, this is wrapped in a shared pointer, and deleted when necessary
        */
        template< typename T >
        void addActionToTrack( int trackID, T * action )
        {
            addActionToTrack( trackID, CreateActionPtr( action ) );
        }

        /*!
        Add the specified action to the specified track. All tracks run concurrently.
        @param trackID the track to add this action to, usually an enum identifier
        @param action a shared pointer to the action
        */
        void addActionToTrack( int trackID, IActionPtr action )
        {
            
            // Sanity check
            assert( trackID >= 0 );
            assert( trackID < mTracks.size() );

            if ( trackID < mTracks.size() )
            {
                // Append the action to the track's sequence
                mTracks[trackID]->addAction( action );
            }
        }

        int getActionUnitsForTrack( int trackID )
        {
            // Sanity check
            assert( trackID >= 0 );
            assert( trackID < mTracks.size() );
            
            return mTracks[trackID]->calculateActionUnits();
        }
        
        /*!
        Add an additional track to the timeline
        */
        void addTrack()
        {
            // Create a new resident action sequence which will be our container for the track
            ActionSequencePtr trackSequence( new ActionSequence() );
            mTracks.push_back( trackSequence );
        }

        /*!
        Ensure that the timeline has the specified number of tracks (capacity)
        @param capacity the number of tracks to ensure that the timeline has
        */
        void reserve( int capacity )
        {
            while ( mTracks.size() < capacity )
            {
                addTrack();
            }
        }

        /*!
        Clear all the actions in the timeline
        */
        void clearActions()
        {
            std::vector< ActionSequencePtr >::iterator trackIterator    = mTracks.begin();
            std::vector< ActionSequencePtr >::iterator trackIteratorEnd = mTracks.end();

            for ( ; trackIterator != trackIteratorEnd; ++trackIterator )
            {
                ActionSequencePtr track = *trackIterator;
                track->clearActions();
            }
        }

        /*!
        clear all the actions from the specified track
        @trackID the id for the track
        */
        void clearTrack( int trackID )
        {
            assert( trackID >= 0 );
            assert( trackID < mTracks.size() );

            mTracks[trackID]->clearActions();
        }

        /*!
        Executes all actions in the timeline
        @return true when all actions have been executed
        */
        virtual bool execute(float elapsedTime)
        {
            std::vector< ActionSequencePtr >::iterator trackIterator    = mTracks.begin();
            std::vector< ActionSequencePtr >::iterator trackIteratorEnd = mTracks.end();

            for ( ; trackIterator != trackIteratorEnd; ++trackIterator )
            {
                ActionSequencePtr track = *trackIterator;

                track->performExecute( elapsedTime );
            }

            return false;
        }

    private:
        ActionTimeline();

        /*!
        Create the specified number of tracks
        */
        void _createAllTracks( int numberOfTracks )
        {
            for ( int trackIterator = 0; trackIterator < numberOfTracks; ++trackIterator )
            {
                addTrack();
            }            
        }

        std::vector< ActionSequencePtr > mTracks;
    };
    MAKE_SHARED_PTR( ActionTimeline );
}

#endif
