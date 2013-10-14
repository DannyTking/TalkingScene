#ifndef _SCREENCOORD_H_
#define _SCREENCOORD_H_

#include <Vector2.h>
#include <Rect.h>
#include <WalaberMath.h>

namespace Walaber
{
	// --------========--------========--------========--------========--------========--------========--------========
    class ScreenCoord
    {
	public:
		// --------========--------========--------========--------========--------========--------========--------========
		ScreenCoord(Vector2 unit) :Unit(unit), Pixels(Vector2::Zero) { }
        ScreenCoord(Vector2 unit, Vector2 pixels) : Unit(unit), Pixels(pixels) { }
        ScreenCoord(float unitX, float unitY) : Unit(unitX, unitY), Pixels(Vector2::Zero) { }
        ScreenCoord(float unitX, float unitY, float pixelX, float pixelY) : Unit(unitX, unitY), Pixels(pixelX, pixelY) { }
		
		// --------========--------========--------========--------========--------========--------========--------========
		// "UNIT" component, in unitized screen space (0, 1) for both X and Y.
        Vector2		Unit;
		
		// --------========--------========--------========--------========--------========--------========--------========
		// "PIXEL" component, in pixel space (0, screenwidth) for X, (0, screenheight) for Y.
        Vector2		Pixels;
		
		
		// --------========--------========--------========--------========--------========--------========--------========
		Vector2 toScreen()
		{
			return Vector2((Unit * sScreenSize) + Pixels);
		}
		
		// --------========--------========--------========--------========--------========--------========--------========
		static Vector2 toScreen(ScreenCoord sc)
        {
            return Vector2((sc.Unit * sScreenSize) + sc.Pixels);
        }
		
		// --------========--------========--------========--------========--------========--------========--------========
		static Vector2 screenSizeFromRealWorldSize(Vector2 s)
        {
            return Vector2((s * sRealScreenConvert));
        }

        // --------========--------========--------========--------========--------========--------========--------========
		static Vector2 toScreenSafe(ScreenCoord sc, float safePercent)
        {
            float e = (1.0f - safePercent) * 0.5f;
			
            Vector2 lowEdge = sScreenSize * e;
            Vector2 highEdge = sScreenSize - lowEdge;
			
            Vector2 screen = toScreen(sc);
            screen.X = clampf(screen.X, lowEdge.X, highEdge.X);
            screen.Y = clampf(screen.Y, lowEdge.Y, highEdge.Y);
			
            return screen;
        }
		
        // --------========--------========--------========--------========--------========--------========--------========
		static Rect toRectangle(ScreenCoord topLeft, ScreenCoord botRight)
        {
            Vector2 tl = toScreen(topLeft);
            Vector2 br = toScreen(botRight);
			
            return Rect((int)tl.X, (int)tl.Y, (int)(br.X - tl.X), (int)(br.Y - tl.Y));
        }
		
        // --------========--------========--------========--------========--------========--------========--------========
		static Rect toRectangleSafe(ScreenCoord topLeft, ScreenCoord botRight, float safePercent)
        {
            Vector2 tl = toScreenSafe(topLeft, safePercent);
            Vector2 br = toScreenSafe(botRight, safePercent);
			
            return Rect((int)tl.X, (int)tl.Y, (int)(br.X - tl.X), (int)(br.Y - tl.Y));
        }
		
		// --------========--------========--------========--------========--------========--------========--------========
		static Vector2 getScreenResolution() { return sScreenSize; }
		static void setScreenResolution( Vector2 res ) { sScreenSize = res; }
		static float getScreenAspectRatio() { return sScreenSize.X / sScreenSize.Y; }
		
        // --------========--------========--------========--------========--------========--------========--------========
		static Vector2 getDesignResolution() { return sDesignRes; }
		static void setDesignResolution( Vector2 res ) { sDesignRes = res; sResConvert = sScreenSize / sDesignRes; }
        
        // --------========--------========--------========--------========--------========--------========--------========
		static Vector2 getResConvertFactor() { return sResConvert; }

		// --------========--------========--------========--------========--------========--------========--------========
		static Vector2 getRealWorldScreenSize() { return sRealScreenSize; }
		static void setRealWorldScreenSize( Vector2 size ) { sRealScreenSize = size; sRealScreenConvert = sScreenSize / sRealScreenSize; }
        
        // --------========--------========--------========--------========--------========--------========--------========
		static Vector2 getRealWorldConvertFactor() { return sRealScreenConvert; }
		
		
		
		
        // --------========--------========--------========--------========--------========--------========--------========
		inline ScreenCoord operator +(const ScreenCoord& right)
        {
            return ScreenCoord( Unit + right.Unit, Pixels + right.Pixels );
        }
		
        // --------========--------========--------========--------========--------========--------========--------========
		inline ScreenCoord operator -(const ScreenCoord& right)
        {
            return ScreenCoord( Unit - right.Unit, Pixels - right.Pixels );
        }
		
        // --------========--------========--------========--------========--------========--------========--------========
		inline ScreenCoord operator *(const ScreenCoord& right)
        {
            return ScreenCoord( Unit * right.Unit, Pixels * right.Pixels );
        }
		
        // --------========--------========--------========--------========--------========--------========--------========
		inline ScreenCoord operator /(const ScreenCoord& right)
        {
            return ScreenCoord( Unit / right.Unit, Pixels / right.Pixels );
        }
		
	protected:
        static Vector2 sScreenSize;
        static Vector2 sDesignRes;
        static Vector2 sResConvert;
		static Vector2 sRealScreenSize;
		static Vector2 sRealScreenConvert;
		
        
    };
}

#endif	// _SCREENCOORD_H_

