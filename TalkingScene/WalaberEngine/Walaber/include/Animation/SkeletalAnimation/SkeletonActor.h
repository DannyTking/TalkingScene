//
//  SkeletonActor.h
//  WalaberEngine
//
//  Created by Jason Pecho on 12/21/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _SKELETON_ACTOR_H_
#define _SKELETON_ACTOR_H_

#include <map>
#include <vector>
#include <Vector2.h>
#include <Skeleton.h>
#include <Sprite.h>
#include <ParticleSet.h>
#include <macros.h>
#include <algorithm>
#include <queue>
#include <SoundManager.h>
#include <limits.h>

namespace Walaber
{
	class Color;
	class SpriteBatch;
	class ParticleEmitter;
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
    class SkeletonActor
	{
    public: 
		struct SkeletonActorAnimationEventCallbackParameters
        {
            std::string animationName;
            AnimationEventType eventType;
            int eventID;
        };
        
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        enum AnimationSoundState
        {
            ASS_On,
            ASS_Muted,
            ASS_PlayOnce,
        };		
		
		enum EventActionType
		{
			EAT_None,
			EAT_Sound,
			EAT_Prop,
			EAT_Animation,
			EAT_Emitter,
		};
		
		enum PropFlags
		{
			PropFlags_FreeStanding      =					0,
			PropFlags_Attached          =			(1  <<  0),
			PropFlags_Scene             =			(1  <<  1),
            PropFlags_PauseOnStart      =           (1  <<  2),
		};
        
        enum PropType
        {
            PropType_Invalid,
            PropType_Sprite,
            PropType_Skeleton,
        };
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		/*!
         Delets its prop object
        */
		class Prop
		{
        public:
			Prop():
			mPropObject( 0 ),
			mPropType( PropType_Invalid ),
            mPropFlags( PropFlags_FreeStanding ),
			mLayer( 0 ),
			mScenes(0),
			mStartAnimation()
			{}
			
			Prop( Sprite* sprite, unsigned int propFlags, int relativeLayer):
			mPropObject( sprite ),
			mPropType( PropType_Sprite),
            mPropFlags( propFlags ),
			mLayer( relativeLayer ),
			mScenes(0),
			mStartAnimation()
			{}
			
            Prop( Skeleton* skeleton, unsigned int propFlags, int relativeLayer):
			mPropObject( skeleton ),
			mPropType( PropType_Skeleton ),
            mPropFlags( propFlags ),
			mLayer( relativeLayer ),
			mScenes(0),
			mStartAnimation()
			{}
            
            ~Prop()
            {
                delete mPropObject;
            }
            
            
			bool operator==(const Prop& r) const
			{
				return this->mPropObject == r.mPropObject;
			}
            
            void setPropObject( Sprite* sprite )
            {
                mPropObject = sprite;
                mPropType = PropType_Sprite;
            }
            
            void setPropObject( Skeleton* skeleton )
            {
                mPropObject = skeleton;
                mPropType = PropType_Sprite;
            }

            std::string getPropName() const
            {
                assert( mPropObject != NULL );
                return mPropObject->getName();
            }
            
            DrawableNode* getPropObject() const
            {
                return mPropObject;
            }
            
            PropType getPropType() const
            {
                return mPropType;
            }
            
            void setPropFlag ( const unsigned int flag )
            {
                mPropFlags |= flag;
            }
            
            void clearPropFlag( const unsigned int flag )
            {
                mPropFlags &= ~flag;
            }
            
            void clearPropFlags ( )
            {
                mPropFlags = 0;
            }
            
			unsigned int getPropFlags() const
            {
                return mPropFlags;
            }
            
            void setScenes( const unsigned scenes )
            {
                mScenes = scenes;
            }
			unsigned int getScenes() const
            {
                return mScenes;
            }
            
            void setLayer( const int layer )
            {
                mLayer = layer;
            }
            
			int getRelativeLayer() const
            {
                return mLayer;
            }
            
            void setStartAnimation( const std::string& animation )
            {
                mStartAnimation = animation;
            }
            
			const std::string& getStartAnimation() const
            {
                return mStartAnimation;
            }
            
        private:
            
            DrawableNode*	mPropObject;
            PropType        mPropType;
			unsigned int	mPropFlags;
			unsigned int	mScenes;
			int				mLayer;
			std::string		mStartAnimation;
		};
        
		class EventActionData
		{
		public:
			short						eventID;
			short						eventActionType;
			
			EventActionData(short peventActionType):
			eventID(-1),
			eventActionType(peventActionType)
			{}
			
			virtual ~EventActionData()
			{
			}
            
            bool operator< ( const EventActionData& ead )
            {
                return ( eventID < ead.eventID );
            }
            
        };
        
		MAKE_SHARED_PTR(EventActionData);
		
		class SoundActionData : public EventActionData
		{
		public:
			enum SoundActionType
			{
				SAT_GroupID,
				SAT_Filename,
			};
		public:
			int soundGroup;
			int soundSpecificIndex;
			std::string filename;
			SoundActionType soundActionType;
			bool isVO;
			
			SoundActionData():
			EventActionData((short)EAT_Sound),
			soundGroup(-1),
			soundSpecificIndex(-1),
			filename(),
			soundActionType(SAT_GroupID),
			isVO( false )
			{}
		};
        
		class PropActionData : public EventActionData
		{
		public:
			
			enum PropActionType
			{
				PAT_Visibility			= (1 <<   1),
				PAT_PlayAnimation		= (1 <<   2),
				PAT_LayerChange			= (1 <<   4),
			};

            enum ScenePropType
            {
                SPT_NotSceneProp = 0,
                SPT_ShowAsSceneProp,
                SPT_HideSceneProp
            };
			
		public:
			AnimationPlaybackMode           playMode;
			unsigned int					actionFlags;
			float							playOffset;
			short							numLoops;
			short							layer;
			bool							visible;
            ScenePropType                   sceneProp;
			std::string						animationName;
			std::string						propName;
			
			PropActionData():
			EventActionData((short)EAT_Prop),
			actionFlags(0),
			playMode( PM_Once ),
			playOffset( 0.0f ),
			numLoops(-1),
			layer(-1),
			visible(true),
            sceneProp(SPT_NotSceneProp),
			animationName(),
			propName()
			{}
			
		};
		
		class AnimationActionData : public EventActionData
		{	
		public:
			std::string animation;
			float playOffset;
			int loops;
			bool crossFade;
			bool isSpecific;
			
			AnimationActionData():
			EventActionData((short)EAT_Animation),
			animation(),
			playOffset(0.0f),
			loops(0),
			crossFade(true),
			isSpecific(false)
			{}
		};
		
		class EmitterActionData : public EventActionData
		{	
		public:
			int emitterIndex;
			int numParticles;
			bool active;
			
			EmitterActionData():
			EventActionData((short)EAT_Emitter),
			emitterIndex(-1),
			numParticles(-2),
			active(false)
			{}
		};
		
		static bool eventActionPtrPredicate(const EventActionDataPtr lhs, const EventActionDataPtr rhs)
		{
			return lhs->eventID < rhs->eventID;
		}
		
		
		typedef std::vector< EventActionDataPtr >							AnimationEventDataList;
		typedef std::map< AnimationEventType, AnimationEventDataList >		EventTypeToActionDataMap;
		
		struct AnimationData
		{
			int groups;
			int scenes;
			EventTypeToActionDataMap eventActionData;
			bool canCrossFade;
			
			AnimationData():
			groups( 0xffffffff ),
			scenes( 0xffffffff ),
			eventActionData(),
			canCrossFade( true )
			{}
		};
		
		typedef std::map< std::string, AnimationData >		AnimationNameToData;
		
        // @todo Clean up the fact that we are using two structures that are extremely similar
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        struct AnimationGroupData
		{
			int nextAnimationGroup;
			int loops;
			int probability;
			bool crossFade;
			
			AnimationGroupData():
			nextAnimationGroup(0),
			loops(0),
			probability(1),
			crossFade(true)
			{}
            
            void reset()
            {
                nextAnimationGroup = 0;
                loops = 0;
                probability = 1;
                crossFade = true;
            }
		};
		
		struct AnimationGroupDataAux
		{
			std::string nextAnimationGroup;
			int loops;
			int probability;
			bool crossFade;
			
			AnimationGroupDataAux():
			nextAnimationGroup(),
			loops(0),			
			probability(1),
			crossFade(true)
			{}
		};
                
        struct CallbackAnimationEvent
        {
            AnimationEventType      eventType;
            int                     eventID;
        };
                
        enum DrawFlags
        {
            DrawFlags_None        =           0,
            DrawFlags_Skeleton    =           1,
            DrawFlags_Props       =   ( 1 <<  2),
            DrawFlags_Emitters    =   ( 1 <<  3),
        };
		
		typedef std::map< int, std::vector< AnimationGroupData > >		GroupNameToGroupData;
	public:
        
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		SkeletonActor();
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        ~SkeletonActor();
                
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void loadSkeleton(const std::string& name, const std::string& skelFilepath);
        void loadAnimationDescription( const std::string& animFilepath, CallbackPtr finishedCallback );
        void loadAnimationDescription( const std::string& animFilepath,
                                       const std::string& groups,
                                       const std::string& scenes,
                                       CallbackPtr finishedCallback );
		
		void loadSkeletonWithAnimationDescriptions(const std::string& name, const std::string& skelFilepath, const std::string& animFilepath);
		void loadSkeletonWithAnimationDescriptions(const std::string& name, const std::string& skelFilepath, const std::string& animFilepath, CallbackPtr callback);

        unsigned int getSceneNameIdentifier( const std::string& sceneName );

		void init(const Vector2& pos, const std::string& sceneName, const Vector2& size = Vector2::One, const bool isScreenSpace = false );
        void init(const Vector2& pos, const unsigned int sceneName, const Vector2& size = Vector2::One, const bool isScreenSpace = false );
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		void update( float elapsed );
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void setColor(const Color& newColor );
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void makePropSceneProp( const std::string & propName, const std::vector< std::string > & scenes, const std::string & animation, bool pauseOnStart );

        /*!
         Draw any combination of the skeleton, emitters and props, this function forces a flush on
         the sprite batch to guarentee pieces draw in the right order
         @param sb the sprite batch to draw the skeleton, props, and emitters into
         @param drawFlags @see SkeletonActor::DrawFlags, these flags tell the skeleton actor if it
                          should draw the skeleton, the props and the emitter
        */
        void draw( SpriteBatch* sb, unsigned int drawFlags );
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		// Doesn't change current animation sound state
		void playAnimation( const std::string aniName, const float playOffset = 0.0f, AnimationPlaybackMode pm = PM_Once, int loops = -1, bool crossFade=true );
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		void playAnimation( const std::string aniName, AnimationSoundState aniSoundState, const float playOffset = 0.0f, AnimationPlaybackMode pm = PM_Once, int loops = -1, bool crossFade=true );
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		std::string getCurrentAnimationName() { return mAnimationNames[mCurrentAnimation]; }
		std::string getCurrentArmatureAnimationName() { return mArmatureNames[mCurrentAnimation]; }
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		std::string getArmatureAnimationForActorAnimation( std::string actorAnimName );
                
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        Sprite* getPropSprite( std::string propName )
        {
            Sprite* sprite = 0;
            std::map< std::string, Prop* >::iterator prop = mProps.find( propName );
            
            if (prop != mProps.end())
            {
                assert( prop->second->getPropType() == PropType_Sprite );
                
                if( prop->second->getPropType() == PropType_Sprite )
                {
                    sprite = static_cast<Sprite*>( prop->second->getPropObject() );
                }
            }
            
            return sprite;
        }

		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        bool isLoaded() { return mXmlLoaded && mInitSuccessful; }

		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		bool isAnimationMemberOfGroup( const std::string& animName, const std::string& groupName );
		bool isCurrentAnimationMemberOfGroup( const std::string& groupName );
		
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void setPosition( Vector2 pos );
		Vector2 getPosition() { return mSkeleton->getWorldPosition2D(); };
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void setAngle( float a );
		float getAngle() { return mSkeleton->getWorldAngleZ(); };
        
        // @todo rename size to scale!!!
        Vector2 getSize() { return mSize; }
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        Skeleton* getSkeleton(){ return mSkeleton; }
        
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		// Use the current sound state
        void playAnimationFromGroup(const std::string groupName, AnimationPlaybackMode pm, int loops=-1, bool crossFade=true);
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void playAnimationFromGroup(const std::string groupName, AnimationSoundState aniSoundState, AnimationPlaybackMode pm, int loops=-1, bool crossFade=true);
      
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		std::string getArmatureNameForAnimation(const std::string& name)
        {
			std::string ret;
            
            std::map< std::string, int>::iterator it = mAnimationNameToIndex.find(name);
            
            if(it != mAnimationNameToIndex.end())
            {
				ret = mArmatureNames[it->second];
            }
            
            return ret;
        }
        
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		float getAnimationLength(const std::string& name)
        {
			std::string armatureName = getArmatureNameForAnimation(name);
			
			float ret = 0;
			
			if( !armatureName.empty() && mSkeleton )
			{
				ret = mSkeleton->getAnimationManager()->getAnimation(armatureName)->getLength();
            }
			
            return ret;
        }

        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        bool groupExists( const std::string& groupName )
        {
            std::map< std::string, int >::iterator it = mGroupNameToGroupBitFlag.find(groupName);
            return it != mGroupNameToGroupBitFlag.end();
        }

        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        bool animationExists( const std::string& animationName )
        {
            std::map< std::string, int >::iterator it = mAnimationNameToIndex.find(animationName);
            return it != mAnimationNameToIndex.end();
        }
        
        void setDisallowSounds(bool disallow)
        {
            mDisallowSounds = disallow;
        }

        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		std::string getAnimationFromGroup(const int groupName)
        {
			std::string ret = mAnimationNames[0];
            
            std::map< int, std::vector< std::string > >::iterator it = mAnimationGroupToNameMap.find(groupName);
            
            if(it != mAnimationGroupToNameMap.end())
            {
                const std::vector< std::string >& group = it->second;
                int random = std::rand() % group.size(); 
                
                ret = group[random];
            }
            else
            {
                assert(0);
            }
            
            return ret;
        }
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~ this is for the animation test screen (don't call it in live game code!!)
        std::string playNextAnimation(bool forward, AnimationPlaybackMode pm, bool crossFade)
        {
            if(forward)
            {
                ++curAnimation;
                if(curAnimation == mAnimationNames.size())
                {
                    curAnimation = 0;
                }
            }
            else
            {
                --curAnimation;
                if(curAnimation < 0 )
                {
                    assert( mAnimationNames.size() - 1 < SHRT_MAX );
                    curAnimation = static_cast<short>( mAnimationNames.size() - 1 );
                }
            }
			
            Logger::printf("Walaber", Logger::SV_DEBUG, "Play Animation %d/%d", curAnimation, (int) (mArmatureNames.size()-1));

            _playAnimation(curAnimation, ASS_On, 0.0f, pm, -1, crossFade);
			
            return mArmatureNames[curAnimation];          
        }
        
        void playCurrentAnimation(AnimationPlaybackMode pm, bool crossFade)
        {
            _playAnimation(curAnimation, ASS_On, 0.0f, pm, -1, crossFade);
        }
        
        void initTest()
        { 
            // animation test screen
            curAnimation = -1;
            playingFromTest = 1;
            // end animation test screen 
            
            // clear scene props, we are in test mode
            mActiveProps.clear();
        }
        
        void cleanupTest()
        {
            playingFromTest = 0;
        }
		
        short curAnimation;
        short playingFromTest;
        /// end
		        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void _animationEventCallback(void* data); 
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void _skeletonLoaded(void* data);
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void _xmlFileLoaded(void* data);
                
        /*!
         Add a callback keyed by the pointer of the object.
         This means statics won't work here...for now
         @param key pointer to object that contains the callback pointer
         @param c callback smart pointer
         */
        void registerEventCallback(void* key, CallbackPtr c ) { mAnimationEventCallbacks[key] = c; }
        
        /*!
         Remove the callback keyed by key
         @param key the key of the callback to be removed
         */
        void unregisterEventCallback( void* key ) 
        { 
            mAnimationEventCallbacks.erase(key); 
        }
        
        /*! Pauses all currently playing sounds that originated with this actor. */
        void pauseSounds();
        
        /*! Resumes all currently paused sounds that originated with this actor. */
        void resumeSounds();

        /*!
        Remove all scene props the Active Props list
        */
        void clearSceneProps();
        
        /*!
        Remove all the non scene props from the active list
        */
		void clearNonSceneProps();


        /*! Stops all looping sound effects that originated with this actor */
        void stopAllLoopingSFX();
        
        /*! Stops all sounds that originated with this actor */
        void stopAllSounds();
		
	private:  

        void _initProp( Prop * prop );
        
        void _propSkeletonEventCallback( void* data );
                
        void _propSpriteEventCallback( void* data );
                
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        enum ParticleType
		{
			PT_Tear_Start = 0,
			PT_Tear_End = PT_Tear_Start + 1,
			
			PT_Sleep
		};
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        void _playAnimation( const int aniIndex, AnimationSoundState aniSoundState, const float playOffset, AnimationPlaybackMode pm, int loops, bool crossFade );
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
		void _playAnimationFromGroup(const int groupName, AnimationSoundState aniSoundState, AnimationPlaybackMode pm, int loops, bool crossFade);
        
        // ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
        int _getAnimationNameIndex(const std::string& name)
        {
            unsigned int size = mArmatureNames.size();
            for(unsigned int i = 0; i < size; ++i)
            {
                if(mArmatureNames[i] == name)
                {
                    return i;
                }
            }
            
            return -1;
        }
		
		void _clearAnimationGroupData();
		
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~  
		void _doEventAction(const EventActionDataPtr eventAction);
        
        /*!
         Callback invoked when sound file that needs playing is loaded. Puts returned
         SoundEffectInstancePtr into a collection for later modification.
         (i.e. pausing, resuming, etc.)
         @param data @see SoundFileInstanceLoadedParameters
         */
        void _gotSoundFile( void* data );
        
        /*!
         @NOTE: THIS FUNCTION IS UNTESTED. Animations typically do not play sounds
                defined in the XML files, rather with raw filenames instead.
         
         Callback invoked when sound that needs playing is loaded. Puts returned 
         SoundEffectInstancePtr into a collection for later modification.
         (i.e. pausing, resuming, etc.)
         @param data @see SoundEffectInstanceLoadedParameters
         */
        void _soundLoaded( void* data );
				
		// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~  
		
        /*!
         Fires all of the callbacks in the manager. It doesn't check if the callback exists so
         make sure it exists before calling. Also it only fires for finished at the moment
         @param eType the type of the event being fire
         @param eventID the ID of the event being fired it eType is AET_Marker
         */
        void _fireCallback(AnimationEventType eType, int eventID);
                
        void _aggregateEvent( AnimationEventType eventType, int eventId );

		
        Skeleton*														mSkeleton;
		std::map< std::string, Prop* >									mProps;
		std::map< std::string, Prop* >									mActiveProps;
        
		// particles
		std::vector<ParticleEmitter*>									mEmitters;
											     
        Vector2															mPos;
		float															mAngle;
                
        Vector2															mSize;
		
		int																mCurrentAnimation;
        int                                                             mCurrentAnimationGroup;
		unsigned int													mCurrentScene;
        
        bool															mIsScreenSpace;
        bool															mHasLoadedSkeleton;
        bool                                                            mSkeletonFinished;
        bool															mXmlLoaded;
        AnimationPlaybackMode                                           mCurrentPlaybackMode;
        AnimationEventType												mCurrentAnimationPlayMode; // Began or Looped
        AnimationSoundState												mAnimationSoundState;
		
		// 
		bool															mInitSuccessful;
        bool															mInitIsScreenSpace;
		Vector2															mInitPos;
		Vector2															mInitSize;
        std::string                                                     mInitSceneName;
        unsigned int                                                    mInitSceneNameIdentifier;
		
		std::vector< std::string >										mAnimationNames;
		std::vector< std::string >										mArmatureNames;
		
		std::map< std::string, int >									mAnimationNameToIndex;
		std::map< int, std::string >									mAnimationIndexToName;
		
		std::map< std::string, int >									mGroupNameToGroupBitFlag;
		std::map< std::string, unsigned int >							mSceneNameToSceneBitFlag;
		
		AnimationNameToData												mAnimationNameToData;
        std::map< int, std::vector< std::string > >						mAnimationGroupToNameMap;
		GroupNameToGroupData											mGroupNameToData;
                
        std::map<void*, CallbackPtr>                                    mAnimationEventCallbacks;
                
        std::vector< void* >                                            mActivePropsForAnimation;
        
        AnimationGroupData                                              mNextAnimationGroup;
        ///< if this animation group exists, we need to play it
                
        std::queue< CallbackAnimationEvent >                            mCallbackEventQueue;
		
        CallbackPtr                                                     mLoadedCallback;

        std::vector<SoundEffectInstancePtr>                             mSounds; ///< List of sounds playing

        static const std::string GROUPS_TO_LOAD_KEY;
        static const std::string SCENES_TO_LOAD_KEY;
        
        
        bool                                                    mDisallowSounds;
	};
}

#endif
