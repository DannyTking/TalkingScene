//
//  SkeletonAnimationManager.h
//  WalaberEngine
//
//  Created by Pecho, Jason on 4/19/13.
//
//

#ifndef _SKELETON_ANIMATION_MANAGER_H_
#define _SKELETON_ANIMATION_MANAGER_H_

#include <AnimationManager.h>

namespace Walaber
{
    class Skeleton;
    class BasicBinaryReader;
    
    /*
     This animation manager manages the animations on a skeleton, thereby controlling the nodes in a
     skeleton.
    */
    class SkeletonAnimationManager : public AnimationManager
    {
    public: // interface
        /*!
         Ctor
        */
        SkeletonAnimationManager();
        
        /*!
         Skeleton ctor
        */
        SkeletonAnimationManager( Skeleton* skeleton);
        
        /*!
         Copy Ctor
        */
        SkeletonAnimationManager(const SkeletonAnimationManager& aniMgr);
        
        /*!
         operator =
        */
        SkeletonAnimationManager& operator=(const SkeletonAnimationManager& aniMgr);
        
        /*!
         Dtor
        */
        virtual ~SkeletonAnimationManager();
        
        /*!
         Set the skeleton this manager controls
        */
        void setSkeleton( Skeleton* skeleton );
        
        /*!
         Load animations into the this manager from the an xml file
         @param xmlPath the path to the xmlfile to load
         @param loadBehavior the behaviour this manager should use when loading animations
         ( Overwrite or Append )
        */
        void loadAnimationsFromXml( const std::string& xmlPath,
                                    const AnimationLoadBehavior loadBehavior );
        
        /*!
         Load animations into the this manager from the an xml file
         @param xmlPath the path to the xmlfile to load
         @param loadBehavior the behaviour this manager should use when loading animations
         ( Overwrite or Append )
         @param animationsToLoad the animation name is checked against this list before it is loaed
        */
        void loadAnimationsFromXml( const std::string& xmlPath,
                                    const AnimationLoadBehavior loadBehavior,
                                    const std::set< std::string >& animationsToLoad
                                   
                                  );
        
        /*!
         Tell the current animation to resume playing
        */
        virtual void playCurrentAnimation();
        
        /*!
         Tell the current animation to pause
        */
        virtual void pauseCurrentAnimation();
        
        /*!
         Load animations into the this manager from the a binary file
         @param binPath the path to the binary to load
         @param loadBehavior the behaviour this manager should use when loading animations
         ( Overwrite or Append )
         */
        void loadAnimationsFromBin( const std::string& binPath,
                                    const AnimationLoadBehavior loadBehavior );
        
        /*!
         Load animations into the this manager from the a binary file
         @param binPath the path to the binary to load
         @param loadBehavior the behaviour this manager should use when loading animations
         ( Overwrite or Append )
         @param animationsToLoad the animation name is checked against this list before it is loaed
         */
        void loadAnimationsFromBin( const std::string& binPath,
                                    const AnimationLoadBehavior loadBehavior,
                                    const std::set< std::string >& animationsToLoad
                                   );
        
    protected: // interface
        
        /*!
         read file call back for xml animationloading, on a per skeleton basis (no static)
         @param params a pointer to a ReadFileCallbackParameters struct, cast it as such
        */
        void _animationsHaveLoaded(void* params);
        
        /*!
         read file call back for binary animationloading, on a per skeleton basis (no static)
         @param data a pointer to a ReadFileCallbackParameters struct, cast it as such
        */
        void _animationBinHasLoaded( void* data );
        
        /*!
         load a version one animation binary and take into account the animations to load
         @param animationsToLoad a pointer to a set that contains the names of animations to load
        */
        void _loadV1AnimationBin( BasicBinaryReader& reader,
                                  const std::set< std::string >& animationsToLoad );
        
        /*!
         Changes the animation manager's animation set according to the parameter passed in
         @param loadBehavior what behavior should the animation manager have during loading
        */
        void _handleLoadBehavior( AnimationLoadBehavior loadBehavior );
        
        
    protected: // members
        
        // This is a weak reference, the manager does not own the skeleton and will not delete it
        Skeleton*  mSkeleton;
        
        static const std::string ANIMATION_LIST_POINTER_KEY;
        
        static std::set< std::string > tempAnimationsToLoad;
        
    }; // SkeletonAnimationManager
}
#endif /* defined(__SKELETON_ANIMATION_MANAGER_H_) */
