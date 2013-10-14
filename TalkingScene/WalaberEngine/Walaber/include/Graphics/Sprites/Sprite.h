#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <vector>
#include <map>

#include <DrawableNode.h>
#include <Vector2.h>
#include <Rect.h>
#include <Texture.h>
#include <AnimationSharedDefinitions.h>
#include <Callback.h>
#include <VectorTools.h>

#include <XMLHelper.h>

#define FRAME_ROUNDING_EPSILON .001f

namespace Walaber
{
	// -------------------------------------------------------------------------------------------
	class BasicBinaryWriter;
    class BasicBinaryReader;
    class SpriteAnimation;
	class SpriteBatch;
	class Sprite;
	MAKE_SHARED_PTR(SpriteAnimation);	
	
	// -------------------------------------------------------------------------------------------
    struct SpriteAnimationEventCallbackParameters
    {
        SpriteAnimationPtr anim;
        AnimationEventType eType;
        int eventID;
        
        SpriteAnimationEventCallbackParameters(SpriteAnimationPtr panim, AnimationEventType peType, int peventID):
        anim(panim),
        eType(peType),
        eventID(peventID)
        {}
    };
	
	// -------------------------------------------------------------------------------------------
    struct SpriteEventCallbackParameters
    {
		Sprite*					sprite;
        SpriteAnimationPtr		anim;
        AnimationEventType		eType;
        int						eventID;
        
        SpriteEventCallbackParameters( Sprite* psprite, SpriteAnimationPtr panim, AnimationEventType peType, int peventID):
		sprite(psprite),
        anim(panim),
        eType(peType),
        eventID(peventID)
        {}
    };	
	
	// -------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------
	// SpriteAnimation - represents a series of frames that can be played back in a variety of ways.
	// -------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------
	class SpriteAnimation
	{
	public:
        struct LoadedCallbackParameters
        {
			ResultCode result;
            SpriteAnimation* anim;
            
            LoadedCallbackParameters(SpriteAnimation* panim):
			result(ResultCode_OK),
            anim(panim)
            {}
        };
		
		// -------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------
		struct AnimationFrame
		{
		public:
			Vector2						PosOffset;
			Vector2						Size;
			float						RotOffset;
			Color						FrameColor;
            TexturePtr                  FrameTexture;
			
			AnimationFrame():
			PosOffset(),
			Size(),
			RotOffset(0.0f),
			FrameColor(Color::White),
            FrameTexture()
			{
				
			}
		};
		
		
		// -------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------
		struct AnimationMarker
		{
		public:
			int									MarkerID;
		};
        
        // -------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------
		struct FrameImport
		{
		public:
            std::string     ImageName;
            Vector2         Offset;
            Vector2         Scale;
			float           AngleRad;
			Color           FrameColor;
            unsigned int    Repeat;
		};
		
		// -------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------
		SpriteAnimation( );
		SpriteAnimation( std::string name, TexturePtr texture, Vector2 upper_left_pixels, Vector2 size_pixels, Vector2 worldSize );
		
		~SpriteAnimation();
		
		void loadFromXML( xmlNodePtr root_node, CallbackPtr callback );
        
        /*!
            This function is purely used to load the data from xml to create a binary, it does not
            fill the runtime data structure
        */
        void loadFramesFromXML( xmlNodePtr root_node )
        {
            _parseXML( root_node );
        }
        
        /* 
            Write loading data into a buffer
        */
        void write( BasicBinaryWriter& writer );
        
        /*
            Read loading data into run-time structure
        */
        void read( BasicBinaryReader& reader );
        
        /*
            This read is only called from the munger to progress through objects
        */
        void readMunger( BasicBinaryReader& reader );
        
		TexturePtr getTexture()
        {
            if (mFrames.empty())
				return TexturePtr();
			
			return mFrames[mCurrentFrame].FrameTexture;
        }
		
		void setPlaybackMode( AnimationPlaybackMode pm ) { mPlaybackMode = pm; }
		AnimationPlaybackMode getPlaybackMode() { return mPlaybackMode; }
		
		void setName( std::string n ) { mName = n; }
		std::string getName() { return mName; }

		void setFPS( float fps ) { mFPS = fps; mDuration = mFrames.size() / mFPS; }
		float getFPS() { return mFPS; }
        
        float getDuration() { return mDuration; }
		
		void setPlaybackSpeedScale( float s ) { mPlaybackSpeedScale = s; }
		float getPlaybackSpeedScale() { return mPlaybackSpeedScale; }
		
		void setLoopCount( int loops )
        {
            assert( loops <= USHRT_MAX );
            mLoopsLeft = static_cast<unsigned short>( loops );
        }
		int getLoopsLeft() { return mLoopsLeft; }
		
		int getPlaybackDirection() { return mPlaybackDir; }
		
		Color getCurrentFrameColor() { if (mFrames.empty()) { return Color::White; } return mFrames[mCurrentFrame].FrameColor; }
		
		AnimationEventType update( float elapsedSec, int& outMarkerEventID );
		
		void addMarkerEvent( float time, int _id );
		void clearMarkerEvents();
		
		void setEventCallback( CallbackPtr c ) { mCallback = c; }
		
		void play();
        void play(const float playbackPosition);
		void play(const float playbackPosition, AnimationPlaybackMode pm, int numLoops);

		void pause();
		void stop();
        
        bool isPlaying()
        {
            return mPlaying;
        }
		
		void projectCurrentFrame(const Vector2& basePos, float baseAngle, const Vector2& baseScale, Vector2& outPos, float& outAngle, Vector2& outSize, Walaber::Rect& outUV, bool isScreenSpace = false );
		
		Vector2 getCurrentFrameOffset()
		{
			if (mFrames.empty())
				return Vector2::Zero;
			
			AnimationFrame& f = mFrames[mCurrentFrame];
			
			return f.PosOffset;
		}
		
        Rect getCurrentFrameUVs()
		{
			if (mFrames.empty())
				return Rect::UnitRect;
			
			AnimationFrame& f = mFrames[mCurrentFrame];
			
			return f.FrameTexture ? f.FrameTexture->getTextureRect() : Rect::UnitRect;
		}
        
        Vector2 getFrameSize();
		
		static AnimationPlaybackMode _parsePlaybackMode( std::string m );
        
		void setCurrentFrameOffset(Vector2 offset)
        {
			if (mFrames.empty())
            {
				return;
            }
			
			AnimationFrame& f = mFrames[mCurrentFrame];
            f.PosOffset = offset;
        }
		
    public: //callbacks
        
        void _loadedAtlas(void* data);
        void _loadedTexture(void* data);
        void _loadFrames();
        
    private:
        
        void _parseXML( xmlNodePtr anim_root );
        
	protected:		

		std::string										mName; ///> name of this animation.
		bool											mPlaying; ///> currently playing or not.
        bool                                            mHasAtlas; /* is the texture path and atlas
                                                                        or an image */
		AnimationPlaybackMode							mPlaybackMode; ///> playback mode.
		float											mFPS; /*! frames-per-second for this 
                                                                  animation */
		std::vector<AnimationFrame>						mFrames; /*! list of frames in this 
                                                                     animation. */
		unsigned int									mCurrentFrame; ///> current frame.
		float											mPlaybackPos; ///> current playback position
		int												mPlaybackDir; ///> direction of playback
		float											mPlaybackSpeedScale; /*! scalar for playback
                                                                                 1.0 = authored FPS,
                                                                                 2.0 - 2x speed etc.
                                                                             */
		float											mDuration; /*! total duration of this 
                                                                       animation. */
		unsigned short 									mLoopCount; ///> natural loop count.
		unsigned short									mLoopsLeft; /*! loops left.  if -1,
                                                                        loops indefinitely. */
		Color											mFrameColor; /*! blend color for the
                                                                         individual frame */
		std::map< float, AnimationMarker>				mMarkers; /*! animation marker events 
                                                                   at arbitrary playback positions.
                                                                  */
		std::map< float, AnimationMarker>::iterator		mCurrentMarker; /*! current animation
                                                                            marker */
		CallbackPtr										mCallback; /*! event callback specific to 
                                                                       this animation. */
		std::vector<FrameImport>						mFrameList; /*! temporary frame list for 
                                                                        loading. */
        std::string                                     mImagePath; /*! temporary string to hold
                                                                        path to atlas */
		CallbackPtr										mLoadedCallback; /*! callback to call when
                                                                             the sprite animation 
                                                                             is loaded. */
    };
	
	
	
	
	// -------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------
	// Sprite - defined as a position, size, and angle definining a quad with a texture on it.
	// textures can be animated by adding animations to the sprite.
	class Sprite : public DrawableNode
	{
	public:
		struct LoadedCallbackParameters
        {
            std::string path;
			ResultCode result;
            Sprite* sprite;
            
            LoadedCallbackParameters(Sprite* psprite, const std::string& ppath):
			path( ppath ),
            result(ResultCode_OK),
            sprite(psprite)
            {}
        };
		
		// Empty sprite, add animations manually.
		Sprite(const std::string& name, Vector2 pos, float angle, Vector2 scale = Vector2::One, const int groupID = NG_Sprite );
			
		// basic single-animation sprite set up automatically.
		Sprite( const std::string& name, TexturePtr texture, Vector2 pos, Vector2 size, float angle,
                std::string animName = "IDLE", const int groupID = NG_Sprite );
		Sprite( const std::string& name, TexturePtr texture, Vector2 pos, Vector2 size, float angle,
                Vector2 pixels_upper_left, Vector2 pixels_size, std::string animName = "IDLE",
                const int groupID = NG_Sprite );
        
        virtual ~Sprite()
        {
        }
			
		// loaded from a configuration file.
		void loadFromXML( const std::string& filename, CallbackPtr callback );
		
        void setPath( const std::string& path )
        {
            mPath = path;
        }
        
		void setScreenSpace(const bool isScreenSpace) { mIsScreenSpace = isScreenSpace; }
		bool isScreenSpace() { return mIsScreenSpace; }
		
		SpriteAnimationPtr addAnimation( SpriteAnimationPtr anim );
		
		void removeAnimation(const std::string& name );
		
		SpriteAnimationPtr setAnimation(const std::string& name );
		
		SpriteAnimationPtr playAnimation(const std::string& name);
		
        SpriteAnimationPtr playAnimation(const std::string& name, const float playbackPosition);
        
		SpriteAnimationPtr playAnimation(const std::string& name, const float playbackPosition,
                                         AnimationPlaybackMode playbackMode, int numLoops);
		
		SpriteAnimationPtr getAnimation(const std::string& name );
		
        SpriteAnimationPtr getCurrentAnimation() { return mCurrentAnim; }
		
		void loadAnimationsFromXML(const std::string& filename);
		
		
		void update( float elapsedSec );
		
		void draw( SpriteBatch* sb );
        
        void setColor( Color newColor ) { mColor = newColor; }
        Color getColor() { return mColor; }
        
        Vector2 getCurrentSize() 
		{ 
			if (mCurrentAnim.use_count()) 
			{ 
				return mCurrentAnim->getFrameSize(); 
			} 
			else 
			{ 
				return Vector2::Zero; 
			} 
		}
		
		// returns the current offset of the frame in world space
		Vector2 getCurrentFrameOffset()
		{
			Vector2 ret;
			if(mCurrentAnim)
			{				
				ret = Walaber::VectorTools::rotateVector( mCurrentAnim->getCurrentFrameOffset()*
                                                          getWorldScale2D(), getWorldAngleZ() );
			}
			
			return ret;
		}
		
		Vector2 getCurrentFramePosition()
		{
			Vector2 ret;
			if(mCurrentAnim)
			{				
				ret = getWorldPosition2D() +
                 Walaber::VectorTools::rotateVector( mCurrentAnim->getCurrentFrameOffset()*
                                                     getWorldScale2D(), getWorldAngleZ() );
			}
			
			return ret;
		}
		
		void setEventCallback( CallbackPtr c ) { mCallback = c; }
        
        void removeEventCallback()
        {
            mCallback.reset();
        }
        
    public: // callbacks
        
        void _animationLoaded( void* data);
		
		
		void _animationFileRead( void* params ); 
		
		
	protected:
		
		
		void _loadAnimation( xmlNodePtr anim_root );
		
		typedef std::map< std::string, SpriteAnimationPtr >	AnimationMap;
		typedef AnimationMap::iterator						AnimationMapIterator;
        std::string                         mPath;
		
		AnimationMap                        mAnimations;		///> list of animations
		SpriteAnimationPtr					mCurrentAnim;		///> current animation.
		
		int									mAnimationsLoading;	/*! running count of animations 
                                                                    loading */
		CallbackPtr                         mLoadedCallback;	/*! callback when all animations are
                                                                    loaded. */
		
		bool								mIsScreenSpace;		/*! off by default, but should be 
                                                                    set if it's in screen space for
                                                                    rotations exported from flash */
        Color                               mColor;             ///> color to blend the sprite with
    
		CallbackPtr                         mCallback;			/*! event callback specific to this
                                                                    sprite (and all animations it
                                                                    uses). */
	};
    
    MAKE_SHARED_PTR( Sprite );
	
}	// end namespace Walaber

#endif	// _SPRITE_H_
