#include "CircleHelper.h"

#include "VectorTools.h"
#include "Vector2.h"
#include "WalaberMath.h"
#include "Logger.h"

namespace Walaber
{
	GLfloat CircleHelper::mVerts[] = {};
	GLuint CircleHelper::mVertColors[] = {};
	float CircleHelper::mSegStep = 360.0f / CIRCLE_HELPER_SEG_COUNT;

	// -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- 
	// -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- 
	void CircleHelper::drawFilledCircleSegment( const Vector2& pos, float angle, float rad, float degrees, bool clockwise, const Color& col )
	{
		_resetVerts(col);
		
		float segmentCutoff = ((degrees / 360.0f) * ((float)CIRCLE_HELPER_SEG_COUNT));
		int segmentsToDraw = (int)segmentCutoff;
		float extra = 0.0f;
		//float baseAngle = (CIRCLE_HELPER_SEG_COUNT - 1) * mSegStep;
		
		if (segmentsToDraw < CIRCLE_HELPER_SEG_COUNT)
		{
			extra = segmentCutoff - segmentsToDraw;
			segmentsToDraw++;
		
			int offset = (segmentsToDraw - 1) * 6;
			float baseAngle = (segmentsToDraw - 1) * mSegStep;
			
			Vector2 nP = VectorTools::rotateVector( Vector2(0.0f, 1.0f), 
													degToRad(baseAngle + (extra * mSegStep)) );
			mVerts[offset + 4] = nP.X;
			mVerts[offset + 5] = nP.Y;
		}
		
        // Set a blending function to use
        GraphicsGL::setBlending(true);
		GraphicsGL::setTextureEnabled(false);
		
		glVertexPointer(2, GL_FLOAT, 8, mVerts);
		GraphicsGL::enableVertexClientState();
		
		glColorPointer(4, GL_UNSIGNED_BYTE, 4, mVertColors);
		GraphicsGL::enableColorClientState();
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glTranslatef( pos.X, pos.Y, 0.0f );
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		
		glScalef(rad,clockwise ? rad : -rad, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, segmentsToDraw * 3);
		
		// Client State Reset
		GraphicsGL::disableVertexClientState();
		GraphicsGL::disableColorClientState();


		glPopMatrix();
	}
	
	// -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- 
	// -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- 
	void CircleHelper::drawCurvedArrow( const Vector2& pos, float angle, float rad, float degrees, bool clockwise, float thickness, const Color& col )
	{
		_resetVerts(col);
		
		float angleStep = degrees / (float)CIRCLE_HELPER_SEG_COUNT;
		//Logger::printf("Walaber", Logger::SV_DEBUG, "deg:%f  aS %f\n", degrees, angleStep );
		
		Vector2 p;
		
		for (int i = 0; i < CIRCLE_HELPER_SEG_COUNT; i++)
		{
			p = VectorTools::rotateVector( Vector2(0,1), degToRad( (float)(i) * angleStep ) );
			
			mVerts[ (i*2) + 0 ] = p.X;
			mVerts[ (i*2) + 1 ] = p.Y;
		}
		
		p = VectorTools::rotateVector( Vector2(0,1), degToRad( degrees ) );
		
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 0 ] = p.X;
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 1 ] = p.Y;
		
		// draw arrow-head.
		Vector2 arrowAngle = VectorTools::rotateVector( Vector2(0.0f, 1.1f), degToRad( degrees - ((degrees > 0.0f) ? 10.0f : -10.0f) ) );
		
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 2 ] = arrowAngle.X;
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 3 ] = arrowAngle.Y;
		
		
		arrowAngle = VectorTools::rotateVector( Vector2(0.0f, 0.85f), degToRad( degrees - ((degrees > 0.0f) ? 10.0f : -10.0f) ) );
		
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 4 ] = arrowAngle.X;
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 5 ] = arrowAngle.Y;
		
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 6 ] = p.X;
		mVerts[ (CIRCLE_HELPER_SEG_COUNT*2) + 7 ] = p.Y;
		
        // Set a blending function to use
        GraphicsGL::setBlending(true);
		GraphicsGL::setTextureEnabled(false);
		
		glVertexPointer(2, GL_FLOAT, 8, mVerts);
		GraphicsGL::enableVertexClientState();
		
		glColorPointer(4, GL_UNSIGNED_BYTE, 4, mVertColors);
		GraphicsGL::enableColorClientState();

		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glTranslatef( pos.X, pos.Y, 0.0f );
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		
		glScalef(clockwise ? rad : -rad, rad, 1.0f);
		
		glLineWidth( thickness );
		glDrawArrays(GL_LINE_STRIP, 0, CIRCLE_HELPER_SEG_COUNT + 4);
		
		// Client State Reset
		GraphicsGL::disableVertexClientState();
		GraphicsGL::disableColorClientState();

		glPopMatrix();
	}
	
	
	// -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- 
	// -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- --  -- -- -- -- 
	void CircleHelper::_resetVerts(const Color& col)
	{
		for (int i = 0; i < CIRCLE_HELPER_SEG_COUNT; i++)
		{
			int offset = (i * 6);
			
			mVerts[offset + 0] = 0.0f;
			mVerts[offset + 1] = 0.0f;
			
			Vector2 pos = VectorTools::rotateVector( Vector2(0.0f, 1.0f), ((float)i/(float)CIRCLE_HELPER_SEG_COUNT)*TWO_PI );
			mVerts[offset + 2] = pos.X;
			mVerts[offset + 3] = pos.Y;
			
			pos = VectorTools::rotateVector( Vector2(0.0f, 1.0f), ((float)(i+1)/(float)CIRCLE_HELPER_SEG_COUNT)*TWO_PI );
			mVerts[offset + 4] = pos.X;
			mVerts[offset + 5] = pos.Y;
		}
		
		unsigned int packedCol = packColor(col.R, col.G, col.B, col.A);
		
		for (int i = 0; i < CIRCLE_HELPER_SEG_COUNT * 3; i++)
		{
			mVertColors[i] = packedCol;
		}
	}
		
}

