#include "Widget_IconList.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for selecting from a list of icons.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_IconList::Widget_IconList( int name, Vector2 pos, Vector2 size ) : 
	Widget( name, WT_ICON_LIST, pos, size, WO_PortraitUp )
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_IconList::Widget_IconList( int name, Vector2 pos, Vector2 iconSize, int iconsPerRow, int height, int edgePadding, int iconPadding ) : 
	Widget( name, WT_ICON_LIST, pos, Vector2((iconSize.X * iconsPerRow) + (edgePadding * 2.0f) + ((iconsPerRow-1) * iconPadding), height), WO_PortraitUp ),
	mIconsPerRow(iconsPerRow),
	mEdgePadding(edgePadding),
	mIconPadding(iconPadding),
	mIconSize(iconSize)
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_IconList::Widget_IconList( int name, Vector2 pos, Vector2 iconSize, int iconsPerRow, int height, int edgePadding, int iconPadding,
									 TexturePtr bgTexture, TexturePtr scrollBarTexture, TexturePtr iconHilightTexture ) : 
	Widget( name, WT_ICON_LIST, pos, Vector2((iconSize.X * iconsPerRow) + (edgePadding * 2.0f) + ((iconsPerRow-1) * iconPadding), height), WO_PortraitUp ),
	mIconsPerRow(iconsPerRow),
	mEdgePadding(edgePadding),
	mIconPadding(iconPadding),
	mIconSize(iconSize)
	{
		setTexture(TextureName_BG, bgTexture);
		setTexture(TextureName_ScrollBar, scrollBarTexture);
		setTexture(TextureName_IconHilight, iconHilightTexture);
		
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_IconList::~Widget_IconList()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_IconList::Icon::Icon( TexturePtr tex, int _id )
	{
		texture = tex;
		identifier = _id;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_IconList::Icon::Icon( std::string texName, int _id )
	{
		textureName = texName;
		texture = TexturePtr();
		identifier = _id;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::init()
	{
		// Actual inits
		mVel = 0.0f;
		mVelDamping = 0.9f;
		mOffsetPos = 0.0f;
		mLastOffsetDelta = 0.0f;
		mMaxScrollAmt = 0.0f;
		mTouchedIcon = -1;
		mCurrentSelection = -1;
		mGoSelect = false;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		Vector2 iconSize = mIconSize;
		int iconsPerRow = mIconsPerRow;
		int edgePadding = mEdgePadding;
		int iconPadding = mIconPadding;
		
		const Property* p = plist.getValueForKey("IconSize");
		if (p)
			iconSize = p->asVector2();
		
		p = plist.getValueForKey("IconsPerRow");
		if (p)
			iconsPerRow = p->asInt();
		
		p = plist.getValueForKey("EdgePadding");
		if (p)
			edgePadding = p->asInt();
		
		p = plist.getValueForKey("IconPadding");
		if (p)
			iconPadding = p->asInt();
		
		setIconListSizes(iconSize, iconsPerRow, getBaseSize().Y, edgePadding, iconPadding);
	}

		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::clearIcons()
	{
		mIcons.clear();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::addIcon( TexturePtr texture, int icon_id )
	{
		mIcons.push_back( Icon(texture, icon_id ) );
		
		int rowCount = (((float)mIcons.size() / (float)mIconsPerRow) + 0.999f);
		float scrollableSize = getSize().Y - (mEdgePadding * 2.0f);
		float actualSize = rowCount * (mIconSize.Y + mIconPadding);
		if(actualSize > scrollableSize)
		{
			mMaxScrollAmt = scrollableSize - actualSize;
		}
		else 
		{
			mMaxScrollAmt = 0.0f;
		}
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::addIcon( std::string texName, int icon_id )
	{
		mIcons.push_back( Icon(texName, icon_id ) );
		
		int rowCount = (((float)mIcons.size() / (float)mIconsPerRow) + 0.999f);
		float scrollableSize = getSize().Y - (mEdgePadding * 2.0f);
		float actualSize = rowCount * (mIconSize.Y + mIconPadding);
		if(actualSize > scrollableSize)
		{
			mMaxScrollAmt = scrollableSize - actualSize;
		}
		else 
		{
			mMaxScrollAmt = 0.0f;
		}
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::setIconListSizes( Vector2 iconSize, int iconsPerRow, int height, int edgePadding, int iconPadding )
	{
		setBaseSize( Vector2((iconSize.X * iconsPerRow) + (edgePadding * 2.0f) + ((iconsPerRow-1) * iconPadding), height) );
		
		mIconsPerRow = iconsPerRow;
		mEdgePadding = edgePadding;
		mIconPadding = iconPadding;
		mIconSize = iconSize;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_IconList::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mFinger == NULL)
		{
			mOffsetPos += (mVel * elapsedSec);
			mVel *= mVelDamping;
			
			if (mOffsetPos > 1.0f)
			{
				// move toward zero!
				float move = (1.0f - mOffsetPos);
				if (abs(move) > 3.0f) { move *= 0.5f; }
				
				mOffsetPos += move;
			}
			else if (mOffsetPos < mMaxScrollAmt)
			{
				// move toward max!
				float move = (mMaxScrollAmt - mOffsetPos);
				if (abs(move) > 3.0f) { move *= 0.5f; }
				mOffsetPos += move;
			}
		}
		else
		{
			mVel = mLastOffsetDelta / elapsedSec;
		}
		
		if (mGoSelect)
		{
			mGoSelect = false;
			ret.valBool = true;
			ret.valInt1 = mIcons[mTouchedIcon].identifier;
			
			mCurrentSelection = mTouchedIcon;
			mTouchedIcon = -1;
			return true;
		}
		
		if (mScrollingToSelection)
		{
			// make sure the selected object is fully on-screen!
			Vector2 min = (getSize() * -0.5f) + Vector2(mEdgePadding, mEdgePadding);
			Vector2 max = (getSize() * 0.5f) - Vector2(mEdgePadding, mEdgePadding);
			AABB scrollRegionAABB(min, max);
			
			Vector2 local_pos = Vector2(0, mOffsetPos) - (getSize() * 0.5f) + Vector2(mEdgePadding, mEdgePadding) + (mIconSize * 0.5f);
			float leftMargin = local_pos.X;
			
			AABB iconAABB;
			
			int rowCount = 0;
			for (int i = 0; i <= mCurrentSelection; i++)
			{
				min = local_pos - (mIconSize * 0.5f);
				max = local_pos + (mIconSize * 0.5f);
				iconAABB.Min = min;
				iconAABB.Max = max;
				
				rowCount++;
				
				local_pos.X += mIconSize.X + mIconPadding;
				
				if (rowCount >= mIconsPerRow)
				{
					local_pos.X = leftMargin;
					local_pos.Y += mIconSize.Y + mIconPadding;
					rowCount = 0;
				}
			}
			
			if ((iconAABB.Min.Y >= scrollRegionAABB.Min.Y) && (iconAABB.Max.Y <= scrollRegionAABB.Max.Y))
			{
				mScrollingToSelection = false;
			}
			else if (iconAABB.Min.Y < scrollRegionAABB.Min.Y)
			{
				mOffsetPos += (mIconSize.Y * 2.0f * elapsedSec);
			}
			else
			{
				mOffsetPos -= (mIconSize.Y * 2.0f * elapsedSec);
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		// basic background.
		AABB fullAABB( getWorldPosition2D() + mDrawOffset - (getSize() * 0.5f), getWorldPosition2D() + mDrawOffset + (getSize() * 0.5f) );
		if(_hasTexture(TextureName_BG))
		{
			AABB backgroundAABB(getWorldPosition2D() + mDrawOffset - (getSize() * 0.5f)-Vector2(mEdgePadding,mEdgePadding), getWorldPosition2D() + mDrawOffset + (getSize() * 0.5f) + Vector2(mEdgePadding + 14,mEdgePadding) );
			sb->drawQuad(mLayer, _getTexture(TextureName_BG), backgroundAABB.getCenter(), angle, backgroundAABB.getSize(), _getTexture(TextureName_BG)->getTextureRect());
		}
		else 
		{
			sb->drawQuadColored( mLayer, fullAABB.getCenter(), angle, fullAABB.getSize(), Color(0.0f, 0.0f, 0.0f, 0.6f) );
		}
		
		// these are all in un-rotated space, we rotate and draw at the last minute.
		AABB scrollRegionAABB( fullAABB.Min + Vector2(mEdgePadding,mEdgePadding), fullAABB.Max - Vector2(mEdgePadding, mEdgePadding) );
		
		// start drawing icons!
		Vector2 local_pos =  Vector2(0,mOffsetPos) - (getSize() * 0.5f) + Vector2(mEdgePadding, mEdgePadding) + (mIconSize * 0.5f);
		float leftMargin = local_pos.X;
		
		int rowCount = 0;
		int size =  (int)mIcons.size();
		for (int i = 0; i < size; i++)
		{
			// draw this icon.
			Vector2 final_pos = (getWorldPosition2D() + mDrawOffset) + local_pos;
			
			AABB iconAABB( final_pos - (mIconSize*0.5f), final_pos + (mIconSize*0.5f) );
			
			
			// draw me if I'm in the scroll region only.
			if (scrollRegionAABB.intersects(iconAABB))
			{	
				// potentially only draw a clipped portion.
				if (mIcons[i].texture.use_count() == 0)
				{
					mIcons[i].texture = TextureManager::getManager().getTexture( mIcons[i].textureName );
				}
				
				Rect tex_rect = mIcons[i].texture->getTextureRect();
				
				if (iconAABB.Min.Y < scrollRegionAABB.Min.Y)
				{
					float percent = 1.0f - ((scrollRegionAABB.Min.Y - iconAABB.Min.Y) / iconAABB.getSize().Y);
					
					iconAABB.Min.Y = scrollRegionAABB.Min.Y;
					tex_rect.upper_left.Y = 1.0f - percent;
					tex_rect.size.Y *= (percent);
				}
				
				if (iconAABB.Max.Y > scrollRegionAABB.Max.Y)
				{
					float percent = (scrollRegionAABB.Max.Y - iconAABB.Min.Y) / iconAABB.getSize().Y;
					
					iconAABB.Max.Y = scrollRegionAABB.Max.Y;
					tex_rect.size.Y *= percent;
				}
				
				Vector2 localCenter = iconAABB.getCenter() - (getWorldPosition2D() + mDrawOffset);
				Vector2 localHalfSize = iconAABB.getSize() * 0.5f;
				
				Vector2 screenCenter = (getWorldPosition2D() + mDrawOffset) + VectorTools::rotateVector(localCenter, degToRad(angle));
				AABB finalAABB;
				finalAABB.expandToInclude( screenCenter - localHalfSize );
				finalAABB.expandToInclude( screenCenter + localHalfSize );
				
				if (i == mCurrentSelection)
				{
					if(_hasTexture(TextureName_IconHilight))
					{
						sb->drawQuad(mLayer, _getTexture(TextureName_IconHilight), finalAABB.getCenter(), angle, finalAABB.getSize()+Vector2(2*mIconPadding,2*mIconPadding), _getTexture(TextureName_IconHilight)->getTextureRect(), Color::Red);
					}
					else
					{
						sb->drawQuadColored(mLayer, finalAABB.getCenter(), angle, finalAABB.getSize()+Vector2(2*mIconPadding,2*mIconPadding), Color::Red);
					}
				}			
				
				sb->drawQuad(mLayer+1, mIcons[i].texture, finalAABB.getCenter(), angle, finalAABB.getSize(), tex_rect );
				
				//if (i == mTouchedIcon)
				//	QuadHelper::drawQuadColored( finalAABB.getCenter(), angle, finalAABB.getSize(), Color(1.0f, 1.0f, 1.0f, 0.5f) );
				
			}
			
			rowCount++;
			
			local_pos.X += mIconSize.X + mIconPadding;
			
			if (rowCount >= mIconsPerRow)
			{
				local_pos.X = leftMargin;
				local_pos.Y += mIconSize.Y + mIconPadding;
				rowCount = 0;
			}
		}
		
		// scroll bar area...
		AABB scrollBarAABB;
		Vector2 pt(scrollRegionAABB.Max.X + mEdgePadding, fullAABB.Min.Y);
		scrollBarAABB.expandToInclude(pt);
		pt.X = scrollRegionAABB.Max.X + mEdgePadding + 12;
		pt.Y = fullAABB.Max.Y;
		scrollBarAABB.expandToInclude(pt);
		
		if(!_hasTexture(TextureName_BG))
		{
			sb->drawQuadColored(mLayer, scrollBarAABB.getCenter(),angle,scrollBarAABB.getSize(),Color(0.0f,0.0f,0.0f,0.6f));
		}
		
		// now the scroll region.
		float scrollHeight = (getSize().Y - (mEdgePadding * 2.0f));
		
		float scrollSize = (scrollHeight / (scrollHeight + abs(mMaxScrollAmt))) * scrollHeight;
		float roomToPlay = scrollHeight - scrollSize;
		float barOffset = roomToPlay * ((mOffsetPos / mMaxScrollAmt) - 0.5f);
		if (barOffset < -roomToPlay*.5f) { barOffset = -roomToPlay*.5f; }
		else if (barOffset > roomToPlay*.5f) { barOffset = roomToPlay*.5f; }
		
		if(_hasTexture(TextureName_ScrollBar))
		{
			sb->drawQuad(mLayer, _getTexture(TextureName_ScrollBar),scrollBarAABB.getCenter()+Vector2(0.0f,barOffset),angle,Vector2(scrollBarAABB.getSize().X-6.0f,(int)scrollSize), _getTexture(TextureName_ScrollBar)->getTextureRect());
		}
		else
		{
			sb->drawQuadColored(mLayer, scrollBarAABB.getCenter()+Vector2(0.0f,barOffset),angle,Vector2(scrollBarAABB.getSize().X-6.0f,(int)scrollSize),Color::White);
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_IconList::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::acceptNewFingerDown(%d)\n", fingerID);
#endif
		
		if (mFinger == NULL)
		{
			mFinger = info;
			
			// check which object we're touching...
			float angle = -90.0f;
			if (mOrientation == WO_LandscapeDown) { angle = -270.0f; }
			else if (mOrientation == WO_PortraitUp) { angle = 0.0f; }
			else if (mOrientation == WO_PortraitDown) { angle = -180.0f; }
			
			Vector2 localTouch = VectorTools::rotateVector(info->curPos - getWorldPosition2D(), degToRad(angle));
			mTouchedIcon = _iconFromPos( localTouch );
			
			_updateFinger();
			
			if (mScrollingToSelection)
				mScrollingToSelection = false;
			
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_IconList::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_IconList::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::releaseFingerStayed(%d)\n", fingerID);
#endif
		
		_updateFinger();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_IconList::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::releaseFingerMoved(%d)\n", fingerID);
#endif
		
		mTouchedIcon = -1;
		
		_updateFinger();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::releaseFingerUp(%d)\n", fingerID);
#endif
		if (mTouchedIcon != -1)
			mGoSelect = true;
		
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_IconList::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::releaseFingerLeft(%d)\n", fingerID);
#endif
		
		mFinger = NULL;
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_IconList::notifyFingerLost(%d)\n", fingerID);
#endif
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_IconList::_updateFinger()
	{
		if (mFinger == NULL)
		{
			return;
		}
		
		float angle = 90.0f;
		if (mOrientation == WO_LandscapeDown) { angle = 270.0f; }
		else if (mOrientation == WO_PortraitUp) { angle = 0.0f; }
		else if (mOrientation == WO_PortraitDown) { angle = 180.0f; }
		
		Vector2 scrollDir = VectorTools::rotateVector( Vector2(0,1), degToRad(angle) );
		
		mLastOffsetDelta = scrollDir.dotProduct(mFinger->curPos - mFinger->lastPos);
		
		// scale this smaller as we drift away from the edges...
		//float maxDist = 40.0f;
		float newPos = mOffsetPos + mLastOffsetDelta;
		
		if (newPos > 0.0f)
		{
			// are we moving more negative?
			if (newPos > mOffsetPos)
			{
				//mLastOffsetDelta *= (maxDist - mOffsetPos) / (maxDist * 1.1f);
				newPos = mOffsetPos + (mLastOffsetDelta * 0.5f);
			}
		}
		else if (newPos < mMaxScrollAmt)
		{
			if (newPos < mOffsetPos)
			{
				//mLastOffsetDelta *= ((mOffsetPos - (mMaxScrollAmt - maxDist))) / (maxDist * 1.1f);
				newPos = mOffsetPos + (mLastOffsetDelta * 0.5f);
			}
		}
		
		mOffsetPos = newPos;
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	   
	int Widget_IconList::_iconFromPos( Vector2 localTouchPos )
	{
#ifdef _DEBUG
		printf("Widget_IconList::_iconFromPos( localTouchPos[%4.2f, %4.2f] )\n", localTouchPos.X, localTouchPos.Y);
#endif
		
		Vector2 corner = Vector2(0,mOffsetPos) - (getSize() * 0.5f) + Vector2(mEdgePadding, mEdgePadding);
		Vector2 toTouch = localTouchPos - corner;
		int column = (int)(toTouch.X / (mIconSize.X + mIconPadding));
		int row = (int)(toTouch.Y / (mIconSize.Y + mIconPadding));
		
		int touched = (row * mIconsPerRow) + column;
		
		if (touched < 0)
			touched = -1;
		else if (touched >= (int)mIcons.size())
			touched = -1;
		
#ifdef _DEBUG
		printf(" r[%d] c[%d] == [%d]\n", row, column, touched );
#endif
		return touched;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	   
	void Widget_IconList::selectIconWithID(int iconID)
	{
		int size = mIcons.size();
		for (int i = 0; i < size; i++)
		{
			if (mIcons[i].identifier == iconID)
			{
				mCurrentSelection = i;
				
				mScrollingToSelection = true;
				break;		
			}
		}
	}
}


