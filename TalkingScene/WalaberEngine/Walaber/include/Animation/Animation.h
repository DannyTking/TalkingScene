//
//  Animation.h
//  WalaberEngine
//
//  Created by Jason Pecho on 4/21/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <vector>
#include <string>

#include <XML.h>
#include <AnimationSharedDefinitions.h>
#include <Callback.h>

namespace Walaber
{
    class NumericAnimationTrack;
    class NodeAnimationTrack;
    class SpriteAnimationTrack;
	class AnimationCueAnimationTrack;
	class SkeletonActorCueAnimationTrack;
    class AnimationManager;
    class SceneGraph;
    class Skeleton;
    class BasicBinaryReader;
    
    /*!
     The Animation class encapsulates the all of the tracks that make up an animations. It also includes the playhead.
     Animations ar econnected to a specific Animation Manager.
     */
    class Animation
    {
    public: // typedefs, enums, and structs
        typedef std::vector< NumericAnimationTrack* >           NumericTrackList;
        typedef std::vector< NodeAnimationTrack* >              NodeTrackList;
        typedef std::vector< SpriteAnimationTrack* >            SpriteTrackList;
		typedef std::vector< AnimationCueAnimationTrack* >		CueTrackList;
		typedef std::vector< SkeletonActorCueAnimationTrack* >	SkeletonActorCueTrackList;
        typedef std::vector< std::pair< float, int > >          TimeMarkerPairs;
        
    public: //interface
        /*!
         Constructor
         @param name the name of the animation
         @param aniMgr a pointer to the animation manager that will run theese animations
        */
        Animation(const std::string name, AnimationManager* aniMgr);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Copy Ctor - deep copy
        // </Summary>
        Animation(const Animation& animation);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op= - deep copy
        // </Summary>
        Animation& operator=(const Animation& animation);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~Animation();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy properties only
        // </Summary>
        void copyProperties(const Animation& animation);
        
        // -------------==============-------------==============-------------==============-------------===
        // Creates an animation given an xml file, returns animation for first animation found in xml
        // </Summary>
        static bool createAnimationFromXml(const std::string& xmlPath, SceneGraph* sceneGraph);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Creates an animation given an xmlNodePtr
        // </Summary>
        static Animation* createAnimationFromXmlNode( XMLDocument::Iterator animationRootNode, SceneGraph* sceneGraph);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Creates an animation given an xmlNodePtr
        // </Summary>
        static Animation* createAnimationFromXmlNode( XMLDocument::Iterator animationRootNode, Skeleton* skeleton);
        
        /*!
          Creates an animation given an BinaryReader containing a animation file binary buffer
          @param reader a BinaryReader containing an animation file binary buffer that has been 
                        seeked to the FPS entry in the buffer
          @param skeleton the skeleton to be animated
        */
        static Animation* createAnimationFromBinary( BasicBinaryReader& reader,
                                                     const std::string& animationName,
                                                     Skeleton* skeleton );

        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Creates an animation given an xmlNodePtr
        // </Summary>
        void setAnimationManager(AnimationManager* aniMgr) { mAnimationManager = aniMgr; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add an animation track to the animation
        // </Summary>
        void addNumericTrack(NumericAnimationTrack* numericAnimationTrack);        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add an animation track to the animation
        // </Summary>
        void addNodeTrack(NodeAnimationTrack* nodeAnimationTrack);  
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add an animation track to the animation
        // </Summary>
        void addSpriteTrack(SpriteAnimationTrack* spriteAnimationTrack);  
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add an animation track to the animation
        // </Summary>
        void addAnimationCueTrack(AnimationCueAnimationTrack* cueAnimationTrack);  
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add an animation track to the animation
        // </Summary>
        void addSkeletonActorCueTrack(SkeletonActorCueAnimationTrack* skeletonActorCueAnimationTrack);  
        
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // check to see if the notified track length is higher than current and set animation length if it is
        // </Summary>
        void notifyTrackLength(float trackLength)
        {
            if(trackLength > mAnimationLength)
            {
                mAnimationLength = trackLength;
            }
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // play this animation with current playback settings from it's current point
        // </Summary>
        void play() 
        { 
            mIsPlaying = true;
            _fireCallback(AET_Began, -1);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // play this animation with playback mode and number of loops from the beginning
        // </Summary>
        void play(const AnimationPlaybackMode playbackMode, int loops = 0, const AnimationBlendMode blendMode = ABM_Default) 
        {
            play(playbackMode, 0.0f, loops, blendMode);
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // play this animation with playback mode and number of loops from the beginning
        // </Summary>
        void play(const AnimationPlaybackMode playbackMode, const float timeOffset, int loops = 0, const AnimationBlendMode blendMode = ABM_Default) 
        {
            mRealTime = timeOffset;
            mIsPlaying = true; 
            mBlendMode = blendMode;
            mPlaybackMode = playbackMode;
            mLoopsLeft = loops;  
			
			if(timeOffset != 0.0f)
			{
				_updateLastHitMarkerIndexWithCurrentTime();
				_updateSpriteTrackEventsWithCurrentTime();
				_updateCueTrackEventsWithCurrentTime();
			}
			else
			{
				mLastHitMarkerIndex = -1;
				_rewindSpriteTracks();
				_rewindCueTracks();
			}
            _fireCallback(AET_Began, -1);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // stop this animation and reset it to beginning
        // </Summary>
        void stop() 
        { 
            mIsPlaying = false; 
            mRealTime = 0.0f;
            _fireCallback(AET_Stopped, -1);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // pause this animation
        // </Summary>
        void pause() 
        {
            mIsPlaying = false; 
            _fireCallback(AET_Paused, -1);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // play the animation in reverse
        // </Summary>
        void playReverse()
        {
            mIsPlaying = true;
            mRealTime = mAnimationLength;
            mPlayForward = false;
            _fireCallback(AET_Began, -1);
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns whether or not the animation is currently playing,
        // </Summary>
        bool isPlaying() 
        {
			return mIsPlaying;            
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // update the animation and apply animation tracks with 'weight'
        // returns a bit mask of events
        // </Summary>
        unsigned int update(float deltaTime, const float weight = 1.0f, const bool isScreenSpace = false);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the play speed of the animation - 1.0 is normal speed
        // </Summary>
        void setPlaybackSpeed(float speed) { mPlaybackSpeed = speed; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the number of loops left
        // </Summary>
        void setLoopsLeft(int loops)
        {
            mLoopsLeft = loops;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the blend mode of the animation
        // </Summary>
        void setBlendMode(const AnimationBlendMode blendMode) { mBlendMode = blendMode; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the animation to play forward
        // </Summary>
        void setForward() { mPlayForward = true; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the animation to play backward
        // </Summary>
        void setReverse() { mPlayForward = false; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // tell the animation if it's being cross faded from
        // </Summary>
        void setCrossFadingFrom(bool v) { mIsCrossFadingFrom = v; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the fps of the animation
        // </Summary>
        void setFPS(const float fps) { mFPS = fps; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set whether or not the animation should force itself to animate at its given FPS
        // </Summary>
        void setForceFPS(const bool v) { mForceFPS = v; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // applies tracks to animable values with weight
        // </Summary>
        void applyTracks(const float weight, const bool isScreenSpace = false);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the animation's name
        // </Summary>
        const std::string& getName() { return mName; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add a custom marker to the animation
        // </Summary>
        void addMarker(const float time, const int id); 
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the id of the last marker event
        // </Summary>
        int getLastMarkerID()
        {
            int ret = -1;
            if( mLastHitMarkerIndex >= 0 )
            {
                ret = mMarkers[mLastHitMarkerIndex].second;
            }
            
            return ret;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the id of the last marker event
        // </Summary>
        const std::vector< int >& getMarkersFiredThisFrame()
        {
            return mMarkersFiredThisFrame;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the length of the animation
        // </Summary>
        float getLength() const { return mAnimationLength; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the number of loops left in the animation
        // </Summary>
        int getLoopsLeft()
        {
            return mLoopsLeft;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Set the callback for animation events
        // </Summary>
        void setEventCallback( CallbackPtr c ) { mCallback = c; }
		
    private: //interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default ctor private to prevent use
        // </Summary>
        Animation();

        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // checks to see if any animation events have been triggered - if any events are triggered that 
        // require action to the time marker, the time marker will be adjusted accordingly
        // </Summary>
        unsigned int _checkForEventsAndHandleTime();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // checks to see if any animation events have been triggered - if any events are triggered that 
        // require action to the time marker, the time marker will be adjusted accordingly
        // </Summary>
        inline void _fireCallback(AnimationEventType eventType, int eventID)
        {            
            if(mCallback.use_count())
            {
                AnimationCallbackParameters params;
                params.anim = this;
                params.eType = eventType;
                params.eventID = eventID;
                
                mCallback->invoke(&params);
            }            
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls rewind on all of the sprite tracks in the animation
        // </Summary>
        void _rewindSpriteTracks();
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls rewind on all of the AnimationCue tracks in the animation
        // </Summary>
        void _rewindCueTracks();

		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls rewind on all of the AnimationCue tracks in the animation
        // </Summary>
        void _rewindSkeletonActorCueTracks();

		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls rewind on all of the AnimationCue tracks in the animation
        // </Summary>
		void _updateLastHitMarkerIndexWithCurrentTime();
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls rewind on all of the AnimationCue tracks in the animation
        // </Summary>
		void _updateSpriteTrackEventsWithCurrentTime();
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls rewind on all of the AnimationCue tracks in the animation
        // </Summary>
		void _updateCueTrackEventsWithCurrentTime();
        
        /*!
         Parses the attributes on a xml element for a node track and adds curves to track
         @param nodeTAnirack the node track to add the curves parsed from the nodeTrackIter to
         @param nodeTrackIter the nodetrack xml to parse to get the curves from
        */
        static void _parseNodeTrack( NodeAnimationTrack* nodeAniTrack,
                                     XMLDocument::NamedIterator nodeTrackIter );
        
        
        /*!
         Based on the flags passed in, constructs curve paths for each channel
         @param nodeTAnirack the node track to add the curves parsed from the nodeTrackIter to
         @param flags the flags that represent which curves need to be loaded for this node track
         @param locationNamePath the path to the location of the curve including the basename of the
                                 node
        */
        static void _buildTrackFromFlags( NodeAnimationTrack* nodeAniTrack,
                                          unsigned int flags,
                                          const std::string& locationNamePath,
                                          BasicBinaryReader& reader );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // marker comparator function
        // </Summary>
        static bool _markerCompare( const std::pair< float, int >& a, const std::pair< float, int >& b )
        {
            return (a.first<b.first);
        }
        
    private: // members
        
        std::string                                 mName;
        
        NumericTrackList                            mNumericTrackList;
        NodeTrackList                               mNodeTrackList;
        SpriteTrackList                             mSpriteTrackList;
		CueTrackList								mCueTrackList;
		SkeletonActorCueTrackList					mSkeletonActorCueTrackList;
        TimeMarkerPairs                             mMarkers;
        CallbackPtr                                 mCallback;
        AnimationManager*                           mAnimationManager;
        
        AnimationBlendMode                          mBlendMode; // blend mode of animation
        AnimationPlaybackMode                       mPlaybackMode;
        float                                       mAnimationLength; // length of longestTrack
        float                                       mRealTime; // real time in seconds of the animation - tjhis is the playhead 
        float                                       mModifiedTime; // time used for tracks - in the case of non-forced FPS, mRealTime and mModifiedTime will be the same
        float                                       mPlaybackSpeed; // how fast to playback the animation
        float                                       mFPS; // the fps this animation was built for
        int                                         mLoopCount; // total loop count
        int                                         mLoopsLeft; // -1 if it loops forever
        int                                         mLastHitMarkerIndex; // last triggered marker event index
		std::vector< int >							mMarkersFiredThisFrame; // holds a list of markers fired this frame, cleared every frame
        bool                                        mIsPlaying; // whether the animation is playing or not
        bool                                        mPlayForward; // false if play backward
        bool                                        mIsCrossFadingFrom; // is true if being crossfaded from
        bool                                        mForceFPS;
    };
}

#endif
