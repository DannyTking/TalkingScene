//
//  AnimationManager.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 4/21/11.
//  Copyright 2011 Disney Mobile. All rights reserved.
//

#include "AnimationManager.h"

#include "Animation.h"
#include "CurveManager.h"

namespace Walaber
{
    // ---------------------------------------------------------------------------------------------
    
    AnimationManager::AnimationManager():
    mAnimationsMap(),
    mCurrentAnimation(NULL),
    mFadeFromAnimation(NULL),
    mManagerCallbacks(),
    mCurveCollection(),
    mCrossFadeTime(0.0f),
    mCrossFadeDuration(0.0f),
    mIsCrossFadingTheSameAnimation(false),
    mIsScreenSpace(false)
    {
                
    }
    
    // ---------------------------------------------------------------------------------------------
    
    AnimationManager::AnimationManager(const AnimationManager& aniMgr):
    mAnimationsMap(aniMgr.mAnimationsMap),
    mCurrentAnimation(aniMgr.mCurrentAnimation),
    mFadeFromAnimation(aniMgr.mFadeFromAnimation),
    mManagerCallbacks(aniMgr.mManagerCallbacks),
    mCurveCollection(aniMgr.mCurveCollection),
    mCrossFadeTime(aniMgr.mCrossFadeTime),
    mCrossFadeDuration(aniMgr.mCrossFadeDuration),
    mIsCrossFadingTheSameAnimation(aniMgr.mIsCrossFadingTheSameAnimation),
    mIsScreenSpace(aniMgr.mIsScreenSpace)
    {
        // deep copy animations
    }
    
    // ---------------------------------------------------------------------------------------------
    
    AnimationManager& AnimationManager::operator=(const AnimationManager& aniMgr)
    {
        mAnimationsMap = aniMgr.mAnimationsMap;
        
        mCurrentAnimation = aniMgr.mCurrentAnimation;
        mFadeFromAnimation = aniMgr.mFadeFromAnimation;
        mManagerCallbacks = aniMgr.mManagerCallbacks;
        mCurveCollection = aniMgr.mCurveCollection;
        mCrossFadeTime = aniMgr.mCrossFadeTime;
        mCrossFadeDuration = aniMgr.mCrossFadeDuration;
        
        mIsCrossFadingTheSameAnimation = aniMgr.mIsCrossFadingTheSameAnimation;
        mIsScreenSpace = aniMgr.mIsScreenSpace;

        return *this;
    }
    
    // ---------------------------------------------------------------------------------------------
    
    AnimationManager::~AnimationManager()
    {
        clear();
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::update(float deltaTime)
    {
        unsigned int eventBitMask = 0;
        if(mFadeFromAnimation)
        {
            mCrossFadeTime += deltaTime;
            if(mCrossFadeTime <= mCrossFadeDuration)
            {
                float normalizedTime = mCrossFadeTime / mCrossFadeDuration;
                float invertedNormalizedTime = 1.0f - normalizedTime;
                
                mFadeFromAnimation->update(deltaTime, invertedNormalizedTime, mIsScreenSpace);
                eventBitMask = mCurrentAnimation->update(deltaTime, normalizedTime, mIsScreenSpace);
            }
            else
            {
                
                if(mIsCrossFadingTheSameAnimation)
                {
                    mFadeFromAnimation->copyProperties(*mCurrentAnimation);
                    Animation* temp = mCurrentAnimation;
                    mCurrentAnimation = mFadeFromAnimation;
                    delete temp;
                    mIsCrossFadingTheSameAnimation = false;
                }
                mCurrentAnimation->setBlendMode(ABM_None);
                mFadeFromAnimation = NULL;
            }
        }
        else if(mCurrentAnimation)
        {
            eventBitMask = mCurrentAnimation->update(deltaTime, 1.0f, mIsScreenSpace);
        }
        
		_handleAnimationEventCallbackWithBitmask( eventBitMask );

    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::addAnimation(Animation* animation)
    {
#if _DEBUG
        AnimationMap::iterator it = mAnimationsMap.find(animation->getName());
        if(it != mAnimationsMap.end())
        {
            Logger::printf("WMW", Logger::SV_FATAL, "Cannot addAnimation. Animation [%s] already "
                           "exists!", animation->getName().c_str());
            assert(0);
        }
#endif
        animation->setAnimationManager(this);
        mAnimationsMap[animation->getName()] = animation;
    }
    
    // ---------------------------------------------------------------------------------------------
    
    Animation* AnimationManager::createAnimation(const std::string& name)
    {
        Animation* animation = new Animation(name, this);
        
        mAnimationsMap[name] = animation;
        
        return animation;
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::addNumericTrackToAnimation(const std::string& aniName,
                                                      NumericAnimationTrack* aniTrack)
    {
        AnimationMap::iterator it = mAnimationsMap.find(aniName);
        
        if(it != mAnimationsMap.end())
        {
            (*it).second->addNumericTrack(aniTrack);
        }
        else
        {
            // animation does not exist
            Logger::printf("WMW", Logger::SV_FATAL, "Cannot addNumericTrackToAnimation. Animation [%s] does not "
                           "exist!", aniName.c_str());
            assert(0);
        }        
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::addNodeTrackToAnimation(const std::string& aniName,
                                                   NodeAnimationTrack* aniTrack)
    {
        AnimationMap::iterator it = mAnimationsMap.find(aniName);
        
        if(it != mAnimationsMap.end())
        {
            (*it).second->addNodeTrack(aniTrack);
        }
        else
        {
            // animation does not exist
            Logger::printf("WMW", Logger::SV_FATAL, "Cannot addNodeTrackToAnimation. Animation [%s] does not "
                           "exist!", aniName.c_str());
            assert(0);
        }        
    }
    
	// ---------------------------------------------------------------------------------------------
    
    void AnimationManager::playAnimation(const std::string& aniName, AnimationPlaybackMode mode,
                                         int loops)
    {
		playAnimation(aniName, 0.0f, mode, loops);
	}
	
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::playAnimation(const std::string& aniName, const float playOffset,
                                         AnimationPlaybackMode mode, int loops)
    {
        AnimationMap::iterator it = mAnimationsMap.find(aniName);
        
        if(it != mAnimationsMap.end())
        {
            if(mIsCrossFadingTheSameAnimation)
            {
                // if we're cross fading the same animation and we call play in the middle, the fade
                // from animation is actually in our map, there current animation is the temp we
                // created to fade to, so get rid of it, we don't need it anymore
                delete mCurrentAnimation;
                mIsCrossFadingTheSameAnimation = false;
            }
            // set the fade from pointer to null becuase we aren't fading when we call play!
            mFadeFromAnimation = NULL;
            
            mCurrentAnimation = (*it).second;
            mCurrentAnimation->setCrossFadingFrom(false);
            mCurrentAnimation->play(mode, playOffset, loops);
            
            // we update with 1 weight to put the sprite animations in their first keyframe and
            // jump the animation to its start frame instantly
            unsigned int eventBitmask = mCurrentAnimation->update(0.0f, 1.0f, mIsScreenSpace);
            
			_fireCallback(mCurrentAnimation, AET_Began, -1);
			_handleAnimationEventCallbackWithBitmask( eventBitmask );
        }
        else
        {
            // animation does not exist
            Logger::printf("WMW", Logger::SV_FATAL, "Cannot playAnimation. Animation [%s] does not "
                           "exist!", aniName.c_str());
            assert(0);
        }  
    }
	
	// ---------------------------------------------------------------------------------------------
    
    void AnimationManager::crossFade(const std::string& aniName, const float fadeLength,
                                     AnimationPlaybackMode mode, int loops)
    {
		crossFade(aniName, 0.0f, fadeLength, mode, loops);
	}
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::crossFade(const std::string& aniName, const float playOffset,
                                     const float fadeLength, AnimationPlaybackMode mode, int loops)
    {
        if(!mCurrentAnimation)
        {
            playAnimation(aniName, mode, loops);
			
			_fireCallback(mCurrentAnimation, AET_Began, -1);
			
            return;
        }
        
        AnimationMap::iterator it = mAnimationsMap.find(aniName);
        
        if(it != mAnimationsMap.end())
        {    
            // if we were in the middle of a crossfade to the same animation, we just want to copy
            // the properties of the current animation to our fade from because in this case,
            // mCurrentAnimation is a temp copy of the fade from animation
            if(mIsCrossFadingTheSameAnimation)
            {
                mFadeFromAnimation->copyProperties(*mCurrentAnimation);
            }     
            else
            {
                // otherwise we just copy the pointer
                mFadeFromAnimation = mCurrentAnimation;
            }
            mFadeFromAnimation->setBlendMode(ABM_None);            
            
            // check if we are about to cross fade the same animation
            if(mCurrentAnimation && (mCurrentAnimation->getName() == aniName))
            {
                if(!mIsCrossFadingTheSameAnimation)
                {
                    mCurrentAnimation = new Animation((*mFadeFromAnimation));
                }
                
                mIsCrossFadingTheSameAnimation = true;
            }
            else
            {
                if(mIsCrossFadingTheSameAnimation)
                {
                    delete mCurrentAnimation;
                    mIsCrossFadingTheSameAnimation = false;
                }
                mCurrentAnimation = (*it).second;
            }
            mFadeFromAnimation->setCrossFadingFrom(true);
            mCurrentAnimation->setCrossFadingFrom(false);
            mCurrentAnimation->play(mode, playOffset, loops, ABM_Additive); 
            
            // we update with 0 weight to put the sprite animations in their first keyframe
            unsigned int eventBitmask = mCurrentAnimation->update(0.0f,0.0f);
                
			_fireCallback(mCurrentAnimation, AET_Began, -1);
            _handleAnimationEventCallbackWithBitmask( eventBitmask );
			
            mCrossFadeTime = 0.0f;
            mCrossFadeDuration = fadeLength;
        }
        else
        {
            // animation does not exist
            Logger::printf("WMW", Logger::SV_FATAL, "Cannot crossFade. Animation [%s] does not "
                           "exist!", aniName.c_str());
            assert(0);
        } 
        
    }
    
    // ---------------------------------------------------------------------------------------------
   
    void AnimationManager::playCurrentAnimation()
    {
       if(mCurrentAnimation)
       {
           mCurrentAnimation->play();
               
		   _fireCallback(mCurrentAnimation, AET_Began, -1);
       }
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::stopCurrentAnimation()
    {
        if(mCurrentAnimation)
        {
            mCurrentAnimation->stop();
            
            _fireCallback(mCurrentAnimation, AET_Stopped, -1);
        }
    }
    
    // ---------------------------------------------------------------------------------------------
   
    void AnimationManager::pauseCurrentAnimation()
    {
        if(mCurrentAnimation)
        {
            mCurrentAnimation->pause();
                
			_fireCallback(mCurrentAnimation, AET_Paused, -1);
        }
    }

	// ---------------------------------------------------------------------------------------------
    
    bool AnimationManager::isCurrentAnimationPlaying()
    {
		bool ret = false;
        if(mCurrentAnimation)
        {
            ret = mCurrentAnimation->isPlaying();
        }
		return ret;
    }
    
    // ---------------------------------------------------------------------------------------------
   
    Animation* AnimationManager::getAnimation(const std::string& aniName)
    {
        AnimationMap::iterator it = mAnimationsMap.find(aniName);
        Animation* ret = 0;
        
        if(it != mAnimationsMap.end())
        {
            ret = (*it).second;
        }
        else
        {
            // animation does not exist
            Logger::printf("WMW", Logger::SV_FATAL, "Cannot getAnimation. Animation [%s] does not "
                           "exist!", aniName.c_str());
            assert(0);
        } 
        
        return ret;
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::clear()
    {
        AnimationMap::iterator it = mAnimationsMap.begin();
        while(it != mAnimationsMap.end())
        {
            delete (*it).second;
            it++;
        }
        mAnimationsMap.clear();
        
        if(mIsCrossFadingTheSameAnimation)
        {
            delete mCurrentAnimation;
        }
        
        mCurrentAnimation = 0;
        mFadeFromAnimation = 0;
        mIsCrossFadingTheSameAnimation = false;
        
        if( mCurveCollection.size() )
		{
			CurveManager::getManager().unloadCurveCollection( mCurveCollection );
		}
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void AnimationManager::_fireCallback(Animation* anim, AnimationEventType eType, int eventID)
    {
		if (!mManagerCallbacks.empty())
		{
			AnimationCallbackParameters params;
			params.anim = anim;
			params.eType = eType;
			params.eventID = eventID;
		 
			std::map< void*, CallbackPtr >::iterator it = mManagerCallbacks.begin();
			std::map< void*, CallbackPtr >::iterator itEnd = mManagerCallbacks.end();
			while (it != itEnd) 
			{			
				it->second->invoke( &params );
				++it;
			}
		}
    }
	
    // ---------------------------------------------------------------------------------------------
    
	void AnimationManager::_handleAnimationEventCallbackWithBitmask( const unsigned int
                                                                        eventBitmask )
	{
		// check bitfield for call backs
        if(eventBitmask && !mManagerCallbacks.empty())
        {
            if(eventBitmask & AET_Looped)
            {
                _fireCallback(mCurrentAnimation, AET_Looped, -1);
            }
            if(eventBitmask & AET_Finished)
            {
                _fireCallback(mCurrentAnimation, AET_Finished, -1);
            }
            if(eventBitmask & AET_Marker)
            {
				const std::vector< int >& markersFired =
                mCurrentAnimation->getMarkersFiredThisFrame();
				for( unsigned int i = 0; i < markersFired.size(); ++i)
				{
					_fireCallback(mCurrentAnimation, AET_Marker, markersFired[i]);
				}
            }
        }
	}
    
}
