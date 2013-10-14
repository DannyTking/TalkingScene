#include "Sprite.h"

#include <WalaberMath.h>
#include <MemberCallback.h>
#include <SpriteBatch.h>
#include <BinaryWriter.h>
#include <BinaryReader.h>


#include <MemberCallback.h>

#ifdef _DEBUG
#define _DEBUG_DRAW_SPRITE 0
#endif

namespace Walaber
{
    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimation::SpriteAnimation( ) :
    mName("NO_NAME"),
    mPlaying(false),
    mPlaybackMode( PM_Loops ),
    mFPS(1.0f),
    mCurrentFrame(0),
    mPlaybackPos(0.0f),
    mPlaybackDir(1),
    mPlaybackSpeedScale(1.0f),
    mDuration(0.0f),
    mLoopCount(-1),
    mLoopsLeft(-1),
    mFrameColor(Color::White),
    mCurrentMarker(mMarkers.begin()),
    mCallback(NULL)
    {

    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimation::SpriteAnimation( std::string name, TexturePtr texture, Vector2 upper_left_pixels, Vector2 size_pixels, Vector2 worldSize ) :
    mName(name),
    mPlaying(false),
    mPlaybackMode( PM_Loops ),
    mFPS(1.0f),
    mCurrentFrame(0),
    mPlaybackPos(0.0f),
    mPlaybackDir(1),
    mPlaybackSpeedScale(1.0f),
    mDuration(0.0f),
    mLoopCount(-1),
    mLoopsLeft(-1),
    mFrameColor(Color::White),
    mCurrentMarker(mMarkers.begin()),
    mCallback(NULL)
    {
        // generate a single frame.
        AnimationFrame f;
        f.PosOffset = Vector2::Zero;
        f.Size = worldSize;
        f.RotOffset = 0.0f;
        f.FrameTexture = texture;

        mFrames.push_back(f);
    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::loadFromXML( xmlNodePtr anim_root, CallbackPtr callback )
    {
        mLoadedCallback = callback;

        _parseXML( anim_root );

        // load texture.
        if( mHasAtlas )
        {
            // load texture.
            MemberCallbackPtr<SpriteAnimation>::type memcallback(new MemberCallback<SpriteAnimation>(this, &SpriteAnimation::_loadedAtlas));
            CallbackPtr callbackptr = static_pointer_cast<Callback>(memcallback);
            TextureManager::getManager().loadTextureAtlas( mImagePath, callbackptr );
        }
        else if( XML::attrExists( anim_root, "texture" ) )
        {
            // load texture.
            MemberCallbackPtr<SpriteAnimation>::type memcallback(new MemberCallback<SpriteAnimation>(this, &SpriteAnimation::_loadedTexture));
            CallbackPtr callbackptr = static_pointer_cast<Callback>(memcallback);
            TextureManager::getManager().getTexture( mImagePath, callbackptr );
        }
    }

    void SpriteAnimation::write( BasicBinaryWriter& writer )
    {
        writer.write( mName );

        writer.write( mImagePath );

        writer.write( mFPS );

        writer.write( mLoopCount );

        writer.write( static_cast< unsigned short >( mPlaybackMode ) );

        writer.write( static_cast< unsigned int >( mFrameList.size() ) );

        for( unsigned int i = 0; i < mFrameList.size(); ++i )
        {
            writer.write( mFrameList[i].ImageName );

            writer.write( mFrameList[i].Offset );

            writer.write( mFrameList[i].Scale );

            writer.write( mFrameList[i].AngleRad );

            writer.write( mFrameList[i].FrameColor );

            writer.write( mFrameList[i].Repeat );
        }
    }

    void SpriteAnimation::read( BasicBinaryReader& reader )
    {
        reader.read( mName );

        reader.read( mImagePath );

        reader.read( mFPS );

        reader.read( mLoopCount );
        mLoopsLeft = mLoopCount;

        // we need to read the playback mode out as a short and cast it to a playback mode
        mPlaybackMode = static_cast<AnimationPlaybackMode>(reader.readUInt16());

        // we need to do a blocking texture read here. We don't have any file manager facilities
        // to guarentee a blocking load - keep this in mind
        TextureManager& textureMgr = TextureManager::getManager();
        textureMgr.loadTextureAtlas( mImagePath );

        unsigned int numFrames = reader.readUInt32();

        mFrames.reserve( numFrames);
        for( unsigned int i = 0; i < numFrames; ++i )
        {
            AnimationFrame frame;

            std::string imageName;
            reader.read( imageName );

            if( !imageName.empty() )
            {
                frame.FrameTexture = textureMgr.getTexture( imageName );
                int flags = frame.FrameTexture->getTextureFlags();

                reader.read( frame.PosOffset );
                if(flags & Texture::TF_SubtextureHasOffset)
                {
                    frame.PosOffset += frame.FrameTexture->getContentOffset();
                }

                reader.read( frame.Size );
                frame.Size *= frame.FrameTexture->getContentSize();

                reader.read( frame.RotOffset );;

                if(flags & Texture::TF_SubtextureHasBeenRotatedNinetyDegrees)
                {
                    frame.RotOffset += Walaber::PI/2.0f;
                }

                reader.read( frame.FrameColor );

                unsigned int repeat = reader.readUInt32();

                for(unsigned int j = 0; j < repeat; ++j )
                {
                    mFrames.push_back( frame );
                }
            }
            else
            {
                reader.seek( sizeof(frame.PosOffset) + sizeof(frame.Size) +
                             sizeof(frame.RotOffset) + sizeof(frame.FrameColor),
                             BasicBinaryReader::SeekStart_Current );

                frame.Size = Vector2::One;
                frame.FrameColor = Color(0,0,0,0);

                unsigned int repeat = reader.readUInt32();

                for(unsigned int j = 0; j < repeat; ++j )
                {
                    mFrames.push_back( frame );
                }
            }

        }

        mDuration = mFrames.size() / mFPS;
        mImagePath.clear();
    }

    void SpriteAnimation::readMunger( BasicBinaryReader& reader )
    {
        reader.read( mName );

        reader.read( mImagePath );

        reader.read( mFPS );

        reader.read( mLoopCount );

        // we need to read the playback mode out as a short and cast it to a playback mode
        mPlaybackMode = static_cast<AnimationPlaybackMode>(reader.readUInt16());

        unsigned int numFrames = reader.readUInt32();

        mFrameList.reserve( numFrames);
        for( unsigned int i = 0; i < numFrames; ++i )
        {
            mFrameList.push_back( FrameImport() );
            FrameImport& import = mFrameList.back();

            reader.read( import.ImageName );

            reader.read( import.Offset );

            reader.read( import.Scale );

            reader.read( import.AngleRad );

            reader.read( import.FrameColor );

            reader.read( import.Repeat );
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimation::~SpriteAnimation()
    {
        mFrames.clear();
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::_loadedAtlas(void* data)
    {
        TextureManager::AtlasLoadedCallbackParameters* params = (TextureManager::AtlasLoadedCallbackParameters*)data;
#ifdef _DEBUG
        printf("SpriteAnimation::_loadedImageList (%s)\n", getName().c_str());
        printf("Loading from (%s)\n", params->xmlPath.c_str());
#endif

        if (params->result == ResultCode_OK)
        {
            _loadFrames();
        }


        if (mLoadedCallback)
        {
            LoadedCallbackParameters callbackParams(this);
            callbackParams.result = params->result;

            mLoadedCallback->invoke( &callbackParams );
        }
    }

    void SpriteAnimation::_loadedTexture(void* data)
    {
        Texture::TextureLoadedCallbackParameters *params = (Texture::TextureLoadedCallbackParameters*)data;

        if(params->result == ResultCode_OK)
        {
            _loadFrames();
        }
        else
        {
            // texture didnt load
            assert(0);
        }

        if (mLoadedCallback)
        {
            LoadedCallbackParameters callbackParams(this);
            callbackParams.result = params->result;

            mLoadedCallback->invoke( &callbackParams );
        }
    }

    void SpriteAnimation::_loadFrames()
    {
        TextureManager& texMgr = TextureManager::getManager();
        // add the frames...
        mFrames.reserve( mFrameList.size() );
        for (std::vector<FrameImport>::iterator it = mFrameList.begin(); it != mFrameList.end(); it++)
        {
            AnimationFrame frame;
            
            std::string imageName = (*it).ImageName;
            if( !(*it).ImageName.empty() )
            {
                TexturePtr tex = texMgr.getTexture((*it).ImageName);                

                frame.FrameTexture = tex;
                int flags = frame.FrameTexture->getTextureFlags();

                frame.PosOffset = (*it).Offset;
                if(flags & Texture::TF_SubtextureHasOffset)
                {
                    frame.PosOffset += frame.FrameTexture->getContentOffset();
                }

                frame.RotOffset = (*it).AngleRad;

                if(flags & Texture::TF_SubtextureHasBeenRotatedNinetyDegrees)
                {
                    frame.RotOffset += Walaber::PI/2.0f;
                }

                frame.Size = frame.FrameTexture->getContentSize() * (*it).Scale;
                frame.FrameColor = (*it).FrameColor;

                
                unsigned int repeat = (*it).Repeat;
                for( unsigned int i = 0; i < repeat; ++i )
                {
                    mFrames.push_back( frame );
                }
            }
            else
            {
                frame.Size = Vector2::One;
                frame.FrameColor = Color(0,0,0,0);
                
                unsigned int repeat = (*it).Repeat;
                for( unsigned int i = 0; i < repeat; ++i )
                {
                    mFrames.push_back( frame );
                }
            }
        }

        mDuration = mFrames.size() / mFPS;

        mFrameList.clear();
        mImagePath.clear();
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    AnimationEventType SpriteAnimation::update( float elapsedSec, int& outMarkerEventID )
    {
        AnimationEventType ret = AET_None;

        if (mFrames.empty())
            return ret;

        if (!mPlaying)
            return ret;

        float oldTime = mPlaybackPos;
        float deltaTime = (elapsedSec * mPlaybackDir * mPlaybackSpeedScale);

        float newTime = oldTime + deltaTime;
        if (newTime >= mDuration)
        {
            float singleFrameDur = (mDuration / mFrames.size());

            // passed end-point!
            switch (mPlaybackMode)
            {
                case PM_Loops:
                    newTime = (newTime - mDuration );
                    ret = AET_Looped;
                    mCurrentMarker = mMarkers.begin();

                    break;

                case PM_PingPongs:
                    newTime = mDuration - (newTime - mDuration) - singleFrameDur;
                    mPlaybackDir = -1;
                    ret = AET_Looped;
                    break;

                default:
                    if (oldTime < mDuration)
                    {
                        ret = AET_Stopped;
                    }
                    newTime = mDuration;

            }
        }
        else if (newTime <= 0.0f)
        {
            float singleFrameDur = (mDuration / mFrames.size());

            // passed beginning
            switch (mPlaybackMode)
            {
                case PM_Loops:
                    newTime = (mDuration + newTime);
                    ret = AET_Looped;
                    break;

                case PM_PingPongs:
                    newTime = -newTime + singleFrameDur;
                    mPlaybackDir = 1;
                    ret = AET_Looped;
                    break;

                default:
                    if (oldTime > 0.0f)
                        ret = AET_Stopped;
                    newTime = mDuration;
            }
        }

        // now update current frame to match the playback head position.
        mPlaybackPos = newTime;

        if( mCurrentMarker != mMarkers.end() )
        {
            if ( mPlaybackPos >= mCurrentMarker->first )
            {
                ret = AET_Marker;
                outMarkerEventID = mCurrentMarker->second.MarkerID;
                ++mCurrentMarker;
            }
        }

        mCurrentFrame = Walaber::clampi( (int)(mPlaybackPos * mFPS), 0, mFrames.size() - 1 );

        return ret;
    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::addMarkerEvent( float time, int _id )
    {
        AnimationMarker m;
        m.MarkerID = _id;
        mMarkers[ time ] = m;
        mCurrentMarker = mMarkers.begin();
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::clearMarkerEvents()
    {
        mMarkers.clear();
        mCurrentMarker = mMarkers.begin();
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::play()
    {
        bool wasPlaying = mPlaying;

        mPlaying = true;

        if ((!wasPlaying) && (mCallback))
        {
            SpriteAnimationEventCallbackParameters params( SpriteAnimationPtr(this), AET_Began, -1 );
            mCallback->invoke(&params);
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::play(const float playbackPosition)
    {
        bool wasPlaying = mPlaying;

        mPlaying = true;
        mPlaybackPos = playbackPosition;

        // we plus 1 because the the time is 0 based but the frames are
        float nearestWhole = std::floor( ( ( mPlaybackPos) * mFPS ) + .5f );
        float newPlaybackPos = mPlaybackPos * mFPS;
        if( ( nearestWhole - newPlaybackPos ) <= FRAME_ROUNDING_EPSILON )
        {
            newPlaybackPos = nearestWhole;
        }

        mCurrentMarker = mMarkers.lower_bound( playbackPosition );

        mCurrentFrame = Walaber::clampi( (int)newPlaybackPos , 0, mFrames.size() - 1 );

        if ((!wasPlaying) && (mCallback))
        {
            SpriteAnimationEventCallbackParameters params( SpriteAnimationPtr(this), AET_Began, -1 );
            mCallback->invoke(&params);
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::play(const float playbackPosition, AnimationPlaybackMode pm, int numLoops)
    {
        bool wasPlaying = mPlaying;

        mPlaying = true;
        mPlaybackPos = playbackPosition;

        // we plus 1 because the the time is 0 based but the frames are
        float nearestWhole = std::floor( ( ( mPlaybackPos) * mFPS ) + .5f );
        float newPlaybackPos = mPlaybackPos * mFPS;
        if( ( nearestWhole - newPlaybackPos ) <= FRAME_ROUNDING_EPSILON )
        {
            newPlaybackPos = nearestWhole;
        }

        mCurrentMarker = mMarkers.lower_bound( playbackPosition );
        mCurrentFrame = Walaber::clampi( (int)newPlaybackPos , 0, mFrames.size() - 1 );

        mPlaybackMode = pm;
        mLoopsLeft = numLoops;

        if ((!wasPlaying) && (mCallback))
        {
            SpriteAnimationEventCallbackParameters params( SpriteAnimationPtr(this), AET_Began, -1 );
            mCallback->invoke(&params);
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::pause()
    {
        bool wasPlaying = mPlaying;

        mPlaying = false;

        if ((wasPlaying) && (mCallback))
        {
            SpriteAnimationEventCallbackParameters params( SpriteAnimationPtr(this), AET_Paused, -1 );
            mCallback->invoke( &params );
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::stop()
    {
        bool wasPlaying = mPlaying;

        mPlaying = false;
        mPlaybackDir = 1;
        mPlaybackPos = 0.0f;
        mCurrentFrame = 0;
        mLoopsLeft = mLoopCount;

        mCurrentMarker = mMarkers.begin();

        if ((wasPlaying) && (mCallback))
        {
            SpriteAnimationEventCallbackParameters params( SpriteAnimationPtr(this), AET_Stopped, -1 );
            mCallback->invoke(&params);
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void SpriteAnimation::projectCurrentFrame(const Vector2& basePos, float baseAngle, const Vector2& baseScale, Vector2& outPos, float& outAngle, Vector2& outSize, Walaber::Rect& outUV, bool isScreenSpace )
    {
        if (mFrames.empty())
            return;

        AnimationFrame& f = mFrames[mCurrentFrame];

        outSize = f.Size * baseScale;

        int mult =  (1 ^ -isScreenSpace) + isScreenSpace;
        outAngle = baseAngle + (mult * f.RotOffset);
        outPos = basePos + Walaber::VectorTools::rotateVector( f.PosOffset*baseScale, baseAngle );
        outUV = f.FrameTexture->getTextureRect();
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    Vector2 SpriteAnimation::getFrameSize()
    {
        if (mFrames.empty())
            return Vector2::Zero;


        AnimationFrame& f = mFrames[mCurrentFrame];
        return f.Size;
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    AnimationPlaybackMode SpriteAnimation::_parsePlaybackMode( std::string m )
    {
        std::string lower = StringHelper::toLower( m );
        if ( lower == "loop" || lower == "loops" )
            return PM_Loops;

        if ( (lower == "pinpong") || (m == "pingpongs") )
            return PM_PingPongs;

        return PM_Once;
    }

    void SpriteAnimation::_parseXML( xmlNodePtr anim_root )
    {
        mName = XML::parseString(anim_root, "name");
        mPlaybackMode = _parsePlaybackMode( XML::parseString(anim_root, "playbackMode") );
        mFPS = XML::parseFloat(anim_root, "fps");
        mLoopCount = XML::parseInt(anim_root, "loopCount");
        mLoopsLeft = mLoopCount;
        if( XML::attrExists(anim_root, "atlas") )
        {
            mImagePath = XML::parseString(anim_root, "atlas");
            mHasAtlas = true;
        }
        else
        {
            mImagePath = XML::parseString(anim_root, "texture");
            mHasAtlas = false;
        }

        std::string textureBasePath = XML::parseString(anim_root, "textureBasePath");

        // list of frames (just names for now, actually loaded after layout file is loaded
        // add the frames...
        for (xmlNodePtr frame = anim_root->children; frame; frame = frame->next)
        {
            if (xmlStrcmp(frame->name, (const xmlChar*)"Frame") == 0)
            {
                std::string name;
                if( XML::attrExists(frame, "name" ) )
                {
                    name = textureBasePath + XML::parseString(frame, "name");
                }

                int repeat = 1;
                float angleRad = 0;
                Vector2 offset;
                Vector2 scale = Vector2::One;
                Color color = Color::White;

                if (XML::attrExists(frame, "repeat"))
                {
                    repeat += XML::parseInt( frame, "repeat" );
                }

                if(XML::attrExists(frame, "offset"))
                {
                    offset = XML::parseVector2(frame, "offset");
                }

                if(XML::attrExists(frame, "angleDeg"))
                {
                    angleRad = Walaber::degToRad( XML::parseFloat(frame, "angleDeg") );
                }
                else if(XML::attrExists(frame, "angleRad"))
                {
                    angleRad = XML::parseFloat(frame, "angleRad");
                }

                if(XML::attrExists(frame, "scale"))
                {
                    scale = XML::parseVector2(frame, "scale");
                }

                if( XML::attrExists(frame, "color") )
                {
                    color = XML::parseColor8bit(frame, "color");
                }

                if( XML::attrExists(frame, "alphaPercent") )
                {
                    float alphaPercent = 1.0;
                    alphaPercent = XML::parseFloat(frame, "alphaPercent");
                    color *= alphaPercent;
                }

                mFrameList.push_back(FrameImport());
                FrameImport& fi = mFrameList.back();
                fi.ImageName = name;
                fi.Offset = offset;
                fi.Scale = scale;
                fi.AngleRad = angleRad;
                fi.FrameColor = color;
                fi.Repeat = repeat;
            }
        }

    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    // Empty sprite, add animations manually.
    Sprite::Sprite( const std::string& name, Vector2 pos, float angle, Vector2 scale, const int groupID ) :
    DrawableNode(name, groupID),
    mIsScreenSpace(false),
    mCallback(NULL),
    mAnimationsLoading(0)
    {
        setLocalPosition2D(pos);
        setLocalScale2D(scale);
        setLocalAngleZ(angle);

        setColor(Walaber::Color::White);
    }

    // basic single-animation sprite set up automatically.
    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    Sprite::Sprite( const std::string& name, TexturePtr texture, Vector2 pos, Vector2 size, float angle, std::string animName, const int groupID ) :
    DrawableNode(name, groupID),
    mIsScreenSpace(false),
    mCallback(NULL),
    mAnimationsLoading(0)
    {
        setLocalPosition2D(pos);
        setLocalAngleZ(angle);

        addAnimation( SpriteAnimationPtr( new SpriteAnimation(animName, texture, Vector2::Zero, texture->getContentSize(), size) ) );
        setAnimation( animName );

        setColor(Walaber::Color::White);
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    Sprite::Sprite( const std::string& name, TexturePtr texture, Vector2 pos, Vector2 size, float angle, Vector2 pixels_upper_left, Vector2 pixels_size, std::string animName, const int groupID ) :
    DrawableNode(name, groupID),
    mIsScreenSpace(false),
    mCallback(NULL),
    mAnimationsLoading(0)
    {
        setLocalPosition2D(pos);
        setLocalAngleZ(angle);

        addAnimation( SpriteAnimationPtr( new SpriteAnimation(animName, texture, pixels_upper_left, pixels_size, size) ) );
        setAnimation( animName );

        setColor(Walaber::Color::White);
    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::loadFromXML( const std::string& filename, CallbackPtr callback )
    {
        mLoadedCallback = callback;

        mPath = filename;

        mAnimationsLoading = 1;

        loadAnimationsFromXML(filename);

        mAnimationsLoading--;
        if (mAnimationsLoading == 0)
        {
            if (mLoadedCallback)
            {
                LoadedCallbackParameters params(this, mPath);
                params.result = ResultCode_OK;

                mLoadedCallback->invoke( &params );
            }
        }

    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimationPtr Sprite::addAnimation( SpriteAnimationPtr anim )
    {
        std::string ani = anim->getName();
        mAnimations[anim->getName()] = anim;

        if (mAnimations.size() == 1)
            setAnimation(anim->getName());

        return anim;
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::removeAnimation(const std::string& name )
    {
        AnimationMapIterator it = mAnimations.find( name );
        if (it != mAnimations.end())
            mAnimations.erase(it);
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimationPtr Sprite::setAnimation(const std::string& name )
    {
        SpriteAnimationPtr anim = getAnimation(name);
        if (anim.use_count())
        {
            mCurrentAnim = anim;
        }

        return anim;
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimationPtr Sprite::playAnimation(const std::string& name )
    {
        return playAnimation(name, 0.0f);
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimationPtr Sprite::playAnimation(const std::string& name, const float playbackPosition )
    {
        SpriteAnimationPtr anim = setAnimation(name);
        if (anim)
        {
            anim->play(playbackPosition);
            SpriteEventCallbackParameters params( this, mCurrentAnim, AET_Began, -1 );
            if(mCallback)
            {
                mCallback->invoke( &params );
            }
        }

        return anim;
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimationPtr Sprite::playAnimation(const std::string& name, const float playbackPosition,
                                             AnimationPlaybackMode playbackMode, int numLoops)
    {
        SpriteAnimationPtr anim = setAnimation(name);
        if (anim)
        {
            anim->play(playbackPosition, playbackMode, numLoops);
            SpriteEventCallbackParameters params( this, mCurrentAnim, AET_Began, -1 );
            if(mCallback)
            {
                mCallback->invoke( &params );
            }
        }

        return anim;
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    SpriteAnimationPtr Sprite::getAnimation(const std::string& name )
    {
        SpriteAnimationPtr ptr;

        AnimationMapIterator it = mAnimations.find( name );
        if (it != mAnimations.end())
            ptr = it->second;

        return ptr;
    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::loadAnimationsFromXML(const std::string& filename )
    {
        PropertyList plist;

        MemberCallbackPtr<Sprite>::type memberCallback( new MemberCallback<Sprite>( this, &Sprite::_animationFileRead ) );
        CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);

        FileManager::getInstancePtr()->readFile(filename, callback, plist, FileManager::PP_NoAbstraction);
    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::update( float elapsedSec )
    {
        if (mCurrentAnim.use_count())
        {
            int eventID = -1;
            AnimationEventType event = mCurrentAnim->update(elapsedSec, eventID);

            switch (event)
            {
                case AET_Looped:
                case AET_Marker:
                    if (mCallback)
                    {
                        SpriteEventCallbackParameters params( this, mCurrentAnim, event, eventID );
                        mCallback->invoke( &params );
                    }
                    break;

                case AET_Stopped:
                    if (mCallback)
                    {
                        SpriteEventCallbackParameters params( this, mCurrentAnim, event, eventID );
                        mCallback->invoke( &params );
                    }
                    break;

                default:
                    break;
            }
        }
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::draw( SpriteBatch* sb )
    {
        if (!mVisible)
            return;

        if (mCurrentAnim)
        {
            Walaber::Rect UVs;

            UVs = mCurrentAnim->getCurrentFrameUVs();
            Color frameColor = mColor * mCurrentAnim->getCurrentFrameColor();

            Vector2 halfSize = mCurrentAnim->getFrameSize() * .5f;
            Vector3 frameOffset = Vector3( mCurrentAnim->getCurrentFrameOffset() );

            Vector3 verts[] = { Vector3( -halfSize.X, halfSize.Y, 0.0f ) + frameOffset,
                Vector3( halfSize.X, halfSize.Y, 0.0f ) + frameOffset,
                Vector3( -halfSize.X, -halfSize.Y, 0.0f ) + frameOffset,
                Vector3( halfSize.X, -halfSize.Y, 0.0f ) + frameOffset
            };

            verts[0] = transformPoint( verts[0] );
            verts[1] = transformPoint( verts[1] );
            verts[2] = transformPoint( verts[2] );
            verts[3] = transformPoint( verts[3] );

            Vector2 uvs[] = { Vector2( UVs.upper_left.X, UVs.getBottomRight().Y ),
                              Vector2( UVs.getBottomRight().X, UVs.getBottomRight().Y),
                              Vector2( UVs.upper_left.X, UVs.upper_left.Y ),
                              Vector2( UVs.getBottomRight().X, UVs.upper_left.Y ) };

            Color colors[] = { frameColor, frameColor, frameColor, frameColor };

            sb->drawVertsWithDepth( getWorldPosition().Z, mLayer, mCurrentAnim->getTexture(), 4 ,
                                   true, verts, uvs, colors);

#if _DEBUG_DRAW_SPRITE
            sb->drawQuadColored( 1000, getWorldPosition2D(), getWorldAngleZ(), Vector2(3.0f,3.0f), Color::Green);

            Vector2 min = Vector2(verts[0]);
            Vector2 max = Vector2(verts[0]);
            for( unsigned int i = 0; i < 4; ++i )
            {
                if( verts[i].X < min.X )
                {
                    min.X = verts[i].X;
                }
                else if( verts[i].X > max.X )
                {
                    max.X = verts[i].X;
                }

                if( verts[i].Y < min.Y )
                {
                    min.Y = verts[i].Y;
                }
                else if( verts[i].Y > max.Y )
                {
                    max.Y = verts[i].Y;
                }
            }
            sb->drawQuadColored(1001, min + ( (max - min ) * .5f ), 0.0f, Vector2(1.5f,1.5f), Color::Magenta);
#endif
        }
    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::_animationFileRead( void* params ) //std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList userData )
    {
        FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;


#ifdef _DEBUG
        printf("Sprite::_animationFileRead( %s, %s )\n", rcp->path.c_str(), rcp->actualPath.c_str() );
#endif
        ResultCode error = ResultCode_OK;


        if (rcp->result == ResultCode_OK)
        {
            xmlDocPtr doc;
            xmlNodePtr root_elem = XML::loadDocMemory( rcp->buffer, rcp->length, "Sprite", doc);

            if (rcp->buffer)
                delete[] rcp->buffer;

            if (root_elem == NULL)
            {
                error = ResultCode_ErrorXMLParseError;
            }
            else
            {

                // first loop, count # of animations we are going to load...
                for( xmlNodePtr anim = root_elem->children; anim; anim = anim->next )
                {
                    if (xmlStrcmp(anim->name, (const xmlChar*)"Animation") == 0)
                    {
                        mAnimationsLoading++;
                    }
                }

                // second loop, actually load the animations...
                for( xmlNodePtr anim = root_elem->children; anim; anim = anim->next )
                {
                    if (xmlStrcmp(anim->name, (const xmlChar*)"Animation") == 0)
                    {
                        _loadAnimation(anim);
                    }
                }
            }

            XML::cleanupDoc( doc );
        }


        if (error != ResultCode_OK)
        {
            // call the callback now...
            if (mAnimationsLoading == 0)
            {
                if (mLoadedCallback)
                {
                    LoadedCallbackParameters callbackParams(this, rcp->path);
                    callbackParams.result = error;

                    mLoadedCallback->invoke( &callbackParams );
                }
            }
        }

    }


    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::_loadAnimation( xmlNodePtr anim_root )
    {
        SpriteAnimation* anim = new SpriteAnimation();

        MemberCallbackPtr<Sprite>::type memcallback( new MemberCallback<Sprite>(this, &Sprite::_animationLoaded));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        anim->loadFromXML( anim_root, callback);
    }

    // -------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    void Sprite::_animationLoaded( void* data )
    {
        SpriteAnimation::LoadedCallbackParameters* params = (SpriteAnimation::LoadedCallbackParameters*)data;
#ifdef _DEBUG
        printf("Sprite::_animationLoaded( %s )\n", params->anim->getName().c_str());
#endif

        addAnimation( SpriteAnimationPtr( params->anim ) );

        mAnimationsLoading--;

        if (mAnimationsLoading == 0)
        {
            if (mLoadedCallback)
            {
                LoadedCallbackParameters loadedCallbackParameters(this, mPath);
                mLoadedCallback->invoke( &loadedCallbackParameters );
            }
        }
    }

}   // end namespace Walaber
