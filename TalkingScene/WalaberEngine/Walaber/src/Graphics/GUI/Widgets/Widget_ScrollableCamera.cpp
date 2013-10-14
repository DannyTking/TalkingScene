#include "Widget_ScrollableCamera.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

#include "Widget_Slider.h"

#define PAGINATION_DOT_SIZE     0.025f
// This is a percentage of the dot size
#define PAGINATION_DOT_PADDING  0.5f

namespace Walaber 
{
    static const float SCROLL_INTENT_THRESHOLD = 30.0f; // Will be converted to % of screen
    
    Widget_ScrollableCamera::Widget_ScrollableCamera( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_SCROLLABLE_CAMERA, pos, size, WO_PortraitUp ),
    mOffset(0.0f),
    mFirstFinger(-1),
    mIndex(0),
    mVel(0),
    mVelDamping(0.6f),
    mFinger(NULL),
    mSize(0),
    mIndexPercentage(0.0f),
    mIndexToSnap(0),
    mCamState(CS_Stable),
    mIsLoopScroll(false),
    mDollyToIndex(0),
    mDollyT(0.0f),
    mDollyDuration(0.0f),
    mLockCameraInteraction(false),
    mUseDampingForSnap(false)
	{
        mScrollIntentThreshold = (SCROLL_INTENT_THRESHOLD * ScreenCoord::getScreenResolution().X) / 768.0f;
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- --       
    Widget_ScrollableCamera::Widget_ScrollableCamera( int name, Vector2 pos, Vector2 size, Camera* cam ):
    Widget( name, WT_SCROLLABLE_CAMERA, pos, size, WO_PortraitUp),
    mCamera(cam),
    mOffset(0.0f),
    mFirstFinger(-1),
    mIndex(0),
    mVel(0),
    mVelDamping(0.6f),
    mFinger(NULL),
    mSize(0),
    mIndexPercentage(0.0f),
    mIndexToSnap(0),
    mCamState(CS_Stable),
    mIsLoopScroll(false),
    mDollyToIndex(0),
    mDollyT(0.0f),
    mDollyDuration(0.0f),
    mLockCameraInteraction(false),
    mUseDampingForSnap(false)
    {
        mScrollIntentThreshold = (SCROLL_INTENT_THRESHOLD * ScreenCoord::getScreenResolution().X) / 768.0f;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- --       
	Widget_ScrollableCamera::~Widget_ScrollableCamera()
	{
        mWidgets.clear();
        mSlider.clear();
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- --       
    void Widget_ScrollableCamera::initCamera(Camera* cam)
    {
        mCamera = cam;
        tmgr = &TextureManager::getManager();
#ifdef TARGET_NDK
        // PERRY_TO_MICKEY
        // - Should these be /Mickey/Textures/...
        // - Actually, these no longer exist in Mickey?
        mCurrentPageTex = tmgr->getTexture("/Mickey/Textures/btn_pagination_on.webp");
        mNonCurrentPageTex = tmgr->getTexture("/Mickey/Textures/btn_pagination_off.webp");
#else // #ifdef TARGET_NDK
        mCurrentPageTex = tmgr->getTexture("/Mickey/Textures/btn_pagination_on.webp");
        mNonCurrentPageTex = tmgr->getTexture("/Mickey/Textures/btn_pagination_off.webp");
#endif // #else // #ifdef TARGET_NDK
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
    bool Widget_ScrollableCamera::update( float elapsedSec, WidgetActionRet& ret )
    {
        bool moved = false;
        
        //Here is the logic to move the camera accordingly.
        switch (mCamState)
        {
            case CS_Stable:
            {
                break;
            }
            case CS_Moving:
            {
                //Since we update the mOffset in _updateFinger(), we no longer need to update it here anymore. Just update the velocity.
                mVel = mLastOffsetDelta / elapsedSec;
                break;
            }
            case CS_Snap:
            {
                mVel *= mVelDamping; // this doesn't really do anything because we += move later
                mOffset += (mVel * elapsedSec);
                
                float move = 0;
                //if (mOffset > -1.0f *((Widget_Group*)mWidgets[0])->getLocalPosition().X)
                if (mIndexToSnap == 0 || mIndexToSnap == mSize-1)
                {
                    move = -1 * (((Widget_Group*)mWidgets[mIndexToSnap])->getLocalPosition().X + mOffset);
                    
                    if (abs(move) > 3.0f)
                    {
                        move *= 0.5f;
                    }
                }
                else 
                {
                    //snap to the mIndexToSnap widgets. mIndexToSnap is set when the camera state changed from CS_Moving to CS_Snap.
                    move = (-1*mWidgets[mIndexToSnap]->getLocalPosition().X - mOffset);
                    if (abs(move) > 1.0f) {
                        move *= 0.25f;
                        if (mUseDampingForSnap)
                        {
                            move *= mVelDamping;
                        }
                    }
                }
                
                if (move == 0)
                {
                    //finished snap, update the camera state and the current index.
                    mCamState = CS_Stable;
                    mIndex = mIndexToSnap;
                    moved = true;
                }
                else
                {
                    mOffset += move;
                }
                break;
            }
            case CS_DollyTo:
            {
                mDollyT += elapsedSec;
                if (mDollyT > mDollyDuration)
                {
                    mOffset = -1*mWidgets[mDollyToIndex]->getLocalPosition().X;
                    mCamState = CS_Stable;
                }
                else
                {
                    mOffset += mVel * elapsedSec;
                }
                moved = true;
                break;
            }
            case CS_InstantMoveTo:
                
                mOffset = -1 * mWidgets[mIndexToSnap]->getLocalPosition().X;
                
                mCamState = CS_Stable;
                
                mIndex = mIndexToSnap;
                
                moved = true;
                break;

            default:
                break;
        }
		
        //update the percentage index
        int i = 0;
        for (; i < mSize; i++)
        {
            if (mOffset > -1*mWidgets[i]->getLocalPosition().X)
            {
                break;
            }
        }
        
        if (i == 0)
        {
            mIndexPercentage = -1 * (mOffset + mWidgets[0]->getLocalPosition().X)/ScreenCoord(0.5f, 0).toScreen().X;
        }
        else if (i == mSize)
        {
            mIndexPercentage = mSize - 1 - (mOffset + mWidgets[mSize-1]->getLocalPosition().X)/ScreenCoord(0.5f, 0).toScreen().X;
        }
        else
        {
            mIndexPercentage = i - (mOffset + mWidgets[i]->getLocalPosition().X)/(mWidgets[i]->getLocalPosition().X - mWidgets[i-1]->getLocalPosition().X);
        }
        
        mIndex = (int)(mIndexPercentage + 0.5);
        if (mIndex >= mSize)
		{
			mIndex = mSize - 1;
		}
        
        // if loop scroll features is enabled
        if (mIsLoopScroll)
        {
            /* set the offset back to the real group */
            // when right on fake first pack
            if (mOffset == -1*mWidgets[mSize-2]->getLocalPosition2D().X)
            {
                // change camera offset to be the one for the real first pack
                mOffset = -1*mWidgets[0]->getLocalPosition2D().X;

                // change percentage and index to be the ones for the real first pack
                mIndexPercentage = 0;
                mIndex = 0;
                mIndexToSnap = 0;
            }
            // when scrolling right on real first pack, you'll see the fake last pack
            else if (mOffset > -1*mWidgets[0]->getLocalPosition2D().X)
            {
                // change the offset to be the one for the fake first pack
                mOffset = -1*mWidgets[mSize-2]->getLocalPosition2D().X - (-1*mWidgets[0]->getLocalPosition2D().X - mOffset);
                
                // change percentage and index to be the ones for the fake first pack
                mIndexPercentage += mSize-2;
                mIndex = mSize-2;
                mIndexToSnap = mSize-2;
            }
        }

        // Update the slider
        if (moved)
        {
            _updateSlider();
            moved = false;
        }
        
        // all the logic to move the camera
        if (mCamera && !mLockCameraInteraction)
        {
            mCamera->setPosition(Vector2(-1 * mOffset, mCamera->getPosition().Y));
        }
        
        // update the position of slider widgets with the camera
        for (int i_=0; i_< mSlider.size(); i_++)
        {
            if (mSlider[i_])
            {
                mSlider[i_]->setLocalPosition2D(Vector2(mSliderOrigPos[i_] + -1 * mOffset, mSlider[i_]->getWorldPosition2D().Y));
            }
        }
        
        // update the position of movable widgets- buttons etc
        for (int j=0; j< mMovableWidgets.size(); j++)
        {
            mMovableWidgets[j]->setLocalPosition2D(Vector2(mMovableOrigPosX[j] + mOffset * -1, mMovableWidgets[j]->getWorldPosition().Y));
        }
        
        return false;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- --
    void Widget_ScrollableCamera::draw( SpriteBatch* sb )
    {
        // debug outline
		Widget::draw(sb);//, offset);
        
        for (int i=0; i<mSlider.size(); i++)
        {
            mSlider[i]->draw(sb);
        }
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
    void Widget_ScrollableCamera::addGroup( Widget_Group* wp, bool createSlider )
	{
		// add this widget to the list
		mWidgets.push_back( wp );
        
        if (createSlider)
        {
            // create a fake slider texture and add it to the list
            createFakeSliderIndex(mSize);
        }
		
		mSize++;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
    void Widget_ScrollableCamera::addMovableGroup( Widget_Group* wp )
    {
        mMovableWidgets.push_back( wp );
        
        mMovableOrigPosX.push_back( wp->getWorldPosition().X );
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
    void Widget_ScrollableCamera::setIndex( int newIndex, float duration )
	{
        if (duration == 0)
        {
            // Only make InstantMoveTo if Stable State
            if (mCamState == CS_Stable)
            {
                mIndexToSnap = clampi(newIndex, 0, mSize - 1);
                //mVel = -3000;
                mCamState = CS_InstantMoveTo;
            }
        }
        else
        {
            mDollyToIndex = clampi(newIndex, 0, mSize - 1);
            mCamState = CS_DollyTo;
            mVel = -1 * (mWidgets[mDollyToIndex]->getLocalPosition().X + mOffset) / duration;
            mDollyT = 0.0f;
            mDollyDuration = duration;
        }
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	int Widget_ScrollableCamera::getCurrentIndex()
	{
		return mIndex;
	}
	
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- --
	void Widget_ScrollableCamera::createFakeSliderIndex(int slider)        // create a fake slider
	{
        //Force the sliders to have a square aspect ratio
        Vector2 dotSize = ScreenCoord(PAGINATION_DOT_SIZE, 0.0f).toScreen();
        dotSize.Y = dotSize.X;
        
        Widget_Label* sliderIndex = new Widget_Label( slider + 5000,
                                                     ScreenCoord(-0.054f, 0.96f).toScreen(),
                                                     dotSize,
                                                     mNonCurrentPageTex
                                                  );
        
        sliderIndex->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
        
        Vector2 pos = sliderIndex->getWorldPosition2D();
        float offset = (sliderIndex->getSize().X) * slider * 1.3f;
        pos.X += offset;
        sliderIndex->setAllowCustomize(true);
        sliderIndex->setLocalPosition2D(pos);
        sliderIndex->setLayer(10);
        sliderIndex->setTextDisplayColor( Color::White );
        mWidgetMgr->addWidget( sliderIndex, 0 );
        
        mSlider.push_back(sliderIndex);
        
        mSliderOrigPos.push_back(sliderIndex->getWorldPosition().X);
        
        _updateSlider();
		
	}
    
    void Widget_ScrollableCamera::offsetSlider(Vector2 offset)
    {
        for(int i = 0; i < mSlider.size(); i++)
        {
            Widget_Label* slider = mSlider[i];
            assert(slider != NULL);
            slider->applyPositionOffset2D(offset);
            mSliderOrigPos[i] = slider->getWorldPosition().X;
        }
    }
    
    void Widget_ScrollableCamera::centerSliderAt(Vector2 pos)
    {
        float totalWidth = mSlider[0]->getSize().X * mSlider.size() + (mSlider[0]->getSize().X * PAGINATION_DOT_PADDING * (
                                                                                                                           mSlider.size()-1));
        float widgetWidth = mSlider[0]->getSize().X + (mSlider[0]->getSize().X * PAGINATION_DOT_PADDING);
        Vector2 firstPos = ScreenCoord(pos.X, pos.Y).toScreen();
        firstPos.X -= totalWidth/2.0f;
        
        for(int i = 0; i < mSlider.size(); i++)
        {
            Widget_Label* slider = mSlider[i];
            assert(slider != NULL);
            slider->setLocalPosition2D(firstPos + Vector2(widgetWidth*i,0));
            mSliderOrigPos[i] = slider->getWorldPosition().X;
        }        
    }
    
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
    bool Widget_ScrollableCamera::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
		{
			return false;
		}
        
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::acceptNewFingerDown(%d)\n", fingerID);
        
		// If this is the first finger down, record the index
		if ((mFirstFinger == -1) && (mCamState != CS_DollyTo))
		{
			mFirstFinger = fingerID;
            
            mPosLastFrame = info->curPos;
            mPositionAtDown = info->curPos;
			
			_updateFinger(true);
            mCamState = CS_Moving;
		}
		
		return false;
	}

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	bool Widget_ScrollableCamera::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
		{
			return false;
		}
        
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::acceptNewFingerEntered(%d)\n", fingerID);
        
        float distanceMoved = absf( info->curPos.X - mPositionAtDown.X );
        
        // Don't take this finger unless we know the player intended to scroll.
        if (distanceMoved < mScrollIntentThreshold)
        {
            return false;
        }
        
		// If the finger moving is the one recorded, start doing some crazy stuff
		if ((mFinger == NULL) && (mCamState != CS_DollyTo))
		{
			mFirstFinger = fingerID;
			mFinger = info;
            
			_updateFinger(true);
            mCamState = CS_Moving;
            
            mPosLastFrame = info->curPos;
			
			return true;
		}
		
		return false;
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	bool Widget_ScrollableCamera::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::releaseFingerStayed(%d)\n", fingerID);
		
		mFinger = info;
        //if update Finger here, sometimes the vel will be zero. The movement of camera will feel unsmooth.
		//_updateFinger();
		
		return false;
	}
	

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	bool Widget_ScrollableCamera::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::releaseFingerMoved(%d)\n", fingerID);
        
		mFinger = info;
        if (mCamState != CS_DollyTo)
        {
            _updateFinger();
            mCamState = CS_Moving;
        }
		
		return false;
	}
	

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	void Widget_ScrollableCamera::releaseFingerUp( int fingerID, FingerInfo* info )
	{
        
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::releaseFingerUp(%d)\n", fingerID);
		
		// Release recorded finger
		mFirstFinger = -1;
        //So if the camera is moving, change the state to snap.
        if (mCamState == CS_Moving)
        {
            if (mVel < 0)
            {
                mIndexToSnap = (int)mIndexPercentage + 1;
            }
            else if (mVel > 0)
            {
                mIndexToSnap = (int)mIndexPercentage;
            }
            else
            {
                mIndexToSnap = (int)(mIndexPercentage + 0.5);
            }
            
            mIndexToSnap = clampi(mIndexToSnap, 0, mSize - 1);
            mCamState = CS_Snap;
        }
        
		mFinger = NULL;
	}
	

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	bool Widget_ScrollableCamera::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::releaseFingerLeft(%d)\n", fingerID);
		
		// Release recorded finger
		mFirstFinger = -1;
        //So if the camera is moving, change the state to snap.
        if (mCamState == CS_Moving)
        {
            if (mVel < 0)
            {
                mIndexToSnap = (int)mIndexPercentage + 1;
            }
            else if (mVel > 0)
            {
                mIndexToSnap = (int)mIndexPercentage;
            }
            else
            {
                mIndexToSnap = (int)(mIndexPercentage + 0.5);
            }
            
            mIndexToSnap = clampi(mIndexToSnap, 0, mSize - 1);
            mCamState = CS_Snap;
        }
        
		mFinger = NULL;
		
		return true;
	}
	

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	void Widget_ScrollableCamera::notifyFingerLost( int fingerID, FingerInfo* info )
	{
        Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableCamera::notifyFingerLost(%d)\n", fingerID);
		
		// Release recorded finger
		mFirstFinger = -1;
        //So if the camera is moving, change the state to snap.
        if (mCamState == CS_Moving)
        {
            if (mVel < 0)
            {
                mIndexToSnap = (int)mIndexPercentage + 1;
            }
            else if (mVel > 0)
            {
                mIndexToSnap = (int)mIndexPercentage;
            }
            else
            {
                mIndexToSnap = (int)(mIndexPercentage + 0.5);
            }
            
            mIndexToSnap = clampi(mIndexToSnap, 0, mSize - 1);
            mCamState = CS_Snap;
        }
		mFinger = NULL;
	}

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
	void Widget_ScrollableCamera::_updateFinger(bool firstFrame)
	{
		if (firstFrame)
		{
			mVel = 0.0f;
			mLastOffsetDelta = 0.0f;
		}
		else 
		{
			mLastOffsetDelta =  mFinger->curPos.X - mPosLastFrame.X;
            
            std::cout << "curPos: " << mFinger->curPos.X << ", lastPos: " << mPosLastFrame.X << std::endl;
		}
		
		// scale this smaller as we drift away from the edges...
		float newPos = mOffset + mLastOffsetDelta;
		
		if (newPos >-1.0f *((Widget_Group*)mWidgets[0])->getLocalPosition().X)
		{
			newPos = mOffset + (mLastOffsetDelta * 0.5f);
		}
		else if (newPos < -1.0f *((Widget_Group*)mWidgets[mSize-1])->getLocalPosition().X)
		{
			newPos = mOffset + (mLastOffsetDelta * 0.5f);
		}
		
		mOffset = newPos;
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- --
	void Widget_ScrollableCamera::_updateSlider()
    {
        for (int i=0; i< mSlider.size(); i++)
        {
            if (mSlider[i])
            {
                if (i == mIndex)
                {
                    mSlider[i]->setTexture(Widget_Label::TextureName_BG, mCurrentPageTex);
                }
                else 
                {
                    mSlider[i]->setTexture(Widget_Label::TextureName_BG, mNonCurrentPageTex);
                }
            }
        }
    }

    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --    -- -- -- -- -- --  -- -- -- 
    float Widget_ScrollableCamera::getCurrentIndexPercentage()
    {
        return mIndexPercentage;
    }
}
