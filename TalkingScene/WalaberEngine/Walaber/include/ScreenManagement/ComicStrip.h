#ifndef _COMIC_STRIP_H_
#define _COMIC_STRIP_H_

#include <Vector2.h>
#include <Camera.h>
#include <CameraController.h>

#include "Transition.h"

#include <vector>

namespace Walaber
{	
    
    enum CameraShake
    {
        CS_None,
        CS_Full,
        CS_Half
    };
    
	// --------========--------========--------========--------========--------========--------========--------========
    class ComicStrip
    {
	public:
        
        // --------========--------========--------========--------========--------========--------========--------========
        ComicStrip();
        ~ComicStrip();
        
        // --------========--------========--------========--------========--------========--------========--------========
        bool update( float elapsedSec );
        
        // --------========--------========--------========--------========--------========--------========--------========
        void draw( SpriteBatch& sb );
        
        // --------========--------========--------========--------========--------========--------========--------========
        void addPanel( TexturePtr panelTexture );
        void addPanel( TexturePtr panelTexture, PropertyList plist );
        
        // Return true if we're at the end of the strip
        bool handleTouch();
        void finishPanel();
        bool advancePanel();
        
    private:
        
        // Panel information
        int                                             mCurrentPanel;
        int                                             mNumPanels;
        
        float                                           mTimer;
        
        // Panel lists
        std::vector< Vector2 >                          mPanelPositions;
        std::vector< Vector2 >                          mPanelSizes;
        
        std::vector< TexturePtr >                       mPanelTextures;
        
        // Sounds
        std::vector< int >                              mSounds;
        std::vector< float >                            mSoundDelays;
        std::vector< int >                              mSoundsPerPanel;
        
        int                                             mCurrentSound;
        int                                             mTotalSounds;
        
        // For transitions on a panel
        std::vector< Rect >                             mStartingRect;
        std::vector< Rect >                             mEndingRect;
        
        // Times per panel
        std::vector< float >                            mPreWait;
        std::vector< float >                            mDuration;
        std::vector< float >                            mPostWait;
        
        // Transitions between panels
        std::vector< int >                              mTransitions;
        std::vector< float >                            mTransitionLengths;
        
        // Shakes
        std::vector< int >                              mCameraShakes;
        std::vector< float >                            mCameraShakeDelays;
        
        // Cameras
        Camera*                                         mCamera;
        CameraController*                               mCameraController;
        
        Transition*                                     mTransition;
    };
}

#endif	// _COMIC_STRIP_H_
