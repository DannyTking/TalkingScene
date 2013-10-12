#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <deque>
#include <vector>
#include <list>
#include <string>
#include <map>

#include <Texture.h>
#include <ProgrammaticTexture.h>

#include <XMLHelper.h>

#include <BroadcastManager.h>
#include <Callback.h>
#include <Utilities.h>

#define ATLAS_FILE_READ_ACTION_KEY "atlas_action_key"

namespace Walaber 
{
	class TextureManager : Utilities::noncopyable {
	public:
        // ------------------------------------------------------------------------------------------------------------------------------------
        struct AtlasLoadedCallbackParameters
        {
			ResultCode result;
            std::string xmlPath;
            TexturePtr texture;
            
			AtlasLoadedCallbackParameters() :
			result(ResultCode_OK),
			xmlPath(""),
			texture()
			{}
			
            AtlasLoadedCallbackParameters(const std::string& ppath, TexturePtr tex):
			result(ResultCode_OK),
            xmlPath(ppath),
            texture(tex)
            {}
        };
		typedef std::map<std::string, TexturePtr> TextureMap;
        typedef std::map<std::string, TextureSettings>  TextureSettingsMap;
        typedef std::map< std::string, std::string > StringToStringMap;
        typedef std::multimap< std::string, CallbackPtr > AtlasToCallbackMap;


        // ------------------------------------------------------------------------------------------------------------------------------------
		static TextureManager& getManager(void)
		{
			if (!inst_) 
            {
				inst_ = new TextureManager;
			}
			return (*inst_);
		}
		
        
        // ------------------------------------------------------------------------------------------------------------------------------------
        void reloadTextureAtlas(const std::string& xmlFile, CallbackPtr callback = CallbackPtr());
        
		// ------------------------------------------------------------------------------------------------------------------------------------
        bool unloadTextureAtlas(const std::string& xmlFile);
        
		// ------------------------------------------------------------------------------------------------------------------------------------
		void loadTextureAtlas(const std::string& xmlFile, CallbackPtr callback = CallbackPtr());
        
		// ------------------------------------------------------------------------------------------------------------------------------------
        void unloadAllTextureAtlases();
		
        
        // ------------------------------------------------------------------------------------------------------------------------------------
		void loadTextureSettings(const std::string& xmlFile);
        
        
		// ------------------------------------------------------------------------------------------------------------------------------------
		TexturePtr getTexture(const std::string& textureName, CallbackPtr callback = CallbackPtr(), FileManager::PlatformPolicy policy = FileManager::PP_AllowAbstraction, bool skipLoad = false);
		
        
		// ------------------------------------------------------------------------------------------------------------------------------------
		void reloadTextureFromDisk(const std::string& textureName, CallbackPtr callback = CallbackPtr());
        
        // ------------------------------------------------------------------------------------------------------------------------------------
		void setTextureSettings(const std::string& textureName, TextureSettings ts);
		TextureSettings getTextureSettings(const std::string& textureName); 
        
        // ------------------------------------------------------------------------------------------------------------------------------------
		void clearTexture(std::string textureName);
        
        // ------------------------------------------------------------------------------------------------------------------------------------
		void clearCache(void);
        
        // ------------------------------------------------------------------------------------------------------------------------------------
		void memoryWarning(bool forceAll = false);
        
        // ------------------------------------------------------------------------------------------------------------------------------------
		void setIgnoreMemoryWarnings( bool ignore ) { ignoreMemoryWarnings_ = ignore; }
			
        // ------------------------------------------------------------------------------------------------------------------------------------
		TextureMap& textureMap(void) { return textureMap_; }
        
		
		// ------------------------------------------------------------------------------------------------------------------------------------
		// returns the total amount of memory used by all textures that are currently loaded.
		// in DEBUG, also prints out the memory, sorted by size.
		unsigned int getMemory();
        
        // callbacks
        void _loadedTextureSettings( void* params ); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handleName, const PropertyList& plist );
		void _loadedTextureAtlas( void* params ); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& plist);	
		void _loadAtlasGotTexture( void* data);
        
		void reloadTextures();

		void invalidateTextures();
		size_t countInvalidateTextures() const;
		bool reloadNextInvalidTexture();
        void reloadSubTextures();
        void dumpTextures() const;

        std::string getTextureFileName(GLuint texId) const;
		
		
		ProgrammaticTexture2DPtr getErrorTexture() { return errorTexture_; }
		
		
    private:
		
		bool _isTextureSupported( const std::string& extension );
		
		void _createErrorTexture();
		
		
		static const std::string mSupportedExtensions[];
		static const int mSupportedExtensionsCount;
		
		// ------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------
		struct SubTexInfo
		{
			std::string		texName;
			TextureData		texData;
		};
		
		typedef std::map< std::string, std::vector<SubTexInfo> >		SubTexListMap;
        typedef std::map< std::string, unsigned int >                   StringToUIntMap;

		
        // ------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------
		TextureMap					textureMap_;
		size_t						lastValidatedTexture_;
        TextureSettingsMap          textureSettingsMap_;
		static TextureManager*		inst_;
		bool						ignoreMemoryWarnings_;
		SubTexListMap				tempSubTexInfoMap_;
        StringToStringMap           atlasLoadedMap_;
        StringToStringMap           atlasLoadedMap_mainTextureKey_;
        AtlasToCallbackMap          atlasToCallbackMap_;
		StringToUIntMap            atlasToNumPagesToLoadMap_;
		
		ProgrammaticTexture2DPtr	errorTexture_;

		
		
        // ------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------
		TextureManager();
		~TextureManager();
		
		// ------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------
		void _loadImageList(xmlDocPtr doc, xmlNodePtr root_element, const std::string& atlasPath);
		void _loadImageListDetails(xmlDocPtr doc, xmlNodePtr root_element, const std::string& atlasPath);
        void _unloadTextureAndSubTextures(const std::string& texture);
        
		// ------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------
		//void _addSubtextureToMap(xmlNodePtr node, const std::string& textureBasePath, const std::vector<std::string>& atlasTextureNames, const Vector2& nativeTexSize);
		
        // ------------------------------------------------------------------------------------------------------------------------------------
        // ------------------------------------------------------------------------------------------------------------------------------------
		void _clearSubtextureParents(void);
		void _removeUnusedTextures(bool forceAll = false);
	};

}


#endif	// _TEXTURE_MANAGER_H_
