#include "Transition_Fade.h"

#include <PropertyList.h>

#include "StringHelper.h"

namespace Walaber
{
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	Transition_Fade::Transition_Fade() : Transition()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Transition_Fade::~Transition_Fade()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Fade::setup( float duration ) 
	{
		mCurrentTime = 0.0f;
		mPastHalfway = false;
		mPauseTime = 0.0f;
		mIsExtraFlushFrame = false;
		
		mDuration = duration;
		
		//mPos;// = Vector2(ScreenCoord::getScreenResolution().X / 2, ScreenCoord::getScreenResolution().Y / 2);
		mAngle = 0.0f;
		mSize = Vector2(ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y);
		mPos = ScreenCoord::toScreen(Vector2(0.5f, 0.5f));
		
		mColorStr = Color(0, 0, 0, 0);
		mColorEnd = Color(0, 0, 0, 255);
		
		mColor = mColorStr;
		mTextColor = mColorStr;
		
		mFont = NULL;
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Fade::setup( float duration, const PropertyList& plist ) 
	{
		// Basic setup
		setup( duration );

		// standard transition setup
		Transition::setup( duration, plist );


		mColorStr = Color(0, 0, 0, 0);
		mColorEnd = Color(mColor.R, mColor.G, mColor.B, 255);
		 
		
		
		// Transition in/out
		if (plist.keyExists(tk_fadeIn))
		{
			mTransitionIn = (plist.getValueForKey(tk_fadeIn)->asInt() == 1);
		}
		if (plist.keyExists(tk_fadeOut))
		{
			mTransitionOut = (plist.getValueForKey(tk_fadeOut)->asInt() == 1);
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	int Transition_Fade::update( float elapsedSec ) 
	{
		mCurrentTime += elapsedSec;
		
		// Determine the state
		
		// Below half, return intro
		if (mCurrentTime < mDuration / 2.0f) 
		{
			mColor = mColorStr.lerp(mColorEnd, mCurrentTime / (mDuration / 2.0f));
			mTextColor = mColorStr.lerp(mFinalTextColor, mCurrentTime / (mDuration / 2.0f));
			
			//mColor = Color(mColor.R * ((int)mColor.A / 255.0f), mColor.G * ((int)mColor.A / 255.0f), mColor.B * ((int)mColor.A / 255.0f), (float) mColor.A);
			
			return TP_Intro;
		}
		// If we're halfway, return flush
		else if (!mPastHalfway)
		{
			mColor = mColorEnd;
			mTextColor = mFinalTextColor;
			
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
				mColor = mColorEnd.lerp(mColorStr, (mCurrentTime - mPauseTime - mDuration / 2.0f) / (mDuration / 2.0f));
				mTextColor = mFinalTextColor.lerp(mColorStr, (mCurrentTime - mPauseTime - mDuration / 2.0f) / (mDuration / 2.0f));
			}
			
			return TP_Outro;
		}
		// Otherwise, we're done
		else 
		{
			mColor = mColorStr;
			return TP_Finished;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Fade::draw()
	{	
		GraphicsGL::setAlphaBlending(true);
		
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
