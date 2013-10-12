#include "Transition_Iris.h"

#include <PropertyList.h>

#include "FontManager.h"
#include "StringHelper.h"

namespace Walaber
{
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	Transition_Iris::Transition_Iris() : Transition(),
	mBladeCount(8),
	mBladeAngle(0.0f),
	mBladeAngleMax(90.0f),
	mStartRadiusScale(1.8f),
	mEndRadiusScale(1.0f),
	mRadiusScale(1.0f)
	{

		for(unsigned int i = 0; i < 4; i++)
			mCornerColors[i] = Color::White;


		mCornerUVs = Rect( Vector2(0,0), Vector2(1,1) );

	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Transition_Iris::~Transition_Iris()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Iris::setup( float duration ) 
	{
		mCurrentTime = 0.0f;
		mPastHalfway = false;
		mPauseTime = 0.0f;
		mIsExtraFlushFrame = false;
		
		mDuration = duration;

		_initBladeAnchors();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Iris::setup( float duration, const PropertyList& plist ) 
	{
		// Basic setup
		setup( duration );

		// standard transition setup
		Transition::setup( duration, plist );


		const Property* prop = plist.getValueForKey(tk_pauseLength);
		if(prop)
		{
			mPauseTime = prop->asFloat();
		}
		
		
		prop = plist.getValueForKey(tk_irisBladeCount);
		if (prop)
		{
			mBladeCount = prop->asInt();
			_initBladeAnchors();
		}

		prop = plist.getValueForKey(tk_irisTextureName);
		if (prop)
		{
			mBladeTex = TextureManager::getManager().getTexture(prop->asString());
		}

		prop = plist.getValueForKey(tk_irisMaxAngle);
		if (prop)
		{
			mBladeAngleMax = prop->asFloat();
		}

		
		// Transition in/out
		if (plist.keyExists(tk_irisIn))
		{
			mTransitionIn = (plist.getValueForKey(tk_irisIn)->asInt() == 1);
		}
		if (plist.keyExists(tk_irisOut))
		{
			mTransitionOut = (plist.getValueForKey(tk_irisOut)->asInt() == 1);
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	int Transition_Iris::update( float elapsedSec ) 
	{
		elapsedSec = clampf( elapsedSec, 0.0f, (1.0f / 30.0f) );
		
		mCurrentTime += elapsedSec;
		
		// Determine the state
		
		// Below half, return intro
		if (mCurrentTime < mDuration / 2.0f) 
		{
			float t = clampf( mCurrentTime / (mDuration / 2.0f), 0.0f, 1.0f );

			mBladeAngle = lerp( 0.0f, mBladeAngleMax, t);
			mRadiusScale = lerp( mStartRadiusScale, mEndRadiusScale, t );
			
			mTextColor = mStartTextColor.lerp(mFinalTextColor, t);

			return TP_Intro;
		}
		// If we're halfway, return flush
		else if (!mPastHalfway)
		{
			mBladeAngle = mBladeAngleMax;
			mRadiusScale = mEndRadiusScale;

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
			float t = clampf( (mCurrentTime - mPauseTime - (mDuration / 2.0f)) / (mDuration / 2.0f), 0.0f, 1.0f );

			mBladeAngle = lerp( mBladeAngleMax, 0.0f, t);
			mRadiusScale = lerp( mEndRadiusScale, mStartRadiusScale, t );
			
			mTextColor = mFinalTextColor.lerp(mStartTextColor, t);
			
			return TP_Outro;
		}
		// Otherwise, we're done
		else 
		{
			return TP_Finished;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Iris::draw()
	{	
		GraphicsGL::setAlphaBlending(true);
		
		mSpriteBatch.start( SpriteBatch::BM_LAYERS );
		
		int screenWidth = ScreenCoord::getScreenResolution().X;
		int screenHeight = ScreenCoord::getScreenResolution().Y;
		
		// draw UI
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);
		
		GraphicsGL::createOrthoProjection( 0, screenWidth, screenHeight, 0, 0.1f, 1.0f );
		

		// draw the blades!
		float radius = (ScreenCoord::getScreenResolution() * 0.5f).length();
		radius *= mRadiusScale;

		Vector2 bladeSize = Vector2( radius, radius );


		int c = 0;
		for (BladeInfoList::iterator it = mBladeAnchors.begin(); it != mBladeAnchors.end(); it++)
		{
			float blade_angle = (*it).base_angle + Walaber::degToRad(mBladeAngle);
			Vector2 toCenter = VectorTools::rotateVector(bladeSize * Vector2(0.5f, 0.5f), blade_angle);

			if (mBladeTex)
			{
				//const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color* colors, const VertexColorBlendMode bm = VCBM_Multiply 
				mSpriteBatch.drawQuad( mBladeTex, (*it).anchor + toCenter, blade_angle + Walaber::degToRad(90.0f), bladeSize, mCornerUVs, mCornerColors );
			}
			else
			{
				unsigned char grey = 128 + (c * 16);

				mSpriteBatch.drawQuadColored( (*it).anchor + toCenter, blade_angle, bladeSize,
												Color(grey, grey, grey, 255) );
			}

			c++;
		}

		_drawFont();
		
		
		mSpriteBatch.flush();
	}


	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition_Iris::_initBladeAnchors()
	{
		mBladeAnchors.clear();

		float radius = (ScreenCoord::getScreenResolution() * 0.5f).length();

		for (unsigned int i = 0; i < mBladeCount; i++)
		{
			float a = ((float)i / (float)(mBladeCount));

			float a_rad = a * TWO_PI;

			Vector2 offset = VectorTools::rotateVector( Vector2(radius, 0.0f), a_rad );

			BladeInfo bi;
			bi.anchor = (ScreenCoord::getScreenResolution() * 0.5f) + offset;
			bi.base_angle = atan2( offset.Y, offset.X );

			mBladeAnchors.push_back( bi );
		}
	}
}
