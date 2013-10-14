#ifndef _WALABER_WIDGET_MANIPULATOR_H_
#define _WALABER_WIDGET_MANIPULATOR_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"


#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a simple manipulator - a button that you can drag around and use it's position to manipulate things.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Manipulator : public Widget
	{
	public:
		enum TextureName
		{
			TextureName_Main
		};
		
		Widget_Manipulator( int name, Vector2 pos, Vector2 size, TexturePtr tex, bool oneShot = false );
		~Widget_Manipulator();
		
		void init();
		
		// valid properties:
		// "OneShot"
		void setProperties( const PropertyList& plist );
		
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
		void setAngle( float a ) { mAngle = a; }
		float getAngle() { return mAngle; }
		
		bool hasFinger() { return mHaveFinger; }
		
	private:
		float				mAngle;
		bool				mMoved;
		Vector2				mFingerPos;
		Vector2				mFingerOffset;
		bool				mHaveFinger;
		bool				mOneShot;
	};
}

#endif	// _WALABER_WIDGET_MANIPULATOR_H_