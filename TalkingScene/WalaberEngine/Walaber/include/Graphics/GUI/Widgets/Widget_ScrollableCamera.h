//
//  Widget_ScrollableCamera.h
//  WalaberEngine
//
//  Created by mchawal on 9/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _WALABER_WIDGET_SCROLLABLE_CAMERA_H_
#define _WALABER_WIDGET_SCROLLABLE_CAMERA_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"
#include "Camera.h"

#include "Widget_Group.h"
#include "Widget_Label.h"
#include "TextManager.h"
#include <Texture.h>
#include <string>
#include <vector>
#include <list>


namespace Walaber {
    
    class SpriteBatch; 
	class BitmapFont; 
    
    class Widget_ScrollableCamera : public Widget
	{
    public:
        Widget_ScrollableCamera( int name,          // Widget ID in a screen
                                Vector2 pos,        // Widget Local Position
                                Vector2 size        // Widget Size (Screen Space)
                                );

        Widget_ScrollableCamera( int name,          // Widget ID in a screen
                                Vector2 pos,        // Widget Local Position
                                Vector2 size,       // Widget Size (Screen Space)
                                Camera* cam         // Camera pointer. Created in the Screen                                that implements this widget
                                );

		~Widget_ScrollableCamera();
        
        void initCamera(Camera* mCam);              // Init this widget with this camera pointer
	
		bool update( float elapsedSec, WidgetActionRet& ret );
		
        void draw( SpriteBatch* sb );//, const Vector2& offset );
        
        /**
         * Add widget to the camera, so it will scroll to it. If add multiple widget to the camera, they need to be added in the ascending order of their position.
         * @param wp the pointer of the widget added to the camera. When camera scroll to this widget, the position of the camera will be the same as the position of the widget.
         * @param createSlider indicate whether to create a pagination for this widget
         */
        void addGroup( Widget_Group* wp, bool createSlider = true );
        
        /**
         * Add widget to the camera, so it will go along with the camera when camera is moving around. 
         * @param wp the pointer of the widget added to the camera. 
         */
        void addMovableGroup( Widget_Group* wp );
        
        /**
         * Set the widget Camera will look at. 
         * @param newIndex the index of the widget in the widgets list of this camera.
         * @param duration indicate how long the camera should take to move to the widget. If it's 0, then do a CS_InstantMoveTo. Otherwise, snap to the newIndex.
         */
        void setIndex( int newIndex, float duration = 0.0f );
        int getCurrentIndex();
        
        float getCurrentIndexPercentage();
        
        void setCurrentIndexPercentage(float percentage) { mIndexPercentage = percentage; }
        
        int getCurrentCamState() { return mCamState; }
        
		int getGroupCount() { return mSize; }
        
        int getWidgetNameAtIndex(int index) { return mWidgets[index]->getWidgetNameAsInt(); }
        
        int getCurrentIndexWidgetName() { return mWidgets[mIndex]->getWidgetNameAsInt(); }
        
        void setLoopScrollEnabled(bool enabled) { mIsLoopScroll = enabled; }
        
        void setLockCameraInteraction(bool lockCamera) { mLockCameraInteraction = lockCamera; }
        
        void dollyTo( int toIndex, float duration );
		void dollyFromTo( int fromIndex, int toIndex, float duration );
		
		void createFakeSliderIndex( int index);
        void offsetSlider(Vector2 offset);
        void centerSliderAt(Vector2 pos);
        void _updateSlider();

        bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
        
        float& getOffsetReference() { return mOffset; };
        void setOffset(float offset) { mOffset = offset; }
        
        void setVelDamping(float damping) { mVelDamping = damping; }
        void setUseDampingForSnap(bool useDamping) { mUseDampingForSnap = useDamping; }
        
        enum CamState{
            CS_Stable,              //Camera is not moving at all.
            CS_Moving,              //Camera is moving according to the position of touch.
            CS_Snap,                //Camera quickly snap to a widget. The index of widget is based on the velocity of touch when touch ended.
            CS_DollyTo,             //Camera will move to specific widget in duration.
            CS_InstantMoveTo,       //Camera will instantly move to specific widget.
        };
        
    private:
        
		typedef std::vector<Widget_Group*>	GroupList;
		typedef std::vector<int>			PosList;
        typedef std::vector<Widget_Label*>  SliderList;

		void _updateFinger(bool firstFrame = false);
		
		FingerInfo*			mFinger;
        
		int					mFirstFinger; 
		int					mIndex;
        float               mIndexPercentage;
        int                 mIndexToSnap;                  // Widget index to snap to
        
        CamState            mCamState;                     // Current Camera state
		
		int					mSize;                         // Number of widget groups added
		
		float				mOffset;                       // Position of the camera.
		float				mLastOffsetDelta;
		
		float				mVel;
		float				mVelDamping;
        
        bool                mIsLoopScroll;                  // are the widgets loopable?
        
        bool                mLockCameraInteraction;          // temporarily disable manipulation of mCamera

        int                 mDollyToIndex;
        float               mDollyT;
        float               mDollyDuration;
		
		GroupList			mWidgets;                       // List of Widget Groups
        
        GroupList           mMovableWidgets;                // Any Widget Groups in this list move with the camera
        
		SliderList          mSlider;                        // List of Labels for pagination
        PosList             mSliderOrigPos;
        PosList             mMovableOrigPosX;
        Camera*             mCamera;
        
        TextureManager*     tmgr;
        TexturePtr          mNonCurrentPageTex;
        TexturePtr          mCurrentPageTex;
        
        bool                mUseDampingForSnap; ///< used to slow down over bookends
        
        Vector2             mPosLastFrame;
        
        Vector2             mPositionAtDown;
        float               mScrollIntentThreshold; ///< How much slack shall we give the user to slide before recognizing that they clearly want to scroll the camera?

    };
}


#endif
