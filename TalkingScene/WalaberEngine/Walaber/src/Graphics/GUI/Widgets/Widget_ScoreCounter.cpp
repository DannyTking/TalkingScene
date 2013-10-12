#include "Widget_ScoreCounter.h"
#include "VectorTools.h"
#include "TextManager.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

#include "math.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	const float Widget_ScoreCounter::DEFAULT_SPEED = 30.0f;
	
	Widget_ScoreCounter::Widget_ScoreCounter( int name, const Vector2& pos, const Vector2& size ) :
	Widget(name, WT_SCORE_COUNTER, pos, size, WO_PortraitUp)
	{
		init();
	}
	
	Widget_ScoreCounter::Widget_ScoreCounter( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& fontID, const MovementDirection direction, int targetScore, const float textScale, const float leftPad, const float rightPad, const float topPad, const float bottomPad, const float speed):
	Widget(name, WT_SCORE_COUNTER, pos, size, WO_PortraitUp),
	mBGTexture(bgTexture),	
    mCounterDirection(direction),
    mTargetScore(targetScore),
    mTextScale(textScale),
	mPaddingLeft(leftPad),
	mPaddingRight(rightPad),
	mPaddingTop(topPad),
	mPaddingBottom(bottomPad),
    mMovementStatus(MS_NORMAL),
    mSpeed(speed),
    mDeltaTime(0.0f),
    mAnimationTime(0.5f)
	{	
		setFont(fontID);
		init();
		_updateBorders();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	Widget_ScoreCounter::~Widget_ScoreCounter()
	{
		mBGColorBlend = Color::White;
		mTextureRect = Rect();
		mBorders = Rect();
		mTextCenterOffsetFromPos = Vector2::Zero;
		mTextDisplayColor = Color::Black;
		mTextScale = 1.0f;
		mFingerID = -1;
		mFireTouched = false;
        for (int i = 0; i < mCurrentStrings.size(); ++i) {
            mCurrentStrings[i] = 0;
        } 
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::init()
	{
        std::string str;
		for (int i = 0; i < 10; ++i) {
            str = Walaber::TextManager::getString(StringHelper::intToStr(i));
            mTextQueues.push_back(TextLineInfo(str, mFont->measureString(str, mTextScale)));
        }
        
        mColumnNum = 0;
        int tempScore = mTargetScore;
        while (tempScore /=10) {
            ++mColumnNum;
        }
        
        if (mCounterDirection == MD_DECREASE)
        {
            mCurrentChangingColumn = 0;
        }else
        {
            mCurrentChangingColumn = mColumnNum;
        }
        
        for (int i = 0; i < mColumnNum+1; ++i)
        {
            mCurrentStrings.push_back(0);
        }
        
        mMovementDirection = MD_INCREASE;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::setBGTexture(TexturePtr bgTexture)
	{
		mBGTexture = bgTexture;  
		if(bgTexture.use_count())
		{
			mTextureRect = bgTexture->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::setPadding( const float leftPad, const float rightPad, const float topPad, const float bottomPad )
	{
		mPaddingLeft = leftPad;
		mPaddingRight = rightPad;
		mPaddingTop = topPad;
		mPaddingBottom = bottomPad;
		
		_updateBorders();
	}

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::advanceToNextString()
	{
        switch (mCounterDirection) {
            case MD_INCREASE:
                if (mCurrentChangingColumn <= mColumnNum)
                {
                    if (mCurrentStrings[mCurrentChangingColumn] > _getDigitAtIndex(mCurrentChangingColumn))
                    {
                        --mCurrentStrings[mCurrentChangingColumn];
                        mMovementDirection = MD_DECREASE;
                    } else if(mCurrentStrings[mCurrentChangingColumn] < _getDigitAtIndex(mCurrentChangingColumn))
                    {
                        ++mCurrentStrings[mCurrentChangingColumn];
                        mMovementDirection = MD_INCREASE;
                    }else
                    {
                        --mCurrentChangingColumn;
                    }
                }
                break;
            case MD_DECREASE:
                if (mCurrentChangingColumn >= 0) {
                    if (mCurrentStrings[mCurrentChangingColumn] > _getDigitAtIndex(mCurrentChangingColumn))
                    {
                       --mCurrentStrings[mCurrentChangingColumn];
                        mMovementDirection = MD_DECREASE;
                    } else if(mCurrentStrings[mCurrentChangingColumn] < _getDigitAtIndex(mCurrentChangingColumn))
                    {
                        ++mCurrentStrings[mCurrentChangingColumn];
                        mMovementDirection = MD_INCREASE;
                    }else
                    {
                        ++mCurrentChangingColumn;
                    }
                }
                break;
            default:
                break;
        }
        
        if (mCurrentStrings[mCurrentChangingColumn] == _getDigitAtIndex(mCurrentChangingColumn))
        {
            mMovementStatus = MS_SPEED_DOWN;
            mDeltaTime = 0;
            mDeltaSpeed = mSpeed * mSpeed / (2 * mBorders.size.Y);
        }
		
		if(!mTextQueues.empty())
		{
			_setTextStartingPos();
		}
	}
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_ScoreCounter::update( float elapsedSec, WidgetActionRet& ret )
	{
        switch (mMovementStatus)
        {
            case MS_NORMAL:
                switch (mMovementDirection) {
                    case MD_DECREASE:
                        mTextCenterOffsetFromPos.Y += mSpeed*elapsedSec;
                        if(((getWorldPosition().Y+mTextCenterOffsetFromPos.Y) - mTextQueues[0].mSize.Y*.5f) > mBorders.getBottomRight().Y)
                        {
                            advanceToNextString();
                        }
                        break;
                    case MD_INCREASE:
                        mTextCenterOffsetFromPos.Y -= mSpeed*elapsedSec;
                        if(((getWorldPosition().Y+mTextCenterOffsetFromPos.Y) + mTextQueues[0].mSize.Y*.5f) < mBorders.upper_left.Y)
                        {
                            advanceToNextString();
                        }
                        break;
                    default:
                        break;
                }
                break;
            case MS_SPEED_DOWN:
                {
                    mDeltaTime += elapsedSec;
                    if (mSpeed - mDeltaSpeed * mDeltaTime < 0)
                    {
                        mDeltaTime = 0;
                        mMovementStatus = MS_SPEED_UP_AND_DOWN;
                    }else
                    {
                        int direction = mMovementDirection == MD_DECREASE? 1 : -1;
                        mTextCenterOffsetFromPos.Y = direction *( (mSpeed * mDeltaTime) - (mDeltaTime * mDeltaTime * mDeltaSpeed / 2) - mBorders.size.Y/2);
                    }
                    
                }
                break;
            case MS_SPEED_UP_AND_DOWN:
                mDeltaTime += elapsedSec;
                if (mDeltaTime > mAnimationTime)
                {
                    mDeltaTime = 0;
                    mMovementStatus = MS_NORMAL;
                    advanceToNextString();
                }else
                {
                    int direction = mMovementDirection == MD_DECREASE? 1 : -1;
                    mTextCenterOffsetFromPos.Y = direction * slerp(mBorders.size.Y/2, 0, mDeltaTime/mAnimationTime);
                }
                break;
                
            default:
                break;
        }
        				
		
		if (mFireTouched)
		{
			printf("firing touch event\n");
			
			mFireTouched = false;
			ret.valBool = true;
			return true;
		}
		
		return false;
	}
	

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	// assume sprite batch has been started
	void Widget_ScoreCounter::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		int layer = mLayer;
		
		if (mBGTexture.use_count())
		{			
			Vector2 drawPos = getWorldPosition2D() + mDrawOffset;
			sb->drawQuad(layer, mBGTexture,
								drawPos+mDrawOffset, 
								angle, 
								getSize(), 
								(mFingerID != -1) ? Color(128,128,128) : mBGColorBlend);	
			
			++layer;
		}
		
		if(mFont )
        {
            Color drawColor = (mEnabled) ? mTextDisplayColor : Color(0,0,0,96);
            for (int i = 0; i < mCurrentStrings.size(); ++i) 
            {
                if (i == mCurrentChangingColumn) 
                {
                    sb->drawStringClipped(layer, mFont, mTextQueues[mCurrentStrings[i]].mText, getWorldPosition2D() + mTextCenterOffsetFromPos + Vector2((mTextQueues[mCurrentStrings[i]].mSize.X/2 + mPaddingLeft)* (i + 1) + (mTextQueues[mCurrentStrings[i]].mSize.X/2 + mPaddingRight) * i - getSize().X/2,0), mBorders, angle, mTextScale, drawColor, SpriteBatch::JUSTIFICATION_CENTER);
                }else 
                {
                    sb->drawStringClipped(layer, mFont, mTextQueues[mCurrentStrings[i]].mText, getWorldPosition2D() + Vector2((mTextQueues[mCurrentStrings[i]].mSize.X/2 + mPaddingLeft)* (i + 1) + (mTextQueues[mCurrentStrings[i]].mSize.X/2 + mPaddingRight)* i - getSize().X/2, -(mTextQueues[0]).mSize.Y/2), mBorders, angle, mTextScale, drawColor, SpriteBatch::JUSTIFICATION_CENTER);
                }
                
            }
        }
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::_setTextStartingPos()
	{
		switch (mMovementDirection) 
		{
			case MD_INCREASE:
				//-(mTextQueues[0]).mSize.Y
				mTextCenterOffsetFromPos = (getWorldPosition2D() + Vector2(0, (getSize().Y)*.5f)) - getWorldPosition2D();
				
				break;
				
			case MD_DECREASE:
				
				mTextCenterOffsetFromPos = (getWorldPosition2D() - Vector2(0, (getSize().Y)*.5f)) - getWorldPosition2D();
				
				break;
			default:
				break;
		}
	}
	
	void Widget_ScoreCounter::_updateBorders()
	{
		mBorders.upper_left = getWorldPosition2D() - (getSize()*.5f);
		mBorders.size = getSize();
		mBorders.size -= Vector2(mPaddingLeft+mPaddingRight, mPaddingTop+mPaddingBottom);
		mBorders.upper_left += Vector2(mPaddingLeft, mPaddingTop);
	}
    
    int Widget_ScoreCounter::_getDigitAtIndex(int pos)
    {
        return (mTargetScore/(int)pow(10, (mColumnNum-pos)) % 10);
    }
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_ScoreCounter::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		printf("MovinxBox::acceptfinger( %d )  mFingerId[%d]\n", fingerID, mFingerID);
		
		if (mFingerID == -1)
		{
			printf("accepted.\n");
			mFingerID = fingerID;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::releaseFingerUp( int fingerID, FingerInfo* info )
	{
		printf("MovinxBox::releaseFingerUp( %d )  mFingerId[%d]\n", fingerID, mFingerID);
		
		if (mFingerID == fingerID)
		{
			printf("setting touched flag!\n");
			mFireTouched = true;
			mFingerID = -1;
		}
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_ScoreCounter::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
		printf("MovinxBox::releaseFingerLeft( %d )  mFingerId[%d]\n", fingerID, mFingerID);
		
		if (mFingerID == fingerID)
		{
			printf("released!\n");
			mFingerID = -1;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_ScoreCounter::notifyFingerLost( int fingerID, FingerInfo* info )
	{
		if(mFingerID == fingerID)
		{
			mFingerID = -1;
		}
	}
}


