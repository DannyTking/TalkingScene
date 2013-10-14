#include "Widget_ColorPicker.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for selecting colors.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ColorPicker::Widget_ColorPicker( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_COLOR_PICKER, pos, size, WO_PortraitUp )
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ColorPicker::Widget_ColorPicker( int name, Vector2 pos, Vector2 size, int columnCount, int rowCount, float edgeAmt ) : 
	Widget( name, WT_COLOR_PICKER, pos, size, WO_PortraitUp ),
	mRowCount(rowCount),
	mColumnCount(columnCount),
	mEdgeAmount(edgeAmt)
	{
		init();
		
		mColors = new Color[columnCount*rowCount];
		
		_setupColorSwatches();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ColorPicker::Widget_ColorPicker( int name, Vector2 pos, Vector2 size, int columnCount, int rowCount, float edgeAmt, int edgePadding,  
										   TexturePtr bgTexture, TexturePtr swatchTexture, TexturePtr swatchHilightTexture ) : 
	Widget( name, WT_COLOR_PICKER, pos, size, WO_PortraitUp ),
	mRowCount(rowCount),
	mColumnCount(columnCount),
	mEdgePadding(edgePadding),
	mEdgeAmount(edgeAmt)
	{
		setTexture(TextureName_BG, bgTexture);
		setTexture(TextureName_Swatch, swatchTexture);
		setTexture(TextureName_SwatchHilight, swatchHilightTexture);

		mColors = new Color[columnCount*rowCount];
		
		_setupColorSwatches();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_ColorPicker::~Widget_ColorPicker()
	{
		delete[] mColors;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::init()
	{
		mColorChanged = false;
		mColorColumn = 0;
		mColorRow = 0;
		mIconTextureSize = Vector2::One;
		mFireOnUp = false;
		mGoFire = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		const Property* p = plist.getValueForKey("Dimensions");
		if (p)
		{
			std::vector< std::string > vals = StringHelper::split( p->asString(), ':' );
			if (vals.size() == 2)
			{
				mRowCount = StringHelper::strToInt(vals[0]);
				mColumnCount = StringHelper::strToInt(vals[1]);
				
				_setupColorSwatches();
			}
		}
		
		p = plist.getValueForKey("EdgeAmount");
		if (p)
			mEdgeAmount = p->asFloat();
		
		p = plist.getValueForKey("EdgePadding");
		if (p)
			mEdgePadding = p->asFloat();
		
		p = plist.getValueForKey("IconDrawSize");
		if (p)
			mIconTextureSize = p->asVector2();
		
		p = plist.getValueForKey("Selection");
		if (p)
			setSelection( p->asColor() );
		
		p = plist.getValueForKey("FireOnUp");
		if (p)
			mFireOnUp = p->asBool();
		
		
	}
	

	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::_setupColorSwatches()
	{
		Vector2 paddedSize = getSize() - Vector2(mEdgePadding*2.0f, mEdgePadding*2.0f);
		mSwatchSize = paddedSize / Vector2(mColumnCount, mRowCount);
		
		// init colors...
		for (int c = 0; c < mColumnCount; c++)
		{
			float hue = ((float)c / (float)(mColumnCount-1)) * 360.0f;
			float sat = 1.0f;
			
			for (int r = 0; r < mRowCount; r++)
			{
				float rowProg = (((float)r / (float)(mRowCount-1)) * 2.0f) - 1.0f;
				
				float val = 1.0f;
				if (rowProg > 0.0f)
				{
					sat = 1.0f;
					val = 1.0f - (rowProg * mEdgeAmount);
				}
				else
				{
					val = 1.0f;
					sat = 1.0f - (rowProg * -mEdgeAmount);
				}
				
				if (c == (mColumnCount-1))
				{
					val = (rowProg * -0.5f) + 0.5f;
					sat = 0.0f; 
				}
				
				mColors[(r * mColumnCount) + c] = Color::fromHSV( hue, sat, val );
			}
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::setIconDrawSize( Vector2 drawSize )
	{
		mIconTextureSize = drawSize;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::setSelection( const Color& col, bool fireEvent )
	{
		float closestDist = 1000.0f;
		int closestC = -1;
		int closestR = -1;
		
		for (int c = 0; c < mColumnCount; c++)
		{
			for (int r = 0; r < mRowCount; r++)
			{
				Color thisCol = mColors[ (r * mColumnCount) + c ];
				float dR = thisCol.R - col.R;
				float dG = thisCol.G - col.G;
				float dB = thisCol.B - col.B;
				float thisDist = ((dR * dR) + (dG * dG) + (dB * dB));
				
				if (thisDist < closestDist)
				{
					closestDist = thisDist;
					closestR = r;
					closestC = c;
				}
			}
		}
		
		// save this out!
		mColorRow = closestR;
		mColorColumn = closestC;
		
		if (fireEvent)
			mColorChanged = true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ColorPicker::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mColorChanged || mGoFire)
		{		
			ret.valBool = mGoFire;
			ret.valInt1 = ((mColorRow * mColumnCount) + mColorColumn);
			
			mGoFire = false;
			mColorChanged = false;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		// background...
		if(_hasTexture(TextureName_BG))
		{
			sb->drawQuad(mLayer, _getTexture(TextureName_BG), getWorldPosition2D() + mDrawOffset, angle, Vector2(getSize().X,getSize().Y), _getTexture(TextureName_BG)->getTextureRect() );
		}
		else
		{
			QuadHelper::drawQuadColored( getWorldPosition2D() + mDrawOffset, angle, getSize(), Color(0.0f, 0.0f, 0.0f, 0.6f) );
		}
		
		// swatches...
		Vector2 localPos = (getSize() * -0.5f) + Vector2(mEdgePadding,mEdgePadding) + (mSwatchSize*0.5f);
		float origX = localPos.X;
		
		Vector2 storedPos = Vector2::Zero;
		
		for (int r = 0; r < mRowCount; r++)
		{
			for (int c = 0; c < mColumnCount; c++)
			{
				Color col = mColors[(r * mColumnCount) + c];
				Vector2 finalPos = getWorldPosition2D() + mDrawOffset + VectorTools::rotateVector( localPos, degToRad(angle) );
				
				// background...
				if(_hasTexture(TextureName_Swatch))
				{
					sb->drawQuad(mLayer, _getTexture(TextureName_Swatch), finalPos, angle, Vector2(mSwatchSize.X,mSwatchSize.Y), _getTexture(TextureName_Swatch)->getTextureRect(), col );
				}
				else 
				{
					QuadHelper::drawQuadColored( finalPos, angle, mSwatchSize, col );
				}
				localPos.X += mSwatchSize.X;
				
				if ((mColorRow == r) && (mColorColumn == c))
					storedPos = finalPos;
			}
			
			localPos.X = origX;
			localPos.Y += mSwatchSize.Y;
		}
		
		// hilight currently selected item...
		if(_hasTexture(TextureName_SwatchHilight))
		{
			sb->drawQuad(mLayer, _getTexture(TextureName_SwatchHilight), storedPos, angle, Vector2(mSwatchSize.X,mSwatchSize.Y), _getTexture(TextureName_SwatchHilight)->getTextureRect(),  mColors[(mColorRow * mColumnCount) + mColorColumn] );
		}
		else 
		{		
			QuadHelper::drawQuadColored( storedPos, angle, mSwatchSize + Vector2(4,4), Color::White );
			QuadHelper::drawQuadColored( storedPos, angle, mSwatchSize, mColors[(mColorRow * mColumnCount) + mColorColumn] );		
		}
		
		if(_hasTexture(TextureName_Icon))
		{
			// and now the icon.
			Vector2 iconLocalPos = (getSize() * 0.5f) + Vector2( mIconTextureSize.X * 0.5f, -mIconTextureSize.Y * 0.5f ) + Vector2( 0.0f, mEdgePadding * 0.5f );
			Vector2 finalPos = getWorldPosition2D() + mDrawOffset + VectorTools::rotateVector( iconLocalPos, degToRad(angle) );
			sb->drawQuad(mLayer, _getTexture(TextureName_Icon), finalPos, angle, Vector2(mIconTextureSize.X, mIconTextureSize.Y), _getTexture(TextureName_Icon)->getTextureRect() );
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ColorPicker::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::acceptNewFingerDown(%d)\n", fingerID);
#endif
		
		_updateFinger(info);
		return true;
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ColorPicker::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ColorPicker::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::releaseFingerStayed(%d)\n", fingerID);
#endif
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ColorPicker::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::releaseFingerMoved(%d)\n", fingerID);
#endif
		_updateFinger(info);
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::releaseFingerUp(%d)\n", fingerID);
#endif
		if (mFireOnUp)
		{
			mGoFire = true;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ColorPicker::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::releaseFingerLeft(%d)\n", fingerID);
#endif
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ColorPicker::notifyFingerLost(%d)\n", fingerID);
#endif
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ColorPicker::_updateFinger(FingerInfo* info)
	{
		float angle = 90.0f;
		if (mOrientation == WO_LandscapeDown)
			angle = 270.0f;
		else if (mOrientation == WO_PortraitUp)
			angle = 0.0f;
		else if (mOrientation == WO_PortraitDown)
			angle = 180.0f;
		
		Vector2 localPos = VectorTools::rotateVector((info->curPos - getWorldPosition2D()), degToRad(-angle));
		
		Vector2 bottomLeft = (getSize() * -0.5f) + Vector2(mEdgePadding,mEdgePadding);
		
		localPos = localPos - bottomLeft;
		
#ifdef _DEBUG
		printf("Widget_ColorPicker::_updateFinger() localPos[%4.2f, %4.2f]\n", localPos.X, localPos.Y);
#endif
		
		localPos /= mSwatchSize;
		
		
		int row = (int)localPos.Y;
		int col = (int)localPos.X;
		
		if ((row >= 0) && (row < mRowCount) && (col >= 0) && (col < mColumnCount))
		{
			mColorRow = row;
			mColorColumn = col;
			
			mColorChanged = true;
			
			if (!mFireOnUp)
				mGoFire = true;
		}
	}
}


