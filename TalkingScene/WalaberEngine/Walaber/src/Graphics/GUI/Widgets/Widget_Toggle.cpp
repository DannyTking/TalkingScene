#include "Widget_Toggle.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"
#include <ActionTimeline.h>
#include <Action_PlayGroupSound.h>
#include <Action_TimedCallback.h>

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a simple checkbox.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Toggle::Widget_Toggle( int name, Vector2 pos, Vector2 size) :
	Widget( name, WT_TOGGLE, pos, size, WO_PortraitUp ),
    mActionTimeline( new ActionTimeline( ActionTrack_SIZE ) )
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Toggle::Widget_Toggle( int name, Vector2 pos, Vector2 size, TexturePtr texOn, TexturePtr texOff,
								 std::string textOn, std::string textOff, const std::string& fontID, float resetDelay, float soundGroupID):
	Widget( name, WT_TOGGLE, pos, size, WO_PortraitUp ),
	mTextureOn( texOn ),
	mTextureOff( texOff ),
	mDisplayTextOn(textOn),
	mDisplayTextOff(textOff),
    mActionTimeline( new ActionTimeline( ActionTrack_SIZE ) )
	{
		setFont(fontID);
		init();
		
		if(texOn.use_count())
		{
			mOnTextureRect = texOn->getTextureRect();
		}
		
		if(texOff.use_count())
		{
			mOffTextureRect = texOff->getTextureRect();
		}
        
        // Did we specify a sound?
        setSoundID(soundGroupID);
        
        // If we specified a reset delay, just fire off the sound.
        setResetDelay(resetDelay);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Toggle::~Widget_Toggle()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::init()
	{
		mOn = false;
		mChanged = false;
		mTextScale = 1.0f;
        mResetDelay = -1.0f;
        mSoundToPlay = -1;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::setTextureOn( TexturePtr onTex )
	{
		mTextureOn = onTex;
		if(mTextureOn.use_count())
		{
			mOnTextureRect = mTextureOn->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::setTextureOff( TexturePtr offTex )
	{
		mTextureOff = offTex;
		if(mTextureOff.use_count())
		{
			mOffTextureRect = mTextureOff->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::setIcon( TexturePtr iconTex, Vector2 drawSize)
	{
		mIconTexture = iconTex;
		mIconTextureSize = drawSize;
		
		if(iconTex.use_count())
		{
			mIconTextureRect = iconTex->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Toggle::update( float elapsedSec, WidgetActionRet& ret )
	{
        // If we're under 0, we don't have a delay so it should be -1.
        assert(mResetDelay >= 0.0f || (mResetDelay < 0.0f && mResetDelay == -1.0f));
        
        mActionTimeline->execute(elapsedSec);
        
        if (mChanged)
        {
            mChanged = false;
            
            ret.valBool = mOn;
            return true;
        }
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::draw( SpriteBatch* sb )//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		if (mTextureOn.use_count() && mTextureOff.use_count())
			sb->drawQuad(mLayer, mOn ? mTextureOn : mTextureOff, getWorldPosition2D() + mDrawOffset, angle, Vector2(getSize().X, getSize().Y), mOn ? mOnTextureRect : mOffTextureRect );
		
		if (mIconTexture.use_count())
		{
			Vector2 icon_offset = VectorTools::rotateVector( Vector2(-mIconTextureSize.X, 0.0f), 
															degToRad(angle) );
			
			sb->drawQuad(mLayer, mIconTexture, getWorldPosition2D() + mDrawOffset + icon_offset, angle, Vector2(getSize().X, -getSize().Y), mIconTextureRect );
			
		}
		
		if (mFont != NULL)
		{
			if (mAlignment == TA_LeftRight)
			{
				// ON text
				Vector2 size = mFont->measureString(mDisplayTextOn)*mTextScale;
				Vector2 to = Vector2((getSize().X * 0.5f) + 16, size.Y * 0.5f);
				sb->drawString(mLayer, mFont, mDisplayTextOn.c_str(),  getWorldPosition2D() + to, angle, mTextScale, mTextColor);
				
				// OFF text.
				size = mFont->measureString(mDisplayTextOff)*mTextScale;
				to = Vector2(-size.X + (getSize().X * -0.5f) - 16.0f, size.Y * 0.5f);
				sb->drawString(mLayer, mFont, mDisplayTextOff.c_str(),  getWorldPosition2D() + to, angle, mTextScale, mTextColor);
			}
			else if (mAlignment == TA_Right)
			{
				// ON text
				Vector2 size = mFont->measureString(mDisplayTextOn)*mTextScale;
				Vector2 to = Vector2((getSize().X * 0.51f), (getSize().Y * -0.5f));
				sb->drawString(mLayer, mFont, mDisplayTextOn.c_str(),  getWorldPosition2D() + to, angle, mTextScale, mTextColor);
				
				// OFF text.
				size = mFont->measureString(mDisplayTextOff)*mTextScale;
				to = Vector2((getSize().X * 0.51f), (getSize().Y * 0.5f)-(size.Y));
				sb->drawString(mLayer, mFont, mDisplayTextOff.c_str(),  getWorldPosition2D() + to, angle, mTextScale, mTextColor);
			}
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Toggle::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
        // Check if enabled and also check if we don't have a timer running.
		if(mEnabled && !mActionTimeline->getActionUnitsForTrack(ActionTrack_DelayTimer))
		{
			mOn = !mOn;
			mChanged = true;
            // If a delay is enabled, start our action list and toggle back when we're done.
            if (mResetDelay > 0.0f || mSoundToPlay != -1)
            {
                if (mResetDelay > 0.0f)
                {
                    mActionTimeline->addActionToTrack(ActionTrack_DelayTimer,
                                                      new Action_TimedCallback(mResetDelay, CreateMemberCallbackPtr(*this, &Widget_Toggle::_delayCallback)));
                    
                    if (mSoundToPlay != -1)
                    {
                        // We have a reset delay, so just fire off the sound if we got one.
                        SoundManager::getInstancePtr()->playSoundFromGroup(mSoundToPlay);
                    }
                }
                else
                {
                    // We don't have a reset delay, so play the sound and wait for it to finish
                    // before switching back. If system sound is playing, just toggle for a second.
                    
                    if( SoundManager::getInstancePtr()->systemMusicIsPlaying() )
                    {
                        float duration = 1.0;
                        mActionTimeline->addActionToTrack(ActionTrack_DelayTimer,
                                                          new Action_Delay(duration));
                    }
                    else
                    {
                        mActionTimeline->addActionToTrack(ActionTrack_DelayTimer,
                                                          new Action_PlayGroupSound(mSoundToPlay, true));
                    }
                    mActionTimeline->addActionToTrack(ActionTrack_DelayTimer, new Action_Callback(CreateMemberCallbackPtr(*this, &Widget_Toggle::_delayCallback)));
                }
            }
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Toggle::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Toggle::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Toggle::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::releaseFingerUp( int fingerID, FingerInfo* info )
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Toggle::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Toggle::notifyFingerLost( int fingerID, FingerInfo* info )
	{
	}
    
    void Widget_Toggle::setResetDelay(float delay)
    {
        // If under 0, assume we don't have a delay.
        if (delay <= 0.0f)
        {
            mResetDelay = -1.0f;
        }
        else
        {
            mResetDelay = delay;
        }
    }
    
    void Widget_Toggle::_delayCallback( void* )
    {
        // If we're here it means we set a delay and it's up, so switch our state back.
        mOn = !mOn;
        mChanged = true;
    }
}


