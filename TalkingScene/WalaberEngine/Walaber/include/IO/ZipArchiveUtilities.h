//
//  ZipArchiveUtilities.h
//  WalaberEngine
//
//  Created by Jason Pecho on 10/13/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _ZIP_ARCHIVE_UTILITIES_H_
#define _ZIP_ARCHIVE_UTILITIES_H_

#include <XML.h>

namespace Walaber
{     
    class ZipUtilities
    {
    public:
        static void LoadArchiveDescription(const std::string& xmlPath, const std::string& userStoreBasePath, const bool deleteAfterLoad = true);
    private:
        static void _xmlFileRead(void* data);
        static void _handleTextures(XMLDocument::NamedIterator textureIterator);
        static void _handleAtlases(XMLDocument::NamedIterator atlasIterator);
        static void _handleSounds(XMLDocument::NamedIterator soundIterator);
        static void _handleStreamedTracks(XMLDocument::NamedIterator trackIterator);
        static void _handleSQLFile(XMLDocument::NamedIterator sqlItertator);
        static void _fileReadCallback(void* data);
		
        static void _sqlScriptRead(void* data);
        
        static std::string mUserStoreBasePath;
    };    
}



#endif
