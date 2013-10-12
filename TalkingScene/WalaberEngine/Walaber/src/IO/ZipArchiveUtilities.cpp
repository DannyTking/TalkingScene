//
//  ZipArchiveUtilities.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 10/13/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <ZipArchiveUtilities.h>

#include <Callback.h>
#include <FileManager.h>
#include <TextureManager.h>
#include <SoundManager.h>
#include <FileHelper.h>
#include <DatabaseManager.h>
#include <StringHelper.h>

namespace Walaber
{
    std::string ZipUtilities::mUserStoreBasePath;
    
    void ZipUtilities::LoadArchiveDescription(const std::string& xmlPath, const std::string& userStoreBasePath, const bool deleteAfterLoad)
    {
        mUserStoreBasePath = userStoreBasePath;
        
        CallbackPtr callback(new Callback(ZipUtilities::_xmlFileRead));

        PropertyList plist;
        plist.setValueForKey("delete_file", Property(deleteAfterLoad ? 1 : 0));
        FileManager::getInstancePtr()->readFile(xmlPath,callback, plist);
    }

    void ZipUtilities::_xmlFileRead(void* data)
    {
        FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
        
        if(params->buffer)
        {
            XMLDocument xmlDoc(params->buffer, params->length, "ArchiveDescription");
            
            if(xmlDoc)
            {
                XMLDocument::Iterator filesIt = xmlDoc.getIteratorTo("Files");
                
                if(filesIt)
                {
                    XMLDocument::Iterator fileTypesIt = filesIt.getChildIterator();
                    
                    while(fileTypesIt)
                    {
                        std::string name = fileTypesIt.getNodeName();
                        if(name == "Sounds")
                        {
                            _handleSounds(fileTypesIt.getChildNamedIterator("Sound"));
                        }
                        else if(name == "Textures")
                        {
                            _handleTextures(fileTypesIt.getChildNamedIterator("Texture"));
                        }
                        else if(name == "Atlases")
                        {
                            _handleAtlases(fileTypesIt.getChildNamedIterator("Atlas"));
                        }
                        else if(name == "StreamedTracks")
                        {
                            _handleStreamedTracks(fileTypesIt.getChildNamedIterator("Track"));
                        }
                        else if(name == "SQLScripts")
                        {
                            _handleSQLFile(fileTypesIt.getChildNamedIterator("Script"));
                        }
                        
                        ++fileTypesIt;
                    }
                }
                
                filesIt = xmlDoc.getIteratorTo("DeleteFiles");
                
                if(filesIt)
                {
                    XMLDocument::NamedIterator fileToDelete = filesIt.getChildNamedIterator("DeleteFile");
                    
                    while(fileToDelete)
                    {
                        // delete the file with attribute path
                        Property prop;
                        if(fileToDelete.getAttribute("path", prop))
                        {
                            // delete the file at path in prop
                            std::string path = prop.asString();
                            
                            if(fileToDelete.getAttribute("relative", prop))
                            {
                                path = mUserStoreBasePath + path;
                            }
							
							if(fileToDelete.getAttribute("version", prop))
							{
								// look for the version file of this amps file
								PropertyList plist;
								plist.setValueForKey("file_path", Property(path));
								plist.setValueForKey("version", prop);
								plist.setValueForKey("delete_file", Property(1));
								
								std::string versionPath = StringHelper::changeExtension(path, ".ver");
								
								CallbackPtr callback(new Callback(&ZipUtilities::_fileReadCallback));
								FileManager::getInstancePtr()->readFile(versionPath, callback, plist, FileManager::PP_NoAbstraction );
							}
							else
							{
								if(!FileHelper::deleteFile(path))
								{
									printf("Delete failed for file at path: %s", path.c_str());
								}								
							}      					
                        }
                        
                        ++fileToDelete;
                    }
                }
            }
            
            delete [] params->buffer;
            
            if(params->userData.getValueForKey("delete_file")->asInt())
            {
                FileHelper::deleteFile(params->actualPath);
            }
        }        
    }
    
    void ZipUtilities::_handleTextures(XMLDocument::NamedIterator textureIterator)
    {
        Property property;
        while(textureIterator)
        {
            // do some stuff to the textures!
            if(textureIterator.getAttribute("reload", property))
            {
                if(property.asInt())
                {
                    // reload the texture!!!
                    if(textureIterator.getAttribute("path", property))
                    {
						const std::string tex = property.asString();
                        TextureManager::getManager().reloadTextureFromDisk(tex);
                    }
                }
            }
            ++textureIterator;
        }
    }
    
    void ZipUtilities::_handleAtlases(XMLDocument::NamedIterator atlasIterator)
    {
        Property property;
        while(atlasIterator)
        {
            bool reloadAtlas = true;
            // reload the atlases since they are loaded at the beginning
            if(atlasIterator.getAttribute("reload", property))
            {
                if(!property.asInt())
                {
                    // don't reload the atlas!!!
                    reloadAtlas = false;
                }
            }
            
            if(reloadAtlas)
            {
                // reload the atlas
                if(atlasIterator.getAttribute("path", property))
                {
                    TextureManager::getManager().reloadTextureAtlas(property.asString());
                }
            }
            
            ++atlasIterator;
        }
    }
    
    void ZipUtilities::_handleSounds(XMLDocument::NamedIterator soundIterator)
    {
        Property property;
        while(soundIterator)
        {
            bool load_now = false;
            // do something with the sounds
            if(soundIterator.getAttribute("reload", property))
            {
                if(property.asInt())
                {
                    // reload the sound!!!
                    load_now = true;
                }
            }
            
            if(soundIterator.getAttribute("path", property))
            {
                SoundManager::getInstancePtr()->refreshSound(property.asString(), load_now );
            }
            
            ++soundIterator;
        }
    }
    
    void ZipUtilities::_handleStreamedTracks(XMLDocument::NamedIterator trackIterator)
    {
        Property property;
        while(trackIterator)
        {            
            if(trackIterator.getAttribute("path", property))
            {
                SoundManager::getInstancePtr()->refreshStreamedTrack(mUserStoreBasePath + property.asString());
            }
            
            ++trackIterator;
        }
    }
    
    void ZipUtilities::_handleSQLFile(XMLDocument::NamedIterator sqlItertator)
    {
        Property property;
        while(sqlItertator)
        {   
			if(sqlItertator.getAttribute("databaseKey", property))
			{
				int databaseKey = property.asInt();
				
				if(sqlItertator.getAttribute("path", property))
				{
					DatabaseManager::runSQL(databaseKey, property.asString(), CallbackPtr(new Callback(&ZipUtilities::_sqlScriptRead)));
				}
			}
            
            ++sqlItertator;
        }
    }
    
    void ZipUtilities::_sqlScriptRead(void* data)
    {
        DatabaseManager::RunSqlScriptCompletedCallbackParameters* params = (DatabaseManager::RunSqlScriptCompletedCallbackParameters*)data;
        
        // delete update file if successful
        if(params->errorCount == 0)
        {
            FileHelper::deleteFile(params->actualPath);
        }      
        
    }
	
	void ZipUtilities::_fileReadCallback(void* data)
	{
		FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
		
		bool shouldDelete = true;
		
		if(params->buffer)
		{
			if(params->userData.keyExists("delete_file"))
			{
				XMLDocument xmlDoc(params->buffer, params->length, "Version");
				
				if(xmlDoc)
				{
					XMLDocument::Iterator iter = xmlDoc.getRootIterator();
					Property prop;
					if(iter.getAttribute("number", prop))
					{
						if(params->userData.keyExists("version"))
						{
							int versionInUpdate = params->userData.getValueForKey("version")->asInt();
							// check to see if the version number in the version file is less than 
							// the update version
							if( prop.asInt() >= versionInUpdate )
							{
								shouldDelete = false;
							}
						}
					}
				}
			}
			
			delete [] params->buffer;
		}
		
		if(params->userData.keyExists("delete_file"))
		{
			if(shouldDelete)
			{
				std::string pathToDelete = params->userData.getValueForKey("file_path")->asString();
				if(!FileHelper::deleteFile(pathToDelete))
				{
					printf("Delete failed for file at path: %s", pathToDelete.c_str());
				}
			}
		}		
	}
}
