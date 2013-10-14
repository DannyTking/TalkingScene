#include "Widget_PushButton.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_PushButton::Widget_PushButton( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_PUSH_BUTTON, pos, size, WO_PortraitUp ),
	mTexture(),
	mHilightTexture(),
	mReactOnDown(false),
	mLabelGroup(NULL)
	{
		init();
		mFont = FontManager::getInstancePtr()->getFont("normal");
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_PushButton::Widget_PushButton( int name, Vector2 pos, Vector2 size, TexturePtr texture, bool reactOnDown ) :
	Widget( name, WT_PUSH_BUTTON, pos, size, WO_PortraitUp ),
	mTexture(texture),
	mHilightTexture(),
	mReactOnDown(reactOnDown),
	mLabelGroup(NULL)
	{
		init();
		mFont = FontManager::getInstancePtr()->getFont("normal");
		
		if (mTexture.use_count())
		{
			mTextureRect = mTexture->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	Widget_PushButton::Widget_PushButton( int name, Vector2 pos, Vector2 size, TexturePtr texture, bool reactOnDown, std::string text, const std::string& fontID) :
	Widget( name, WT_PUSH_BUTTON, pos, size, WO_PortraitUp ),
	mTexture(texture),
	mHilightTexture(),
	mReactOnDown(reactOnDown),
	mDisplayText(text),
	mLabelGroup(NULL)
	{	
		setFont(fontID);
		
		init();
		
		if(mFont)
		{
			_resizeFont();
		}
		
		if (mTexture.use_count())
		{
			mTextureRect = mTexture->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_PushButton::Widget_PushButton( int name, Vector2 pos, Vector2 size, TexturePtr texture, TexturePtr hilightTex, bool reactOnDown, std::string text, const std::string& fontID)
	: Widget( name, WT_PUSH_BUTTON, pos, size, WO_PortraitUp ),
	mTexture(texture),
	mHilightTexture(hilightTex),
	mReactOnDown(reactOnDown),
	mDisplayText(text),
	mLabelGroup(NULL)
	{
		setFont(fontID);
		
		init();
		
		if (mFont)
		{
			_resizeFont();	
		}
		
		if (mTexture.use_count())
		{
			mTextureRect = mTexture->getTextureRect();
		}
		if (mHilightTexture.use_count())
		{
			mHilightTextureRect = mHilightTexture->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_PushButton::~Widget_PushButton()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_PushButton::init()
	{
		mFingerID = -1;
		mGoPush = false;
		mGoRelease = false;
		mPressed = false;
		mPressOnEnter = false;
		mTextColor = Color::Black;
		mHilightTextColor = Color(0.5f, 0.5f, 0.5f);
		mHilightColor = Color(0.5f, 0.5f, 0.5f);
		mDisabledColor = Color(0.5f, 0.5f, 0.5f);
		mDisabledTextColor = Color(0.5f, 0.5f, 0.5f);
		mEdgePadding = Vector2(20.0f, 5.0f);
		mTextOffset = Vector2::Zero;
		mTextHilightOffset = Vector2::Zero;
		mTextureTile = Vector2(1, 1);
		mTileAnimation = Vector2::Zero;
		mTextureColor = Color::White;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_PushButton::_resizeFont()
	{
		if(mFont)
		{
			Vector2 size = mFont->measureString(mDisplayText);
			
			mTextScale = 1.0f;
			
			// Take the more restrictive size
			Vector2 newTextScale = Vector2( (getSize().X-(mEdgePadding.X))/size.X, (getSize().Y-(mEdgePadding.Y))/size.Y );
			
			// Use the shorter side to pad
			if (newTextScale.X < newTextScale.Y)
			{
				if(size.X > (getSize().X-(mEdgePadding.X)))
				{
					mTextScale = newTextScale.X;
				}
			}
			else
			{
				if(size.Y > (getSize().Y-(mEdgePadding.Y)))
				{
					mTextScale = newTextScale.Y;
				}
			}
			/*
			 else 
			 {
			 mTextScale = 1.0f;
			 }
			 */
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_PushButton::_applyTile()
	{	
		if (mTexture.use_count())
		{
			mTextureRect = mTexture->getTextureRect();
			mTextureRect = Rect( Vector2( mTextureRect.upper_left.X, mTextureRect.upper_left.Y ),
								 Vector2( mTextureRect.size.X * mTextureTile.X, mTextureRect.size.Y * mTextureTile.Y ) );
		}
		if (mHilightTexture.use_count())
		{
			mHilightTextureRect = mHilightTexture->getTextureRect();
			mHilightTextureRect = Rect( Vector2( mHilightTextureRect.upper_left.X, mHilightTextureRect.upper_left.Y ),
										Vector2( mHilightTextureRect.size.X * mTextureTile.X, mHilightTextureRect.size.Y * mTextureTile.Y ) );
		}
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void Widget_PushButton::setTextureFlipped( bool flipHorizontal, bool flipVertical )
    {
        if ( flipHorizontal ) {
            mTextureRect.upper_left.X += mTextureRect.size.X;
            mTextureRect.size.X *= -1.0f;
        }
        
        if ( flipVertical ) {
            mTextureRect.upper_left.Y += mTextureRect.size.Y;
            mTextureRect.size.Y *= -1.0f;
        }
    }
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_PushButton::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mGoPush)
		{
#ifdef _DEBUG
			printf("Widget_PushButton::update() - firing Pressed! %s\n", mName);
#endif
			mGoPush = false;		// reset button.
			ret.valBool = true;
			return true;
		}
		else if (mGoRelease)
		{
#ifdef _DEBUG
			printf("Widget_PushButton::update() - firing Released! %s\n", mName);
#endif
			mGoRelease = false;
			ret.valBool = false;
			return true;
		}
		
		if (mLabelGroup)
		{
			mLabelGroup->update( elapsedSec, ret );
		}
		
		// Animate the tiling (if necessary)
		if (mTileAnimation != Vector2::Zero)
		{
			mTextureRect.upper_left += mTileAnimation;
			mHilightTextureRect.upper_left += mTileAnimation;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_PushButton::draw( SpriteBatch* sb )//, const Vector2& offset )
	{
		if (mTexture.use_count())
		{
			float angle = _getAngle();
			
			Vector2 drawPos = getWorldPosition2D() + mDrawOffset;
			sb->drawQuad( mLayer, (mHilightTexture.use_count()) ? ((mPressed ? mHilightTexture : mTexture)) : mTexture,
						 drawPos + mDrawOffset, 
						 angle, 
						 Vector2(getSize().X, getSize().Y), 
						 (mHilightTexture.use_count()) ? ((mPressed ? mHilightTextureRect : mTextureRect)) : mTextureRect, 
						 ((!mEnabled) ? mDisabledColor : ((mPressed) ? mHilightColor : mTextureColor)) * mTransparency);
			
			if ((mFont) && (mDisplayText.size()))
			{
				Vector2 size = mFont->measureString(mDisplayText);
				Vector2 offset = (mPressed) ? mTextHilightOffset : mTextOffset;
				sb->drawString(mLayer + 1, mFont, mDisplayText.c_str(), getWorldPosition2D() + mDrawOffset + offset - (size*mTextScale*0.5f), angle, mTextScale,
							   ((!mEnabled) ? mDisabledTextColor : ((mPressed) ? mHilightTextColor : mTextColor)) * mTransparency);
			}
			
			// Change all of our labels offsets
			for (int i = 0; i < mLabels.size(); i++)
			{
				mLabels[i]->setDrawOffset((mPressed) ? mTextHilightOffset : mTextOffset);
				
				Color newColor = (!mEnabled) ? mDisabledTextColor : ((mPressed) ? mHilightTextColor : mTextColor);
				
				//mLabels[i]->setBGColorBlend(newColor);
				mLabels[i]->setTextDisplayColor(newColor);
			}
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_PushButton::setTile( Vector2 numberTiles )
	{
		mTextureTile = numberTiles;
		
		_applyTile();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_PushButton::setTile( Vector2 numberTiles, float aspectRatio )
	{
		numberTiles.Y = aspectRatio * getSize().Y / (getSize().X / numberTiles.X);
		mTextureTile = numberTiles;
		
		_applyTile();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_PushButton::setTileOffset( Vector2 offset )
	{
		mTextureRect.upper_left = offset;
		mHilightTextureRect.upper_left = offset;
	}   
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_PushButton::setTileAnimation( Vector2 direction )
	{
		mTileAnimation = direction;
	}    
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_PushButton::addLabel( Widget_Label* newLabel )
	{
		// First label? Create a group as well
		if (mLabels.empty())
		{
			mLabelGroup = new Widget_Group(-1, getWorldPosition2D(), Vector2::One);
		}
		
		mLabels.push_back( newLabel );
		mLabelGroup->addWidget( newLabel );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_PushButton::setLocalPosition2D(const Vector2& pos)
	{
		// Move the group if we have one
		if (mLabelGroup) 
		{
			mLabelGroup->setLocalPosition2D(pos);
		}
		
		Widget::setLocalPosition2D( pos );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_PushButton::applyPositionOffset2D(const Vector2& pos)
	{
		// Move the group if we have one
		if (mLabelGroup)
		{
			mLabelGroup->applyPositionOffset2D( pos );
		}
		
		Widget::applyPositionOffset2D( pos );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_PushButton::setLocalScale2D(const Vector2& scale)
	{
		Widget::setLocalScale2D( scale );
		
		// Resize the font
		_resizeFont();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_PushButton::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::acceptNewFingerDown(%d)\n", fingerID);
#endif
		
		mPressed = true;
		
		if (!mEnabled)
		{
			// take focus, but don't react to it.
			if (mFingerID == -1)
			{
				mFingerID = fingerID;
				return true;
			}
		}
		
		if (mReactOnDown)
		{
			mGoPush = true;
			mFingerID = -1;
			return true;
		}
		else
		{
			if (mFingerID == -1)
			{
				mFingerID = fingerID;
				return true;
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_PushButton::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		
		if (mPressOnEnter)
		{
			mPressed = true;
			
			if (mReactOnDown)
			{
				mGoPush = true;
				mFingerID = -1;
				return true;
			}
			else
			{
				if (mFingerID == -1)
				{
					mFingerID = fingerID;
					mPressed = true;
					return true;
				}
			}	
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_PushButton::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::releaseFingerStayed(%d)\n", fingerID);
#endif
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_PushButton::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::releaseFingerMoved(%d)\n", fingerID);
#endif
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_PushButton::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::releaseFingerUp(%d)\n", fingerID);
#endif
		if (fingerID == mFingerID)
		{
			if (!mReactOnDown  && (mEnabled))
			{
				mGoPush = true;
			}
			
			mFingerID = -1;
		}
		else if (mReactOnDown)
		{
			if(mEnabled)
				mGoRelease = true;
			
			mFingerID = -1;
		}
		
		mPressed = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_PushButton::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::releaseFingerLeft(%d)\n", fingerID);
#endif
		if (fingerID == mFingerID)
		{
			mFingerID = -1;
			
		}
		else if (mReactOnDown && mEnabled)
		{
			mGoRelease = true;
		}
		mPressed = false;
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_PushButton::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("PushButton::releaseFingerLeft(%d)\n", fingerID);
#endif
		if (fingerID == mFingerID)
		{
			mFingerID = -1;
			
		}
		/*
		 else if (mReactOnDown && mEnabled)
		 {
		 mGoRelease = true;
		 }
		 */
		mPressed = false;
	}
}


