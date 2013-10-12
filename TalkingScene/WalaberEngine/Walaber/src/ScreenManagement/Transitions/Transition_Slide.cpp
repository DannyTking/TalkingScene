#include "Transition_Slide.h"
#include "GraphicsGL.h"

#include "ScreenManager.h"
#include "TextureManager.h"
#include "QuadHelper.h"
#include "FontManager.h"
#include "MemberCallback.h"
#include "Logger.h"

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	Transition_Slide::Transition_Slide() : Transition(), 
	mDirection(TD_Up), 
	mIgnoreDraw(false)
	{
		_setupRenderTextureBuffer();
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	Transition_Slide::~Transition_Slide()
	{
		GL_DELETE_FRAMEBUFFERS(1, &mRenderTextureFrameBuffer);
		glDeleteTextures(1, &mRenderTextureTexture);
	}
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::setup( float duration ) 
	{
		mCurrentTime = 0.0f;
		mDuration = duration;
		mFlushCounter = 0;
		
		mMaskTexSize = ScreenCoord::getScreenResolution();
		mMaskColor = Walaber::Color::Black;
		
		mTextColor = Color(0,0,0,0);
		
		_goIntro();
	}
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::setup( float duration, const PropertyList& plist ) 
	{	

		// standard transition setup
		Transition::setup( duration, plist );


		// Defaults
		mMaskTexSize = ScreenCoord::getScreenResolution();
		mMaskColor = Walaber::Color::Black;
		mTextColor = Color(0,0,0,0);
		
		// load mask texture
		if (plist.keyExists(tk_slideMaskTexture))
		{
			mMaskTex = TextureManager::getManager().getTexture( plist[tk_slideMaskTexture].asString() );
		}
		
		if (plist.keyExists(tk_slideMaskSize))
		{
			mMaskTexSize = plist[tk_slideMaskSize].asVector2();
		}
		
		if (plist.keyExists(Transition::tk_color))
		{
			mMaskColor = plist[Transition::tk_color].asColor();
		}
		
		if (plist.keyExists(tk_slideDirection))
		{
			mDirection = (TransitionDirection)plist[tk_slideDirection].asInt();
		}
		
		
		// Transition in/out
		if (plist.keyExists(tk_slideIn))
		{
			mTransitionIn = (plist.getValueForKey(tk_slideIn)->asInt() == 1);
		}
		if (plist.keyExists(tk_slideOut))
		{
			mTransitionOut = (plist.getValueForKey(tk_slideOut)->asInt() == 1);
		}
		
		
		// Basic setup
		mCurrentTime = 0.0f;
		mDuration = duration;
		mFlushCounter = 0;
		
		_goIntro();
	}
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	int Transition_Slide::update( float elapsedSec ) 
	{
		mCurrentTime += elapsedSec;
	
		printf("elapsed: %f  current_time: %f\n", elapsedSec, mCurrentTime);
		
		float mid = mDuration * 0.5f;
		
		if (mCurrentTime < mid)
		{
			// intro phase.
			if (mCurrentTime > (mid * 0.5f))
			{
				mTextColor = Color(0,0,0,0).lerp(mFinalTextColor, (mCurrentTime - (mid * 0.5f)) / (mDuration / 4.0f));
			}
			
			return Transition::TP_Intro;
		}
		else
		{
			if (mFlushCounter == 0)
			{
				mFlushCounter++;
				
				return Transition::TP_Flush;
			}
			else if (mFlushCounter == 1)
			{
				mFlushCounter++;
				
				_goOutro();
				
				return Transition::TP_Outro;
			}
			else if (mCurrentTime >= mDuration)
			{
				// done!
				return Transition::TP_Finished;
			}
			else
			{
				// outro phase
				mTextColor = mFinalTextColor.lerp(Color(0,0,0,0), (mCurrentTime - mDuration / 2.0f) / (mDuration / 4.0f));

				return Transition::TP_Outro;
			}
		}
		
	}
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::draw()
	{	
		if (mIgnoreDraw)
			return;
		
		float mid = mDuration * 0.5f;
		
		float interp = (mCurrentTime / mid);
		
		if ((mCurrentTime >= mid) && (mFlushCounter > 1))
		{
			interp = ((mCurrentTime - mid) / mid);
		}
		
		Vector2 center = mStartPos_FB.slerp( mEndPos_FB, interp );
		Vector2 size = ScreenCoord::getScreenResolution();

		Vector2 maskCenter = mStartPos_Mask.slerp( mEndPos_Mask, interp );
		
		int screenWidth = ScreenCoord::getScreenResolution().X;
		int screenHeight = ScreenCoord::getScreenResolution().Y;
		
		// draw UI
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);
		GraphicsGL::loadIdentityMatrix();
		GraphicsGL::createOrthoProjection( 0, screenWidth, screenHeight, 0, 0.1f, 1.0f );
		
		
		Rect uvs;
		uvs.upper_left = Vector2(0,0);
		uvs.size = Vector2( mFb_max_u, mFb_max_v );
		
		QuadHelper::drawQuad(mRenderTextureTexture, center, 0.0f, size, uvs);
		
		
		if (mMaskTex)
		{
			// mask texture
			QuadHelper::drawQuad( mMaskTex->getName(), maskCenter, 0.0f, mMaskTexSize * Vector2(1,-1), mMaskTex->getTextureRect(), Color::White);
		}
		else
		{
			// mask color
			QuadHelper::drawQuadColored(maskCenter, 0.0f, mMaskTexSize, mMaskColor);
		}
		
		if (mFont)
		{
			mSpriteBatch.start( SpriteBatch::BM_LAYERS );
			
			_drawFont();
			
			mSpriteBatch.flush();
		}
	}
	
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::_goIntro()
	{
		if ((mDirection == TD_Up) || (mDirection == TD_Down))
		{
			float yOffsetSize = (ScreenCoord::getScreenResolution().Y * 0.5f) + (mMaskTexSize.Y * 0.5f);
			float dirScalar = (mDirection == TD_Up) ? 1.0f : -1.0f;
			
			mStartPos_FB = ScreenCoord::getScreenResolution() * 0.5f;
			mStartPos_Mask = mStartPos_FB + Vector2(0.0f, yOffsetSize * dirScalar);
			
			mEndPos_Mask = ScreenCoord::getScreenResolution() * 0.5f;
			mEndPos_FB = mEndPos_Mask + Vector2(0.0f, -yOffsetSize * dirScalar);
		}
		else if ((mDirection == TD_Left) || (mDirection == TD_Right))
		{
			float xOffsetSize = (ScreenCoord::getScreenResolution().X * 0.5f) + (mMaskTexSize.X * 0.5f);
			float dirScalar = (mDirection == TD_Left) ? 1.0f : -1.0f;
			
			mStartPos_FB = ScreenCoord::getScreenResolution() * 0.5f;
			mStartPos_Mask = mStartPos_FB + Vector2(xOffsetSize * dirScalar, 0.0f);
			
			mEndPos_Mask = ScreenCoord::getScreenResolution() * 0.5f;
			mEndPos_FB = mEndPos_Mask + Vector2(-xOffsetSize * dirScalar, 0.0f);
		}
		
		_renderGameScreenToTexture();
	}
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::_goOutro()
	{
		if ((mDirection == TD_Up) || (mDirection == TD_Down))
		{
			float yOffsetSize = (ScreenCoord::getScreenResolution().Y * 0.5f) + (mMaskTexSize.Y * 0.5f);
			float dirScalar = (mDirection == TD_Up) ? 1.0f : -1.0f;
			
			mStartPos_Mask = ScreenCoord::getScreenResolution() * 0.5f;
			mStartPos_FB = mStartPos_Mask + Vector2(0.0f, yOffsetSize * dirScalar);
			
			mEndPos_FB = ScreenCoord::getScreenResolution() * 0.5f;
			mEndPos_Mask = mEndPos_FB + Vector2(0.0f, -yOffsetSize * dirScalar);
		}
		else if ((mDirection == TD_Left) || (mDirection == TD_Right))
		{
			float xOffsetSize = (ScreenCoord::getScreenResolution().X * 0.5f) + (mMaskTexSize.X * 0.5f);
			float dirScalar = (mDirection == TD_Left) ? 1.0f : -1.0f;
			
			mStartPos_Mask = ScreenCoord::getScreenResolution() * 0.5f;
			mStartPos_FB = mStartPos_Mask + Vector2(xOffsetSize * dirScalar, 0.0f);
			
			mEndPos_FB = ScreenCoord::getScreenResolution() * 0.5f;
			mEndPos_Mask = mEndPos_FB + Vector2(-xOffsetSize * dirScalar, 0.0f);
		}
		
		_renderGameScreenToTexture();
	}
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::_setupRenderTextureBuffer()
    {
		GLint mainFrameBuffer = GraphicsGL::getBoundFramebuffer();
		
        // FRAMEBUFFER - this is the buffer for capturing the screen to "slide" off / on
		mFb_width = (int)ScreenCoord::getScreenResolution().X;
		mFb_height = (int)ScreenCoord::getScreenResolution().Y;
		
		mFb_width *= PlatformManager::getInstancePtr()->getDisplayScale();
		mFb_height *= PlatformManager::getInstancePtr()->getDisplayScale();
		
		mFb_max_u = (float)mFb_width / (float)Walaber::nextPOT(mFb_width);
		mFb_max_v = (float)mFb_height / (float)Walaber::nextPOT(mFb_height);
		
		GL_GEN_FRAMEBUFFERS(1, &mRenderTextureFrameBuffer);
		Walaber::GraphicsGL::bindFramebuffer( mRenderTextureFrameBuffer);
		
		
		glGenTextures(1, &mRenderTextureTexture);
		glBindTexture(GL_TEXTURE_2D, mRenderTextureTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  Walaber::nextPOT(mFb_width), Walaber::nextPOT(mFb_height), 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, NULL);
		GL_FRAMEBUFFER_TEXTURE2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTextureTexture, 0);
		
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
		GLenum status = GL_CHECK_FRAMEBUFFER_STATUS(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE) 
		{
			Walaber::Logger::printf("WMW", Walaber::Logger::SV_FATAL, "failed to make complete framebuffer object %x", status);
		}
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		
		// rebind original fre
		Walaber::GraphicsGL::bindFramebuffer( mainFrameBuffer);
		
    }
	
	// --------========--------========---- ----========--------========--------========--------========--------========
	void Transition_Slide::_renderGameScreenToTexture()
    {
		GLint mainFrameBuffer = GraphicsGL::getBoundFramebuffer();
		
		
		// this will draw the current game screen stack into our render texture.
		mIgnoreDraw = true;
		
		
		// bind our framebuffer and render into it!
		Walaber::GraphicsGL::bindFramebuffer( mRenderTextureFrameBuffer);
		GraphicsGL::setViewport(0, 0, mFb_width, mFb_height);
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		GraphicsGL::setAlphaBlending( true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
		GraphicsGL::setMatrixMode(GL_MODELVIEW);
		GraphicsGL::loadIdentityMatrix();
		GraphicsGL::setMatrixMode(GL_PROJECTION);
		GraphicsGL::loadIdentityMatrix();
		
		// DRAW!
		ScreenManager::draw(ScreenManager::DM_Screens);
		
		
		// restore frame buffer
		Walaber::GraphicsGL::bindFramebuffer( mainFrameBuffer);
		
		// turn our drawing back on.
		mIgnoreDraw = false;
	}

	
}
