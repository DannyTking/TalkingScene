/*
 *  SkeletonActor.cpp
 *  WalaberEngine
 *
 *  Created by Jason Pecho on 12/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <SkeletonActor.h>

#include <Bone.h>
#include <SoundManager.h>
#include <AchievementManager.h>
#include <MemberCallback.h>
#include <CurveManager.h>
#include <XML.h>
#include <StringHelper.h>
#include <ParticleEmitter.h>
#include <SpriteBatch.h>

namespace Walaber 
{
	const std::string SkeletonActor::GROUPS_TO_LOAD_KEY = "load_groups";
    const std::string SkeletonActor::SCENES_TO_LOAD_KEY = "load_scenes";
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    SkeletonActor::SkeletonActor() :
    playingFromTest(0),
    mSkeleton(0),
	mProps(),
	mActiveProps(),
	mEmitters(),
	mPos(Vector2::Zero),
    mCurrentAnimation( -1 ),
    mCurrentAnimationGroup( -1 ),
    mCurrentScene( 0 ),
	mAngle(0.0f),
    mSize( 1.0f, 1.0f ),
    mIsScreenSpace(false),
    mHasLoadedSkeleton(false),
    mSkeletonFinished( false ),
    mCurrentPlaybackMode( PM_Once ),
    mCurrentAnimationPlayMode(AET_Began),
    mAnimationSoundState(ASS_On),
    mActivePropsForAnimation(),
	mXmlLoaded(false),
	mInitSuccessful(true),
    mInitSceneNameIdentifier( -1 ),
    mDisallowSounds(false)
    {   		
	}
	
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    SkeletonActor::~SkeletonActor()
	{						
		mActiveProps.clear();

        if ( mSkeleton != NULL )
        {
            mSkeleton->getAnimationManager()->unregisterEventCallback(this);
        }
		
		std::map< std::string, Prop* >::iterator it = mProps.begin();
		std::map< std::string, Prop* >::iterator itEnd = mProps.end();
		while ( it != itEnd )
		{
			delete it->second;
			++it;
		}
		
		unsigned int size = mEmitters.size();
		for(unsigned int i = 0; i < size; ++i)
		{
			delete mEmitters[i];
		}
		
		// we delete the skeleton after the props and emitters kuz those could be a part of the skeletons
		// hierarchy, or not, we have no way of knowing ( actually we do, but it's better this way!)
		if (mSkeleton)
			delete mSkeleton;
		
		AnimationNameToData::iterator aniNameIt = mAnimationNameToData.begin();
		AnimationNameToData::iterator aniNameItEnd = mAnimationNameToData.end();
		while(aniNameIt != aniNameItEnd)
		{
			EventTypeToActionDataMap& eventTypeToData = aniNameIt->second.eventActionData;
			EventTypeToActionDataMap::iterator eventTypeIt = eventTypeToData.begin();
			EventTypeToActionDataMap::iterator eventTypeItEnd = eventTypeToData.end();
			
			while(eventTypeIt != eventTypeItEnd)
			{
				AnimationEventDataList& dataList = eventTypeIt->second;
				dataList.clear();				
				++eventTypeIt;
			}
			
			eventTypeToData.clear();
			++aniNameIt;
		}
		mAnimationNameToData.clear();
		
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void SkeletonActor::loadSkeleton(const std::string& name, const std::string& skelFilepath)
	{
		if(mSkeleton)
		{ 
			delete mSkeleton;
			mSkeleton = 0;
		}
		
		MemberCallbackPtr<SkeletonActor>::type memcallback(new MemberCallback<SkeletonActor>(this, &SkeletonActor::_skeletonLoaded));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
        Skeleton::loadSkeletonWithCallback(skelFilepath, callback, mPos, mAngle, mSize, name );
	}

	void SkeletonActor::loadSkeletonWithAnimationDescriptions(const std::string& name, const std::string& skelFilepath, const std::string& animFilepath, CallbackPtr callback )
	{
        if ( callback )
        {
            mLoadedCallback = callback;
        }

        loadSkeletonWithAnimationDescriptions( name, skelFilepath, animFilepath );
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    
    void SkeletonActor::loadAnimationDescription( const std::string& animFilepath, CallbackPtr finishedCallback )
    {
        loadAnimationDescription( animFilepath, std::string(), std::string(), finishedCallback );
    }
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    
    void SkeletonActor::loadAnimationDescription( const std::string& animFilepath,
                                                  const std::string& groups,
                                                  const std::string& scenes,
                                                  CallbackPtr finishedCallback )
    {
        // Make sure we have a skeleton
        assert ( mSkeleton );

        // Save off the finished loading callback if one exists
        if ( finishedCallback )
        {
            mLoadedCallback = finishedCallback;
        }
        
        // Clear out the data (we're loading a new skeleton)
        
		_clearAnimationGroupData();
        
		// Load the animation XML
		PropertyList plist;
        if( !groups.empty() )
        {
            plist.setValueForKey( GROUPS_TO_LOAD_KEY, groups );
        }
        
        if( !scenes.empty() )
        {
            plist.setValueForKey( SCENES_TO_LOAD_KEY, scenes );
        }
        
		MemberCallbackPtr<SkeletonActor>::type memberCallback( new MemberCallback<SkeletonActor>(this, &SkeletonActor::_xmlFileLoaded) );
		CallbackPtr cb = static_pointer_cast<Callback>(memberCallback);
		FileManager::getInstancePtr()->readFile( animFilepath, cb, plist, FileManager::PP_NoAbstraction );
    }
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    
    void SkeletonActor::loadSkeletonWithAnimationDescriptions(const std::string& name, const std::string& skelFilepath, const std::string& animFilepath)
	{
		loadSkeleton(name, skelFilepath);
		
		loadAnimationDescription( animFilepath, CallbackPtr() );
	}
    
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~

    unsigned int SkeletonActor::getSceneNameIdentifier( const std::string& sceneName )
    {
		int iSceneName;
		std::map< std::string, unsigned int >::iterator it = mSceneNameToSceneBitFlag.find(sceneName);
		if(it != mSceneNameToSceneBitFlag.end())
		{
			iSceneName = it->second;
		}
		else
		{
			if( "" != sceneName)
			{
				// scene name does not match any scenes in this animation
				assert(0);
			}
			iSceneName = 0xFFFFFFFF;
		}
        
        return iSceneName;
    }

	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::init(const Vector2& pos, const std::string& sceneName, const Vector2& size, const bool isScreenSpace)
    {
        mInitSceneName = sceneName;
        if (!mXmlLoaded)
		{
			mInitSuccessful = false;
			mInitPos = pos;
			mInitSize = size;
			mInitIsScreenSpace = isScreenSpace;
			
			return;
		}

		int iSceneName = SkeletonActor::getSceneNameIdentifier( sceneName );
		
		init(pos, iSceneName, size, isScreenSpace);
	}
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::init(const Vector2& pos, const unsigned int sceneName, const Vector2& size, const bool isScreenSpace)
    {
        if (!mXmlLoaded)
		{
			mInitSuccessful = false;
			mInitPos = pos;
			mInitSize = size;
            mInitSceneNameIdentifier = sceneName;
			mInitIsScreenSpace = isScreenSpace;
			
			return;
		}
        
        // Set the coordinates
        mPos = pos;
        mSize = size;        
		
        mIsScreenSpace = isScreenSpace;
        
        if(mSkeleton)
        {
            mSkeleton->setLocalPosition2D(mPos);
			mSkeleton->setLocalAngleZ(mAngle);
            mSkeleton->setLocalScale2D(mSize);
            mSkeleton->setScreenSpace(isScreenSpace);
			
            mHasLoadedSkeleton = true;
        }
		
		mCurrentScene = sceneName;
		
		mActiveProps.clear();
		
		std::map< std::string, Prop* >::iterator it = mProps.begin();
		std::map< std::string, Prop* >::iterator itEnd = mProps.end();
		for ( ; it != itEnd; ++it )
		{
            _initProp( it->second );
		}
        
        mInitSuccessful = true;
    }

    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::_initProp( Prop * prop )
    {
        if( prop->getScenes() & mCurrentScene )
        {
            // play start animation
            const std::string& startAnimation = prop->getStartAnimation();
            if( !startAnimation.empty() )
            {
                if( prop->getPropType() == PropType_Sprite )
                {
                    Sprite* sprite = static_cast<Sprite*>( prop->getPropObject() );
                    sprite->playAnimation( startAnimation );
                    if( prop->getPropFlags() & PropFlags_PauseOnStart )
                    {
                        sprite->getCurrentAnimation()->pause();
                    }
                }
                else if( prop->getPropType() == PropType_Skeleton )
                {
                    Skeleton* skeleton = static_cast<Skeleton*>( prop->getPropObject() );
                    skeleton->getAnimationManager()->playAnimation( startAnimation, PM_Once );
                    if( prop->getPropFlags() & PropFlags_PauseOnStart )
                    {
                        skeleton->getAnimationManager()->pauseCurrentAnimation();
                    }
                }
            }
            
            // insert the prop from the prop map section into the active props
            mActiveProps.insert( std::make_pair(prop->getPropName(), prop) );
        }    
    }
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::update( float elapsed )
    {
        // Update SkeletonActor
        if(mSkeleton)
        {
            mSkeleton->update(elapsed);
        }
        
        if(mDisallowSounds)
        {
            stopAllSounds();
        }
        
		std::map< std::string, Prop* >::iterator it = mActiveProps.begin();
		std::map< std::string, Prop* >::iterator itEnd = mActiveProps.end();
		while ( it != itEnd )
		{
            Prop* prop = it->second;

            if( prop->getPropObject()->getVisible() )
            {
                if( prop->getPropType() == PropType_Sprite )
                {
                    Sprite* sprite = static_cast<Sprite*>( prop->getPropObject() );
                    sprite->update(elapsed);
                }
                else if( prop->getPropType() == PropType_Skeleton)
                {
                    Skeleton* skeleton = static_cast<Skeleton*>( prop->getPropObject() );
                    skeleton->update(elapsed);                    
                }
            }
			++it;
		}
		
		unsigned int size = mEmitters.size();
		for(unsigned int i = 0; i < size; ++i)
		{
			mEmitters[i]->update(elapsed);
		}
	 	
        // fire callback events
        while( ! mCallbackEventQueue.empty() )
        {
            CallbackAnimationEvent& event = mCallbackEventQueue.front();
            if( event.eventType == AET_Looped )
            {
                mActivePropsForAnimation.clear();
                
                // feels hacky to take care of looping here but i think this might require a rewrite
                // if we did it the right way :/
                Animation* currentAnimation = mSkeleton->getAnimationManager()->getCurrentAnimation();
                int loops = currentAnimation->getLoopsLeft();
                if( loops > 0 )
                {
                    --loops;
                }
                _playAnimation(mCurrentAnimation, mAnimationSoundState, 0.0f, PM_Loops, loops, false);
            }
            _fireCallback( event.eventType, event.eventID );
            mCallbackEventQueue.pop();
        }
        
        if(mNextAnimationGroup.nextAnimationGroup)
        {
            AnimationPlaybackMode apm = PM_Once;
            if(mNextAnimationGroup.loops != 0)
            {
                apm = PM_Loops;
            }
            _playAnimationFromGroup(mNextAnimationGroup.nextAnimationGroup, mAnimationSoundState, apm, mNextAnimationGroup.loops, mNextAnimationGroup.crossFade);
            
            mNextAnimationGroup.reset();
        }
        
        // Stop tracking sounds that are finished playing.
        std::vector<SoundEffectInstancePtr>::iterator soundItr = mSounds.begin();
        
        while(soundItr != mSounds.end())
        {
            if((*soundItr)->getState() == SoundEffectInstance::Stopped)
            {
                soundItr = mSounds.erase(soundItr);
            }
            else
            {
                ++soundItr;
            }
        }
    }
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::setColor(const Color& newColor )
    {
        mSkeleton->setColor( newColor );
    }

    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::makePropSceneProp( const std::string & propName, const std::vector< std::string > & scenes, const std::string & animation, bool pauseOnStart )
    {
        std::map< std::string, Prop* >::iterator it = mProps.find( propName );
		std::map< std::string, Prop* >::iterator itEnd = mProps.end();
		if ( it != itEnd )
		{
            Prop* prop = it->second;

            unsigned int scenesFlags = 0;
            for( unsigned int i=0; i < scenes.size(); ++i)
            {
                scenesFlags |= mSceneNameToSceneBitFlag[scenes[i]];
            }
            prop->setScenes( scenesFlags );
            prop->setStartAnimation( animation );
            prop->setPropFlag( PropFlags_Scene );

            if ( pauseOnStart )
            {
                prop->setPropFlag( PropFlags_PauseOnStart );
            }

            _initProp( prop );
		}        
    }
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::draw( SpriteBatch* sb, unsigned int drawFlags)
    {
        SpriteBatch::BatchMode prevBatchMode = sb->getBatchMode();
        sb->flush();
        sb->start( SpriteBatch::BM_DEPTH );
        
        if(mSkeleton)
        {
            if( drawFlags & DrawFlags_Skeleton )
            {
                mSkeleton->draw(sb);
            }
        }  	
		
		if( drawFlags & DrawFlags_Props )
		{
			std::map< std::string, Prop* >::iterator it = mActiveProps.begin();
			std::map< std::string, Prop* >::iterator itEnd = mActiveProps.end();
			while ( it != itEnd )
			{
                Prop* prop = it->second;

                if( prop->getPropType() == PropType_Sprite )
                {
                    Sprite* sprite = static_cast<Sprite*>( prop->getPropObject() );
                    sprite->draw(sb);
                }
                else if( prop->getPropType() == PropType_Skeleton )
                {
                    Skeleton* skeleton = static_cast<Skeleton*>( prop->getPropObject() );
                    skeleton->draw(sb);
                }
				
				++it;
			}
            
		}
        
        if ( drawFlags & DrawFlags_Emitters )
        {
            unsigned int size = mEmitters.size();
            for(unsigned int i = 0; i < size; ++i)
            {
                mEmitters[i]->draw(sb);
            }
        }
        
        sb->flush();
        sb->start( prevBatchMode );
    }	

	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::playAnimation( const std::string aniName, const float playOffset, AnimationPlaybackMode pm, int loops, bool crossFade)
    {
        mCurrentAnimationGroup = -1;
		_playAnimation( mAnimationNameToIndex[aniName], mAnimationSoundState, playOffset, pm, loops, crossFade );
	}
    
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::playAnimation( const std::string aniName, AnimationSoundState aniSoundState, const float playOffset, AnimationPlaybackMode pm, int loops, bool crossFade)
    {
        mCurrentAnimationGroup = -1;
		_playAnimation( mAnimationNameToIndex[aniName], aniSoundState, playOffset, pm, loops, crossFade );
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	std::string SkeletonActor::getArmatureAnimationForActorAnimation( std::string actorAnimName )
	{
		std::map< std::string, int >::iterator it = mAnimationNameToIndex.find( actorAnimName );
		
		if (it != mAnimationNameToIndex.end())
			return mArmatureNames[ it->second ];
			
		return "ERR";		
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::_playAnimation(const int aniIndex, AnimationSoundState aniSoundState, const float playOffset, AnimationPlaybackMode pm , int loops, bool crossFade)
	{
		if (!mXmlLoaded)
		{
			return;
		}

        clearNonSceneProps();

		// deactivate all active emitters
		unsigned int size = mEmitters.size();
		for(unsigned int i=0; i < size; ++i)
		{
			mEmitters[i]->setActive(false);
		}
		
        mAnimationSoundState = aniSoundState;
        mCurrentAnimation = aniIndex;
        mCurrentPlaybackMode = pm;
        
        if( pm == PM_Loops )
        {
            pm = PM_Once;
        }
        
		mActivePropsForAnimation.clear();
        
        mSkeletonFinished = false;
        
		// override the crossfade if its not aloud to cross fade
		if( mAnimationNameToData[mAnimationNames[aniIndex]].canCrossFade == false )
		{
			crossFade = false;
		}
		
        if(crossFade)
        {
            mSkeleton->getAnimationManager()->crossFade(mArmatureNames[aniIndex], playOffset, .5f, pm, loops);
        }
        else
        {
            mSkeleton->getAnimationManager()->playAnimation(mArmatureNames[aniIndex], playOffset, pm, loops);
        }
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::playAnimationFromGroup(const std::string groupName, AnimationPlaybackMode pm, int loops, bool crossFade )
    {
		_playAnimationFromGroup(mGroupNameToGroupBitFlag[groupName], mAnimationSoundState, pm, loops, crossFade);
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::playAnimationFromGroup(const std::string groupName, AnimationSoundState aniSoundState, AnimationPlaybackMode pm, int loops, bool crossFade )
    {
		_playAnimationFromGroup(mGroupNameToGroupBitFlag[groupName], aniSoundState, pm, loops, crossFade);
	}
	
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::_playAnimationFromGroup(const int groupName, AnimationSoundState aniSoundState, AnimationPlaybackMode pm, int loops, bool crossFade)
    {
        std::map< int, std::vector< std::string > >::iterator it = mAnimationGroupToNameMap.find(groupName);
        
        if(it != mAnimationGroupToNameMap.end())
        {
            const std::vector< std::string >& group = it->second;
			
			if(!group.empty())
			{
				int random = std::rand() % group.size(); 
				
				AnimationNameToData::iterator it_ = mAnimationNameToData.find(group[random]);
				
				int count = 0;
				while(!(it_->second.scenes & mCurrentScene))
				{
                    int randomRetriesCount = 0;
					int newRand = std::rand() % group.size();
					while( newRand == random
                          && group.size() > 1
                          && randomRetriesCount < 10 )
					{
						newRand = std::rand() % group.size();
                        ++randomRetriesCount;
					}
					random = newRand;
					
					it_ = mAnimationNameToData.find(group[random]);
					++count;
					
					// arbitrary number to break out of endless loop
					if( count > 10 )
					{
						break;
					}
				}
                
				if(count <= 10)
				{
                    mCurrentAnimationGroup = groupName;
					// call with string name
					_playAnimation( mAnimationNameToIndex[it_->first], aniSoundState, 0.0f, pm, loops, crossFade);
				}
				else
				{
                    if( mCurrentAnimation >= 0 )
                    {
                        _playAnimation(mCurrentAnimation, aniSoundState, 0.0f, pm, loops, crossFade);
                    }
				}
			}
        }
        else
        {
            assert(0);
        }
    }
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	bool SkeletonActor::isAnimationMemberOfGroup( const std::string& animName, const std::string& groupName )
	{
		std::map< std::string, int >::iterator it_g = mGroupNameToGroupBitFlag.find(groupName);
		
		if (it_g == mGroupNameToGroupBitFlag.end())
		{
			return false;
		}
		
		AnimationNameToData::iterator it_n = mAnimationNameToData.find( animName );
		if (it_n == mAnimationNameToData.end())
		{
			return false;
		}
		
		return (it_n->second.groups | it_g->second);		
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	bool SkeletonActor::isCurrentAnimationMemberOfGroup( const std::string& groupName )
	{
		std::map< std::string, int >::iterator it_g = mGroupNameToGroupBitFlag.find(groupName);
		
		if (it_g == mGroupNameToGroupBitFlag.end())
		{
			return false;
		}
		
		if ((mCurrentAnimation >= 0) && (mCurrentAnimation < mAnimationNames.size()))
		{
			AnimationNameToData::iterator it_n = mAnimationNameToData.find( mAnimationNames[mCurrentAnimation] );
			if (it_n == mAnimationNameToData.end())
			{
				return false;
			}
			
			return (it_n->second.groups & it_g->second);
		}
		
		return false;
	}
	
    
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::setPosition( Vector2 pos )
	{
		mPos = pos;
		mSkeleton->setLocalPosition2D(mPos);
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::setAngle(float a)
	{
		mAngle = a;
		mSkeleton->setLocalAngleZ(a);
	}
    
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::_skeletonLoaded(void* data)
    {
        Skeleton::SkeletonLoadedCallbackParameters* params = (Skeleton::SkeletonLoadedCallbackParameters*)data;

        // Verify that the skeleton was loaded
        // if you are asserting here then double check the exitence and file name of the skeleton
        assert( params->skel );

        mSkeleton = params->skel;

        MemberCallbackPtr<SkeletonActor>::type memcallback(new MemberCallback<SkeletonActor>(this, &SkeletonActor::_animationEventCallback));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
        mSkeleton->getAnimationManager()->registerEventCallback(this, callback);
        
        if(!mHasLoadedSkeleton)
        {
            mSkeleton->setLocalPosition2D(mPos);
            mSkeleton->setLocalScale2D(mSize);
			
            mSkeleton->setScreenSpace(mIsScreenSpace);
        }
    }
	
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void SkeletonActor::_xmlFileLoaded( void* data )
	{
		FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
        
        if(params->buffer)
        {
            std::vector< std::string > groupsToLoad;
            std::vector< std::string > scenesToLoad;
            
            std::set< std::string >  animationsToLoad;
            std::set< std::string >  propsToLoad;
            std::set< int >          emittersToLoad;
            
            if( params->userData.keyExists( GROUPS_TO_LOAD_KEY ) )
            {
                StringHelper::split( params->userData[GROUPS_TO_LOAD_KEY].asString(), '|',
                                    groupsToLoad );
            }
            
            if( params->userData.keyExists( SCENES_TO_LOAD_KEY ) )
            {
                StringHelper::split( params->userData[SCENES_TO_LOAD_KEY].asString(), '|',
                                    scenesToLoad );
            }
            
            XMLDocument xmlDoc(params->buffer, params->length, "AnimationList");
            
            // Malformed xml!!!
            assert( xmlDoc );
            if(xmlDoc)
            {                
				// Get all of the animation scenes
                XMLDocument::Iterator scenesIt = xmlDoc.getIteratorTo("Scenes");
                if(scenesIt)
                {
					int num = 0;
                    XMLDocument::Iterator sceneIt = scenesIt.getChildIterator();
                    
                    while(sceneIt)
                    {
						Property property;
						
						// Another animation group!
						if(sceneIt.getAttribute("name", property))
						{
							std::string sceneName = property.asString();
						
                            // we need to check if both groups and scenes are empty here because
                            // if scenes is empty but groups isn't, we don't want to load based on
                            // scenes
                            if( ( scenesToLoad.empty() && groupsToLoad.empty() ) || std::find( scenesToLoad.begin(), scenesToLoad.end(), sceneName ) != scenesToLoad.end() )
                            {
                                mSceneNameToSceneBitFlag[ sceneName ] = (1 << num);
							
                                ++num;
                            }
						}
						
                        ++sceneIt;
					}
				}
				
				// Get all of the animation groups
                XMLDocument::Iterator groupsIt = xmlDoc.getIteratorTo("Groups");
                if(groupsIt)
                {
					int num = 0;
					std::map<int, std::vector< AnimationGroupDataAux > > groupNameToGroupDataAux;
                    XMLDocument::Iterator groupIt = groupsIt.getChildIterator();
                    
                    while(groupIt)
                    {
						Property property;
						
						// Another animation group!
						if(groupIt.getAttribute("name", property))
						{
							std::string groupName = property.asString();
							
							if(StringHelper::toLower(groupName) == "none")
							{
								mGroupNameToGroupBitFlag[ groupName ] = 0;
							}
							else
							{
                                if( groupsToLoad.empty() ||
                                    std::find( groupsToLoad.begin(), groupsToLoad.end(), groupName )
                                    != groupsToLoad.end() )
                                {
                                    int groupFlag = (1 << num);
                                    mGroupNameToGroupBitFlag[ groupName ] = groupFlag;
                                    
                                    // And onto the group name map thing
                                    std::vector< std::string > groupNameList;
                                    mAnimationGroupToNameMap[ groupFlag ] = groupNameList;
                                    
                                    XMLDocument::Iterator groupDataIt = groupIt.getChildIterator();
                                    
                                    std::vector< AnimationGroupDataAux >* aux = 0;
                                    if(groupDataIt)
                                    {
                                         aux = &groupNameToGroupDataAux[groupFlag];
                                    }
                                    
                                    while(groupDataIt)
                                    {
                                        std::string nodeName = groupDataIt.getNodeName();
                                        if("NextAnimationGroup" == nodeName)
                                        {
                                            Property prop;
                                            aux->push_back(AnimationGroupDataAux());
                                            AnimationGroupDataAux& laux = aux->back();
                                            
                                            if(groupDataIt.getAttribute("name", prop))
                                            {
                                                laux.nextAnimationGroup = prop.asString();
                                            }			
                                                                                
                                            if(groupDataIt.getAttribute("crossFade", property))
                                            {
                                                laux.crossFade = property.asInt() ? true : false;
                                            }
                                            
                                            if(groupDataIt.getAttribute("loops", property))
                                            {
                                                laux.loops = property.asInt();
                                            }
                                            
                                            if(groupDataIt.getAttribute("probability", property))
                                            {
                                                laux.probability = property.asInt();
                                            }
                                        }
                                        
                                        ++groupDataIt;
                                    }
                                    
                                    ++num;
                                }
							}
						}
						
                        ++groupIt;
					}
					
					std::map<int, std::vector< AnimationGroupDataAux > >::iterator auxIt = groupNameToGroupDataAux.begin();
					std::map<int, std::vector< AnimationGroupDataAux > >::iterator auxItEnd = groupNameToGroupDataAux.end();
					while(auxIt != auxItEnd)
					{						
						std::pair< GroupNameToGroupData::iterator, bool > ret = mGroupNameToData.insert(std::make_pair(auxIt->first, std::vector< AnimationGroupData >()));

						if(ret.second)
						{
							// get aux data
							std::vector< AnimationGroupDataAux >& auxGroups = auxIt->second;
							
							// get the list we just inserted
							std::vector< AnimationGroupData >& agd = ret.first->second;
							
							// copy aux data to live data
							unsigned int size = auxGroups.size();
							for(unsigned int i = 0; i < size; ++i)
							{						
								std::map< std::string, int >::iterator flagIt = mGroupNameToGroupBitFlag.find(auxGroups[i].nextAnimationGroup);
								
								if(flagIt != mGroupNameToGroupBitFlag.end())
								{											
									agd.push_back(AnimationGroupData());
									AnimationGroupData& lagd = agd.back();
									
									lagd.nextAnimationGroup = flagIt->second;
									lagd.crossFade = auxGroups[i].crossFade;
									lagd.loops = auxGroups[i].loops;
									lagd.probability = auxGroups[i].probability;						
								}
							}
						}
						
						++auxIt;
					}
					
				}
				
				// Now get all of the animations
                typedef std::map< std::string, XMLDocument::NamedIterator > AniToEventXML;
                AniToEventXML aniToEventsXML;
				XMLDocument::Iterator animationsIt = xmlDoc.getIteratorTo("Animations");
                if(animationsIt)
                {
                    XMLDocument::Iterator animationIt = animationsIt.getChildIterator();
                    
                    while(animationIt)
                    {
						Property animationName;
						Property armatureName;
						Property property;
						
                        // Animation name, and armature name (we can't have an animation without both!)
						if (animationIt.getAttribute("name", animationName) && animationIt.getAttribute("armatureName", armatureName))
						{
							// Grab armature
							std::string armatureNameStr = armatureName.asString();
							
							// Animation name
							std::string animationNameStr = animationName.asString();
                            
                            AnimationData animData;
							animData.groups = 0; // default to no groups
							animData.scenes = 0xFFFFFFFF; // default to all scenes
                            
                            bool loadAnimation = false;
                            
                            XMLDocument::NamedIterator aniSceneIt = animationIt.getChildNamedIterator("Scenes");
                            if( aniSceneIt )
                            {
                                XMLDocument::Iterator animSceneIt = aniSceneIt.getChildIterator();
                                while (animSceneIt)
                                {
                                    if(animSceneIt.getAttribute("name", property))
                                    {
                                        // check to see if we should load this animation based on its
                                        // scene name
                                        std::string animScene = property.asString();
                                        
                                        std::map< std::string, unsigned int >::iterator sceneBitflagIt =
                                        mSceneNameToSceneBitFlag.find( animScene );
                                        
                                        if( sceneBitflagIt != mSceneNameToSceneBitFlag.end() )
                                        {
                                            // the scene that this animation is in should be loaded
                                            loadAnimation = true;
                                            
                                            int bitflag = sceneBitflagIt->second;
                                            
                                            // Turn the string into a int value, and OR them all together
                                            if(animData.scenes == 0xFFFFFFFF)
                                            {
                                                animData.scenes = bitflag;
                                            }
                                            else
                                            {
                                                animData.scenes |= bitflag;
                                            }
                                        }
                                    }
                                    
                                    ++animSceneIt;
                                }
                            }
                            else if( groupsToLoad.empty() || !scenesToLoad.empty() )
                            {
                                // if this animation is a part of every scene and we're not loading
                                // based on groups, we need to load this animation OR
                                // if this animation is part of every scene and we're loading based
                                // on scenes
                                loadAnimation = true;                                
                            }
                            
                            // check if the animation should be loaded based on the group it's in
                            XMLDocument::NamedIterator aniGroupsIt = animationIt.getChildNamedIterator("Groups");
                            if( aniGroupsIt )
                            {
                                XMLDocument::Iterator animGroupIt = aniGroupsIt.getChildIterator();
                                while (animGroupIt)
                                {
                                    if(animGroupIt.getAttribute("name", property))
                                    {
                                        std::string animGroup = property.asString();
                                        
                                        // if we're loading purely based on scenes and this animation
                                        // hasn't been deemed loadable, the groups shouldn't be a
                                        // deciding factor as to whether it is loaded
                                        if( groupsToLoad.empty() && !scenesToLoad.empty() &&
                                            !loadAnimation )
                                        {
                                            break;
                                        }
                                        
                                        std::map< std::string, int >::iterator groupBitflagIt =
                                            mGroupNameToGroupBitFlag.find( animGroup );
                                        
                                        // Turn the string into a int value, and OR them all together
                                        if( groupBitflagIt != mGroupNameToGroupBitFlag.end() )
                                        {
                                            int groupFlag = groupBitflagIt->second;
                                            animData.groups |= groupFlag;
                                            
                                            // Also add it to the map group name thing
                                            mAnimationGroupToNameMap[groupFlag].push_back(animationNameStr);
                                            
                                            loadAnimation = true;
                                        }
                                        
                                    }
                                    
                                    ++animGroupIt;
                                }
                            }
                            
                            
                            if( loadAnimation )
                            {
                                animationsToLoad.insert( armatureNameStr );
                                
                                if( animationIt.getAttribute("canCrossFade", property) )
                                {
                                    animData.canCrossFade = (property.asInt() == 1);
                                }
                                
                                XMLDocument::NamedIterator eventsIt =
                                                    animationIt.getChildNamedIterator("Events");
                                aniToEventsXML[ animationNameStr ] = eventsIt;
                                
                                /*
                                // we need to got through and see if theres any props attached to this animation that need to be loaded
                                if(eventsIt)
                                {
                                    XMLDocument::Iterator eventIterator = eventsIt.getChildIterator();
                                    while( eventIterator )
                                    {
                                        XMLDocument::NamedIterator propActions =
                                            eventIterator.getChildNamedIterator("PropAction");
                                        while( propActions )
                                        {
                                            Property propName;
                                            propActions.getAttribute( "propName", propName );
                                            propsToLoad.insert( propName.asString() );
                                         
                                            ++propActions;
                                        }
                                        
                                        ++eventIterator;
                                    }
                                }*/
                                
                                mAnimationNameToData[ animationNameStr ] = animData;
                                mArmatureNames.push_back( armatureNameStr );
                                mAnimationNameToIndex[animationNameStr] = mAnimationNames.size();
                                mAnimationIndexToName[mAnimationNames.size()] = animationNameStr;
                                mAnimationNames.push_back(animationNameStr);
                            }
						}
						
                        ++animationIt;
					}
				}
                
                // now we actually need to load the animations and their curves, so let's detour
                Property animationsProperty;
                if( xmlDoc.getRootIterator().getAttribute("aniFiles", animationsProperty ) )
                {
                    // split by pipe
                    std::vector< std::string > aniFiles;
                    StringHelper::split( animationsProperty.asString(), '|', aniFiles );
                    
                    unsigned int size = aniFiles.size();
                    for( unsigned int i=0; i < size; ++i )
                    {
                        AnimationManager::AnimationLoadBehavior loadBehavior =
                        AnimationManager::AnimationLoadBehavior_Append;
                        
                        if( i == 0 )
                        {
                            loadBehavior = AnimationManager::AnimationLoadBehavior_Overwrite;
                        }
                        
                        if( StringHelper::getExtension( aniFiles[i] ) == ".ani" )
                        {
                            // load the animation data
                            mSkeleton->getAnimationManager()->loadAnimationsFromXml( aniFiles[i],
                                                                                 loadBehavior,
                                                                                 animationsToLoad );
                        }
                        else
                        {
                            // load the animation data
                            mSkeleton->getAnimationManager()->loadAnimationsFromBin( aniFiles[i],
                                                                                     loadBehavior,
                                                                                     animationsToLoad );
                        }

                    }
                }
				
                // now we can process the events on the animations since they could potentially
                // add events to the animation
                AniToEventXML::iterator aniEventIt =  aniToEventsXML.begin();
                AniToEventXML::iterator aniEventItEnd = aniToEventsXML.end();
                while ( aniEventIt != aniEventItEnd )
                {
                    XMLDocument::NamedIterator internalIt = aniEventIt->second;
                 
                    Property property;
                    int count = 0;
                    XMLDocument::Iterator eventIt = internalIt.getChildIterator();
                    while (eventIt)
                    {
                        if(eventIt.getAttribute("type", property))
                        {
                            std::vector< std::string > events = StringHelper::split(property.asString(),'|');
                            
                            // now create the actions because they may be used for more than one event
                            std::vector< EventActionDataPtr > actionData;
                            
                            XMLDocument::Iterator actionIt = eventIt.getChildIterator();
                            while( actionIt )
                            {
                                if("SoundAction" == actionIt.getNodeName())
                                {
                                    // create a sound action
                                    SoundActionData* sad = new SoundActionData();
                                    
                                    if(actionIt.getAttribute("sound", property))
                                    {
                                        sad->soundActionType = SoundActionData::SAT_GroupID;
                                        sad->soundGroup = property.asInt();
                                        
                                        if(actionIt.getAttribute("soundSpecific", property))
                                        {
                                            sad->soundSpecificIndex = property.asInt();
                                        }
                                    }
                                    else if( actionIt.getAttribute("soundFile", property) )
                                    {
                                        sad->filename = property.asString();
                                        sad->soundActionType = SoundActionData::SAT_Filename;
                                        
                                        if( actionIt.getAttribute("isVO", property) )
                                        {
                                            sad->isVO = (property.asInt() == 1);
                                        }
                                    }
                                    
                                    actionData.push_back(EventActionDataPtr(sad));
                                }
                                else if("PropAction" == actionIt.getNodeName())
                                {
                                    if(actionIt.getAttribute("propName", property))
                                    {
                                        // create a prop action
                                        PropActionData* pad = new PropActionData();
                                        pad->propName = property.asString();
                                        
                                        propsToLoad.insert( pad->propName );
                                        
                                        if(actionIt.getAttribute("animationName", property))
                                        {
                                            pad->actionFlags |= (int)PropActionData::PAT_PlayAnimation;
                                            pad->animationName = property.asString();
                                        }
                                        
                                        if(actionIt.getAttribute("playbackMode", property))
                                        {
                                            pad->playMode = SpriteAnimation::_parsePlaybackMode(property.asString());
                                            
                                            if(actionIt.getAttribute("numLoops", property))
                                            {
                                                pad->numLoops = property.asInt();
                                            }
                                        }
                                        
                                        if(actionIt.getAttribute("playOffset", property))
                                        {
                                            pad->playOffset = property.asFloat();
                                        }
                                        
                                        if(actionIt.getAttribute("visible", property))
                                        {
                                            pad->actionFlags |= (int)PropActionData::PAT_Visibility;
                                            pad->visible = property.asInt() ? true : false;
                                        }
                                        
                                        if(actionIt.getAttribute("layer", property))
                                        {
                                            pad->actionFlags |= (int)PropActionData::PAT_LayerChange;
                                            
                                            // we can cast to short here without worry for now
                                            pad->layer = (short)property.asInt();
                                        }

                                        if(actionIt.getAttribute("sceneProp", property))
                                        {
                                            if ( property.asString() == "show" )
                                            {
                                                pad->sceneProp = PropActionData::SPT_ShowAsSceneProp;
                                            }
                                            else if ( property.asString() == "hide" )
                                            {
                                                pad->sceneProp = PropActionData::SPT_HideSceneProp;
                                            }
                                        }
                                        
                                        actionData.push_back(EventActionDataPtr(pad));
                                    }
                                }
                                else if("AnimationAction" == actionIt.getNodeName())
                                {
                                    if(actionIt.getAttribute("animation", property))
                                    {
                                        // create animation action
                                        AnimationActionData* aad = new AnimationActionData();
                                        
                                        aad->animation = property.asString();
                                        
                                        if(actionIt.getAttribute("isSpecific", property))
                                        {
                                            aad->isSpecific = property.asInt() ? true : false;
                                        }
                                        
                                        if(actionIt.getAttribute("crossFade", property))
                                        {
                                            aad->crossFade = property.asInt() ? true : false;
                                        }
                                        
                                        if(actionIt.getAttribute("loops", property))
                                        {
                                            aad->loops = property.asInt();
                                        }
                                        
                                        if(actionIt.getAttribute("playOffset", property))
                                        {
                                            aad->playOffset = property.asFloat();
                                        }
                                        
                                        actionData.push_back(EventActionDataPtr(aad));
                                    }
                                }
                                else if("EmitterAction" == actionIt.getNodeName())
                                {
                                    if(actionIt.getAttribute("emitterIndex", property))
                                    {
                                        // create animation action
                                        EmitterActionData* ead = new EmitterActionData();
                                        
                                        ead->emitterIndex = property.asInt();
                                        
                                        emittersToLoad.insert( ead->emitterIndex );
                                        
                                        if(actionIt.getAttribute("active", property))
                                        {
                                            ead->active = property.asInt() ? true : false;
                                        }
                                        
                                        if(actionIt.getAttribute("numParticles", property))
                                        {
                                            ead->numParticles = property.asInt();
                                        }
                                        
                                        actionData.push_back(EventActionDataPtr(ead));
                                    }
                                }
                                
                                ++actionIt;
                            }
                            
                            // get some structures we'll need
                            int aniNameIndex = mAnimationNameToIndex[ aniEventIt->first ];
                            AnimationData& animData = mAnimationNameToData[ aniEventIt->first ];
                            
                            unsigned int size = events.size();
                            for(unsigned int i = 0; i < size; ++i)
                            {
                                std::string typeStr = StringHelper::toLower(events[i]);
                                
                                if("marker" == typeStr)
                                {
                                    // we have merker, look for some marker specific stuff
                                    float markerTime = -1.0f;
                                    
                                    if(eventIt.getAttribute("time", property))
                                    {
                                        markerTime = property.asFloat();
                                    }
                                    
                                    if (markerTime != -1.0f)
                                    {
                                        Animation *ani =
                                            mSkeleton->getAnimationManager()->getAnimation( mArmatureNames[ aniNameIndex ] );
                                        if(ani)
                                        {
                                            ani->addMarker(markerTime, count);
                                        }
                                    }
                                    
                                    unsigned int actionSize = actionData.size();
                                    AnimationEventDataList& dataList = animData.eventActionData[AET_Marker];
                                    for(unsigned int j = 0; j < actionSize ; ++j)
                                    {
                                        actionData[j]->eventID = count;
                                        dataList.push_back(actionData[j]);
                                    }
                                    std::sort(dataList.begin(), dataList.end(), &SkeletonActor::eventActionPtrPredicate);
                                    count++;
                                }
                                else if( "finished" == typeStr )
                                {
                                    unsigned int actionSize = actionData.size();
                                    AnimationEventDataList& dataList = animData.eventActionData[AET_Finished];
                                    for(unsigned int j = 0; j < actionSize ; ++j)
                                    {
                                        dataList.push_back(actionData[j]);
                                    }
                                }
                                else if( "began" == typeStr )
                                {
                                    unsigned int actionSize = actionData.size();
                                    AnimationEventDataList& dataList = animData.eventActionData[AET_Began];
                                    for(unsigned int j = 0; j < actionSize ; ++j)
                                    {
                                        dataList.push_back(actionData[j]);
                                    }
                                    
                                }
                                else if( "looped" == typeStr )
                                {
                                    unsigned int actionSize = actionData.size();
                                    AnimationEventDataList& dataList = animData.eventActionData[AET_Looped];
                                    for(unsigned int j = 0; j < actionSize ; ++j)
                                    {
                                        dataList.push_back(actionData[j]);
                                    }
                                    
                                }
                                else if( "pause" == typeStr )
                                {
                                    unsigned int actionSize = actionData.size();
                                    AnimationEventDataList& dataList = animData.eventActionData[AET_Paused];
                                    for(unsigned int j = 0; j < actionSize ; ++j)
                                    {
                                        dataList.push_back(actionData[j]);
                                    }
                                    
                                }
                                else if( "stopped" == typeStr )
                                {
                                    unsigned int actionSize = actionData.size();
                                    AnimationEventDataList& dataList = animData.eventActionData[AET_Stopped];
                                    for(unsigned int j = 0; j < actionSize ; ++j)
                                    {
                                        dataList.push_back(actionData[j]);
                                    }
                                    
                                }
                            }
                            
                        }
                        ++eventIt;
                    }
                    
                    ++aniEventIt;
                }
                
                // Now selectively load the props. The only props that get loaded are props that
                // are in the propsToLoad set or are scene props for one of the loaded scenes
				XMLDocument::Iterator propsIt = xmlDoc.getIteratorTo("Props");
				if(propsIt)
				{
					int num = 0;
					XMLDocument::NamedIterator prop = propsIt.getChildNamedIterator("Prop");
					Property property;
					while(prop)
					{
                        PropType propType = PropType_Invalid;
                        
                        std::string path;
						if(prop.getAttribute("sprite", property))
						{
							path = property.asString();
							propType = PropType_Sprite;
                        }
                        else if( prop.getAttribute( "skeleton", property ) )
                        {
                            path = property.asString();
                            propType = PropType_Skeleton;
                        }
							
                        
                        assert( propType != PropType_Invalid );
                        if( propType != PropType_Invalid )
                        {
                            std::string propName =
                            StringHelper::removeExtension( StringHelper::basename( path ) ) +
                            std::string("_") + StringHelper::intToStr(num);
                            
							if(prop.getAttribute("name", property))
							{
								propName = property.asString();
							}
							
							unsigned int propFlags = PropFlags_FreeStanding;
							std::map<std::string, std::string> typeInfo;
							if( prop.getAttribute("type", property) )
							{
								std::vector< std::string > types;
								StringHelper::split(property.asString(), '|', types);
								
								for(unsigned int i = 0; i < types.size(); ++i)
								{
									if( types[i] == "skeleton" )
									{
										propFlags |= PropFlags_Attached;
										
										Property pBone;
										if( prop.getAttribute("bone", pBone) )
										{
											typeInfo["bone"] = pBone.asString();
										}
									}
									else if( types[i] == "scene" )
									{
										propFlags |= PropFlags_Scene;
										
										Property scene;
										if( prop.getAttribute("scenes", scene) )
										{
											typeInfo["scenes"] = scene.asString();
										}
									}
								}
							}
                            
                            // now check if we should do anything with the rest of this data
                            
                            // we'll split the scene type first, this will be needed later to so
                            // no reason to do it twice
                            std::vector< std::string > scenes;
                            if( propFlags & PropFlags_Scene )
                            {
                                StringHelper::split(typeInfo["scenes"], '|', scenes);
                            }
                            
                            // then check if the name of the prop is in the load list, because if
                            // it is, we don't need to worry if its a scene prop or not, it's
                            // getting loaded
                            if( propsToLoad.find( propName ) == propsToLoad.end() )
                            {
                                bool skip = true;

                                // now we check if its a scene prop and the scene should be loaded
                                if( propFlags & PropFlags_Scene )
                                {
                                    for( unsigned int iscene=0; iscene < scenes.size(); ++iscene )
                                    {
                                        if( std::find( scenesToLoad.begin(), scenesToLoad.end(),
                                                       scenes[iscene] ) != scenesToLoad.end() )
                                        {
                                            // this prop is part of a scene that should be loaded,
                                            // so don't skip
                                            skip = false;
                                            break;
                                        }
                                    }
                                }
                                
                                if( skip )
                                {
                                    // this prop doesn't need to be load, skip the rest of this
                                    ++prop;
                                    continue;
                                }
                            }
							
							std::string startAni;
							if( prop.getAttribute("startAnimation", property ) )
							{
								startAni = property.asString();
							}
							
                            if( prop.getAttribute("pauseOnStart", property ) )
							{
                                bool pauseOnStart = property.asBool();
                                if( pauseOnStart )
                                {
                                    propFlags |= PropFlags_PauseOnStart;
                                }
							}
                            
							int layer = 0;
							if( prop.getAttribute("layer", property) )
							{
								layer = property.asInt();
							}
							
							Vector2 pos;
							if(prop.getAttribute("relativePos", property))
							{
								pos = property.asVector2();
							}
							
							Vector2 localScale = Vector2::One;
							if(prop.getAttribute("relativeScale", property))
							{
								localScale *= property.asVector2();
								
							}
							
							// TODO: Change this to be relative to the current angle of the skeleton
							float angle = 0.0f;
							if(prop.getAttribute("relativeAngle", property))
							{
								angle = property.asFloat();
							}
							
                            Prop* p = 0;
                            if( propType == PropType_Sprite )
                            {
                                // we need to flip the sprite because sprites assume they are
                                // drawing in screen space and the skeleton is built for world
                                // space
                                localScale.Y *= -1.0f;
                                Sprite* sprite = new Sprite(propName, Vector2::Zero, angle, localScale);
                                sprite->loadFromXML(path, CallbackPtr());
                                sprite->setEventCallback(
                                        CreateMemberCallbackPtr( *this,
                                                    &SkeletonActor::_propSpriteEventCallback ) );
                                
                                p = new Prop(sprite, propFlags, layer);

                            }
                            else if ( propType == PropType_Skeleton )
                            {
                                Skeleton* skeleton = new Skeleton(propName, Vector2::Zero, angle, localScale);
                                skeleton->loadFromXMLFile( path );
                                skeleton->getAnimationManager()->registerEventCallback( this,
                                        CreateMemberCallbackPtr(*this,
                                                &SkeletonActor::_propSkeletonEventCallback ) );
                                
                                p = new Prop( skeleton, propFlags, layer );
                            }
                            
                            assert ( p );
                            assert ( p->getPropObject() );
                            
                            if(prop.getAttribute("visible", property))
                            {
                                p->getPropObject()->setVisible(property.asInt());
                            }
                            
                            if ( propFlags & PropFlags_Attached )
							{									
								if( typeInfo.find("bone") != typeInfo.end() )
								{
									// attach sprite to bone
									Bone* bone = mSkeleton->getBone( typeInfo["bone"] );
									assert( bone );
									if( bone )
									{
										bone->addChild( p->getPropObject() );
									}
									
								}
								else 
								{
									mSkeleton->addChild( p->getPropObject() );
								}
							}
							
							if ( propFlags & PropFlags_Scene )
							{								
                                unsigned int scenesFlags = 0;
								for( unsigned int i=0; i < scenes.size(); ++i)
								{
									scenesFlags |= mSceneNameToSceneBitFlag[scenes[i]];
								}
                                
                                p->setScenes( scenesFlags );
							}
							
							if( ! propFlags )
							{
								pos = mSkeleton->getWorldPosition2D() + (mSkeleton->getWorldScale2D() * pos);
							}		
							
							if( !startAni.empty() )
							{
								p->setStartAnimation( startAni );
							}
							
							p->getPropObject()->setLocalPosition2D(pos);
							
							mProps[propName] = p;
						}
						
						++prop;
						++num;
					}
				}
				
				XMLDocument::Iterator emittersIt = xmlDoc.getIteratorTo( "Emitters" );
				if( emittersIt )
				{
					XMLDocument::NamedIterator emitterIt = emittersIt.getChildNamedIterator( "Emitter" );
					Property property;
					while( emitterIt )
					{
						if( emitterIt.getAttribute( "path", property ) )
						{
							ParticleEmitter* partEmit = new ParticleEmitter();
							
							partEmit->loadFromXML( property.asString() );
							
							Vector2 basePos;
							if( emitterIt.getAttribute( "baseNode", property ) )
							{
								Node* node = mSkeleton->getNode( property.asString() );
								
								if(node)
								{
									node->addChild( partEmit );
								}
							}
							
							XMLDocument::NamedIterator zoneIt = emitterIt.getChildNamedIterator( "EmitterZone" );
                            
							while( zoneIt )
							{
	                            ParticleEmitter::EmitterZone ez;
                            
								if( zoneIt.getAttribute( "position", property ) )
								{
									
									ez.mPosition = property.asVector2();
									
									if( zoneIt.getAttribute( "radius", property ) )
										ez.mRadius = property.asFloat();

	                                if( zoneIt.getAttribute( "rect", property ) )
	                                    ez.mRect = property.asVector2();      
                                                                                
                                    if( zoneIt.getAttribute( "ringparticles", property ) )
                                        ez.mRingParticles = property.asInt();
                                    
                                    if( zoneIt.getAttribute( "ringradius", property ) )
                                        ez.mRingRadius = property.asFloat();                                          

                                    
									partEmit->addEmitterZone( ez );
								}
                                
                                
                                
								
								++zoneIt;
							}
                            
                            
                            
							mEmitters.push_back( partEmit );
						}
						
						++emitterIt;
					}
				}
			}
			
			delete params->buffer;
		}
		
		mXmlLoaded = true;
		
		if (!mInitSuccessful)
		{
            if ( !mInitSceneName.empty() )
            {
                init( mInitPos, mInitSceneName, mInitSize, mInitIsScreenSpace );
            }
            else
            {
                init( mInitPos, mInitSceneNameIdentifier, mInitSize, mInitIsScreenSpace );
            }
		}

        if( mLoadedCallback )
        {
            mLoadedCallback->invoke( NULL );
            mLoadedCallback.reset();
        }
	}
    
    void SkeletonActor::_propSkeletonEventCallback( void* data )
    {
        AnimationCallbackParameters* params = static_cast<AnimationCallbackParameters*>(data);
            
        if( params->eType == AET_Finished )
        {
            std::vector< void* >::iterator it = std::find( mActivePropsForAnimation.begin(),
                       mActivePropsForAnimation.end(),
                      static_cast<void*>(params->anim) );
            if( it != mActivePropsForAnimation.end() )
            {
                mActivePropsForAnimation.erase( it );
            }
            
            _aggregateEvent( params->eType, params->eventID );
        }
    }
    
    void SkeletonActor::_propSpriteEventCallback( void* data )
    {
        SpriteEventCallbackParameters* params = static_cast<SpriteEventCallbackParameters*>(data);
        
        if( params->eType == AET_Finished )
        {
            std::vector< void* >::iterator it = std::find( mActivePropsForAnimation.begin(),
                                                          mActivePropsForAnimation.end(),
                                                          static_cast<void*>( params->anim.get() ) );
            if( it != mActivePropsForAnimation.end() )
            {
                mActivePropsForAnimation.erase( it );
            }
            _aggregateEvent( params->eType, params->eventID );
        }
    }
	
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    void SkeletonActor::_animationEventCallback(void* data)
    {
        AnimationCallbackParameters* params = (AnimationCallbackParameters*)data;
        
		int name = _getAnimationNameIndex(params->anim->getName());
		
        // if name returns -1, its an error
        assert( name >= 0 );
        
		std::string animName = mAnimationNames[name];
        bool didAnimationAction = false;

		AnimationNameToData::iterator eventMap = mAnimationNameToData.find(animName);
		if(eventMap != mAnimationNameToData.end())
		{
            if(params->eType != AET_Marker)
			{
				mCurrentAnimationPlayMode = params->eType;
			}
			
			EventTypeToActionDataMap& eventToActions = eventMap->second.eventActionData;
			EventTypeToActionDataMap::iterator it = eventToActions.find(params->eType);
			if(it != eventToActions.end())
			{
				// There is an event entry in the action map for this animation, let's see what it is
				AnimationEventDataList& aniEventList = it->second;
				
				// if the AET is a marker, we need to get the right marker
				if( AET_Marker == params->eType )
				{
					// do a linear search for the eventID
					unsigned int size = aniEventList.size();
					for(unsigned int i = 0; i < size; ++i)
					{
						if(aniEventList[i]->eventID == params->eventID)
						{
							// we found the marker entry!!
							_doEventAction(aniEventList[i]);
                            didAnimationAction = aniEventList[i]->eventActionType == EAT_Animation;
						}
					}
				}
				else
				{
					// do a linear search for the eventID
					unsigned int size = aniEventList.size();
					for(unsigned int i = 0; i < size; ++i)
					{
						// we found the marker entry!!
						_doEventAction(aniEventList[i]);
                        didAnimationAction = aniEventList[i]->eventActionType == EAT_Animation;
					}
				}
            }
		}
        
        if( params->eType == AET_Finished && !didAnimationAction )
        {
            mSkeletonFinished = true;
            if( !mActivePropsForAnimation.empty() )
            {
                mSkeleton->getAnimationManager()->playCurrentAnimation();
            }
            _aggregateEvent( params->eType, params->eventID );
        }
    }
	
	void SkeletonActor::_clearAnimationGroupData()
	{
		mAnimationGroupToNameMap.clear();
		mArmatureNames.clear();
		mAnimationNameToIndex.clear();
		mAnimationIndexToName.clear();
		mAnimationNames.clear();
		mGroupNameToGroupBitFlag.clear();
		mSceneNameToSceneBitFlag.clear();
		
		AnimationNameToData::iterator aniNameIt = mAnimationNameToData.begin();
		AnimationNameToData::iterator aniNameItEnd = mAnimationNameToData.end();
		while(aniNameIt != aniNameItEnd)
		{
			EventTypeToActionDataMap& eventTypeToData = aniNameIt->second.eventActionData;
			EventTypeToActionDataMap::iterator eventTypeIt = eventTypeToData.begin();
			EventTypeToActionDataMap::iterator eventTypeItEnd = eventTypeToData.end();
			
			while(eventTypeIt != eventTypeItEnd)
			{
				AnimationEventDataList& dataList = eventTypeIt->second;
				dataList.clear();				
				++eventTypeIt;
			}
			
			eventTypeToData.clear();
			++aniNameIt;
		}
		mAnimationNameToData.clear();
	}
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~  
	void SkeletonActor::_doEventAction(const EventActionDataPtr eventAction)
	{
		switch(eventAction->eventActionType)
		{
			case EAT_Sound:
			{ // sound event, play a sound!
				       
				if(mAnimationSoundState == ASS_On || (mAnimationSoundState == ASS_PlayOnce && mCurrentAnimationPlayMode == AET_Began))
				{
					SoundActionData* sad = (SoundActionData*)eventAction.get();
					
					switch( sad->soundActionType )
					{
						case SoundActionData::SAT_GroupID:
												
							if (sad->soundGroup != -1)
							{
								if(sad->soundSpecificIndex == -1)
								{
									// play sound from group
									//SoundManager::getInstancePtr()->playSoundFromGroup(sad->soundGroup);
                                    
                                    // load sound and mark it as to be played.
                                    PropertyList plist;
                                    plist.setValueForKey("playOnLoad", Property(true));
                                    
                                    CallbackPtr callback = CreateMemberCallbackPtr(*this, &SkeletonActor::_soundLoaded);
                                    
                                    SoundManager::getInstancePtr()->newSoundFromGroup( sad->soundGroup, callback, plist);
								}
								else
								{
									//SoundManager::getInstancePtr()->playSoundSpecific(sad->soundGroup, sad->soundSpecificIndex);
                                    
                                    // load sound and mark it as to be played.
                                    PropertyList plist;
                                    plist.setValueForKey("playOnLoad", Property(true));
                                    
                                    CallbackPtr callback = CreateMemberCallbackPtr(*this, &SkeletonActor::_soundLoaded);
                                    
                                    SoundManager::getInstancePtr()->newSoundSpecific(sad->soundGroup, sad->soundSpecificIndex, callback, plist);
								}
							}
							
							break;
						case SoundActionData::SAT_Filename:
							
							if ( !sad->filename.empty() )
							{
								//SoundManager::getInstancePtr()->playSoundFile(sad->filename, 1.0f, 1.0f, sad->isVO);
                                
                                // load sound and mark it as to be played.
                                PropertyList plist;
                                plist.setValueForKey("playOnLoad", Property(true));
                                plist.setValueForKey("vol", Property(1.0f));
                                plist.setValueForKey("pitch", Property(1.0f));
                                
                                CallbackPtr callback = CreateMemberCallbackPtr(*this, &SkeletonActor::_gotSoundFile);
                                
                                SoundManager::getInstancePtr()->getSoundFile(sad->filename, callback, plist, sad->isVO);
							}
							break;
							
						default:
							break;
					}
				}				
			}
				break;
				
			case EAT_Prop:
			{ // do something with a prop
				PropActionData* pad = (PropActionData*)eventAction.get();
				if( ! pad->propName.empty() )
				{	
					std::map< std::string, Prop* >::iterator propIt = mProps.find( pad->propName);
					if( propIt != mProps.end() )
					{
						std::map< std::string, Prop* >::iterator activeProp = mActiveProps.find( pad->propName);
						if( activeProp == mActiveProps.end() )
						{
							// this prop isnt active yet, make it active
							mActiveProps.insert( std::make_pair( pad->propName, propIt->second ) );
                        }
                        
                        Prop* prop = propIt->second;
                        if( pad->actionFlags & PropActionData::PAT_Visibility )
                        {
                            prop->getPropObject()->setVisible(pad->visible);
                        }
                        if( pad->actionFlags & PropActionData::PAT_LayerChange )
                        {
                            prop->getPropObject()->setLayer( pad->layer );
                        }
                        if( pad->sceneProp == PropActionData::SPT_ShowAsSceneProp )
                        {
                            unsigned int scenesFlags = 0;

                            AnimationNameToData::iterator animationDataIterator = mAnimationNameToData.find( pad->animationName );
                            if ( animationDataIterator != mAnimationNameToData.end() )
                            {
                                scenesFlags = (*animationDataIterator).second.scenes;
                            }
                            
                            prop->setScenes( scenesFlags );
                            prop->setPropFlag( PropFlags_Scene );
                        }
                        else if ( pad->sceneProp == PropActionData::SPT_HideSceneProp )
                        {
                            // Clear the prop from the scenes
                            prop->setScenes(0);
                            prop->clearPropFlag( PropFlags_Scene );
                        }

                        if( prop->getPropType() == PropType_Sprite )
                        {
                            Sprite* sprite = static_cast<Sprite*>( prop->getPropObject() );                            
                            
                            if( pad->actionFlags & PropActionData::PAT_PlayAnimation)
                            {
                                sprite->getCurrentAnimation()->stop();
                                                                
                                sprite->playAnimation(pad->animationName, pad->playOffset,
                                                      pad->playMode, pad->numLoops);
                                
                                mActivePropsForAnimation.push_back( sprite->getCurrentAnimation().get() );
                                
                            }
                        }
                        else if( prop->getPropType() == PropType_Skeleton )
                        {
                            Skeleton* skeleton = static_cast<Skeleton*>( prop->getPropObject() );
                            
                            if( pad->actionFlags & PropActionData::PAT_PlayAnimation)
                            {
                                skeleton->getAnimationManager()->stopCurrentAnimation();                                
                                
                                skeleton->getAnimationManager()->playAnimation(pad->animationName,
                                                                               pad->playOffset,
                                                                               pad->playMode,
                                                                               pad->numLoops);
                                
                                mActivePropsForAnimation.push_back( skeleton->getAnimationManager()->getCurrentAnimation() );
                            }
                        }                      
					}
				}
				
			}
				break;
				
			case EAT_Animation:
			{// do something with an animation
				AnimationActionData* aad = (AnimationActionData*)eventAction.get();
				if(aad->animation.size())
				{
					if(aad->isSpecific)
					{
						AnimationPlaybackMode apm = PM_Once;
						if(aad->loops != 0)
						{
							apm = PM_Loops;
						}
						playAnimation(aad->animation, aad->playOffset, apm, aad->loops, aad->crossFade);
					}
					else
					{
						AnimationPlaybackMode apm = PM_Once;
						if(aad->loops != 0)
						{
							apm = PM_Loops;
						}
						playAnimationFromGroup(aad->animation, apm, aad->loops, aad->crossFade);
					}
				}				
			}
				break;
				
			case EAT_Emitter:
			{ // do something with a prop
				EmitterActionData* ead = (EmitterActionData*)eventAction.get();
				if(ead->emitterIndex != -1)
				{
					ParticleEmitter* emitter = mEmitters[ead->emitterIndex];
					
					emitter->setActive( ead->active );
					emitter->setScreenSpace( mIsScreenSpace );
					
					// kludgey, -2 signifies no action for this since -1 means infinite
					if(ead->numParticles > -2)
					{
						emitter->setNumParticles( ead->numParticles );
					}
				}			
				
			}
				break;
				
			default:
				assert(0);
		}
	}
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::_gotSoundFile(void *data)
	{
		SoundManager::SoundFileInstanceLoadedParameters* params = (SoundManager::SoundFileInstanceLoadedParameters*)data;
        
        if ( params->result != ResultCode_OK )
        {
            if (params->result == ResultCode_ErrorFileNotFound)
            {
                //assert(false); // Didn't find the file!
                Logger::printf("Animation", Logger::SV_ERROR, "Cannot find sound file: [ %s ]\n", params->filename.c_str());
            }
            else
            {
                //assert(false);
                Logger::printf("Animation", Logger::SV_ERROR, "Error playing sound [ %s ]\n", params->filename.c_str());
            }
            
            return;
        }
        
        // Slight concern - this function is only similarly implemented in SoundManager itself,
        // which has direct access to the SoundResource pointer inside the SoundEffectInstancePtr
        // (params->sound). So we can't check to see if the SoundResource pointer itself is null.
        if (params->sound)
        {
            // Keep track of this sound.
            mSounds.push_back(params->sound);
            
            float vol = 1.0f;
            float pitch = 1.0f;
            
            if (params->userData.keyExists("vol"))
                vol = params->userData["vol"].asFloat();
            
            if (params->userData.keyExists("pitch"))    
                pitch = params->userData["pitch"].asFloat();
            
            if (pitch != 1.0f)
                params->sound->setPitch( pitch );
            
            params->sound->setVolume(vol);
            
            bool playNow = false;
            // Check to see if it should be played right now or not.
            if (params->userData.keyExists("playOnLoad"))
            {
                playNow = params->userData["playOnLoad"].asBool();
            }
            
            if (playNow)
            {
                // Play the sound immediately!
                params->sound->play();
            }
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::_soundLoaded( void *data )
    {
        SoundManager::SoundEffectInstanceLoadedParameters* params = (SoundManager::SoundEffectInstanceLoadedParameters*)data;
        
        if (params->sound)
        {
            // Keep track of this sound.
            mSounds.push_back(params->sound);
            
            float vol = 1.0f;
            float pitch = 1.0f;
            
            if (params->userData.keyExists("vol"))
                vol = params->userData["vol"].asFloat();
            
            if (params->userData.keyExists("pitch"))
                pitch = params->userData["pitch"].asFloat();
            
            if (pitch != 1.0f)
                params->sound->setPitch( pitch );
            
            params->sound->setVolume(vol);
            
            bool playNow = false;
            // Check to see if it should be played right now or not.
            if (params->userData.keyExists("playOnLoad"))
            {
                playNow = params->userData["playOnLoad"].asBool();
            }
            
            if (playNow)
            {
                // Play the sound immediately!
                params->sound->play();
            }
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::pauseSounds()
    {
        for (int i = 0; i < mSounds.size(); i++)
        {
            if (mSounds[i])
            {
                mSounds[i]->pause();
            }
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::resumeSounds()
    {
        for( int i = 0; i < mSounds.size(); i++ )
        {
            if (mSounds[i])
            {
                mSounds[i]->resume();
            }
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::stopAllLoopingSFX()
    {
        for( int i = 0; i < mSounds.size(); i++ )
        {
            if (mSounds[i])
            {
                mSounds[i]->stopIfLooping();
            }
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::stopAllSounds()
    {
        for( int i = 0; i < mSounds.size(); i++ )
        {
            if (mSounds[i])
            {
                mSounds[i]->stop();
            }
        }
        
        mSounds.clear();
    }
    
    //----------------------------------------------------------------------------------------------

	void SkeletonActor::clearSceneProps()
	{
		std::map< std::string, Prop* >::iterator it = mActiveProps.begin();
		std::map< std::string, Prop* >::iterator itEnd = mActiveProps.end();
		
		while( it != itEnd )
		{
			if( it->second->getPropFlags() & PropFlags_Scene )
			{
				std::map< std::string, Prop* >::iterator toDelete = it;
                
                Prop* prop = it->second;
                if( prop->getPropType() == PropType_Sprite )
                {
                    Sprite* sprite = static_cast<Sprite*>( prop->getPropObject() );
                    
                    sprite->getCurrentAnimation()->stop();
                    
                }
                else if( prop->getPropType() == PropType_Skeleton )
                {
                    Skeleton* skeleton = static_cast<Skeleton*>( prop->getPropObject() );
                    
                    skeleton->getAnimationManager()->stopCurrentAnimation();
                }
                
				++it;
				mActiveProps.erase(toDelete);
			}
			else 
			{
				++it;
			}			
		}
	}

    //----------------------------------------------------------------------------------------------
    
	void SkeletonActor::clearNonSceneProps()
	{
		std::map< std::string, Prop* >::iterator it = mActiveProps.begin();
		std::map< std::string, Prop* >::iterator itEnd = mActiveProps.end();
		
		while( it != itEnd )
		{
			if( !(it->second->getPropFlags() & PropFlags_Scene) )
			{
				std::map< std::string, Prop* >::iterator toDelete = it;
                
                Prop* prop = it->second;
                if( prop->getPropType() == PropType_Sprite )
                {
                    Sprite* sprite = static_cast<Sprite*>( prop->getPropObject() );
                    
                    sprite->getCurrentAnimation()->stop();
                   
                }
                else if( prop->getPropType() == PropType_Skeleton )
                {
                    Skeleton* skeleton = static_cast<Skeleton*>( prop->getPropObject() );
                    
                    skeleton->getAnimationManager()->stopCurrentAnimation();
                }
                
				++it;
				mActiveProps.erase(toDelete);
			}
			else
			{
				++it;
			}
		}
	}
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::_fireCallback(AnimationEventType eType, int eventID)
    {
        if (!mAnimationEventCallbacks.empty())
        {
            int name = _getAnimationNameIndex( mSkeleton->getAnimationManager()->getCurrentAnimation()->getName() );
            
            // if name returns -1, its an error
            assert( name >= 0 );
            
            std::string animName = mAnimationNames[name];
            
            SkeletonActorAnimationEventCallbackParameters params;
            params.animationName = animName;
            params.eventType = eType;
            params.eventID = eventID;
            
            std::map< void*, CallbackPtr >::iterator it = mAnimationEventCallbacks.begin();
            std::map< void*, CallbackPtr >::iterator itEnd = mAnimationEventCallbacks.end();
            while (it != itEnd)
            {
                it->second->invoke( &params );
                ++it;
            }
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    void SkeletonActor::_aggregateEvent( AnimationEventType eventType, int eventId )
    {
        if( eventType == AET_Finished )
        {
            if( mActivePropsForAnimation.empty() && mSkeletonFinished )
            {
                Animation* currentAnimation = mSkeleton->getAnimationManager()->getCurrentAnimation();
                if( mCurrentPlaybackMode == PM_Loops && currentAnimation->getLoopsLeft() != 0 )
                {
                    // set the event type for the call back later
                    eventType = AET_Looped;
                }
                else
                {
                    if( mCurrentAnimationGroup > 0 )
                    {
                        GroupNameToGroupData::iterator groupIt =
                            mGroupNameToData.find( mCurrentAnimationGroup );

                        if( groupIt != mGroupNameToData.end() )
                        {
                            std::vector< AnimationGroupData >& groupDataList = groupIt->second;
                            
                            // loop over data to see what our probabilities are
                            unsigned int size = groupDataList.size();
                            
                            if(size)
                            {
                                int randHigh = 0;
                                for(unsigned int i=0; i < size; ++i)
                                {
                                    randHigh += groupDataList[i].probability;
                                }
                                
                                int randNum = rand() % randHigh + 1;
                                randHigh = 0;
                                for(unsigned int i=0; i < size; ++i)
                                {
                                    randHigh += groupDataList[i].probability;
                                    if(randNum <= randHigh)
                                    {
                                        // probability in range, break
                                        mNextAnimationGroup = groupDataList[i];
                                        break;
                                    }
                                }
                            }
                        }
                    }                    
                }
                
                mSkeleton->getAnimationManager()->stopCurrentAnimation();
                
                CallbackAnimationEvent event;
                event.eventType = eventType;
                event.eventID = eventId;
                mCallbackEventQueue.push( event );
            }            
        }
    }
}
