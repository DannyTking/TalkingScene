#include "BitmapFont.h"

#include <XMLHelper.h>
#include <GraphicsGL.h>

#include <StringHelper.h>
#include <QuadHelper.h>
#include <PlatformManager.h>
#include <VectorTools.h>
#include <UTF8Helper.h>

#include <MemberCallback.h>

namespace Walaber
{
    const float	BitmapFont::SCALING_EPSILON = 5.0f;
	PositionTextureColorVert2D BitmapFont::mVerts[4] = {};
	unsigned char BitmapFont::mUTF8Bytes[4];	
	
	BitmapFont::BitmapFont() : mLineWrapMode(LWM_Word), mColorBlendMode(VCBM_Multiply)
	{	
	}
	
	BitmapFont::~BitmapFont()
	{
	}
	
	
	void BitmapFont::load( std::string xmlFile, CallbackPtr callback )
	{
		mLoadedCallback = callback;
		
		PropertyList plist;
		MemberCallbackPtr<BitmapFont>::type memberCallback( new MemberCallback<BitmapFont>( this, &BitmapFont::_xmlFileLoaded ) );
		CallbackPtr cb = static_pointer_cast<Callback>(memberCallback);
		
		FileManager::getInstancePtr()->readFile( xmlFile, cb, plist );
	}
    
    void BitmapFont::addGlyph( const std::string& utf8Char, const Rect& coordRectInPixels, const Vector2& nativeSize, const Vector2& offset, const float xAdvance, TexturePtr texture)
    {
        unsigned int index = 0;
        unsigned int glyphKey = _getGlyphKey(index, utf8Char);
        
        addGlyphKey(glyphKey, coordRectInPixels, nativeSize, offset, xAdvance, texture);
    }
    
    void BitmapFont::addGlyphKey( const unsigned int glyphKey, const Rect& coordRectInPixels, const Vector2& nativeSize, const Vector2& offset, const float xAdvance, TexturePtr texture)
    {
        //GlyphMap::iterator it = mGlyphs.find(glyphKey);
        //if(it == mGlyphs.end() && texture)
        {
            GlyphInfo g;
            g.id = glyphKey;
            
            float glyphScale = 1.0f;
			
            g.nativeSize = nativeSize;
			if (g.nativeSize.Y > mLineHeight)
			{
				glyphScale = mLineHeight / g.nativeSize.Y;
			}
			
			g.nativeSize *= glyphScale;
			
            //Vector2 texCoordSize = nativeSize/texture->getTextureSize();	
            //Vector2 texCoordUpperLeft = coord/texture->getTextureSize();	
			
            g.texCoords = Rect( coordRectInPixels.upper_left / texture->getTextureSize(), coordRectInPixels.size / texture->getTextureSize() );
            
            g.offset = offset;
            g.offset *= mDrawScale;
            g.xAdvance = xAdvance * glyphScale;
            g.xAdvance *= mDrawScale;
            g.nativeSize *= mDrawScale;
            
            // see if this texture already exists as a page
            unsigned int size = mTextures.size();
            unsigned int i;
            for(i = 0 ; i < size; ++i)
            {
                if(mTextures[i].get() == texture.get())
                {
                    break;
                }
            }
            if( i != size)
            {
                g.page = i;
            }
            else
            {
                mTextures.push_back(texture);
                g.page = mTextures.size()-1;
            }            
            
            mGlyphs[g.id] = g;
            
            Logger::printf("Walaber", Logger::SV_DEBUG, " -> Glyph added: [%d]  size:[%f x %f]  texcoords:[%f,%f] -> [%f,%f]  offset:[x: %f, y: %f] xadvance:[%f] page:[%d]\n",
													    (int)g.id, 
														g.nativeSize.X, g.nativeSize.Y,
														g.texCoords.upper_left.X, g.texCoords.upper_left.Y, g.texCoords.size.X, g.texCoords.size.Y,
														g.offset.X, g.offset.Y,
														g.xAdvance,
														g.page);
        }
    }
	
	void BitmapFont::_xmlFileLoaded( void* params ) //std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList plist )
	{
		FileManager::ReadFileCallbackParameters* rfp = (FileManager::ReadFileCallbackParameters*)params;
		
		BitmapFontCallbackParameters callbackParams;
		callbackParams.bitmapFont = this;
		callbackParams.result = ResultCode_OK;
		
		if (rfp->result == ResultCode_OK)
		{
			// parse the XML document.
			xmlDocPtr doc = NULL;
			xmlNodePtr root_element = XML::loadDocMemory( rfp->buffer, rfp->length, "font", doc );
			
			
			if (rfp->buffer)
				delete[] rfp->buffer;
			
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "\n\nBitmapFont constructor - filename:%s\n", rfp->actualPath.c_str());

			if (root_element == NULL)
				return;
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "root element: %s\n", root_element->name);

			// okay, get the texture info first.
			Vector2 texNativeSize = Vector2::One;
			
			xmlNodePtr tex = root_element->children;
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "\n\nBitmapFont constructor - finding texture properties\n");

			while (tex)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "node name: %s\n", tex->name);

				if (xmlStrcmp(tex->name, (const xmlChar*)"info") == 0)
				{
					// info section
					mName = XML::parseString(tex, "face");
					
				}
				else if (xmlStrcmp(tex->name, (const xmlChar*) "common") == 0)
				{
					Logger::printf("Walaber", Logger::SV_DEBUG, "\n\nBitmapFont constructor - found common");

					texNativeSize = XML::parseVector2( tex, "scaleW", "scaleH" );
					
					if(XML::attrExists(tex, "drawScale"))
					{
						mDrawScale = XML::parseFloat(tex, "drawScale");
					}
					else 
					{
						mDrawScale = 1.0f;
					}
					
					int pages = XML::parseInt(tex,"pages");
					mVertBuffers.reserve(pages);
					for(int i = 0; i < pages; ++i)
					{
						mVertBuffers.push_back(std::vector<PositionTextureColorVert2D>());
					}
					
					mLineHeight = mDrawScale * XML::parseFloat(tex,"lineHeight");
					mBase = mDrawScale * XML::parseFloat(tex, "base");
					
					break;
				}
				tex = tex->next;
			}
			
			
			
			// reset
			tex = root_element->children;
			
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "after common\n");
			
			while (tex)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "looping through sub categories\n");

				if (xmlStrcmp(tex->name, (const xmlChar*) "pages") == 0)
				{
					Logger::printf("Walaber", Logger::SV_DEBUG, "looping through pages\n");
					
					xmlNodePtr filename = tex->children;
					while(filename)
					{
						Logger::printf("Walaber", Logger::SV_DEBUG, "found file name\n");

						if (xmlStrcmp(filename->name, (const xmlChar*) "page") == 0)
						{
							mFileNames.push_back(XML::parseString( filename, "file" ));
						}
						filename = filename->next;
					}
				}
				tex = tex->next;
			}
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "after page\n");

			int size = (int)mFileNames.size();
			for(int i = 0; i < size; ++i)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, " texture: %s  size: %f x %f\n", mFileNames[i].c_str(), texNativeSize.X, texNativeSize.Y );
			}
			
			// now get the glyphs!
			xmlNodePtr chars = root_element->children;
			while (chars)
			{
				if (xmlStrcmp(chars->name, (const xmlChar*)"chars") == 0)
				{				
					xmlNodePtr glyphs = chars->children;
					while(glyphs)
					{
						if (xmlStrcmp(glyphs->name, (const xmlChar*)"char") == 0)
						{
							
							GlyphInfo g;
							//const char* the_char = (const char*)xmlGetProp(glyph, (const xmlChar*)"char");
							g.id = (unsigned int)XML::parseInt(glyphs, "id");
							
							//xmlFree((char*)the_char);
							//the_char = NULL;
							Vector2 texCoordUpperLeft = XML::parseVector2(glyphs, "x", "y");	
							g.nativeSize = XML::parseVector2(glyphs, "width", "height");
							Vector2 texCoordSize = g.nativeSize/texNativeSize;	
							//texCoordUpperLeft.Y = texNativeSize.Y - (texCoordUpperLeft.Y + g.nativeSize.Y);
							
							texCoordUpperLeft /= texNativeSize;				
							g.texCoords = Rect(texCoordUpperLeft, texCoordSize);
							
							g.offset = XML::parseVector2(glyphs, "xoffset", "yoffset");
							g.offset *= mDrawScale;
							g.xAdvance = XML::parseFloat(glyphs, "xadvance");
							g.xAdvance *= mDrawScale;
							g.page = XML::parseInt(glyphs, "page");
							g.nativeSize *= mDrawScale;
							
							mGlyphs[g.id] = g;
							
							Logger::printf("Walaber", Logger::SV_DEBUG, " -> Glyph added: [%d]  size:[%f x %f]  texcoords:[%f,%f] -> [%f,%f]  offset:[x: %f, y: %f] xadvance:[%f] page:[%d]\n",
																		(int)g.id, 
																		g.nativeSize.X, g.nativeSize.Y,
																		g.texCoords.upper_left.X, g.texCoords.upper_left.Y, g.texCoords.size.X, g.texCoords.size.Y,
																		g.offset.X, g.offset.Y,
																		g.xAdvance,
																		g.page);
						}
						glyphs = glyphs->next;
					}
					break;
				}		
				chars = chars->next;
			}
			
			// add in special characters
			GlyphInfo g;
			g.id = '\0';
			mGlyphs[g.id] = g;
			g.id = '\n';		
			mGlyphs[g.id] = g;
			g.id = '\r';		
			mGlyphs[g.id] = g;
			g.id = 8232;		
			mGlyphs[g.id] = g;
			
			// get kerning info!
			for (xmlNodePtr kerns = root_element->children; kerns; kerns = kerns->next)
			{
				if (xmlStrcmp(kerns->name, (const xmlChar*)"kernings") == 0)
				{
					for (xmlNodePtr kern = kerns->children; kern; kern = kern->next)
					{
						if (xmlStrcmp(kern->name, (const xmlChar*)"kerning") == 0)
						{
							int first = XML::parseInt(kern, "first");
							int second = XML::parseInt(kern, "second");
							int amt = XML::parseInt(kern, "amount");
							
							char key[32];
							snprintf(key, 32, "%d_%d", first, second);
							
							Logger::printf("Walaber", Logger::SV_DEBUG, "got kerning first(%d) second(%d) amt(%d) key(%s)\n", first, second, amt, key);
			
							mKerning[ std::string(key) ] = (amt * mDrawScale);
						}
					}
				}
			}
			
			
			XML::cleanupDoc( doc );	
		}
		else
		{
			// error!
			callbackParams.result = ResultCode_ErrorFileNotFound;
		}
		
		
		
		if (mLoadedCallback)
        {
            mLoadedCallback->invoke(&callbackParams);
        }
	}
	
	
	
	
	
	// need string for UTF8
	const BitmapFont::GlyphInfo& BitmapFont::getGlyph(unsigned int& index, const std::string& str)const
	{
		unsigned int glyphKey = _getGlyphKey(index,str);
		
		GlyphMap::const_iterator it = mGlyphs.find(glyphKey);
		if(it != mGlyphs.end())
		{				
			return it->second;
		}
		else 
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "Glyph with key: [%d] does not exist!\n", glyphKey);
			//assert(0); 
			
			it = mGlyphs.find((unsigned int)'?');
			if (it != mGlyphs.end())
			{
				return it->second;
			}
			else
			{
				// no "?" character in this font, just use the first character in the font... 
				return mGlyphs.begin()->second;
			}
		}
	}
	
	// somewhat naive string size function
	Vector2 BitmapFont::measureString(const std::string& str, float scale)const
	{
		Vector2 retSize = Vector2::Zero;
		float sizeX = 0.0f;
		float lines = 1;
		
		unsigned int glyphKey;
		std::map<unsigned int, GlyphInfo>::const_iterator it;

		const char* cstr = str.c_str();
		
		if(str.size())
		{		
			for (unsigned int i = 0; true; i++)
			{
				glyphKey = _getGlyphKey(i,cstr);
				
				if (glyphKey == '\0')
				{
					if(sizeX > retSize.X)
					{
						retSize.X = sizeX;
					}
					retSize.Y = ((lines) * mLineHeight);
					retSize *= scale;
					break;
				}
				
				if ((glyphKey == '\n' || glyphKey == '\r' || glyphKey == 8232))
				{
					++lines;
					
					if(sizeX > retSize.X)
					{
						retSize.X = sizeX;
					}
					
					sizeX = 0.0f;
					continue;
				}
				
				it = mGlyphs.find(glyphKey);
				if(it != mGlyphs.end())
				{				
					const GlyphInfo& g = it->second;
					
					// look for next character...
					float kern = 0.0f;
					if (glyphKey != '\0')
					{
						unsigned int ni = i+1;
						unsigned int nextKey = _getGlyphKey(ni, cstr);
						std::map<unsigned int, GlyphInfo>::const_iterator nx = mGlyphs.find(nextKey);
						if (nx != mGlyphs.end())
						{
							const GlyphInfo& ng = nx->second;
							kern = _getKern( g, ng );
						}
					}
					
					sizeX += (g.xAdvance + kern);
				}
			}
		}
		
		return retSize;
	}
				

	
	Vector2 BitmapFont::measureCharacter(unsigned int glyphKey, float scale)const
	{
		Vector2 retSize = Vector2::Zero;
			
		if (glyphKey == '\0')
		{
		
		}			
		if ((glyphKey == '\n' || glyphKey == '\r' || glyphKey == 8232))
		{
			retSize.Y = mLineHeight;
		}
		else
		{			
			std::map<unsigned int, GlyphInfo>::const_iterator it = mGlyphs.find(glyphKey);
			if(it != mGlyphs.end())
			{				
				const GlyphInfo& g = it->second;
				
				retSize.X = g.xAdvance*scale;
				retSize.Y = mLineHeight*scale;
			}
		}
		
		return retSize;
	}
	

	std::string BitmapFont::wrapString(const std::string& str, const float scaleFactor, const float wrapWidth)const
	{
		int i=1;
		return (mLineWrapMode == LWM_Word) ? _wrapStringAtWord(i,str,scaleFactor,wrapWidth) : _wrapStringAtChar(i,str,scaleFactor,wrapWidth);
	}

	
	std::string BitmapFont::wrapString(int& numLinesOut, const std::string& str, const float scaleFactor,const float wrapWidth)const
	{
		return (mLineWrapMode == LWM_Word) ? _wrapStringAtWord(numLinesOut,str,scaleFactor,wrapWidth) : _wrapStringAtChar(numLinesOut,str,scaleFactor,wrapWidth);
	}
	
	
	std::string BitmapFont::_wrapStringAtWord(int& numLinesOut, const std::string& str, const float scaleFactor,const float wrapWidth)const
	{
		//get width of text to wrap
		float textWidth = measureString(str,scaleFactor).X;
		
		//text that will be returned
		std::string outputText = "";
		
		//check to see if text needs to be wrapped
		if (textWidth > wrapWidth)
		{
			std::string space(" ");
			std::string lineBreak("\n");
			float spaceWidth = measureString(space, scaleFactor).X;
			
			//split text into an array of words
			std::vector<std::string> words;
			StringHelper::split(str, ' ', words);
			
			float spaceLeft = wrapWidth;
			
			unsigned int size = words.size();
			for(unsigned int i = 0; i < size; ++i)
			{
				size_t pos = words[i].find(lineBreak[0]);
				if(pos == std::string::npos)
				{					
					// no linebreak exists in this 
					float wordWidth = measureString(words[i], scaleFactor).X;
				
					if(wordWidth > spaceLeft)
					{
						++numLinesOut;
						outputText += lineBreak;
						spaceLeft = wrapWidth - (wordWidth + spaceWidth);
					}
					else
					{
						spaceLeft -= (wordWidth + spaceWidth);					
					}
					outputText += (words[i] + space);
				}
				else
				{				
					size_t start = 0;
					do
					{						
						std::string substring = words[i].substr(start, (pos - start) +1);
						
						float wordWidth = measureString(substring, scaleFactor).X;
						
						if(wordWidth > spaceLeft)
						{
							++numLinesOut;
							outputText += lineBreak;
							//spaceLeft = wrapWidth - (wordWidth + spaceWidth);
						}
						
						outputText += substring;
						spaceLeft = wrapWidth;
					
						++numLinesOut;
						start = pos + 1;
						pos = words[i].find(lineBreak[0], pos+1);
					}while(pos != std::string::npos);
					
					if( start <= words[i].size()-1 )
					{
						std::string last = words[i].substr(start, ((words[i].size()-1) - start) + 1);
						float wordWidth = measureString( last, scaleFactor ).X;
						spaceLeft -= wordWidth;
						
						outputText += last + space;
					}

				}
			}
			return outputText;
		}
		else
		{
			// count number of new lines
			size_t pos=0;
			pos = str.find('\n', pos);
			while(pos != std::string::npos)
			{
				++numLinesOut;
				pos = str.find('\n', pos+1);
			}
			//text does not need to be wrapped
			return str;
		}
	}
	
	std::string BitmapFont::_wrapStringAtChar(int& numLinesOut, const std::string& str, const float scaleFactor,const float wrapWidth)const
	{
		//get width of text to wrap
		float textWidth = measureString(str,scaleFactor).X;
		
		//text that will be returned
		std::string outputText = "";
		std::string character = "x";
		//check to see if text needs to be wrapped
		if (textWidth > wrapWidth)
		{
			std::string lineBreak("\n");
			
			float spaceLeft = wrapWidth;
			
			for (unsigned int i = 0; true; i++)
			{
				unsigned int startIndex = i;
				unsigned int glyphKey = _getGlyphKey(i,str);
				unsigned int endIndex = i;
				if(glyphKey == '\0')
				{
					break;
				}
				if(glyphKey == '\n' || glyphKey == '\r')
				{
					++numLinesOut;
					outputText += lineBreak;
					spaceLeft = wrapWidth;
					continue;
				}
				
				float charWidth = measureCharacter(glyphKey, scaleFactor).X;
				if(charWidth > spaceLeft)
				{
					++numLinesOut;
					outputText += lineBreak;
					spaceLeft = wrapWidth - charWidth;
				}
				else
				{
					spaceLeft -= charWidth;					
				}
				
				outputText += str.substr(startIndex,(endIndex-startIndex)+1);
			}
			return outputText;
		}
		else
		{
			//text does not need to be wrapped
			return str;
		}
	}
	
	std::string BitmapFont::wrapStringInSize(float& scaleOut, const std::string& str, const float startingScale, const Vector2& size)const
	{
		int i = 1;
		return wrapStringInSize(i,scaleOut, str, startingScale, size);
	}
	
	
	std::string BitmapFont::wrapStringInSize(int& numLinesOut, float& scaleOut, const std::string& str, float startingScale, const Vector2& size)const
	{
		std::string ret = str;
		scaleOut = startingScale;
		
		
		////
		// HACK!!
		////
		
		std::string newStr = str;
		
			// Perry has a lot of "-" in text, and we can't wrap that properly.
			
			// FIRST - find all of the newlines (so we can preserve them)
			std::vector< std::string > newlineSeperated = StringHelper::split(str, '\n');
			newStr = "";
			
			unsigned int newlineListSize = newlineSeperated.size();
			for(unsigned int i = 0; i < newlineListSize; ++i)
			{
				// SECOND - find all of the dashes, and put a space afterwards
				std::vector<std::string> dashSeperated = StringHelper::split(newlineSeperated[i], '-');
				
				unsigned int listSize = dashSeperated.size();
				for(unsigned int j = 0; j < listSize; ++j)
				{
					newStr += dashSeperated[j];
					
					if (j != listSize - 1)
					{
						newStr += "- ";
					}
				}
				
				// Add a newline (as long as we're not the last split
				if (i != newlineListSize - 1)
				{
					newStr += "\n";
				}
			}
		
		
		
		Vector2 box = measureString(newStr,startingScale);
		Walaber::Logger::printf("Walaber", Walaber::Logger::SV_DEBUG, "[wrapStringInSize]: box -> %f, %f", box.X, box.Y);
		if(box.X > size.X || box.Y > size.Y )
		{
			int lines = 1;
			std::string retString = wrapString(lines, newStr, startingScale, size.X); 
			float y = lines * (mLineHeight*startingScale);
			
			Walaber::Logger::printf("Walaber", Walaber::Logger::SV_DEBUG, "[wrapStringInSize]: lines -> %i", lines);
			
			if(y > size.Y)
			{	
				retString = _recursiveBinaryMeasure(scaleOut,newStr, startingScale, (size.Y/y)*startingScale, size);
			}
			ret = retString;
		}
		
		
		
		
		////
		// HACK cont.
		////
		
		newStr = ret;
		if (StringHelper::stringContains(ret, "-"))
		{
			// Now that the string is wrapped, find all of the "- " and remove the spaces
			 
			// FIRST - find all of the newlines (so we can preserve them)
			std::vector<std::string> newlineSeperated_ = StringHelper::split(ret, '\n');
			newStr = "";

			unsigned int newlineListSize_ = newlineSeperated_.size();
			for(unsigned int i = 0; i < newlineListSize_; ++i)
			{
				// SECOND - find all of the return carriages (so we can preserve them
				std::vector<std::string> returnSeperated = StringHelper::split(newlineSeperated_[i], '\r');
				
				unsigned int returnListSize = returnSeperated.size();
				for(unsigned int j = 0; j < returnListSize; ++j)
				{
					// SECOND - find all of the dashes, and put a space afterwards
					std::vector<std::string> dashSeperated = StringHelper::split(returnSeperated[j], '-');
					
					unsigned int listSize = dashSeperated.size();
					for(unsigned int k = 0; k < listSize; ++k)
					{
						if (k != 0)
						{
							if (dashSeperated[k][0] == ' ')
							{
								newStr += "-" + dashSeperated[k].substr( 1 );
							}
							else 
							{
								newStr += "-" + dashSeperated[k];
							}
						}
						else 
						{
							newStr += dashSeperated[k];
						}
					}
					
					// Add a newline (as long as we're not the last split
					if (j != returnListSize - 1)
					{
						//newStr += "\r";
					}
				}
				
				// Add a newline (as long as we're not the last split
				if (i != newlineListSize_ - 1)
				{
					newStr += "\n";
				}
			}
		}
		
		
		return newStr;
	}
	
	void BitmapFont::drawString( const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c, const Justification just ) 
	{
		_clearVertBuffers();
		
		switch(just)
		{
			case JUSTIFICATION_CENTER:
				_drawStringCenter(str, pos, angle, scaleFactor, c);
				break;
				
			case JUSTIFICATION_RIGHT:
				_drawStringCenter(str, pos, angle, scaleFactor, c);
				break;
				
			case JUSTIFICATION_LEFT:
			default:
				_drawStringLeft(str, pos, angle, scaleFactor, c);
				break;
				
		}
		
		flush();
		
	}
	
	void BitmapFont::drawStringWrap( const char* str, const Vector2& pos, float angle, float scaleFactor, float wrapWidth, const Color& c, const Justification just)
	{
		int x = 1;
		drawString(_wrapText(x, std::string(str), scaleFactor, wrapWidth).c_str(), pos, angle, scaleFactor, c, just);
	}
	
	void BitmapFont::drawStringWrapScale( const char* str, const Vector2& pos, float angle, float startingScale, const Vector2& size, const Color& c, const Justification just)
	{
#if _DEBUG
		QuadHelper::drawQuadColored(pos+VectorTools::rotateVector(Vector2(size.X*.5f,0.0f), angle*(PI/180.0f)), angle, Vector2(size.X, 2.0f), Color::Red);
		QuadHelper::drawQuadColored(pos+VectorTools::rotateVector(Vector2(size.X*.5f,size.Y), angle*(PI/180.0f)), angle, Vector2(size.X, 2.0f), Color::Red);
		QuadHelper::drawQuadColored(pos-VectorTools::rotateVector(Vector2(0.0f,-size.Y*.5f), angle*(PI/180.0f)), angle, Vector2(2.0f, size.Y), Color::Red);
		QuadHelper::drawQuadColored(pos-VectorTools::rotateVector(Vector2(-size.X,-size.Y*.5f), angle*(PI/180.0f)), angle, Vector2(2.0f, size.Y), Color::Red);
		QuadHelper::drawQuadColored(pos, angle, Vector2(10,10), Color::Red);
#endif	
		float scaleOut;
		std::string wrappedText = _wrapTextScale(scaleOut, startingScale, std::string(str), size);
		drawString(wrappedText.c_str(), pos, angle, scaleOut, c, just);
	}
	
	std::string BitmapFont::_wrapText(int& numLinesOut, std::string textWrap, float scale, float maxWidth)
	{
		//get width of text to wrap
		float textWidth = measureString(textWrap,scale).X;
		
		//text that will be returned
		std::string outputText = "";
		
		//check to see if text needs to be wrapped
		if (textWidth > maxWidth)
		{
			std::string space(" ");
			std::string lineBreak("\n");
			float spaceWidth = measureString(space, scale).X;
			//split text into an array of words
			std::vector<std::string> words = StringHelper::split(textWrap, ' ');
			
			float spaceLeft = maxWidth;
			
			unsigned int size = words.size();
			for(unsigned int i = 0; i < size; ++i)
			{
				float wordWidth = measureString(words[i], scale).X;
				if(wordWidth > spaceLeft)
				{
					++numLinesOut;
					outputText += lineBreak;
					spaceLeft = maxWidth - (wordWidth + spaceWidth);
				}
				else
				{
					spaceLeft -= (wordWidth + spaceWidth);					
				}
				outputText += (words[i] + space);
			}
			return outputText;
		}
		else
		{
			//text does not need to be wrapped
			return textWrap;
		}
	}
	
	// naive text wrapping routine with scaling for rect
	std::string BitmapFont::_wrapTextScale(float& scaleOut, float startingScale, std::string textWrap, const Vector2& size)
	{
		std::string ret = textWrap;
		scaleOut = startingScale;
		
		Vector2 box = measureString(textWrap,startingScale);
		if(box.X > size.X || box.Y > size.Y )
		{
			int lines = 1;
			std::string retString = _wrapText(lines, textWrap, startingScale, size.X); 
			float y = lines * (mLineHeight*startingScale);
			
			if(y > size.Y)
			{	
				if(lines > 1)
				{
					startingScale = (size.Y/2.0f)/ mLineHeight;
				}
				retString = _recursiveBinaryMeasure(scaleOut,textWrap, startingScale, (size.Y/y)*startingScale, size);
			}
			ret = retString;
		}
		
		return ret;
	}
			   
	std::string BitmapFont::_recursiveBinaryMeasure(float& scaleOut_Mid, const std::string& textWrap, const float max, const float min, const Vector2& size)const
	{
		scaleOut_Mid = (min + max) / 2.0f;
		
		int lines = 1;
		std::string retString = wrapString(lines, textWrap, scaleOut_Mid, size.X);
		float y = lines * (mLineHeight*scaleOut_Mid);
		
		Walaber::Logger::printf("Walaber", Walaber::Logger::SV_DEBUG, "[_recursiveBinaryMeasure]: y_height -> %f", y);
		
		if(abs(y - size.Y) < SCALING_EPSILON || min >= max)
		{
			Walaber::Logger::printf("Walaber", Walaber::Logger::SV_DEBUG, "[_recursiveBinaryMeasure]: GOOD!");
			return retString;
		}
		else if(y > size.Y)
		{
			Walaber::Logger::printf("Walaber", Walaber::Logger::SV_DEBUG, "[_recursiveBinaryMeasure]: TOO BIG!");
			return _recursiveBinaryMeasure(scaleOut_Mid, textWrap, scaleOut_Mid-.01f, min, size);
		}
		else 
		{
			Walaber::Logger::printf("Walaber", Walaber::Logger::SV_DEBUG, "[_recursiveBinaryMeasure]: TOO SMALL!");
			return _recursiveBinaryMeasure(scaleOut_Mid, textWrap, max, scaleOut_Mid+.01f, size);
		}

	}
	
	void BitmapFont::_drawStringLeft(const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c)
	{
		float radAngle = degToRad(angle);
		float cosAngle = (float)cosf(radAngle);
		float sinAngle = (float)sinf(radAngle);
		
		Vector2 drawPos = pos;
		
		// set vert colors once!
		mVerts[0].setColor(c);
		mVerts[1].setColor(c);
		mVerts[2].setColor(c);
		mVerts[3].setColor(c);
		
		unsigned int glyphKey;
		std::map<unsigned int, GlyphInfo>::const_iterator it;
		
		for (unsigned int i = 0; true; i++)
		{
			glyphKey = _getGlyphKey(i,str);
				
			if (glyphKey == '\0')
				break;
			
			if ((glyphKey == '\n' || glyphKey == '\r' || glyphKey == 8232))
			{
				drawPos.X = pos.X;
				drawPos.Y += mBase*scaleFactor;
				continue;
			}
			
			it = mGlyphs.find(glyphKey);
			if(it != mGlyphs.end())
			{				
				const GlyphInfo& g = it->second;
				
				_drawGlyph( g, drawPos, scaleFactor, cosAngle, sinAngle, scaleFactor );
				
				
				
				// kerning!
				float kern = 0.0f;
				if (glyphKey != '\0')
				{
					unsigned int ni = i + 1;
					unsigned int nextKey = _getGlyphKey(ni, str);
					std::map<unsigned int, GlyphInfo>::const_iterator it_n = mGlyphs.find(nextKey);
					if (it_n != mGlyphs.end())
					{
						const GlyphInfo& ng = it_n->second;
						
						kern = _getKern( g, ng );
					}
					
				}
				
				drawPos += VectorTools::rotateVector(Vector2((g.xAdvance+kern)*scaleFactor, 0), cosAngle, sinAngle);
			}
			else 
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "Glyph with key: [%d] does not exist!\n", glyphKey);
			}
		}
	}
	
	void BitmapFont::_drawStringRight(const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c)
	{
		
	}
	
	void BitmapFont::_drawStringCenter(const char* str, const Vector2& pos, float angle, float scaleFactor, const Color& c)
	{
		Vector2 size = measureString(std::string(str), scaleFactor); 
		_drawStringLeft(str, pos - Vector2(size.X*.5f, 0.0f), angle, scaleFactor, c);
	}
	
	unsigned int BitmapFont::_getGlyphKey(unsigned int& index, const std::string& str)const
	{
		unsigned char c = (unsigned char)str[index];
		
		// insert first byte
		unsigned int ind = 0;
		mUTF8Bytes[ind++] = c;
		
		// see if there are more bytes
		int numBytes = UTF8Helper::getNumCodeBytes(c);
		for(int j=1; j < numBytes; ++j)
		{
			mUTF8Bytes[ind++] =str[++index];
		}
		
		// get the decimal value to use as a look up for the glyph
		unsigned int decimalVal = UTF8Helper::decimalForUTF8Bytes(mUTF8Bytes, ind);
		
		return decimalVal;
	}
	
	void BitmapFont::flush()
	{		
		// Set a blending function to use
		GraphicsGL::setBlending(true);
		
		// enable textures.
		GraphicsGL::setTextureEnabled(true);
		
		unsigned int size = mVertBuffers.size();
		for(unsigned int i=0; i<size; ++i)
		{			
			GraphicsGL::bindTexture( mTextures[i]->getName());
			
			glVertexPointer(2, GL_FLOAT, sizeof(PositionTextureColorVert), &mVertBuffers[i][0].pos);
			GraphicsGL::enableVertexClientState();
			
			glTexCoordPointer(2, GL_FLOAT, sizeof(PositionTextureColorVert), &mVertBuffers[i][0].tex);
			GraphicsGL::enableTextureClientState();
			
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(PositionTextureColorVert), &mVertBuffers[i][0].col);
			GraphicsGL::enableColorClientState();
			
			unsigned int numVerts = (unsigned int)((float)mVertBuffers[i].size());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);

			// Client State Reset
			GraphicsGL::disableVertexClientState();
			GraphicsGL::disableTextureClientState();
			GraphicsGL::disableColorClientState();
		}
	}
	
	
	Vector2 BitmapFont::_drawGlyph( const GlyphInfo& gi, const Vector2& drawPos, const float scaleFactor, const float cosAngle, const float sinAngle, const float drawScale )
	{
		// update vertices...
		mVerts[0].setPos(drawPos +
						 VectorTools::rotateVector(
												   Vector2((gi.offset.X * drawScale),((gi.offset.Y+gi.nativeSize.Y)*drawScale)), 
												   cosAngle,sinAngle));
		
		mVerts[1].setPos(drawPos +
						 VectorTools::rotateVector(
												   Vector2(((gi.offset.X+gi.nativeSize.X)*drawScale),((gi.offset.Y+gi.nativeSize.Y)*drawScale)),
												   cosAngle,sinAngle));
		
		mVerts[2].setPos(drawPos +
						 VectorTools::rotateVector( 
												   Vector2((gi.offset.X * drawScale), (gi.offset.Y * drawScale)),
												   cosAngle,sinAngle));
		
		mVerts[3].setPos(drawPos +
						 VectorTools::rotateVector(
												   Vector2(((gi.offset.X+gi.nativeSize.X)*drawScale),(gi.offset.Y * drawScale)),
												   cosAngle,sinAngle));		
		
		//Logger::printf("Walaber", Logger::SV_DEBUG, "TexCoords: [%f][%f] => [%f][%f]\n", gi.texCoords.upper_left.X, gi.texCoords.upper_left.Y, gi.texCoords.getBottomRight().X,  gi.texCoords.getBottomRight().Y);
		
		// and texture coordinates...
		mVerts[0].setUV(gi.texCoords.upper_left.X,
						gi.texCoords.getBottomRight().Y);
		
		mVerts[1].setUV(gi.texCoords.getBottomRight().X,
						gi.texCoords.getBottomRight().Y);
		
		mVerts[2].setUV(gi.texCoords.upper_left.X,
						gi.texCoords.upper_left.Y);
		
		mVerts[3].setUV(gi.texCoords.getBottomRight().X,
						gi.texCoords.upper_left.Y);
		
		_insertQuadIntoBuffer(gi.page);
		
		return drawPos;
	}
	
	
	
	float BitmapFont::_getKern( const GlyphInfo& from, const GlyphInfo& to ) const
	{
	
		char key[32];
		snprintf(key, 32, "%d_%d", from.id, to.id );
		
		float kern = 0.0f;
		std::string keyStr(key);
		
		KerningList::const_iterator it = mKerning.find(keyStr);
		if (it != mKerning.end())
		{
			kern = it->second;
		}
		
		return kern;
	}
	
	
	void BitmapFont::_insertQuadIntoBuffer(int bufferNum) // inserts what is in mVerts
	{
		
		// we repeat the first and last vert so the strip looks like this
		// AABCDD
		// the render will ignore bogus triangles so you get
		// AAB, ABC, BCD, CDD, only drawing ABC & BCD
		// Now when you add EFGH you get
		// AABCDDEEFGHH
		// and triangles AAB, ABC, BCD, CDD, DDE, DEE, EEF, EFG, FGH, GHH
		// only draw ABC, BCD, EFG, & FGH
		mVertBuffers[bufferNum].push_back(mVerts[0]);
		
		mVertBuffers[bufferNum].push_back(mVerts[0]);		
		mVertBuffers[bufferNum].push_back(mVerts[1]);
		mVertBuffers[bufferNum].push_back(mVerts[2]);
		mVertBuffers[bufferNum].push_back(mVerts[3]);
		
		// extra vert
		mVertBuffers[bufferNum].push_back(mVerts[3]);
		
	}
	
	
	void BitmapFont::_clearVertBuffers()
	{
		unsigned int size = mVertBuffers.size();
		for(unsigned int i=0; i < size; ++i)
		{
			mVertBuffers[i].clear();
		}
	}
}
