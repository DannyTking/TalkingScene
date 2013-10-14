//
//  AnimationManager.h
//  WalaberEngine
//
//  Created by Jason Pecho on 4/21/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include <map>
#include <vector>
#include <string>

#include <AnimationSharedDefinitions.h>
#include <Animation.h>

namespace Walaber
{
    class NumericAnimationTrack;
    class NodeAnimationTrack;
    
    /*!
     Class that manages the all of the animation tracks that apply to a animations and runs the 
     playhead for an animation.
    */
    class AnimationManager
    {
    public: // enums
        enum AnimationLoadBehavior
        {
            AnimationLoadBehavior_INVALID = 0,
            AnimationLoadBehavior_Overwrite,
            AnimationLoadBehavior_Append,
        };
        
    public: // interface
        
        /*!
         Ctor
        */
        AnimationManager();
        
        /*!
         Copy Ctor
        */
        AnimationManager(const AnimationManager& aniMgr);
        
        /*!
         operator =
        */
        AnimationManager& operator=(const AnimationManager& aniMgr);
        
        /*!
         Dtor
        */
        virtual ~AnimationManager();
        
        /*!
         update the manager and any running animations
         @param deltaTime the elapse time since last frame
        */
        virtual void update(float deltaTime);
        
        /*!
         Adds an animation to the animation mgr
         @param animation animation to be added to the manager. Manager will clean up the memory
        */
        virtual void addAnimation(Animation* animation);
        
        /*!
         creates an animation with name, and stores it in the map
         @param name name of the animation to create
         @return a pointer to the created animation
        */
        virtual Animation* createAnimation(const std::string& name);
        
        /*!
         Adds numeric animation track to an animation 
         @param aniName name of the animation to add the track to
         @param aniTrack the animation track to add to the animation
        */
        virtual void addNumericTrackToAnimation(const std::string& aniName, NumericAnimationTrack* aniTrack);
        
        /*!
         Adds node animation track to an animation
         @param aniName name of the animation to add the track to
         @param aniTrack the animation track to add to the animation
        */
        virtual void addNodeTrackToAnimation(const std::string& aniName, NodeAnimationTrack* aniTrack);
		
        /*!
         Plays an animation 
         @param aniName name of animation to play
         @param mode play mode of the animation to play
         @param loops number of times to loop the animation if play mode is AET_Loops - plays 
          'loops+1' times - -1 loops = infinite
        */
        virtual void playAnimation(const std::string& aniName, AnimationPlaybackMode mode, int loops = 0);
        
		/*!
         Plays an animation
         @param aniName name of animation to play
         @param playOffset where to start the playhead for this animation. Value is in seconds
         @param mode play mode of the animation to play
         @param loops number of times to loop the animation if play mode is AET_Loops - plays
         'loops+1' times - -1 loops = infinite
        */
        virtual void playAnimation(const std::string& aniName, const float playOffset,
                           AnimationPlaybackMode mode, int loops = 0);
		
        /*!
         Cross fades from current animation to a new animation
         @param aniName name of animation to cross fade to
         @param fadeLength length of the cross fade in seconds
         @param mode play mode of the animation to play
         @param loops number of times to loop the animation if play mode is AET_Loops - plays
         'loops+1' times - -1 loops = infinite
        */
        virtual void crossFade(const std::string& aniName, const float fadeLength,
                       AnimationPlaybackMode mode, int loops = 0);
		
		/*!
         Cross fades from current animation to a new animation
         @param aniName name of animation to cross fade to
         @param playOffset where to start the playhead for this animation. Value is in seconds
         @param fadeLength length of the cross fade in seconds
         @param mode play mode of the animation to play
         @param loops number of times to loop the animation if play mode is AET_Loops - plays
         'loops+1' times - -1 loops = infinite
        */
        virtual void crossFade(const std::string& aniName, const float playOffset, const float fadeLength, AnimationPlaybackMode mode, int loops = 0);
        
        /*!
         Just play current animation
        */
        virtual void playCurrentAnimation();
        
        /*!
         Just stop current animation
        */
        virtual void stopCurrentAnimation();
        
        /*!
         Just pause current animation
        */
        virtual void pauseCurrentAnimation();
		
		/*!
         Check to see if current animation is playing
         @return true if current animation is playing, false otherwise
        */
        virtual bool isCurrentAnimationPlaying();
                
        /*!
         @return a pointer to the current animation being played, 0 if no animation is playing
        */
        Animation* getCurrentAnimation() { return mCurrentAnimation; }
        
        /*!
         Get an animation
         @param aniName name of animation to get a pointer to
         @return pointer to animation with name aniName, returns 0 if it doesn;t exists
        */
        Animation* getAnimation(const std::string& aniName);
        
        /*!
         Apply animations in screen space (revers the angles in animation)
         @param isScreenSpace true if the animation will be rendered in scree space (+Y down)
        */
        void setScreenSpace(const bool isScreenSpace)
        { 
            mIsScreenSpace = isScreenSpace;
        }
        
        /*!
          returns the name of the curve collection if the animation manager uses one
          @return path to curve collection aka its name
        */
		std::string getCurveCollectionName()
		{
			return mCurveCollection;
		}
		
        /*!
         sets the name of the curve collection this animation manager is using
         @param curveCollection name of curve collection
        */
		void setCurveCollectionName(const std::string& curveCollection)
		{
			mCurveCollection = curveCollection;
		}
        
        /*!
         Add a callback keyed by the pointer of the object.
         This means statics won't work here...for now
         @param key pointer to object that contains the callback pointer
         @param c callback smart pointer
        */
        void registerEventCallback(void* key, CallbackPtr c ) { mManagerCallbacks[key] = c; }
		
		/*!
         Remove the callback keyed by key
         @param key the key of the callback to be removed
        */
        void unregisterEventCallback( void* key ) 
		{ 
			mManagerCallbacks.erase(key); 
		}
        
        /*!
         This function removes all loaded animations from this animation manager
        */
        void clear();
        
    private: // members
        
        /*!
         Fires all of the callbacks in the manager. It doesn't check if the callback exists so 
         make sure it exists before calling
         @param anim the animation that is causing a callback to be fired
         @param eType the type of the event being fire
         @param eventID the ID of the event being fired it eType is AET_Marker
        */
        void _fireCallback(Animation* anim, AnimationEventType eType, int eventID);
		
		/*!
         Pass in a bitmask of animation events (typically returned from calling update on an animation) to
		 fire any callbacks associated with the Animation Manager.
         @param eventBitmask a bitmask of events that happened during an animation update step
        */
		void _handleAnimationEventCallbackWithBitmask( const unsigned int eventBitmask );
        
        
        typedef std::map< std::string, Animation* > AnimationMap;
        
        AnimationMap                        mAnimationsMap;
        
        Animation*                          mCurrentAnimation;        
        Animation*                          mFadeFromAnimation;
		std::map< void*, CallbackPtr >      mManagerCallbacks;
        std::string							mCurveCollection;

        float                               mCrossFadeTime;
        float                               mCrossFadeDuration;
        
        bool                                mIsCrossFadingTheSameAnimation;
        bool                                mIsScreenSpace;
        
        friend class Animation;
    };    
}

#endif
