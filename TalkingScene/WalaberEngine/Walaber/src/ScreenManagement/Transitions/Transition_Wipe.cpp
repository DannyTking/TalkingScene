#include "Transition_Wipe.h"

#include <PropertyList.h>

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	Transition_Wipe::Transition_Wipe() : Transition()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Transition_Wipe::~Transition_Wipe()
	{
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Wipe::setup( float duration ) 
	{
		mCurrentTime = 0.0f;
		mPastHalfway = false;
		mPauseTime = 0.0f;
		
		mDuration = duration;
		
		//mPos;// = Vector2(ScreenCoord::getScreenResolution().X / 2, ScreenCoord::getScreenResolution().Y / 2);
		mAngle = 0.0f;
		mSize = Vector2(ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y);
		mColor = Color(0, 0, 0);
		
		mPosStr = ScreenCoord::toScreen(Vector2(0.5f, 1.5f));
		mPosMid = ScreenCoord::toScreen(Vector2(0.5f, 0.5f));
		mPosEnd = ScreenCoord::toScreen(Vector2(0.5f, -1.5f));
		mPos = mPosStr;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Wipe::setup( float duration, const PropertyList& plist ) 
	{
		// Basic setup
		setup( duration );
		
		// standard transition setup
		Transition::setup( duration, plist );

		
		// Direction of the wipe
		if (plist.keyExists(tk_wipeDirection)) 
		{
			
			int direction = plist[tk_wipeDirection].asInt();
			
			if ( direction == TD_Left ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(1.5f, 0.5f));
				mPosEnd = ScreenCoord::toScreen(Vector2(-0.5f, 0.5f));
			}
			else if ( direction == TD_Right ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(-0.5f, 0.5f));
				mPosEnd = ScreenCoord::toScreen(Vector2(1.5f, 0.5f));
			}
			else if ( direction == TD_Up ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(0.5f, 1.5f));
				mPosEnd = ScreenCoord::toScreen(Vector2(0.5f, -0.5f));
			}
			else if ( direction == TD_Down ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(0.5f, -0.5f));
				mPosEnd = ScreenCoord::toScreen(Vector2(0.5f, 1.5f));
			}
			
			mPos = mPosStr;
		}
		
		// Even more specific direction
		if (plist.keyExists(tk_wipeDirectionIn))
		{
			
			int directionIn = plist[tk_wipeDirectionIn].asInt();
			
			if ( directionIn == TD_Left ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(1.5f, 0.5f));
			}
			else if ( directionIn == TD_Right ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(-0.5f, 0.5f));
			}
			else if ( directionIn == TD_Up ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(0.5f, 1.5f));
			}
			else if ( directionIn == TD_Down ) 
			{
				mPosStr = ScreenCoord::toScreen(Vector2(0.5f, -0.5f));
			}
			
			mPos = mPosStr;
		}
		
		if (plist.keyExists(tk_wipeDirectionOut))
		{
			
			int directionOut = plist[tk_wipeDirectionOut].asInt();
			
			if ( directionOut == TD_Left ) 
			{
				mPosEnd = ScreenCoord::toScreen(Vector2(-0.5f, 0.5f));
			}
			else if ( directionOut == TD_Right ) 
			{
				mPosEnd = ScreenCoord::toScreen(Vector2(1.5f, 0.5f));
			}
			else if ( directionOut == TD_Up ) 
			{
				mPosEnd = ScreenCoord::toScreen(Vector2(0.5f, -0.5f));
			}
			else if ( directionOut == TD_Down ) 
			{
				mPosEnd = ScreenCoord::toScreen(Vector2(0.5f, 1.5f));
			}
			
			mPos = mPosStr;
		}
		
		
		// Transition in/out
		if (plist.keyExists(tk_wipeIn))
		{
			mTransitionIn = (plist.getValueForKey(tk_wipeIn)->asInt() == 1);
		}
		if (plist.keyExists(tk_widgetSlideOut))
		{
			mTransitionOut = (plist.getValueForKey(tk_wipeOut)->asInt() == 1);
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	int Transition_Wipe::update( float elapsedSec ) 
	{
		mCurrentTime += elapsedSec;
		
		// Determine the state
		
		// Below half, return intro
		if (mCurrentTime < mDuration / 2.0f) 
		{
			mPos = mPosStr.slerp(mPosMid, mCurrentTime / (mDuration / 2.0f));
			
			return TP_Intro;
		}
		// If we're halfway, return flush
		else if (!mPastHalfway)
		{
			mPos = mPosMid;
			
			if (mIsExtraFlushFrame)
			{
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
			
			if (mCurrentTime >= (mDuration / 2.0f) + mPauseTime)
			{
				mPos = mPosMid.slerp(mPosEnd, (mCurrentTime - mPauseTime - mDuration / 2.0f) / (mDuration / 2.0f));
			}
			
			return TP_Outro;
		}
		// Otherwise, we're done
		else 
		{
			mPos = mPosEnd;
			return TP_Finished;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Wipe::draw()
	{	
		mSpriteBatch.start( SpriteBatch::BM_LAYERS );
		
		int screenWidth = ScreenCoord::getScreenResolution().X;
		int screenHeight = ScreenCoord::getScreenResolution().Y;
		
		// draw UI
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);
		
		GraphicsGL::createOrthoProjection( 0, screenWidth, screenHeight, 0, 0.1f, 1.0f );
        
		mSpriteBatch.drawQuadColored(mPos, mAngle, mSize, mColor);
		
		_drawFont();
		
		mSpriteBatch.flush();
	}
}
