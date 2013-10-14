//
//  Animation.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 4/21/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <Animation.h>
#include <AnimationManager.h>
#include <SceneGraph.h>
#include <NumericAnimationTrack.h>
#include <NodeAnimationTrack.h>
#include <SpriteAnimationTrack.h>
#include <AnimationCueAnimationTrack.h>
#include <SkeletonActorCueAnimationTrack.h>
#include <Skeleton.h>
#include <BinaryReader.h>
#include <AnimationBinary.h>

#include <cmath>

namespace Walaber
{
    Animation::Animation(const std::string name, AnimationManager* aniMgr):
    mName(name),
    mNumericTrackList(),
    mNodeTrackList(),
    mCallback(NULL),
    mAnimationManager(aniMgr),
    mBlendMode(ABM_Default),
    mPlaybackMode(PM_Once),
    mAnimationLength(0.0f),
    mRealTime(0.0f),
    mModifiedTime(0.0f),
    mPlaybackSpeed(1.0f),
    mFPS(0.0f),
    mLoopCount(0),
    mLoopsLeft(0),
    mLastHitMarkerIndex(-1),
    mIsPlaying(false),
    mPlayForward(true),
    mIsCrossFadingFrom(false),
    mForceFPS(false)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Animation::Animation(const Animation& animation):
    mName(animation.mName),
    mNumericTrackList(), // need to deep copy numeric tracks
    mNodeTrackList(), // need to deep copy node tracks
    mSpriteTrackList(), // need to deep copy these
    mMarkers(animation.mMarkers),
    mCallback(animation.mCallback),
    mAnimationManager(animation.mAnimationManager),
    mBlendMode(animation.mBlendMode),
    mPlaybackMode(animation.mPlaybackMode),
    mAnimationLength(animation.mAnimationLength),
    mRealTime(animation.mRealTime),
    mModifiedTime(animation.mModifiedTime),
    mPlaybackSpeed(animation.mPlaybackSpeed),
    mFPS(animation.mFPS),
    mLoopCount(0),
    mLoopsLeft(animation.mLoopsLeft),
    mLastHitMarkerIndex(animation.mLastHitMarkerIndex),
    mIsPlaying(animation.mIsPlaying),
    mPlayForward(animation.mPlayForward),
    mIsCrossFadingFrom(animation.mIsCrossFadingFrom),
    mForceFPS(animation.mForceFPS)
    {
        // go through all of the tracks, get their positions and update them in the manager
        unsigned int size = animation.mNumericTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mNumericTrackList.push_back(new NumericAnimationTrack((*animation.mNumericTrackList[i])));        
        }
        
        size = animation.mNodeTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mNodeTrackList.push_back(new NodeAnimationTrack((*animation.mNodeTrackList[i])));        
        }  
        
        size = animation.mSpriteTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mSpriteTrackList.push_back(new SpriteAnimationTrack((*animation.mSpriteTrackList[i])));        
        } 
		
		size = animation.mCueTrackList.size();
		for(unsigned int i = 0; i < size; ++i)
		{
			mCueTrackList.push_back( new AnimationCueAnimationTrack((*animation.mCueTrackList[i])));
		}
		
		size = animation.mSkeletonActorCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
			mSkeletonActorCueTrackList.push_back( new SkeletonActorCueAnimationTrack((*animation.mSkeletonActorCueTrackList[i])));
        } 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Animation& Animation::operator=(const Animation& animation)
    {
        mName = animation.mName;
        
        // deep copy animation tracks
        unsigned int size = animation.mNumericTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mNumericTrackList.push_back(new NumericAnimationTrack((*animation.mNumericTrackList[i])));        
        }
        
        size = animation.mNodeTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mNodeTrackList.push_back(new NodeAnimationTrack((*animation.mNodeTrackList[i])));        
        } 
		
		size = animation.mSpriteTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mSpriteTrackList.push_back(new SpriteAnimationTrack((*animation.mSpriteTrackList[i])));        
        } 
		
		size = animation.mCueTrackList.size();
		for(unsigned int i = 0; i < size; ++i)
		{
			mCueTrackList.push_back( new AnimationCueAnimationTrack((*animation.mCueTrackList[i])));
		}
		
		size = animation.mSkeletonActorCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
			mSkeletonActorCueTrackList.push_back( new SkeletonActorCueAnimationTrack((*animation.mSkeletonActorCueTrackList[i])));
        } 
        
        mAnimationLength = animation.mAnimationLength;
        mCallback = animation.mCallback;
        mAnimationManager = animation.mAnimationManager;
        
        mBlendMode = animation.mBlendMode;
        mPlaybackMode = animation.mPlaybackMode;
        mRealTime = animation.mRealTime;
        mModifiedTime = animation.mModifiedTime;
        mPlaybackSpeed = animation.mPlaybackSpeed;
        mLoopCount = animation.mLoopCount;
        mLoopsLeft = animation.mLoopsLeft;
        mMarkers = animation.mMarkers;
        mLastHitMarkerIndex = animation.mLastHitMarkerIndex;
        mFPS = animation.mFPS;
        mIsPlaying = animation.mIsPlaying;
        mPlayForward = animation.mPlayForward;
        mIsCrossFadingFrom = animation.mIsCrossFadingFrom;
        mForceFPS = animation.mForceFPS;
        
        return *this;        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Animation::~Animation()
    {
        // go through all of the tracks and delete them
        unsigned int size = mNumericTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            delete mNumericTrackList[i];        
        }
        mNumericTrackList.clear();
        
        size = mNodeTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            delete mNodeTrackList[i];       
        } 
        mNodeTrackList.clear();
        
        size = mSpriteTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            delete mSpriteTrackList[i];       
        } 
        mSpriteTrackList.clear();
		
		size = mCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            delete mCueTrackList[i];       
        } 
        mCueTrackList.clear();
		
		size = mSkeletonActorCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            delete mSkeletonActorCueTrackList[i];       
        } 
        mSkeletonActorCueTrackList.clear();
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::copyProperties(const Animation& animation)
    {
        bool sameAni = false;
        if(mName == animation.mName)
        {
            sameAni = true;
        }
        
        mName = animation.mName;
        
        mAnimationLength = animation.mAnimationLength;
        mCallback = animation.mCallback;
        mAnimationManager = animation.mAnimationManager;
        
        mBlendMode = animation.mBlendMode;
        mPlaybackMode = animation.mPlaybackMode;        
        mRealTime = animation.mRealTime;
        mModifiedTime = animation.mModifiedTime;
        mPlaybackSpeed = animation.mPlaybackSpeed;
        mFPS = animation.mFPS;
        mLoopCount = animation.mLoopCount;
        mLoopsLeft = animation.mLoopsLeft;
        mMarkers = animation.mMarkers;
        mLastHitMarkerIndex = animation.mLastHitMarkerIndex;
        mIsPlaying = animation.mIsPlaying;
        mPlayForward = animation.mPlayForward;
        mIsCrossFadingFrom = animation.mIsCrossFadingFrom;
        mForceFPS = animation.mForceFPS;
        
        // copy sprite animation stuff
        if(sameAni)
        {
            unsigned int size = mSpriteTrackList.size();
            for(unsigned int i = 0; i < size; ++i)
            {
                mSpriteTrackList[i]->copyLastEvent(*animation.mSpriteTrackList[i]);       
            } 
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    bool Animation::createAnimationFromXml(const std::string& xmlPath, SceneGraph* sceneGraph)
    {
        bool ret = false;
        XMLDocument xmlDoc( xmlPath, "Animations" );
        
        if( xmlDoc )
        {
            ret = true;
            
            XMLDocument::NamedIterator child =
                xmlDoc.getRootIterator().getChildNamedIterator("Animation");
            
            while(child)
            {
                ret = createAnimationFromXmlNode( XMLDocument::Iterator( child.getCurrentNode() ),
                                                  sceneGraph);
                
                ++child;
            }
            
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Animation* Animation::createAnimationFromXmlNode( XMLDocument::Iterator animationRootNode, SceneGraph *sceneGraph)
    {
        Animation* ret = 0;
        Property prop;
        animationRootNode.getAttribute("name", prop);
        std::string aniName = prop.asString();
        
        ret = new Animation(aniName, 0);
        animationRootNode.getAttribute( "fps", prop );
        ret->setFPS( prop.asFloat() );
        animationRootNode.getAttribute( "forceFPS", prop );
        ret->setForceFPS( prop.asBool() );

        XMLDocument::NamedIterator trackIterator =
            animationRootNode.getChildNamedIterator( "NodeAnimationTrack" );
        while(trackIterator)
        {
            trackIterator.getAttribute( "node", prop );
            Node* node = sceneGraph->getNodeByName( prop.asString() );
            if(node)
            {
                NodeAnimationTrack* nodeAniTrack = new NodeAnimationTrack(node);
                
                _parseNodeTrack( nodeAniTrack, trackIterator );
                
                ret->addNodeTrack(nodeAniTrack);
            }
            else
            {
                // Node does not exists in scene graph
                assert(0);
            }
            
            ++trackIterator;
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Animation* Animation::createAnimationFromXmlNode( XMLDocument::Iterator animationRootNode, Skeleton *skeleton)
    {
        Animation* ret = 0;
        Property prop;
        animationRootNode.getAttribute("name", prop);
        std::string aniName = prop.asString();
        
        ret = new Animation(aniName, 0);
        animationRootNode.getAttribute( "fps", prop );
        ret->setFPS( prop.asFloat() );
        animationRootNode.getAttribute( "forceFPS", prop );
        ret->setForceFPS( prop.asBool() );
        
        XMLDocument::NamedIterator trackIterator =
            animationRootNode.getChildNamedIterator( "NodeAnimationTrack" );
        while(trackIterator)
        {
            trackIterator.getAttribute( "node", prop );
            std::string nodeName = prop.asString();
            Node* node = skeleton->getNode( prop.asString() );
            
            if(node)
            {
                NodeAnimationTrack* nodeAniTrack = new NodeAnimationTrack(node);
                
                _parseNodeTrack( nodeAniTrack, trackIterator );
                
                ret->addNodeTrack(nodeAniTrack);
            }
            else
            {
                // node does not exist in skeleton
                skeleton->printBoneNames();
                
                Logger::printf( "WalaberAnimation", Logger::SV_FATAL, "Bone [%s] does not exists in skeleton for animation [%s]\n", nodeName.c_str(), aniName.c_str() );
                assert(0);
            }
            
            ++trackIterator;
        }
        
        trackIterator =
            animationRootNode.getChildNamedIterator( "SpriteAnimationTrack" );
        while( trackIterator )
        {
            trackIterator.getAttribute( "sprite", prop );
            Sprite* sprite = skeleton->getSprite( prop.asString() );

            if(sprite)
            {
                SpriteAnimationTrack* spriteAnimationTrack = new SpriteAnimationTrack(sprite);
                
                XMLDocument::NamedIterator spriteEventIterator =
                    trackIterator.getChildNamedIterator( "SpriteAnimationEvent" );
                while(spriteEventIterator)
                {                           
                    float playOffset = 0.0f;
                    if( spriteEventIterator.getAttribute( "playOffset", prop ) )
                    {
                        playOffset = prop.asFloat();
                    }
                    
                    float time;                    
                    spriteEventIterator.getAttribute("time", prop);
                    time = prop.asFloat();
                    
                    std::string aniName;
                    spriteEventIterator.getAttribute("animationName", prop);
                    aniName = prop.asString();
                    
                    bool loops;
                    spriteEventIterator.getAttribute("loops", prop);
                    loops = prop.asBool();
                    
                    bool singleFrame;
                    spriteEventIterator.getAttribute("singleFrame", prop);
                    singleFrame = prop.asBool();
                    
                    spriteAnimationTrack->addSpriteAnimationEvent( time,
                                                                   aniName,
                                                                   loops,
                                                                   playOffset,
                                                                   singleFrame
                                                                 );
                    
                    ++spriteEventIterator;
                }
                
                ret->addSpriteTrack(spriteAnimationTrack);
            }
        
            ++trackIterator;
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===

    Animation* Animation::createAnimationFromBinary( BasicBinaryReader& reader,
                                                     const std::string& animationName,
                                                     Skeleton* skeleton )
    {
        Animation* ret = new Animation( animationName, 0);
        
        float fps = reader.readFloat();
        bool forceFps = ( reader.readUInt32() ? true : false );
        ret->setFPS( fps );
        ret->setForceFPS( forceFps );
        
        unsigned int numNodeTracks = reader.readUInt32();
        for(unsigned int i = 0; i < numNodeTracks; ++ i )
        {            
            std::string nodeName;
            reader.read( nodeName );
            
            Node* node = skeleton->getNode( nodeName );
            if( node )
            {
                unsigned int flags = reader.readUInt32();

                std::string curveLocation;
                reader.read( curveLocation );
                
                NodeAnimationTrack* nodeAniTrack = new NodeAnimationTrack( node );
                
                _buildTrackFromFlags( nodeAniTrack, flags, curveLocation, reader );
                
                ret->addNodeTrack(nodeAniTrack);
            }
            
        }
        
        unsigned int numSpriteTracks = reader.readUInt32();
        for(unsigned int i = 0; i < numSpriteTracks; ++ i )
        {
            std::string spriteName;
            reader.read( spriteName );
            
            Sprite* sprite = skeleton->getSprite( spriteName );
            if( sprite )
            {
                SpriteAnimationTrack* spriteAnimationTrack = new SpriteAnimationTrack(sprite);

                unsigned int numSpriteEvents = reader.readUInt32();
                for( unsigned int j = 0; j < numSpriteEvents; ++j )
                {
                    std::string spriteAnimName;
                    reader.read( spriteAnimName );
                    
                    float playOffset = reader.readFloat();
                    float time = reader.readFloat();
                    bool singleFrame = ( reader.readUInt16() ? true : false );
                    bool loops = ( reader.readUInt16() ? true : false );
                    
                    spriteAnimationTrack->addSpriteAnimationEvent( time,
                                                                   spriteAnimName,
                                                                   loops,
                                                                   playOffset,
                                                                   singleFrame
                                                                  );
                }
                
                ret->addSpriteTrack( spriteAnimationTrack );
            }
            
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::addNumericTrack(NumericAnimationTrack* numericAnimationTrack)
    {
        numericAnimationTrack->setAnimation(this);
        mNumericTrackList.push_back(numericAnimationTrack);
        
        float trackLen = numericAnimationTrack->getTrackLength();
        if( trackLen > mAnimationLength)
        {
            mAnimationLength = trackLen;
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::addNodeTrack(NodeAnimationTrack* nodeAnimationTrack)
    {
        nodeAnimationTrack->setAnimation(this);
        mNodeTrackList.push_back(nodeAnimationTrack);
        
        float trackLen = nodeAnimationTrack->getTrackLength();
        if( trackLen > mAnimationLength)
        {
            mAnimationLength = trackLen;
        }
    }    
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::addSpriteTrack(SpriteAnimationTrack* spriteAnimationTrack)
    {
        spriteAnimationTrack->setAnimation(this);
        mSpriteTrackList.push_back(spriteAnimationTrack);
		
		float lastEvent = spriteAnimationTrack->getLastEventTime();
		if (lastEvent > mAnimationLength)
		{
			mAnimationLength = lastEvent;
		}
    }  
	
	// -------------==============-------------==============-------------==============-------------===
    void Animation::addAnimationCueTrack(AnimationCueAnimationTrack* cueAnimationTrack)
    {
        cueAnimationTrack->setAnimation(this);
        mCueTrackList.push_back(cueAnimationTrack);
		
		float lastEvent = cueAnimationTrack->getLastEventTime();
		if (lastEvent > mAnimationLength)
		{
			mAnimationLength = lastEvent;
		}
    }  
	
	// -------------==============-------------==============-------------==============-------------===
    void Animation::addSkeletonActorCueTrack(Walaber::SkeletonActorCueAnimationTrack *skeletonActorCueAnimationTrack)
    {
        //cueAnimationTrack->setAnimation(this);
		mSkeletonActorCueTrackList.push_back(skeletonActorCueAnimationTrack);
		
		float lastEvent = skeletonActorCueAnimationTrack->getLastEventTime();
		if (lastEvent > mAnimationLength)
		{
			mAnimationLength = lastEvent;
		}
    }  
    
    // -------------==============-------------==============-------------==============-------------===
    unsigned int Animation::update(float deltaTime, const float weight, const bool isScreenSpace)
    {
        unsigned int ret = AET_None;
		mMarkersFiredThisFrame.clear();
		
        // if the animation is playing, advance the playhead and check for events
        if(mIsPlaying)
        {               
            if(mPlayForward)
            {
                mRealTime += deltaTime * mPlaybackSpeed;
            }
            else
            {
                mRealTime -= deltaTime * mPlaybackSpeed;
            }
            
            ret = _checkForEventsAndHandleTime(); 
            
            // convert to fps if necessary
            if(mForceFPS)
            {
                float frame = std::floor(mRealTime * mFPS);
                mModifiedTime = frame/mFPS;
            }
            else
            {
                mModifiedTime = mRealTime;
            }
        }                      
        
        // we always apply tracks because the cross fading is additive
        applyTracks(weight, isScreenSpace);
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::applyTracks(const float weight, const bool isScreenSpace)
    {
        // go through all of the tracks, get their positions and update them in the manager
        unsigned int size = mNumericTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mNumericTrackList[i]->apply(mBlendMode, mModifiedTime, weight, 1.0f, isScreenSpace);        
        }   
        
        size = mNodeTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mNodeTrackList[i]->apply(mBlendMode, mModifiedTime, weight, 1.0f,  isScreenSpace);        
        }
        
        // we don't want to apply the sprite tracks of the cross fade from animation because we can't
        // blend sprite animations
        if(!mIsCrossFadingFrom)
        {            
            size = mSpriteTrackList.size();
            for(unsigned int i = 0; i < size; ++i)
            {
                mSpriteTrackList[i]->apply(mModifiedTime);        
            } 
		}
        
		size = mCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mCueTrackList[i]->apply(mModifiedTime);        
        }
		
		size = mSkeletonActorCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mSkeletonActorCueTrackList[i]->apply(mModifiedTime);        
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::addMarker(const float time, const int id)
    {
        // we insert the marker into the vector in sorted order to keep the markers sorted
        // we start from the back of the vector because the majority use of this function 
        // will be to insert markers in order
        mMarkers.push_back(std::make_pair(time, id));
        std::sort(mMarkers.begin(), mMarkers.end(), _markerCompare);
        
#if _DEBUG
        for ( unsigned int i = 0; i < mMarkers.size(); ++i )
        {
            Walaber::Logger::printf("Walaber_Animation", Walaber::Logger::SV_DEBUG, "Marker id: "
                                    "%d -> time: %f", mMarkers[i].second, mMarkers[i].first );
        }
#endif
    }
    
    // -------------==============-------------==============-------------==============-------------===
    unsigned int Animation::_checkForEventsAndHandleTime()
    {
        unsigned int ret = AET_None;
        
        if(!mMarkers.empty())
        {
            int numMarkers = mMarkers.size();
            // see how many markers you may have hit
            while(mLastHitMarkerIndex != (numMarkers-1) && mRealTime >= mMarkers[mLastHitMarkerIndex + 1].first)
            {
                Walaber::Logger::printf("WMW_ANIMATION", Logger::SV_DEBUG, "Event Loop: "
                                        "markerSize: %d   mLastHitIndex: %d   numMarkers: %d  "
                                        "mRealTime: %f", mMarkers.size(), mLastHitMarkerIndex,
                                        numMarkers, mRealTime );
                // set return value to marker
                ret |= AET_Marker;
                              
                int id = mMarkers[++mLastHitMarkerIndex].second;
                
                // call event callback with marker id
                _fireCallback(AET_Marker, id); 
				
				mMarkersFiredThisFrame.push_back( id );
            }
        }
        
        if(mRealTime > mAnimationLength)
        {
            // the animation has passed a run going forward
            
            switch(mPlaybackMode)
            {
                case PM_Once:
                    
                    mIsPlaying = false;
                    
                    ret |= AET_Finished;
                    
                    // call the animation event callback 
                    _fireCallback(AET_Finished, -1);                    
                    
                    break;
                    
                case PM_Loops:
                    
                    if(mLoopsLeft != 0)
                    {                
                        mRealTime = (mRealTime - mAnimationLength);
                        ++mLoopCount;
                        
                        // if not infinite loops, decrement loops left
                        if(mLoopsLeft > 0)
                        {
                            --mLoopsLeft;
                        }
                        
                        ret |= AET_Looped;
                        
                        // call the animation event callback 
                        _fireCallback(AET_Looped, -1);
                        mLastHitMarkerIndex = -1;
                        
                        // rewind all sprite tracks
                        _rewindSpriteTracks();
						_rewindCueTracks();
                    }
                    else
                    {
                        // if loops run out, stop looping
                        mIsPlaying = false;
                        
                        ret |= AET_Finished;
                        
                        // call the animation event callback                         
                        _fireCallback(AET_Finished, -1);                        
                    }
                    
                    break;
                    
                case PM_PingPongs:
                    
                    mRealTime = mAnimationLength - (mRealTime - mAnimationLength);
                    mPlayForward = false;
                    
                    break;
                    
                default:
                    assert(0);                   
            }
            
        }
        else if(mRealTime < 0.0f )
        {
            // the animation has passed a run going backward
            
            switch(mPlaybackMode)
            {                    
                case PM_PingPongs:
                    
                    if(mLoopsLeft != 0)
                    {            
                        mPlayForward = true;
                        mRealTime = -mRealTime;
                        ++mLoopCount;
                        
                        // if not infinite loops, decrement loops left
                        if(mLoopsLeft > 0)
                        {
                            --mLoopsLeft;
                        }
                        
                        ret |= AET_Looped;
                        
                        // call the animation event callback 
                        _fireCallback( AET_Looped, -1);
                        
                        // rewind all sprite tracks
                        _rewindSpriteTracks();
						_rewindCueTracks();
                    }
                    else
                    {
                        // if loops run out, stop looping
                        mIsPlaying = false;
                        
                        ret |= AET_Finished; 
                        
                        // call the animation event callback 
                        _fireCallback(AET_Finished, -1);
                    }
                    
                    break;
                    
                default:
                    assert(0);                   
            }
        }
        return ret; 
    }
    
	// -------------==============-------------==============-------------==============-------------===
    void Animation::_rewindSpriteTracks()
    {
        unsigned int size = mSpriteTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mSpriteTrackList[i]->rewind();
        }
    }
	
	// -------------==============-------------==============-------------==============-------------===
	void Animation::_rewindCueTracks()
    {
        unsigned int size = mCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mCueTrackList[i]->rewind();
        }
    }
	
	// -------------==============-------------==============-------------==============-------------===
	void Animation::_rewindSkeletonActorCueTracks()
    {
        unsigned int size = mSkeletonActorCueTrackList.size();
        for(unsigned int i = 0; i < size; ++i)
        {
            mSkeletonActorCueTrackList[i]->rewind();
        }
    }
	
	// JTP 23 DEC 2011
	// TODO: Make this a binary search!!
	// -------------==============-------------==============-------------==============-------------===
	void Animation::_updateLastHitMarkerIndexWithCurrentTime()
	{
		mLastHitMarkerIndex = -1;
		unsigned int size = mMarkers.size();
		for(unsigned int i=0; i < size; ++i)
		{
			if(mRealTime > mMarkers[i].first)
			{
				mLastHitMarkerIndex = i;
			}
			else
			{
				break;
			}
		}
	}
	
	// -------------==============-------------==============-------------==============-------------===
	void Animation::_updateSpriteTrackEventsWithCurrentTime()
	{
		unsigned int size = mSpriteTrackList.size();
		for(unsigned int i = 0; i < size; ++i)
		{
			// we need to apply the animation tracks up until the current time to get the sprites in the right state
			mSpriteTrackList[i]->apply(mRealTime);
		}
	}
	
	// -------------==============-------------==============-------------==============-------------===
	void Animation::_updateCueTrackEventsWithCurrentTime()
	{
		unsigned int size = mCueTrackList.size();
		for(unsigned int i = 0; i < size; ++i)
		{
			mCueTrackList[i]->trackTo(mRealTime);
		}
	}
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::_parseNodeTrack( NodeAnimationTrack* nodeAniTrack, XMLDocument::NamedIterator nodeTrackIter )
    {
        std::string angleXPath;
        std::string angleYPath;
        std::string angleZPath;
        
        std::string positionXPath;
        std::string positionYPath;
        std::string positionZPath;
        
        std::string scaleXPath;
        std::string scaleYPath;
        std::string scaleZPath;
        
        std::string skewPath;
        
        std::string pivotXPath;
        std::string pivotYPath;
        std::string pivotZPath;
        
        Property prop;
        
        if( nodeTrackIter.getAttribute( "angleXRelCurvePath", prop ) )
        {
            angleXPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "angleYRelCurvePath", prop ) )
        {
            angleYPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "angleZRelCurvePath", prop ) )
        {
            angleZPath = prop.asString();
        }
        else if( nodeTrackIter.getAttribute( "angleRelCurvePath", prop ) )
        {
            angleZPath = prop.asString();
        }
        
        nodeAniTrack->loadCurvesAngle( angleXPath,
                                       angleYPath,
                                       angleZPath );
        
        
        if( nodeTrackIter.getAttribute( "posXRelCurvePath", prop ) )
        {
            positionXPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "posYRelCurvePath", prop ) )
        {
            positionYPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "posZRelCurvePath", prop ) )
        {
            positionZPath = prop.asString();
        }
        
        nodeAniTrack->loadCurvesPosition( positionXPath,
                                          positionYPath,
                                          positionZPath );
        
        
        
        if( nodeTrackIter.getAttribute( "scaleXRelCurvePath", prop ) )
        {
            scaleXPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "scaleYRelCurvePath", prop ) )
        {
            scaleYPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "scaleZRelCurvePath", prop ) )
        {
            scaleZPath = prop.asString();
        }
        
        nodeAniTrack->loadCurvesScale( scaleXPath,
                                       scaleYPath,
                                       scaleZPath );
        
        
        if( nodeTrackIter.getAttribute( "skewRelCurvePath", prop ) )
        {
            skewPath = prop.asString();
        }
        
        nodeAniTrack->loadCurveSkewGradient( skewPath );
        
        
        if( nodeTrackIter.getAttribute( "pivotXRelCurvePath", prop ) )
        {
            pivotXPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "pivotYRelCurvePath", prop ) )
        {
            pivotYPath = prop.asString();
        }
        if( nodeTrackIter.getAttribute( "pivotZRelCurvePath", prop ) )
        {
            pivotZPath = prop.asString();
        }
        
        
        nodeAniTrack->loadCurvesPivot( pivotXPath,
                                       pivotYPath,
                                       pivotZPath );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Animation::_buildTrackFromFlags( NodeAnimationTrack* nodeAniTrack,
                                          unsigned int flags,
                                          const std::string& locationNamePath,
                                          BasicBinaryReader& reader)
    {
        std::string angleXPath;
        std::string angleYPath;
        std::string angleZPath;
        
        std::string positionXPath;
        std::string positionYPath;
        std::string positionZPath;
        
        std::string scaleXPath;
        std::string scaleYPath;
        std::string scaleZPath;
        
        std::string skewPath;
        
        std::string pivotXPath;
        std::string pivotYPath;
        std::string pivotZPath;
        
        // ******************************************************************************
        //the order of this is very important, nmes will be written in order of the flags
        // ******************************************************************************
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_PosX ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            positionXPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_PosY ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            positionYPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_PosZ ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            positionZPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        
        nodeAniTrack->loadCurvesPosition( positionXPath,
                                          positionYPath,
                                          positionZPath );
        
        
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_ScaleX ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            scaleXPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_ScaleY ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            scaleYPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_ScaleZ ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            scaleZPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        
        nodeAniTrack->loadCurvesScale( scaleXPath,
                                      scaleYPath,
                                      scaleZPath );
        
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_Angle ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            angleZPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        else
        {
            if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_AngleX ) ) )
            {
                std::string curveName;
                reader.read( curveName );
                angleXPath = StringHelper::appendPath( locationNamePath, curveName );
            }
            if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_AngleY ) ) )
            {
                std::string curveName;
                reader.read( curveName );
                angleYPath = StringHelper::appendPath( locationNamePath, curveName );
            }
            if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_AngleZ ) ) )
            {
                std::string curveName;
                reader.read( curveName );
                angleZPath = StringHelper::appendPath( locationNamePath, curveName );
            }
        }
        nodeAniTrack->loadCurvesAngle( angleXPath,
                                       angleYPath,
                                       angleZPath );       
        
        
        
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_PivotX ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            pivotXPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_PivotY ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            pivotYPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_PivotZ ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            pivotZPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        
        
        nodeAniTrack->loadCurvesPivot( pivotXPath,
                                       pivotYPath,
                                       pivotZPath );
        
        if( ( flags & ( 1 << AnimationBinary::NodeAnimationTracks_SkewGradient ) ) )
        {
            std::string curveName;
            reader.read( curveName );
            skewPath = StringHelper::appendPath( locationNamePath, curveName );
        }
        
        nodeAniTrack->loadCurveSkewGradient( skewPath );

    }
}
