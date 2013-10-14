//
//  SkeletonAnimationManagercpp
//  WalaberEngine
//
//  Created by Pecho, Jason on 4/19/13.
//
//

#include "SkeletonAnimationManager.h"

#include <Skeleton.h>
#include <MemberCallback.h>
#include <CurveManager.h>
#include <XML.h>
#include <BinaryReader.h>
#include <AnimationBinary.h>

namespace Walaber
{
    const std::string SkeletonAnimationManager::ANIMATION_LIST_POINTER_KEY = "anilist_pointer";
    std::set< std::string> SkeletonAnimationManager::tempAnimationsToLoad;
    
    // ---------------------------------------------------------------------------------------------

    SkeletonAnimationManager::SkeletonAnimationManager():
    AnimationManager(),
    mSkeleton(0)
    {
        
    }
    
    // ---------------------------------------------------------------------------------------------
    
    SkeletonAnimationManager::SkeletonAnimationManager( Skeleton* skeleton):
    AnimationManager(),
    mSkeleton( skeleton )
    {
        
    }

    // ---------------------------------------------------------------------------------------------
    
    SkeletonAnimationManager::SkeletonAnimationManager(const SkeletonAnimationManager& aniMgr):
    AnimationManager( aniMgr )
    {
        
    }    

    // ---------------------------------------------------------------------------------------------
    
    SkeletonAnimationManager& SkeletonAnimationManager::operator=(const SkeletonAnimationManager& aniMgr)
    {
        AnimationManager::operator=( aniMgr );
        
        mSkeleton = aniMgr.mSkeleton;
        
        return *this;
    }
   
    // ---------------------------------------------------------------------------------------------
    
    SkeletonAnimationManager::~SkeletonAnimationManager()
    {
        
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void SkeletonAnimationManager::setSkeleton( Skeleton* skeleton )
    {
        mSkeleton = skeleton;
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void SkeletonAnimationManager::loadAnimationsFromXml( const std::string& xmlPath,
                                                          const AnimationLoadBehavior loadBehavior )
    {
        
        _handleLoadBehavior( loadBehavior );
        
        PropertyList plist;
		CallbackPtr callback = CreateMemberCallbackPtr( *this,
                                    &SkeletonAnimationManager::_animationsHaveLoaded );
		
        FileManager::getInstancePtr()->readFile(xmlPath, callback, plist, FileManager::PP_NoAbstraction);
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void SkeletonAnimationManager::loadAnimationsFromXml( const std::string& xmlPath,
                                                          const AnimationLoadBehavior loadBehavior,
                                                          const std::set< std::string >& animationsToLoad )
    {        
        _handleLoadBehavior( loadBehavior );
        
        std::set< std::string >* copy = new std::set< std::string >( animationsToLoad );
        
        PropertyList plist;
        plist.setValueForKey( ANIMATION_LIST_POINTER_KEY, Property( copy ) );
		CallbackPtr callback = CreateMemberCallbackPtr( *this,
                                                       &SkeletonAnimationManager::_animationsHaveLoaded );
		
        FileManager::getInstancePtr()->readFile(xmlPath, callback, plist, FileManager::PP_NoAbstraction);
    }
    
    // ---------------------------------------------------------------------------------------------

    void SkeletonAnimationManager::playCurrentAnimation()
    {
        AnimationManager::playCurrentAnimation();
        
        mSkeleton->playSpriteAnimations();
    }
    
    // ---------------------------------------------------------------------------------------------

    void SkeletonAnimationManager::pauseCurrentAnimation()
    {
        AnimationManager::pauseCurrentAnimation();
        
        mSkeleton->pauseSpriteAnimations();
    }
    
    // ---------------------------------------------------------------------------------------------

    void SkeletonAnimationManager::loadAnimationsFromBin( const std::string& binPath,
                                                          const AnimationLoadBehavior loadBehavior )
    {
        _handleLoadBehavior( loadBehavior );
        
        PropertyList plist;
		CallbackPtr callback = CreateMemberCallbackPtr( *this,
                                                       &SkeletonAnimationManager::_animationBinHasLoaded );
		
        FileManager::getInstancePtr()->readFile( binPath, callback, plist );
    }
    
    // ---------------------------------------------------------------------------------------------

    void SkeletonAnimationManager::loadAnimationsFromBin( const std::string& binPath,
                                                          const AnimationLoadBehavior loadBehavior,
                                                          const std::set< std::string >& animationsToLoad
                                                        )
    {
        _handleLoadBehavior( loadBehavior );
        
        tempAnimationsToLoad = animationsToLoad;
        
        PropertyList plist;
		CallbackPtr callback = CreateMemberCallbackPtr( *this,
                                                       &SkeletonAnimationManager::_animationBinHasLoaded );
		
        FileManager::getInstancePtr()->readFile( binPath, callback, plist, FileManager::PP_NoAbstraction );
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void SkeletonAnimationManager::_animationsHaveLoaded(void* params)
    {
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
        if(rcp->result == ResultCode_OK)
        {
            // check to see if there is an animation load list
            std::set< std::string >* animationsToLoad = 0;
            
            if( rcp->userData.keyExists( ANIMATION_LIST_POINTER_KEY ) )
            {
                const Property* prop = rcp->userData.getValueForKey( ANIMATION_LIST_POINTER_KEY );
                animationsToLoad = prop->asPointer< std::set< std::string> >();
            }
            
            XMLDocument xmlDoc(rcp->buffer, rcp->length, "Animations");
            
            if(xmlDoc)
            {
                Property property;                
                if(xmlDoc.getRootIterator().getAttribute( "curveCollection", property ) )
                {
                    setCurveCollectionName( property.asString() );
                    if( animationsToLoad )
                    {
                        CurveManager::getManager().loadCurveCollection( getCurveCollectionName(),
                                                                        CallbackPtr(),
                                                                        *animationsToLoad );
                    }
                    else
                    {
                        CurveManager::getManager().loadCurveCollection(getCurveCollectionName());
                    }
                }
                
                XMLDocument::NamedIterator animationIt = xmlDoc.getNamedIteratorTo( "Animation" );
                
                while( animationIt )
                {
                    // we need to compare the animation name to the list of animations we want to
                    // export to see if we should export it...
                    bool shouldAddAnimation = true;
                    if( animationsToLoad )
                    {
                        Property aniNameProp;
                        animationIt.getAttribute( "name", aniNameProp );
                        std::string animationName = aniNameProp.asString();
                        
                        if( animationsToLoad->find( animationName ) == animationsToLoad->end() )
                        {
                            // this animation isn't in the load list, skip it
                            shouldAddAnimation = false;
                        }
                    }
                    
                    if( shouldAddAnimation )
                    {
                        Animation* ani =
                            Animation::createAnimationFromXmlNode( XMLDocument::Iterator(
                                                                    animationIt.getCurrentNode() ),
                                                                   mSkeleton );
                        
                        addAnimation(ani);
                    }
                    
                    ++animationIt;
                }
            }
            
            // we created a dynamic copy of the set because we need it to last through functions,
            // so we pass it through by pointer and delete it on this end
            if( animationsToLoad )
            {
                delete animationsToLoad;
            }
            
            delete [] rcp->buffer;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void SkeletonAnimationManager::_animationBinHasLoaded( void* data )
    {
        FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
		
        if( params->result == ResultCode_OK )
        {            
            AnimationBinary::AnimationBinaryFileHeader* header =
                (AnimationBinary::AnimationBinaryFileHeader*)params->buffer;
            
            assert( header->mMagic == AnimationBinary::MAGIC );
            if( header->mMagic == AnimationBinary::MAGIC )
            {
                // this is definitely an animation binary
                if( header->mVersion == 1 )
                {
                    BasicBinaryReader reader( params->buffer, params->length );
                    _loadV1AnimationBin( reader, tempAnimationsToLoad );
                }
                    
            }
            
            tempAnimationsToLoad.clear();
            
            delete [] params->buffer;
        }
    }
    
    // ---------------------------------------------------------------------------------------------

    void SkeletonAnimationManager::_loadV1AnimationBin( BasicBinaryReader& reader,
                                                        const std::set< std::string >& animationsToLoad )
    {
        AnimationBinary::AnimationBinaryFileHeader header;
        reader.read( header );
        
        std::string curveCollection;
        reader.read( curveCollection );
        
        if( !curveCollection.empty() )
        {
            setCurveCollectionName( curveCollection );
            if( ! tempAnimationsToLoad.empty() )
            {
                CurveManager::getManager().loadCurveCollection( getCurveCollectionName(),
                                                                CallbackPtr(),
                                                                tempAnimationsToLoad );
            }
            else
            {
                CurveManager::getManager().loadCurveCollection(getCurveCollectionName());
            }
        }
        
        for( unsigned int i=0; i < header.mNumAnimations; ++i )
        {
            unsigned long long preAnimationOffset = reader.getCurrentOffset();
            std::string animationName;
            reader.read( animationName );
            
            unsigned int aniSize = reader.readUInt32();
            
            bool shouldAddAnimation = true;
            if( ! tempAnimationsToLoad.empty() )
            {
                if( tempAnimationsToLoad.find( animationName ) == tempAnimationsToLoad.end() )
                {
                    shouldAddAnimation = false;
                }
            }
            
            if( shouldAddAnimation )
            {
                Animation* ani =
                    Animation::createAnimationFromBinary( reader, animationName, mSkeleton );
                
                if( ani )
                {
                    addAnimation( ani );
                }
            }
            else
            {
                reader.seek( aniSize-(reader.getCurrentOffset()-preAnimationOffset),
                             BasicBinaryReader::SeekStart_Current );
            }
        }
        
        assert( reader.getBufferSize() == reader.getCurrentOffset() );
    }
    
    // ---------------------------------------------------------------------------------------------
    
    void SkeletonAnimationManager::_handleLoadBehavior( AnimationManager::AnimationLoadBehavior loadBehavior)
    {
        switch ( loadBehavior )
        {
            case AnimationManager::AnimationLoadBehavior_Overwrite:
                
                clear();
                
                break;
                
            case AnimationManager::AnimationLoadBehavior_Append:
                
                break;
                
            default:
                
                // Undefined behavior
                assert(0);
                
        }
    }
    
}
