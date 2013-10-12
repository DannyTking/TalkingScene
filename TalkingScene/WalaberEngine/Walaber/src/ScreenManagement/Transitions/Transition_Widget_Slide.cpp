#include "Transition_Widget_Slide.h"

#include <PropertyList.h>

namespace Walaber
{
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	Transition_Widget_Slide::Transition_Widget_Slide() : Transition()
	{
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	Transition_Widget_Slide::~Transition_Widget_Slide()
	{
	}
		
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::setup( float duration ) 
	{
		Transition::setup(duration);
		
		mCurrentTime = 0.0f;
		mPastHalfway = false;
		mPauseTime = 0.0f;
		
		mDuration = duration;
		
		mHighestWidgetIndex = -1;
		mNumScreensInStack = -1;
		mOffsetNewScreen = false;
		
		mDirectionIn = Vector2( 0.0f, 1.0f );
		mDirectionOut = Vector2( 0.0f, -1.0f );
		
		//_setPositionsByDirection();
		
		mIsIntro = true;
		mDoStagger = true;
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::setup( float duration, const PropertyList& plist ) 
	{
		// Basic setup
		setup( duration );
		
		// standard transition setup
		Transition::setup( duration, plist );

		// The highest widget index (we only change widgets up to this point)
		if (plist.keyExists(tk_highestWidgetIndex)) 
		{
			mHighestWidgetIndex = plist[tk_highestWidgetIndex].asInt();
		}
		
		// The number of screens (from the top) to manipulate
		if (plist.keyExists(tk_numScreensInStack))
		{
			mNumScreensInStack = plist[tk_numScreensInStack].asInt();
		}
		
		// Direction of animation
		if (plist.keyExists(tk_slideDirection)) 
		{
			mDirectionIn = plist[tk_slideDirection].asVector2();
			mDirectionOut = mDirectionIn * -1.0f;
		}
		
		if (plist.keyExists(tk_slideDirectionIn)) 
		{
			mDirectionIn = plist[tk_slideDirectionIn].asVector2();
		}
		
		if (plist.keyExists(tk_slideDirectionOut)) 
		{
			mDirectionOut = plist[tk_slideDirectionOut].asVector2();
		}
		
		
		// Should we stagger the slide in?
		if (plist.keyExists(tk_slideStagger)) 
		{
			mDoStagger = (plist[tk_slideStagger].asInt() == 1);
		}
		
		
		
		// Transition in/out
		if (plist.keyExists(tk_widgetSlideIn))
		{
			mTransitionIn = (plist.getValueForKey(tk_widgetSlideIn)->asInt() == 1);
		}
		if (plist.keyExists(tk_widgetSlideOut))
		{
			mTransitionOut = (plist.getValueForKey(tk_widgetSlideOut)->asInt() == 1);
		}
		
		
		// Repopulate the widget managers (in this case for the first time)
		if (mTransitionIn)
		{
			_populateWidgetManagerMap();
		}
		
		mPos = mPosStr;
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	int Transition_Widget_Slide::update( float elapsedSec ) 
	{
		mCurrentTime += elapsedSec;
		
		
		// Determine the state
		
		// Below half, return intro
		if (mCurrentTime < mDuration / 2.0f) 
		{
			if (mTransitionIn)
			{
				// Move the AABB down, where anything inside starts animating
				Vector2 mOldPos = mPos;
				mPos = mPosStr.slerp(mPosMid, getInterpVal( mCurrentTime, 0, mDuration / 4.0f ));
				
				mTransBox.Min += mPos - mOldPos;
				mTransBox.Max += mPos - mOldPos;
				
				// Change text color
				mTextColor = mStartTextColor.lerp(mFinalTextColor, getInterpVal( mCurrentTime, mDuration / 4.0f, mDuration / 2.0f ));
				
				_findWidgetsInAABB();
				_updateWidgetManagers(elapsedSec);
			}
			
			return TP_Intro;
		}
		// If we're halfway, return flush
		else if (!mPastHalfway)
		{
			Vector2 mOldPos = mPos;
			mPos = mPosMid;
			
			mTransBox.Min += mPos - mOldPos;
			mTransBox.Max += mPos - mOldPos;
			
			_findWidgetsInAABB();
			_updateWidgetManagers(elapsedSec);
			
			if (mIsExtraFlushFrame)
			{
				mIsIntro = false;
				mPastHalfway = true;
				return TP_Flush;
			}
			else
			{
				mIsExtraFlushFrame = true;
				return TP_Intro;
			}
		}
		// Below duration, return outro
		else if (mCurrentTime < mDuration + mPauseTime) 
		{
			if (mTransitionOut)
			{
				// Offset the screen so it'll animate from offscreen
				if (!mOffsetNewScreen)
				{
					// Repopulate the widget managers
					_populateWidgetManagerMap();
					
					// Move AABB if there's no stagger
					if (!mDoStagger)
					{
						Vector2 halfSize = ScreenCoord(0.5f, 0.5f).toScreen();
						mTransBox = AABB( mPosEnd, mPosEnd + halfSize * 2 );
					}
					
					
					// And now offset all of the widgets
					Vector2 diff = mPosEnd - mPosMid;
					_offsetWidgetManagers(diff);
					mOffsetNewScreen = true;
				}
				
				
				// Move the AABB down, where anything inside starts animating
				float adjTime = mCurrentTime - (mDuration / 2.0f) - mPauseTime;
				
				Vector2 mOldPos = mPos;
				mPos = mPosMid.slerp(mPosEnd, getInterpVal( adjTime, 0, mDuration / 4.0f ));
				
				mTransBox.Min += mPos - mOldPos;
				mTransBox.Max += mPos - mOldPos;
				
				mTextColor = mFinalTextColor.lerp(mStartTextColor, getInterpVal( adjTime, 0, mDuration / 4.0f ));
				
				
				_findWidgetsInAABB();
				_updateWidgetManagers(elapsedSec);
			}
			
			return TP_Outro;
		}
		// Otherwise, we're done
		else 
		{
			if (mTransitionOut)
			{
				Vector2 mOldPos = mPos;
				mPos = mPosEnd;
				
				mTransBox.Min += mPos - mOldPos;
				mTransBox.Max += mPos - mOldPos;
				
				_findWidgetsInAABB();
				_updateWidgetManagers(elapsedSec);
			}
			
			return TP_Finished;
		}
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::_setPositionsByDirection( AABB extents )
	{
		// Make sure to include the extents with teh positions
		Vector2 size = ScreenCoord(1.0f, 1.0f).toScreen();
		if (extents.Valid)
		{
			extents.expandToInclude( AABB( Vector2::Zero, size ) );
			size = extents.getSize();
		}
		
		mTransBox = extents;
		
		Vector2 center = extents.getCenter();
		
		// Start and end positions
		mPosStr = center + size * mDirectionIn;
		mPosEnd = center + size * mDirectionOut;
		mPosMid = center;
		
		if (mIsIntro)
		{
			mPos = mPosStr;
		}
		else
		{
			mPos = mPosMid;
		}
			
		mTransBox.Min = mPos + size * -0.5f;
		mTransBox.Max = mPos + size * 0.5f;
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::_populateWidgetManagerMap()
	{
		mWidgetsToTimers.clear();
		
		// Setup all of the widget timers
		AABB extents;
		
		int minScreenIndex = clampi( ScreenManager::getScreenCount() - 1 - mNumScreensInStack, 0, ScreenManager::getScreenCount() - 1);
		
		for (int i = ScreenManager::getScreenCount() - 1; i >= minScreenIndex; i--)
		{
			WidgetManager* mWidgetMgr = ScreenManager::getScreenAt(ScreenManager::getScreenCount() - 1)->getWidgetManager();
			
			if (mWidgetMgr)
			{
				int highestIndex = maxi( mWidgetMgr->getHighestWidgetIndex(), mHighestWidgetIndex );
				
				for (int j = 0; j <= highestIndex; j++)
				{
					Widget* tempWidget = mWidgetMgr->getWidget(j);
					if (tempWidget)
					{
						if (tempWidget->getWidgetType() != WT_GROUP)
						{
							mWidgetsToTimers[tempWidget] = -1.0f;
							
							extents.expandToInclude(tempWidget->getAABB());
						}
					}
				}
			}
		}
		
		// Update the start/end positions, so that we're sure to catch all of the widgets
		_setPositionsByDirection( extents );
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::_findWidgetsInAABB()
	{
		// Move all of the widgets in the WidgetMgr this far
		for (std::map< Widget*, float >::iterator it = mWidgetsToTimers.begin(); it != mWidgetsToTimers.end(); ++it)
		{
			// Not set
			if (it->second == -1.0f)
			{
				if (mTransBox.contains( it->first->getWorldPosition2D() ))
				{
					it->second = mDuration / 4.0f;
					
					if (!mDoStagger)
					{
						it->second *= 2.0f;
					}
				}
			}
		}
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::_updateWidgetManagers(const float elapsedSec)
	{
		// Move all of the widgets in the WidgetMgr this far
		for (std::map< Widget*, float >::iterator it = mWidgetsToTimers.begin(); it != mWidgetsToTimers.end(); ++it)
		{
			// Not set
			if (it->second > 0.0f)
			{
				float duration = mDuration / 4.0f;
				if (!mDoStagger)
				{
					duration *= 2.0f;
				}
				
				Vector2 oldPos;
				if (mIsIntro)
					oldPos = mPosStr.slerp(mPosMid, getInterpVal( it->second, 0.0f, duration ));
				else
					oldPos = mPosMid.slerp(mPosEnd, getInterpVal( it->second, 0.0f, duration ));
				
				it->second -= elapsedSec;
				
				if (it->second < 0.0f)
				{
					it->second = 0.0f;
				}
				
				Vector2 newPos;
				if (mIsIntro)
					newPos = mPosStr.slerp(mPosMid, getInterpVal( it->second, 0.0f, duration ));
				else
					newPos = mPosMid.slerp(mPosEnd, getInterpVal( it->second, 0.0f, duration ));
				
				
				it->first->applyPositionOffset2D(newPos - oldPos);
			}
		}
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::_offsetWidgetManagers(const Vector2& offset)
	{
		// Move all of the widgets in the WidgetMgr this far
		for (std::map< Widget*, float >::iterator it = mWidgetsToTimers.begin(); it != mWidgetsToTimers.end(); ++it)
		{
			// Update
			if (it->first)
			{
				it->first->applyPositionOffset2D(offset);
			}
		}
	}
	
	
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	// ~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~/\~~~~~
	void Transition_Widget_Slide::draw()
	{	
		// Draw the debug position of the current pos
		GraphicsGL::setAlphaBlending(true);
		
		mSpriteBatch.start( SpriteBatch::BM_LAYERS );
		
		int screenWidth = ScreenCoord::getScreenResolution().X;
		int screenHeight = ScreenCoord::getScreenResolution().Y;
		
		// draw UI
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);
		
		GraphicsGL::createOrthoProjection( 0, screenWidth, screenHeight, 0, 0.1f, 1.0f );
		
#ifdef DEBUG
		Vector2 size = mTransBox.Max - mTransBox.Min;
		
		//mSpriteBatch.drawQuadColored(ScreenCoord( 0.5f, 0.5f ).toScreen(), 0.0, ScreenCoord( 1.0f, 1.0f ).toScreen(), Color(0, 0, 255, 10));
		mSpriteBatch.drawQuadColored(mTransBox.Min + size / 2.0f, 0.0, size, Color(255, 0, 0, 10));
#endif
		
		_drawFont();        
		
		mSpriteBatch.flush();
	}
}
