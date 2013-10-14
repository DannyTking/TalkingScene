#include "TextureManager.h"
#include "Texture2D.h"
#include "Subtexture.h"
#include "GraphicsGL.h"

#include <StringHelper.h>
#include <FileManager.h>

#include <MemberCallback.h>
#include <Logger.h>

namespace Walaber 
{
	const std::string TextureManager::mSupportedExtensions[] = 
	{
        ".webp",
		".png",
		".jpg",
#ifdef TARGET_IPHONE
		".pvr"
#endif
	};
	
	const int TextureManager::mSupportedExtensionsCount = sizeof(mSupportedExtensions) / sizeof(std::string);
	
	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	TextureManager* TextureManager::inst_ = NULL;
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	TextureManager::TextureManager()
	: ignoreMemoryWarnings_(false),
	  lastValidatedTexture_(0)
	{
		_createErrorTexture();
	}
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	TextureManager::~TextureManager() 
	{
	}
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
    void TextureManager::reloadTextureAtlas(const std::string& xmlFile, CallbackPtr callback)
    {
        std::string strippedXmlFile = PlatformManager::getInstancePtr()->stripPlatformSpecificFilename(xmlFile);
        unloadTextureAtlas(strippedXmlFile);
        
        loadTextureAtlas(strippedXmlFile, callback);
    }
    
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	bool TextureManager::unloadTextureAtlas(const std::string& xmlFile)
    {
        StringToStringMap::iterator it = atlasLoadedMap_.find(xmlFile);
        if(it != atlasLoadedMap_.end())
        {
#if _DEBUG
            printf("Unloading Atlas (%s)!\n", xmlFile.c_str());
#endif
            _unloadTextureAndSubTextures(it->second);
            
            atlasLoadedMap_.erase(it);
            
            return true;
        }
        else
        {
#if _DEBUG
            printf("No atlas exists with base name (%s)\n", xmlFile.c_str());
#endif
            return false;
        }	
    }
    
	
	// ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::unloadAllTextureAtlases()
	{
		for (StringToStringMap::iterator it = atlasLoadedMap_.begin(); it != atlasLoadedMap_.end(); it++)
		{
#if _DEBUG
            printf("UnloadAllTextureAtlases: Unloading (%s)!\n", it->first.c_str());
#endif
            _unloadTextureAndSubTextures(it->second);
		}
		
		atlasLoadedMap_.clear();
	}
	
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::loadTextureAtlas(const std::string& xmlFile, CallbackPtr callback)
	{
        StringToStringMap::iterator it = atlasLoadedMap_.find(xmlFile);
        if(it == atlasLoadedMap_.end())
        {
#if _DEBUG
            printf("Loading Atlas (%s) for the first time!\n", xmlFile.c_str());
#endif
            bool isFirst= false;
            if(atlasToCallbackMap_.find(xmlFile) == atlasToCallbackMap_.end())
            {
                isFirst = true;
            }
            
            atlasToCallbackMap_.insert(std::make_pair(xmlFile, callback));            
            
                                       
            if(isFirst)
            {
                PropertyList plist;
				
				MemberCallbackPtr<TextureManager>::type memberCallback( new MemberCallback<TextureManager>(this, &TextureManager::_loadedTextureAtlas) );
				CallbackPtr callback_ = static_pointer_cast<Callback>(memberCallback);
				
                FileManager::getInstancePtr()->readFile(xmlFile, callback_, plist );
            }
        }
        else
        {
#if _DEBUG
            printf("Already loaded atlas (%s)\n", xmlFile.c_str());
#endif
            if(callback)
            {
                AtlasLoadedCallbackParameters params(xmlFile, getTexture(it->second));
                callback->invoke( &params );
            }
        }	
	}

	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::_loadedTextureAtlas( void* params ) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& plist)
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		// error code in the event we fail to load / parse this file...
		ResultCode result = ResultCode_OK;
		
		
		if (rcp->result == ResultCode_OK)
		{
			// parse the XML document.
			xmlDocPtr doc = NULL;
	
			// try "ImageList" format...
			xmlNodePtr root_element = XML::loadDocMemory(rcp->buffer, rcp->length, "ImageList", doc);
        
			if (root_element != NULL)
			{
				std::string strippedPath = PlatformManager::getInstancePtr()->stripPlatformSpecificFilename(rcp->path);
				_loadImageList( doc, root_element, strippedPath );
			}
			else
			{
				result = ResultCode_ErrorXMLParseError;
			}
				
        
			if (rcp->buffer)
				delete[] rcp->buffer;
		
			XML::cleanupDoc(doc);
		}
		else
		{
			result = ResultCode_ErrorFileNotFound;
		}
		
		
		if (result != ResultCode_OK)
		{
			// call callback(s) with error code...
			std::pair<AtlasToCallbackMap::iterator, AtlasToCallbackMap::iterator> range;
            range = atlasToCallbackMap_.equal_range(rcp->path);
            for(AtlasToCallbackMap::iterator it = range.first; it != range.second; ++it)
            {
                if(it->second)
                {
                    AtlasLoadedCallbackParameters callbackParams;
					callbackParams.xmlPath = rcp->path;
					callbackParams.result = result;
					
                    it->second->invoke(&callbackParams);
                }
            }
            
            atlasToCallbackMap_.erase(range.first, range.second);

		}
	}
    
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
    void TextureManager::_unloadTextureAndSubTextures(const std::string& texture)
    {
        TextureMap::iterator it = textureMap_.begin();
        
        while(it != textureMap_.end())
        {
            if(it->second->getTextureType() == TT_Subtexture)
            {
                SubtexturePtr subt(static_pointer_cast<Subtexture> (it->second));
                if(subt->isTextureParent(texture))
                {
                    TextureMap::iterator toErase = it;
                    ++it;
                    textureMap_.erase(toErase);                    
                }
                else
                {
                    ++it;
                }
            }
            else if(it->first == texture)
            {
                TextureMap::iterator toErase = it;
                ++it;
                textureMap_.erase(toErase); 
            }
            else
            {
                ++it;
            }
        }       
    }
    
	/*
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
    void TextureManager::_addSubtextureToMap(xmlNodePtr node, const std::string& textureBasePath, const std::vector<std::string>& atlasTextureNames, const Vector2& nativeTexSize)
	{
		SubTexInfo si;
		
		si.texName = textureBasePath + XML::parseString(node, "filename");
		
		//doing this for every texture could be wasteful, however, it's never done during gameplay
		Vector2 pos = XML::parseVector2(node, "x", "y");
		Vector2 size = XML::parseVector2(node, "width", "height");
		
		si.texData.width = size.X;
		si.texData.height = size.Y;
		
		size += pos;
		pos /= nativeTexSize;
		size /= nativeTexSize;
		
		si.texData.minS = pos.X;
		si.texData.minT = pos.Y;
		si.texData.maxS = size.X;
		si.texData.maxT = size.Y;
		
		int atlas = XML::parseInt(node, "page");
		std::string atlasTexName = atlasTextureNames[atlas];
		
		SubTexListMap::iterator it = tempSubTexInfoMap_.find(atlasTexName);
		if (it == tempSubTexInfoMap_.end())
		{
			// doesn't exist in map yet...
			tempSubTexInfoMap_[ atlasTexName ] = std::vector< SubTexInfo >();
		}
		
		it = tempSubTexInfoMap_.find(atlasTexName);
		
		it->second.push_back( si );
				
	}
	*/
    
    // ---------------------------------------------------------------------------------------------
	void TextureManager::_loadImageList(xmlDocPtr doc, xmlNodePtr root_element, const std::string& atlasPath)
	{
        if (XML::attrExists(root_element, "imgSize"))
        {
            atlasToNumPagesToLoadMap_[atlasPath] = 1;
            _loadImageListDetails(doc, root_element, atlasPath);
        }
        else
        {
            std::vector< xmlNodePtr > pages;
            // new format with multiple pages - go
            for (xmlNodePtr pageNode = root_element->children; pageNode; pageNode = pageNode->next)
            {
                if (xmlStrcmp(pageNode->name, (const xmlChar*) "Page") == 0)
                {
                    pages.push_back( pageNode );
                }
            }
            
            if( !pages.empty() )
            {
                int numPages = pages.size();
                atlasToNumPagesToLoadMap_[atlasPath] = numPages;
                for( unsigned int i = 0; i < numPages; ++i )
                {
                    _loadImageListDetails(doc, pages[i], atlasPath);
                }
            }
        }
    }
    
    // ---------------------------------------------------------------------------------------------
	void TextureManager::_loadImageListDetails(xmlDocPtr doc, xmlNodePtr root_element, const std::string& atlasPath)
	{
		Vector2 texNativeSize = XML::parseVector2(root_element, "imgSize");
		std::string atlasTextureName = XML::parseString(root_element, "file");
		std::string textureBasePath = XML::parseString(root_element, "textureBasePath");
		
		std::string tsName = StringHelper::removeExtension(atlasTextureName);
		std::string ext = StringHelper::getExtension(atlasTextureName);
		
		TextureSettingsMap::iterator itSet = textureSettingsMap_.find(tsName);
		
		if( !_isTextureSupported( ext ) )
		{
            if (itSet != textureSettingsMap_.end())
            {
				std::string newExt = itSet->second.getUncompressedExt();
				if( !newExt.empty() )
				{
					atlasTextureName = tsName + std::string(".") + newExt;
				}
			}			
		}
        
        float drawScale = 1.0f;
        if(XML::attrExists(root_element, "drawScale"))
		{
           drawScale = XML::parseFloat(root_element, "drawScale");
		}
		
        atlasLoadedMap_mainTextureKey_[atlasTextureName] = atlasPath;
		
		// create temporary data, which is used to create atlas once the main atlas texture is loaded...
		std::vector< SubTexInfo > texInfoList;
		
		for (xmlNodePtr img = root_element->children; img; img = img->next)
		{
			if (xmlStrcmp(img->name, (const xmlChar*) "Image") == 0)
			{
				SubTexInfo si;
				
				si.texName = textureBasePath + XML::parseString(img, "name");
				
				//doing this for every texture could be wasteful, however, it's never done during gameplay
				Walaber::Rect pixelRect = XML::parseRect(img, "rect");
				Vector2 pos = pixelRect.upper_left;
				Vector2 size = pixelRect.size;
				
				si.texData.width = size.X * drawScale;
				si.texData.height = size.Y * drawScale;
				
				size += pos;
				pos /= texNativeSize;
				size /= texNativeSize;
				
				si.texData.minS = pos.X;
				si.texData.minT = pos.Y;
				si.texData.maxS = size.X;
				si.texData.maxT = size.Y;
                
                si.texData.offset = XML::parseVector2(img, "offset");
                si.texData.flags |= Texture::TF_SubtextureHasOffset;
				
				// read rotation data if it's included
                //if(textureHasBeenRotated90Deg)
                //{
                //  si.texData.flags &= Texture::TF_SubtextureHasBeenRotatedNinetyDegrees;
                //}
                
				texInfoList.push_back(si);
			}
		}
		
		tempSubTexInfoMap_[ atlasTextureName ] = texInfoList;
		
		// now load the atlas texture!
        MemberCallbackPtr<TextureManager>::type memcallback(new MemberCallback<TextureManager>(this, &TextureManager::_loadAtlasGotTexture));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
		TexturePtr atlasTexture = getTexture( atlasTextureName, callback, FileManager::PP_AllowAbstraction, true );
		
	}
	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::_loadAtlasGotTexture( void* data )
	{
        Texture::TextureLoadedCallbackParameters* params = (Texture::TextureLoadedCallbackParameters*)data;
		
		
		// prepare the callback parameters now...
		AtlasLoadedCallbackParameters callbackParams;
		callbackParams.xmlPath = atlasLoadedMap_mainTextureKey_[params->path];
		callbackParams.result = ResultCode_OK;
		
		SubTexListMap::iterator it = tempSubTexInfoMap_.find(params->path);
		
		if (it != tempSubTexInfoMap_.end())
		{
			TexturePtr atlasTex = getTexture( params->path, CallbackPtr(), FileManager::PP_AllowAbstraction, true );
			
			for (std::vector< SubTexInfo >::iterator j = it->second.begin(); j != it->second.end(); ++j)
			{
				// insert subtexture into map
				SubtexturePtr newTexture(createSubtexture((*j).texName, atlasTex, (*j).texData));
				TexturePtr castTexture(static_pointer_cast<Texture>(newTexture));
				
				// A subtexture CANNOT have the same name as it's parent texture (imagelist)
				assert( (*j).texName != atlasTex->getFileName() );
				
				textureMap_.insert(std::make_pair((*j).texName, castTexture));
				
				newTexture->nullifyParent();
			}
			
			callbackParams.texture = atlasTex;
			
			tempSubTexInfoMap_.erase(params->path);
            atlasLoadedMap_[callbackParams.xmlPath] = atlasTex->getFileName();
		}
		else
		{
			callbackParams.result = ResultCode_Error;
		}
		
        unsigned int numPagesToLoad = atlasToNumPagesToLoadMap_[callbackParams.xmlPath] - 1;
        atlasToNumPagesToLoadMap_[callbackParams.xmlPath] = numPagesToLoad;
        
		if( numPagesToLoad == 0 )
        {		
            std::pair<AtlasToCallbackMap::iterator, AtlasToCallbackMap::iterator> range;
            range = atlasToCallbackMap_.equal_range(callbackParams.xmlPath);
            for(AtlasToCallbackMap::iterator it_ = range.first; it_ != range.second; ++it_)
            {
                if(it_->second)
                {
                    it_->second->invoke(&callbackParams);
                }
            }
            
            atlasToCallbackMap_.erase(range.first, range.second);
            atlasToNumPagesToLoadMap_.erase( callbackParams.xmlPath );
        }
	}
	
    
    
	
    
    
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
    void TextureManager::loadTextureSettings(const std::string& xmlFile)
    {
		PropertyList plist;
		
		MemberCallbackPtr<TextureManager>::type memberCallback( new MemberCallback<TextureManager>(this, &TextureManager::_loadedTextureSettings) );
		CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
		
		FileManager::getInstancePtr()->readFile( xmlFile, callback, plist );
	}
	
	// ------------------------------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::_loadedTextureSettings( void* params ) // const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handleName, const PropertyList& plist )
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		
        // parse the XML document.
		xmlDocPtr doc = NULL;
		xmlNodePtr root_element = XML::loadDocMemory(rcp->buffer, rcp->length, "TextureSettings", doc);
		
		
		if (rcp->buffer)
			delete[] rcp->buffer;
		
		
		if (root_element != NULL)
		{
			// this is a "TextureSettings" file... load it up...
			for (xmlNodePtr setting = root_element->children; setting; setting = setting->next)
            {
                if (xmlStrcmp(setting->name, (const xmlChar*)"Texture") == 0)
                {
                    std::string name = XML::parseString(setting, "name");
                    
                    TextureSettings ts;
                    
                    if (XML::attrExists(setting, "premultiplyAlpha"))
                        ts.setPremultiplyAlpha(XML::parseBool(setting, "premultiplyAlpha"));
					
					if (XML::attrExists(setting, "alphaClip"))
                    {
                        ts.setAlphaClip( XML::parseFloat(setting, "alphaClip") );
                    }
                    
                    if ( XML::attrExists(setting, "clipColor") )
                    {
                        ts.setClipColor( XML::parseColor8bit(setting, "clipColor" ) );
                    }
                    
                    if (XML::attrExists(setting, "clipTolerance"))
                    {
                        ts.setClipTolerance( XML::parseFloat(setting, "clipTolerance") );
                    }
                    
                    if (XML::attrExists(setting, "minFilter"))
                    {
                        std::string filter = StringHelper::toLower(XML::parseString(setting, "minFilter"));
                        if (filter == "nearest_mipmap_nearest")
                            ts.setMinFilter( GL_NEAREST_MIPMAP_NEAREST );
                        else if (filter == "linear_mipmap_nearest")
                            ts.setMinFilter( GL_LINEAR_MIPMAP_NEAREST );
                        else if (filter == "nearest_mipmap_linear")
                            ts.setMinFilter( GL_NEAREST_MIPMAP_LINEAR );
                        else if (filter == "linear_mipmap_linear")
                            ts.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
                        else if (filter == "nearest")
                            ts.setMinFilter( GL_NEAREST );
                        else if (filter == "linear")
                            ts.setMinFilter( GL_LINEAR );
                    }
                    
                    if (XML::attrExists(setting, "magFilter"))
                    {
                        std::string filter = StringHelper::toLower(XML::parseString(setting, "magFilter"));
                        
                        if (filter == "nearest")
                            ts.setMagFilter( GL_NEAREST );
                        else if (filter == "linear")
                            ts.setMagFilter( GL_LINEAR );  
                    }
                    
                    if (XML::attrExists(setting, "wrapU"))
                    {
                        std::string filter = StringHelper::toLower(XML::parseString(setting, "wrapU"));
                        if (filter == "clamp")
                            ts.setWrapU( GL_CLAMP_TO_EDGE );
                        else if (filter == "clamp_to_edge")
                            ts.setWrapU( GL_CLAMP_TO_EDGE );
                        else if (filter == "repeat")
                            ts.setWrapU( GL_REPEAT );
                    }
                    
                    if (XML::attrExists(setting, "wrapV"))
                    {
                        std::string filter = StringHelper::toLower(XML::parseString(setting, "wrapV"));
                        if (filter == "clamp")
                            ts.setWrapV( GL_CLAMP_TO_EDGE );
                        else if (filter == "clamp_to_edge")
                            ts.setWrapV( GL_CLAMP_TO_EDGE );
                        else if (filter == "repeat")
                             ts.setWrapV( GL_REPEAT );
                    }
					
					if (XML::attrExists(setting, "colorspace"))
					{
						std::string cs = StringHelper::toLower(XML::parseString(setting, "colorspace"));
						
						if ((cs == "r5g6b5") || (cs == "rgb565"))
							ts.setColorspace( TextureInMemoryColorspace_16_R5_G6_B5 );
						else if ((cs == "r5g5b5a1") || (cs == "rgba5551"))
							ts.setColorspace( TextureInMemoryColorspace_16_R5_G5_B5_A1 );
						else if ((cs == "r4g4b4a4") || (cs == "rgba4444"))
							ts.setColorspace( TextureInMemoryColorspace_16_R4_G4_B4_A4 );
					}
                    
					if (XML::attrExists(setting, "alwaysInMemory"))
					{
						ts.setStayInMemory(XML::parseBool(setting, "alwaysInMemory"));
					}
					
					if (XML::attrExists(setting, "uncompressedExt"))
					{
						ts.setUncompressedExt( XML::parseString(setting, "uncompressedExt") );
					}
                    
#ifdef _DEBUG
                    printf("loadTextureSettings... added settings for [%s] : ", name.c_str());
                    ts.debugLog();
                    printf("\n");
#endif
                    
					// store these settings, and apply settings if this texture is already loaded.
					// if texture is NOT yet loaded, don't leave the "reload" and "reapply" flags turned ON.
					if (textureMap_.find(name) == textureMap_.end())
					{
						ts.reloaded();
						ts.reapplied();
					}
					
					setTextureSettings( name, ts );
                }
            }
		}
		
		XML::cleanupDoc(doc);

    }
    
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	TexturePtr TextureManager::getTexture(const std::string& textureName, CallbackPtr callback, FileManager::PlatformPolicy policy, bool skipLoad) 
    {
		std::string tsName = StringHelper::removeExtension(textureName);
		std::string ext = StringHelper::getExtension(textureName);
		
		std::string texName = textureName;
		TextureSettingsMap::iterator itSet = textureSettingsMap_.find(tsName);
		
		if( !_isTextureSupported( ext ) )
		{
            if (itSet != textureSettingsMap_.end())
            {
				std::string newExt = itSet->second.getUncompressedExt();
				if( !newExt.empty() )
				{
					texName = tsName + std::string(".") + newExt;
				}
			}			
		}
		
		TextureMap::iterator it = textureMap_.find(texName);		
		
		if (it == textureMap_.end()) 
        {
			Texture2DPtr newTexture;
            
            // try settings previously stored texture settings...
            if (itSet != textureSettingsMap_.end())
            {
				itSet->second.reloaded();
				itSet->second.reapplied();
                newTexture = createTexture2D(texName, itSet->second);
            }
            else
            {
				// add default texture settings now...
				textureSettingsMap_[ tsName ] = TextureSettings(TextureSettings());
				itSet = textureSettingsMap_.find(tsName);
				
                newTexture = createTexture2D(texName);
            }

			TexturePtr castTexture(static_pointer_cast<Texture>(newTexture));
			textureMap_.insert(std::make_pair(texName, castTexture));
            
			// in the map now, so load from disk!
			if (!skipLoad)
			{ 
				newTexture->loadFromFile(callback, policy);
			}
			else 
			{
				// texture needs to be loaded the first time it's actually accessed
				// (because we're skipping it now)
				itSet->second.setShouldReload(true);
				
				// if we're skipping loading, call the callback now.
				if (callback)
				{
					Texture::TextureLoadedCallbackParameters params(texName, ResultCode_OK);
					callback->invoke( &params );
				}
			}
			
			return newTexture;
		}
		else
		{
			bool alreadyLoaded = true;
			
			TexturePtr theTexture = it->second;
				
            if(theTexture->getTextureType() == TT_Subtexture)
            {
                SubtexturePtr subt(static_pointer_cast<Subtexture>(theTexture));
				if(subt->isParentNull() || subt->parentNotLoaded())
				{
					alreadyLoaded = false;
					subt->reloadParent(callback);
					
					// find all other sub-textures with this parent, and notify them as well.
					for (TextureMap::iterator it_s = textureMap_.begin(); it_s != textureMap_.end(); it_s++)
					{
						if (it_s->second->getTextureType() == TT_Subtexture)
						{
							SubtexturePtr subtex(static_pointer_cast<Subtexture>(it_s->second));
							if (subtex->getParentPath() == subt->getParentPath())
								subtex->reloadParent();
						}
					}
				}
				else
				{
					std::string parentPath = subt->getParentPath();
					std::string parentTS = StringHelper::removeExtension(parentPath);
					
					if (itSet != textureSettingsMap_.end())
					{
						if (itSet->second.getShouldReload())
						{
							alreadyLoaded = false;
							itSet->second.reloaded();
							
							TexturePtr parentTex = getTexture( parentPath );
							parentTex->setSettings(itSet->second);
							
							// we need to manually reload the parent and the reloadParent 
							// should just find the reloaded parent
							parentTex->reload( CallbackPtr() );
							subt->reloadParent(callback);
							
							// find all other sub-textures with this parent, and notify them as well.
							for (TextureMap::iterator it_s = textureMap_.begin(); it_s != textureMap_.end(); it_s++)
							{
								if (it_s->second->getTextureType() == TT_Subtexture)
								{
									SubtexturePtr subtex(static_pointer_cast<Subtexture>(it_s->second));
									if (subtex->getParentPath() == parentPath)
										subtex->reloadParent();
								}
							}
							
						}
						else if (itSet->second.getShouldReapply())
						{
							itSet->second.reapplied();
														
							TexturePtr parentTex = getTexture( parentPath );
							parentTex->setSettings(itSet->second);	
							
							Texture2DPtr tex2d( static_pointer_cast<Texture2D>(parentTex) );
							tex2d->reapplySettings();
						}
					}
				}
            }
			else 
			{
				// if the settings are dirty, force a reload!
				TextureSettingsMap::iterator it_ = textureSettingsMap_.find(tsName);
				if (it_ != textureSettingsMap_.end())
				{
					if (it_->second.getShouldReload() && (!skipLoad))
					{
						alreadyLoaded = false;
						it_->second.reloaded();
						theTexture->setSettings(it_->second);
						
						Texture2DPtr castTexture(static_pointer_cast<Texture2D>(theTexture));
						castTexture->reload(callback, policy);
					}
					else if (it_->second.getShouldReapply())
					{
						it_->second.reapplied();
						theTexture->setSettings(it_->second);
						
						Texture2DPtr tex2d(static_pointer_cast<Texture2D>(theTexture));
						tex2d->reapplySettings();
					}
				}
			}
			
			
			
			if (alreadyLoaded && callback)
            {
                Texture::TextureLoadedCallbackParameters params(texName, ResultCode_OK);
				callback->invoke( &params );
			}
			return theTexture;
		}
	}
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::reloadTextureFromDisk(const std::string& textureName, CallbackPtr callback)
	{
		std::string ext = StringHelper::getExtension(textureName);
		std::string texName = textureName;

		if( !_isTextureSupported( ext ) )
		{
			std::string tsName = StringHelper::removeExtension(textureName);
			TextureSettingsMap::iterator it = textureSettingsMap_.find(tsName);
            if (it != textureSettingsMap_.end())
            {
				std::string newExt = it->second.getUncompressedExt();
				if( !newExt.empty() )
				{
					texName =  tsName + std::string(".") + newExt;
				}
			}			
		}
		
		TexturePtr tex = textureMap_[texName];
        if(tex->getTextureType() == TT_Subtexture)
        {
            SubtexturePtr subt(static_pointer_cast<Subtexture>(tex));
			subt->reloadParent(callback);
		}
		else 
		{			
			Texture2DPtr newTexture(createTexture2D(texName));
			TexturePtr castTexture(static_pointer_cast<Texture>(newTexture));
			textureMap_[texName] = castTexture;
			
			newTexture->loadFromFile(callback);
		}		
	}

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::reloadTextures()
	{
		TextureMap::iterator p;

		// reload all textures
		for(p = textureMap_.begin(); p != textureMap_.end(); ++p)
		{
			p->second->reload( CallbackPtr() );
		}

		// now that textures are loaded, update subtextures to point to new gl textures
		for(p = textureMap_.begin(); p != textureMap_.end(); ++p)
		{
			if(p->second->getTextureType() == TT_Subtexture)
			{
				SubtexturePtr subt(static_pointer_cast<Subtexture>(p->second));
				subt->reloadParent(CallbackPtr());
			}
		}
	}

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::invalidateTextures()
	{
		for (TextureMap::iterator i = textureMap_.begin(); i != textureMap_.end(); ++i)
		{
			i->second->invalidate();
		}

		// set the iterator used during texture reloading at the beginning of texturemap
		lastValidatedTexture_ = 0;
	}

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	size_t TextureManager::countInvalidateTextures() const
	{
		// Note: sub-textures are not considered to be invalid even if their name is set to NO_TEXTURE
		// as only their parents need to be reloaded

		size_t ret = 0;
		for (TextureMap::const_iterator i = textureMap_.begin(); i != textureMap_.end(); ++i)
		{
			const TexturePtr& tex = i->second;

			if ( ( ! tex->isValid() ) && tex->getTextureType() != TT_Subtexture)
				ret++;
		}

		return ret;
	}

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	bool TextureManager::reloadNextInvalidTexture()
	{		
		// Note: sub-textures are not considered to be invalid even if their name is set to NO_TEXTURE
		// as only their parents need to be reloaded
		Texture* tex = 0;

		Logger::printf("Walaber", Logger::SV_DEBUG, "Validating texture: %i", lastValidatedTexture_);
		
		if ( lastValidatedTexture_ >= textureMap_.size() )
			return false;


		TextureMap::iterator beginning = textureMap_.begin();
		std::advance(beginning, lastValidatedTexture_++);

		// always go to another texture, even if it does not reload successfully
		for (TextureMap::iterator i = beginning; i != textureMap_.end(); ++i)
		{

			if ( ( ! i->second->isValid() ) && i->second->getTextureType() != TT_Subtexture)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "Getting texture!");
				
				tex = i->second.get();
				break;
			}
		}

		if (tex)
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "Reloading texture: %s", tex->getFileName().c_str());
			
			tex->reload( CallbackPtr() );
		}

		Logger::printf("Walaber", Logger::SV_DEBUG, "Leaving reloadNextInvalidTexture");
		
		return tex != 0;
	}

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::reloadSubTextures()
	{
		for(TextureMap::iterator i = textureMap_.begin(); i != textureMap_.end(); ++i)
		{
			if(i->second->getTextureType() == TT_Subtexture)
			{
				SubtexturePtr subt(static_pointer_cast<Subtexture>(i->second));
				subt->reloadParent(CallbackPtr());
			}
		}
	}
    
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
    void TextureManager::setTextureSettings(const std::string& textureName, TextureSettings ts)
    {
		// if this texture is already loaded, notify it of its settings.
		std::string tsName = StringHelper::removeExtension(textureName);
		
		// try all supported extensions...
		for (unsigned int i = 0; i < mSupportedExtensionsCount; i++)
		{
			std::string testName = tsName + mSupportedExtensions[i];
			
			TextureMap::iterator it = textureMap_.find(testName);
			if (it != textureMap_.end())
			{
				// if we need to reload, do it now!
				if (ts.getShouldReload())
				{
					ts.reloaded();
					it->second->setSettings(ts);
					
					Texture2DPtr castTexture( static_pointer_cast<Texture2D>(it->second) );
					castTexture->reload(CallbackPtr(), FileManager::PP_AllowAbstraction);
					
				}
				
				it->second->setSettings( ts );
				
				break;
			}
		}
		
		// add / update entry!
		textureSettingsMap_[tsName] = ts;
        
			
    }
	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
    TextureSettings TextureManager::getTextureSettings(const std::string& textureName)
	{
		std::string texName = StringHelper::removeExtension(textureName);
		
		TextureSettingsMap::iterator it = textureSettingsMap_.find(texName);
		
		// if no settings exist, insert a default entry into the map before returning!
		if (it == textureSettingsMap_.end())
		{
			textureSettingsMap_[texName] = TextureSettings();
			
			it = textureSettingsMap_.find(texName);
		}
		
		return it->second;	
	}
    
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::clearTexture(std::string textureName) 
	{
		std::string ext = StringHelper::getExtension(textureName);
		std::string texName = textureName;
		
		if( !_isTextureSupported( ext ) )
		{
			std::string tsName = StringHelper::removeExtension( textureName );
			TextureSettingsMap::iterator it = textureSettingsMap_.find(tsName);
            if (it != textureSettingsMap_.end())
            {
				std::string newExt = it->second.getUncompressedExt();
				if( !newExt.empty() )
				{
					texName = tsName + std::string(".") + newExt;
				}
			}			
		}
		
		if (textureMap_.find(textureName) != textureMap_.end())
		{
			TexturePtr theTexture = textureMap_[textureName];
		
			textureMap_.erase(textureName);
			theTexture.reset();
		}
	}
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::clearCache(void) 
	{
		textureMap_.clear();
        atlasLoadedMap_.clear();
        atlasToCallbackMap_.clear();
	}
	
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::memoryWarning(bool forceAll) 
	{
		if (ignoreMemoryWarnings_)
		{
			printf("TextureManager received memory warning, but ignoreMemoryWarning was set!\n");
			return;
		}
		
		printf("Memory Warning (%d) cache count: %ld\n", forceAll, this->textureMap().size());
		
		_clearSubtextureParents();
		_removeUnusedTextures(forceAll);
		
		// we may have removed the currently bound OpenGL texture from memory, so notify our GraphicsGL class that it should reset it's cache.
		GraphicsGL::invalidateTextures();
		
		printf("Cache count after warning: %ld\n", this->textureMap().size());
	}
	
    
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::_clearSubtextureParents(void)
	{
		TextureMap::iterator p;
		std::vector<TextureMap::key_type> deleteList;
		
		for(p = textureMap_.begin(); p != textureMap_.end(); ++p) 
		{
			TextureMap::key_type key = p->first;
			
			if (p->second.unique()) 
			{
                if(p->second->getTextureType() == TT_Subtexture)
                {
                    SubtexturePtr subt(static_pointer_cast<Subtexture> (p->second));
					subt->nullifyParent();
				}
			}
		}		
	}
	
    
    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::_removeUnusedTextures(bool forceAll)
	{
        TextureMap::iterator pTemp;
		TextureMap::iterator p;
		
		for(p = textureMap_.begin(); p != textureMap_.end();) 
		{
			TextureMap::key_type key = p->first;
			
#ifdef _DEBUG
			printf("%s use count: %d\n", key.c_str(), (int)p->second.use_count());
#endif
			
			if (p->second.unique() && p->second->getTextureType() != TT_Subtexture) 
			{
				if (!forceAll)
				{
					if (p->second->getSettings().getStayInMemory())
					{
#ifdef _DEBUG
						printf("Texture %s is set to STAY IN MEMORY. skipping...\n", p->second->getFileName().c_str());
#endif
						++p;
						continue;
					}
				}
				
				TextureMap::value_type::second_type texture = p->second;
				printf("Unloading %s\n", texture->getFileName().c_str());
                pTemp = p;
                ++p;
                textureMap_.erase(pTemp);
			}
            else
            {
                ++p;
            }
		}	
	}

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::dumpTextures() const
	{
		Logger::printf("Walaber", Logger::SV_DEBUG, "===============[ Texture Dump ]=============================\n");
		for (TextureMap::const_iterator i = textureMap_.begin(); i != textureMap_.end(); ++i)
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "%10u   %s\n", i->second->getName(), i->second->getFileName().c_str());
		}
		Logger::printf("Walaber", Logger::SV_DEBUG, "===============[ End of Texture Dump ]======================\n");

	}

	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	unsigned int TextureManager::getMemory()
	{
		unsigned int total = 0;
		
#ifdef _DEBUG
		std::multimap<unsigned int, std::string> memMap;
#endif
		Logger::printf("Walaber", Logger::SV_DEBUG, "===============[ Texture Memory ]=============================\n");
		for (TextureMap::const_iterator i = textureMap_.begin(); i != textureMap_.end(); ++i)
		{
			if (i->second->getTextureType() == TT_Subtexture)
				continue;
			
			unsigned int thisTex = i->second->getMemoryInBytes();
			total += thisTex;
			
#ifdef _DEBUG
			memMap.insert(std::pair< unsigned int, std::string >(thisTex, i->second->getFileName()));
#endif
		}
		
#ifdef _DEBUG
		for (std::multimap<unsigned int, std::string>::iterator it = memMap.begin(); it != memMap.end(); ++it)
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "%d\t%x\t%s\n", it->first, it->first, it->second.c_str());
		}
#endif
		
		Logger::printf("Walaber", Logger::SV_DEBUG, " TOTAL TEXTURE MEMORY: %d : %x\n", total, total);
		Logger::printf("Walaber", Logger::SV_DEBUG, "===============[ End of Texture Memory ]======================\n");

		return total;		
	}
	

    // ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	std::string TextureManager::getTextureFileName(GLuint texId) const
	{
		for (TextureMap::const_iterator i = textureMap_.begin(); i != textureMap_.end(); ++i)
		{
			if (i->second->getName() == texId)
				return i->second->getFileName();
		}

		return "";
	}
	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	bool TextureManager::_isTextureSupported( const std::string& extension )
	{
		bool ret = false;
		for( int i = mSupportedExtensionsCount-1; i >= 0; --i )
		{
			if( extension == mSupportedExtensions[i] )
			{
				ret = true;
				break;
			}
		}
		
		return ret;
	}
	
	// ------------------------------------------------------------------------------------------------------------------------------------
    // ------------------------------------------------------------------------------------------------------------------------------------
	void TextureManager::_createErrorTexture()
	{
		errorTexture_ = createProgrammaticTexture2D("ERROR_TEX", 8, 8);
		Texture::buffer_type_ptr data = errorTexture_->create_empty_buffer();
		
		// set the data...
		Color empty = Color(255,25,255);
		Color full = Color(255,26,85);
		
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				bool filled = (((y/2) % 2 == 0) ? (((x/2) % 2 == 0) ? true : false) : (((x/2) % 2 == 0) ? false : true));
				
				(*data)[ y * 8 + x ] = (filled ? full : empty);
			}
		}
		
		errorTexture_->setData(data);
	}
	
}



