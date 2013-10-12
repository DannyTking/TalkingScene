#include "Transition_Block.h"

#include <PropertyList.h>

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	Transition_Block::Transition_Block() : Transition()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Transition_Block::~Transition_Block()
	{
	}
	
	void Transition_Block::setup( float duration ) 
	{
		mCurrentTime = 0.0f;
		mPastHalfway = false;
		mPauseTime = 0.0f;
		
		mDuration = duration;
		
		mBlocksX = 10;
		mBlocksY = 15;
		
		//mPos;// = Vector2(ScreenCoord::getScreenResolution().X / 2, ScreenCoord::getScreenResolution().Y / 2);
		mAngle = 0.0f;
		mSize = Vector2(ScreenCoord::getScreenResolution().X / mBlocksX, ScreenCoord::getScreenResolution().Y / mBlocksY);
		 
		mColor = Color(0, 0, 0);
		
		mBlocks = mBlocksX * mBlocksY;
		mBlocksDone = 0;
		for (int i = 0; i < mBlocks; i++) {
			mBlockPresent.push_back( false );
		}
		
	}
	
	void Transition_Block::setup( float duration, const PropertyList& plist ) 
	{
		// Basic setup
		setup( duration );

		Transition::setup( duration, plist );
		
		if (plist.keyExists(tk_blockDimensions))
		{
			
			mBlocksX = plist[tk_blockDimensions].asVector2().X;
			mBlocksY = plist[tk_blockDimensions].asVector2().Y;
			
			mSize = Vector2(ScreenCoord::getScreenResolution().X / mBlocksX, ScreenCoord::getScreenResolution().Y / mBlocksY);
			
			mBlockPresent.clear();
			
			mBlocks = mBlocksX * mBlocksY;
			mBlocksDone = 0;
			for (int i = 0; i < mBlocks; i++) {
				mBlockPresent.push_back( false );
			}
		}
		
		// Transition in/out
		if (plist.keyExists(tk_blockIn))
		{
			mTransitionIn = (plist.getValueForKey(tk_blockIn)->asInt() == 1);
		}
		if (plist.keyExists(tk_blockOut))
		{
			mTransitionOut = (plist.getValueForKey(tk_blockOut)->asInt() == 1);
		}
	}
	
	int Transition_Block::update( float elapsedSec ) 
	{
		mCurrentTime += elapsedSec;
		
		// Determine the state
		
		// Below half, return intro
		if (mCurrentTime < mDuration / 2.0f) 
		{
			
			int numDone = 0;
			int todo = Walaber::lerp(0, mBlocks, mCurrentTime / (mDuration / 2.0f));
			todo -= mBlocksDone;
			
			if (todo > 0) 
			{
				mBlocksDone += todo;
			}
			
			while (numDone < todo)
			{
				int randIndex = rand() % mBlocks;
				
				if ( mBlockPresent[randIndex] == false ) 
				{
					mBlockPresent[randIndex] = true;
					numDone++;
				}
			}
			
			return TP_Intro;
		}
		// If we're halfway, return flush
		else if (!mPastHalfway)
		{
			
			for (int i = 0; i < mBlocks; i++) {
				if (mBlockPresent[i] == false) {
					mBlockPresent[i] = true;
				}
			}
			
			mBlocksDone = 0;
			
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
			
			int numDone = 0;
			int todo = Walaber::lerp(0, mBlocks, (mCurrentTime - mPauseTime - mDuration / 2.0f) / (mDuration / 2.0f));
			todo -= mBlocksDone;
			
			if (todo > 0) 
			{
				mBlocksDone += todo;
			}
			
			while (numDone < todo)
			{
				int randIndex = rand() % mBlocks;
				
				if ( mBlockPresent[randIndex] ) 
				{
					mBlockPresent[randIndex] = false;
					numDone++;
				}
			}
			
			return TP_Outro;
		}
		// Otherwise, we're done
		else 
		{
			
			for (int i = 0; i < mBlocks; i++) {
				if (mBlockPresent[i]) {
					mBlockPresent[i] = false;
				}
			}
			
			return TP_Finished;
		}
	}
	
	void Transition_Block::draw()
	{	
		GraphicsGL::setAlphaBlending(true);
		
		mSpriteBatch.start( SpriteBatch::BM_LAYERS );
		
		int screenWidth = ScreenCoord::getScreenResolution().X;
		int screenHeight = ScreenCoord::getScreenResolution().Y;
		
		// draw UI
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);

		GraphicsGL::createOrthoProjection( 0, screenWidth, screenHeight, 0, 0.1f, 1.0f );

		for (int i = 0; i < mBlocks; i++) {
			if (mBlockPresent[i]) {
				mPos = ScreenCoord::toScreen(Vector2( (1.0f / (float)mBlocksX) * (i % mBlocksX) + (1.0f / (float)mBlocksX / 2.0f), 
													  (1.0f / (float)mBlocksY) * (i / mBlocksX) + (1.0f / (float)mBlocksY / 2.0f)));
				mSpriteBatch.drawQuadColored(mPos, mAngle, mSize, mColor);
			}
		}

		_drawFont();
		
		mSpriteBatch.flush();
	}
}
