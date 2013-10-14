#ifndef _BITMAP_FONT_H_
#define _BITMAP_FONT_H_

#include <CommonGL.h>

#include <map>
#include <vector>
#include <string>

#include <Vector2.h>
#include <Rect.h>
#include <Color.h>
#include <PlatformManager.h>
#include <VertexTypes.h>
#include <TextureManager.h>
#include <Callback.h>



namespace Walaber
{
	
	class BitmapFont
	{
	public: 
		// enums and structs
        struct BitmapFontCallbackParameters
        {
			ResultCode result;
            BitmapFont* bitmapFont;
        };
		
		struct GlyphInfo
		{
			unsigned int		id;
			Rect				texCoords;
			Vector2				nativeSize;
			Vector2				offset;
			float				xAdvance;
			int					page;
		};
		
		enum Justification 
		{
			JUSTIFICATION_CENTER,
			JUSTIFICATION_RIGHT,
			JUSTIFICATION_LEFT,
		};
		
		enum LineWrapMode
		{
			LWM_Character,
			LWM_Word
		};
		
		
	public:
		BitmapFont();
		~BitmapFont();
		
		
		void load( std::string xmlFile, CallbackPtr callback = CallbackPtr() );
		
		
		std::string getName() { return mName; }
		
		
		
        // add glyph keys
        void addGlyph( const std::string& utf8Char, const Rect& coordRectInPixels, const Vector2& nativeSize, const Vector2& offset, const float xAdvance, TexturePtr texture);
        void addGlyphKey(const unsigned int glyphKey, const Rect& coordRectInPixels, const Vector2& nativeSize, const Vector2& offset, const float xAdvance, TexturePtr texture);
        
		int getNumTexturePages() { return mFileNames.size(); }
		const char* getTextureFilename(int i) 
		{ 
			const char* ret = "";
			
			if(i < (int)mFileNames.size())
			{
				ret = mFileNames[i].c_str();
			}
			
			return ret; 
		}
		void notifyTexture( TexturePtr name ) 
		{
			mTextures.push_back(name); 
		}
		
		void setLineWrapMode(LineWrapMode lineWrapMode) { mLineWrapMode = lineWrapMode; }
		LineWrapMode getLineWrapMode() { return mLineWrapMode; }
		
		void setColorBlendMode(VertexColorBlendMode m) { mColorBlendMode = m; }
		VertexColorBlendMode getColorBlendMode() { return mColorBlendMode; }
		
		void setDrawScale(float v) { mDrawScale = v; }
		float getDrawScale() { return mDrawScale; }
		
		const float getBaseHeight()const { return mBase; }
		const float getLineHeight()const { return mLineHeight; }
		const float getDrawScale()const { return mDrawScale; }
		const TexturePtr& getTexture(const int page)const { return mTextures[page]; }
		
		// need string for UTF8
		const GlyphInfo& getGlyph(unsigned int& index, const std::string& str)const;
		
		
		// Depending on the justification, pos means different things
		// for LEFT, pos = left margin
		// for CENTER, pos = position to center around
		// for RIGHT, pos = right margin
		// pos.Y is always the top!!!
		
		DEPRECATED(void drawString( const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c, const Justification just = JUSTIFICATION_LEFT));
		DEPRECATED(void drawStringWrap( const char* str, const Vector2& pos, float angle, float scaleFactor, float wrapWidth, const Color& c, const Justification just = JUSTIFICATION_LEFT));
		DEPRECATED(void drawStringWrapScale( const char* str, const Vector2& pos, float angle, float startingScale, const Vector2& size, const Color& c, const Justification just = JUSTIFICATION_LEFT));
		
		std::string wrapString(const std::string& str, const float scaleFactor, const float wrapWidth)const;
		// numLinesOut is a output paramter where the number of lines of the wrapped string is stored
		std::string wrapString(int& numLinesOut, const std::string& str, const float scaleFactor,const float wrapWidth)const;
		
		std::string wrapStringInSize(float& scaleOut, const std::string& str, const float startingScale, const Vector2& size)const;
		// numLinesOut is an output paramter where the number of lines of the wrapped string is stored
		// scaleOut is an output parameter where the final scale is stored
		std::string wrapStringInSize(int& numLinesOut, float& scaleOut, const std::string& str, float startingScale, const Vector2& size)const;
		
		void flush();
		
		Vector2 measureString(const std::string& str, float scale=1.0f)const;
		Vector2 measureCharacter(unsigned int glyphKey, float scale=1.0f)const;
		
		
		// callbacks
		void _xmlFileLoaded( void* params );// std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList plist );
		
	private:
		
		typedef std::map< unsigned int, GlyphInfo >							GlyphMap;
		typedef std::vector<std::string>									FilenameList;
		typedef std::vector<TexturePtr>										TextureList;
		typedef std::vector< std::vector<PositionTextureColorVert2D> >		BufferList;
		typedef std::map< std::string, float >								KerningList;
		
		
		
		
		unsigned int _getGlyphKey(unsigned int& index, const std::string& str)const; // doesn't do any bounds checking, decodes a UTF8 char and advances the string index
		std::string _recursiveBinaryMeasure(float& scaleOut_Mid, const std::string& textWrap, const float max, const float min, const Vector2& size)const;
		
		
		void _drawStringLeft(const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c);
		void _drawStringRight(const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c);
		void _drawStringCenter(const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c);
		Vector2 _drawGlyph( const GlyphInfo& g, const Vector2& drawPos, const float scaleFactor, const float cosAngle, const float sinAngle, const float drawScale );
		float _getKern( const GlyphInfo& from, const GlyphInfo& to ) const;
		
		void _clearVertBuffers();
		inline void _insertQuadIntoBuffer(int bufferNum); // inserts what is in mVerts
		std::string _wrapText(int &numLines, std::string textWrap, float scale, float maxWidth);
		std::string _wrapTextScale(float& scaleOut, float startingScale, std::string textWrap, const Vector2& size);
		
		std::string _wrapStringAtWord(int& numLinesOut, const std::string& str, const float scaleFactor,const float wrapWidth)const;
		std::string _wrapStringAtChar(int& numLinesOut, const std::string& str, const float scaleFactor,const float wrapWidth)const;
		
		std::string							mName;		// font face name
		BufferList							mVertBuffers;
		GlyphMap							mGlyphs;
		KerningList							mKerning;
		FilenameList						mFileNames;
		TextureList							mTextures;
		float								mDrawScale;
		float								mLineHeight;
		float								mBase;
		LineWrapMode						mLineWrapMode;
		VertexColorBlendMode				mColorBlendMode;
		
		static unsigned char				mUTF8Bytes[4];
		static PositionTextureColorVert2D	mVerts[4];
		static const float					SCALING_EPSILON;
		
		
		CallbackPtr                         mLoadedCallback;
	};
	
	
}

#endif	// _BITMAP_FONT_H_
