#include "ComicStrip.h"
#include "SoundManager.h"
#include "Transition.h"
#include "TransitionLoader.h"
#include "StringHelper.h"

namespace Walaber
{
    // --------========--------========--------========--------========--------========--------========--------========
    ComicStrip::ComicStrip() :
    mCurrentPanel( 0 ),
    mNumPanels( 0 ),
    mTimer( 0.0f ),
    mTransition(NULL),
    mCurrentSound( 0 ),
    mTotalSounds( 0 )
    {
        mCamera = new Camera( ScreenCoord(0.5f, 0.5f).toScreen(), ScreenCoord::getScreenResolution() );
        mCameraController = new CameraController();
        
        mCameraController->initCamera( mCamera );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    ComicStrip::~ComicStrip()
    {
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool ComicStrip::update( float elapsedSec )
    {
        // Timer
        mTimer += elapsedSec;
        
        // Update the camera and controller
        mCamera->update( elapsedSec );
        mCameraController->update( elapsedSec ); 
        
        // See if the next panel is ready
        if (!mCameraController->isAnimating())
        {
            if ((mTransitions[mCurrentPanel - 1] != -1) && (mTransition == NULL))
            {
                TransitionLoader::TransitionLoaderParameters params( mTransitions[mCurrentPanel - 1] );
                TransitionLoader::transitionFromNameDefault( &params  );
                
                mTransition = params.outTransition;
                mTransition->setup( mTransitionLengths[mCurrentPanel - 1] ); 
            }
            else if (mTransition == NULL) 
            {
                advancePanel();
            }
        }
        
        // See if we're doing a transition
        if (mTransition != NULL)
        {
            int returnVal = mTransition->update( elapsedSec );
            if (returnVal == Transition::TP_Flush)
            {
                advancePanel();
            }
            else if (returnVal == Transition::TP_Finished)
            {
                delete( mTransition );
                mTransition = NULL;
            }
        }
        
        // Play any sounds
        if (mCurrentSound < mSoundsPerPanel[mCurrentPanel - 1])
        {
            // If it's time
            if (mTimer >= mSoundDelays[mTotalSounds + mCurrentSound])
            {
                // Play that sound, move onto the next one
                SoundManager::getInstancePtr()->playSoundFromGroup( mSounds[mTotalSounds + mCurrentSound] );
                mCurrentSound++;
            }
        }
        
        // Shake the camera
        if (mCameraShakes[mCurrentPanel - 1] != (int) CS_None)
        {
            // Shake that camera!
            if (mCameraShakeDelays[mCurrentPanel - 1] >= mTimer)
            {
                if (mCameraShakes[mCurrentPanel - 1] == (int) CS_Half)
                {
                    mCamera->halfShake( 1.5f, 4.0f, 0.5f );
                }
                else if (mCameraShakes[mCurrentPanel - 1] == (int) CS_Full)
                {
                    mCamera->halfShake( 1.5f, 4.0f, 0.5f );
                }
                
                mCameraShakes[mCurrentPanel - 1] = CS_None;
            }
        }
        
        // See if we're all done
        if ((!mCameraController->isAnimating()) && (mCurrentPanel >= mNumPanels))
        {
            return true;
        }
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void ComicStrip::draw( SpriteBatch& sb )
    {
        // Update the camera
        mCamera->updateViewport();
        
        // Draw the current and it's surrounding panels
        for (int i = -1; i <= 1; i++)
        {
            if ((mCurrentPanel + i >= 0) && (mCurrentPanel + i < mNumPanels))
            {
                sb.drawQuad(1, mPanelTextures[mCurrentPanel + i], mPanelPositions[mCurrentPanel + i] * Vector2(1, -1), 0, mPanelSizes[mCurrentPanel + i] * Vector2(1,-1), Color(255, 255, 255));
            }
        }
        
        // Draw a transition if it exists
        if (mTransition != NULL) 
        {
            sb.flush();
            sb.start( SpriteBatch::BM_LAYERS );
        
            mCamera->restoreViewport();
        
            mTransition->draw();
        }
        
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void ComicStrip::addPanel( TexturePtr panelTexture )
    {
        // Add a panel at the proper position
        PropertyList plist;
        addPanel( panelTexture, plist );
        
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void ComicStrip::addPanel( TexturePtr panelTexture, PropertyList plist )
    {
        // Add the texture
        mPanelTextures.push_back( panelTexture );
        
        float mDiff = mNumPanels * 1.0f;
        mPanelPositions.push_back( ScreenCoord( 0.5f + mDiff, 0.0f ).toScreen() );
        
        // Now we have a whole lot of stuff to figure out!
        
        // Size
        if(plist.keyExists("Size"))
		{
			mPanelSizes.push_back( plist["Size"].asVector2() );
		}
        else 
        {
            mPanelSizes.push_back( Vector2( ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y ));
        }
        
        // Rects
        if(plist.keyExists("StartRect"))
		{
			mStartingRect.push_back( plist["StartRect"].asRect() );
		}
        else 
        {
            mStartingRect.push_back( Rect( 0, 0, ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y ));
        }
        
        if(plist.keyExists("EndRect"))
		{
			mEndingRect.push_back( plist["EndRect"].asRect() );
		}
        else 
        {
            mEndingRect.push_back( Rect( 0, 0, ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y ));
        }
        
        // Timing
        if(plist.keyExists("PrePause"))
		{
			mPreWait.push_back( plist["PrePause"].asFloat() );
		}
        else 
        {
            mPreWait.push_back( 0 );
        }
        
        if(plist.keyExists("PostPause"))
		{
			mPostWait.push_back( plist["PostPause"].asFloat() );
		}
        else 
        {
            mPostWait.push_back( 0 );
        }
        
        if(plist.keyExists("Duration"))
		{
			mDuration.push_back( plist["Duration"].asFloat() );
		}
        else 
        {
            mDuration.push_back( 5 );
        }
        
        // Transitions
        if(plist.keyExists("Transition"))
		{
			mTransitions.push_back( plist["Transition"].asInt() );
		}
        else 
        {
            mTransitions.push_back( -1 );
        }
        
        if(plist.keyExists("TransitionLength"))
		{
			mTransitionLengths.push_back( plist["TransitionLength"].asFloat() );
		}
        else 
        {
            mTransitionLengths.push_back( 1.0f );
        }
        
        // Sounds
        const Property* audioProp = plist.getValueForKey("SoundIndex0");
        const Property* delayProp = plist.getValueForKey("SoundDelay0");
        
        if (audioProp)
        {
            int index = 0;
            
            do
            {
                mSounds.push_back( audioProp->asInt() );
                mSoundDelays.push_back( delayProp->asFloat() );
                
                index++;
                
                audioProp = plist.getValueForKey("SoundIndex" + StringHelper::intToStr(index));
                delayProp = plist.getValueForKey("SoundDelay" + StringHelper::intToStr(index));
                
            } while (audioProp != NULL);
            
            mSoundsPerPanel.push_back( index );
        }
        else 
        {
            mSoundsPerPanel.push_back( 0 );
        }
        
        // Camera shake
        if(plist.keyExists("CameraShake"))
		{
			mCameraShakes.push_back( plist["CameraShake"].asInt() );
		}
        else 
        {
            mCameraShakes.push_back( (int) CS_None );
        }
        
        if(plist.keyExists("CameraShakeDelay"))
		{
			mCameraShakeDelays.push_back( plist["CameraShakeDelay"].asFloat() );
		}
        else 
        {
            mCameraShakeDelays.push_back( 0.0f );
        }
        
        mNumPanels++;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool ComicStrip::handleTouch()
    {
        if (mTransition == NULL) 
        {
            if (mCameraController->isAnimating())
            {
                // Run the transition
                if ((mTransitions[mCurrentPanel - 1] != -1) && (mTransition == NULL))
                {
                    TransitionLoader::TransitionLoaderParameters params( mTransitions[mCurrentPanel - 1] );
                    TransitionLoader::transitionFromNameDefault( &params  );
                    
                    mTransition = params.outTransition;
                    mTransition->setup( mTransitionLengths[mCurrentPanel - 1] ); 
                }
                // Otherwise, just move to the next panel
                else 
                {
                    advancePanel();
                }
                
                return true;
            }
            else 
            {
                return advancePanel();
            }
        }
        
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool ComicStrip::advancePanel()
    {
        // We ran out of panels, return false!
        if (mCurrentPanel >= mNumPanels)
        {
            return false;
        }
        // Otherwise move to the next panel
        else
        {
            // Update sounds
            if (mCurrentPanel != 0)
            {
                mTotalSounds = mSoundsPerPanel[mCurrentPanel - 1];
            }
            mCurrentSound = 0;
            
            // Move from the starting rect to the end rect
            mCameraController->clearQueue();
            mCameraController->setMovement( mPanelPositions[mCurrentPanel] + mStartingRect[mCurrentPanel].upper_left, 
                                            mStartingRect[mCurrentPanel].size );
            
            //mCameraController->endMovement( true );
            
            mCameraController->queueMovement(mPanelPositions[mCurrentPanel] + mEndingRect[mCurrentPanel].upper_left, 
                                             mEndingRect[mCurrentPanel].size, mDuration[mCurrentPanel], mPreWait[mCurrentPanel], mPostWait[mCurrentPanel]);
            
            // Move to the next slide
            mCurrentPanel++;
            
            mTimer = 0;
            
            return true;
        }
    }
}
