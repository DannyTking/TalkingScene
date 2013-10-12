#ifndef _GAME_SCREEN_H_
#define _GAME_SCREEN_H_

#include <Vector2.h>
#include <PropertyList.h>
#include <WidgetHelper.h>
#include <Action.h>


namespace Walaber
{
    /// <summary>
    /// Any game screen that can be shown on-screen. intended to be kept in a stack, allowing multiple screens to run on top of each other.
    /// </summary>
    class GameScreen
    {
	public:
		
		GameScreen( unsigned int screenName ) : mName(screenName), mBackKeyPressed(false),
        mPoppable(true),
        mUpdateBelow(true),
        mWidgetMgr(NULL),
        mPropertiesLoadedOverMultipleFrames(false),
        mMultiFrameLoadComplete(false),
        mActionTimeline( 3 ),
        mIsPaused( false )
        {
        }
        
		virtual ~GameScreen() { }

        /// <summary>
        /// called when this screen has been added to the stack asynchronously.
        /// </summary>
        virtual void beginAsyncEnter( PropertyList& screenProperties ) {}
        
        /// <summary>
        /// called when this screen has been added to the stack.
        /// </summary>
        virtual void enter() = 0;

        /// <summary>
        /// called while loading, can be called multiple times, similar to update
        /// update will never be called until loading has completed
        /// </summary>
        virtual void loading( float elapsedSec ) { mLoadingActions.execute( elapsedSec ); };

        /// <summary>
        /// called just before this screen is removed from the stack.
        /// </summary>
        virtual void exit() = 0;

        /// <summary>
        /// called on Android when Application comes in foreground after being on background.
        /// Implementation of this method should recreate VBOs and setup reset OpenGL state,
        /// so calling draw() method after this method will work.
        /// </summary>
        virtual void recreateGraphicsContext() { }

        /// <summary>
        /// called just before another screen is "pushed" onto the stack above this one.
        /// </summary>
        virtual void beingStacked() {}

        /// <summary>
        /// called after another stack has been popped, leaving this one on the "top" again.
        /// </summary>
        virtual void regainedTop() { }

        /// <summary>
        /// main update call for this screen. onTop indicates whether or not this screen is on the "top" of the stack.
        /// </summary>
        /// <param name="elapsedSec"></param>
        virtual void update( float elapsedSec, bool onTop) = 0;

        /// <summary>
        /// do any drawing for this screen on the given layer.  Called from bottom to top (for each individual layer) for proper layering of elements.
        /// </summary>
        virtual void draw(int layer) = 0;
		
        /// <summary>
        /// do any drawing for this screen on the given layer.  Called from bottom to top (for each individual layer) for proper layering of elements.
        /// </summary>
		virtual void loadPropertyList(const PropertyList& plist) {};
        
		/// raw touch input.
		virtual void touchDown( int touchID, Vector2 pos ) {}
		virtual void touchMoved( int touchID, Vector2 curPos, Vector2 lastPos ) {}
		virtual void touchUp( int touchID, Vector2 pos ) {}
		virtual void touchLost( int touchID ) {}
        
        /// back key pressed - android specific
        virtual void backKeyPressed() { }
		
		virtual void accelerometerChanged( float x, float y, float z ) {}

        /*!
        Pause the screen, a paused screen does not receive update calls
        @param isPaused the new state value for the pause
        */
        virtual void pause( bool isPaused )
        {
            mIsPaused = isPaused;
        }
		
        /// called to determine if the multi frame load is complete
        /// (also returns TRUE if there isn't a multi frame load for this screen).
        bool isMultiFrameLoadComplete() { return !mPropertiesLoadedOverMultipleFrames || mMultiFrameLoadComplete; }

        /// called to determine if there are actions to be loaded, if there are the loading() method will be called
        bool isLoading() { return mLoadingActions.calculateActionUnits() > 0; }
        
		/// get the user-assigned "name" (unique ID) for this screen.
		unsigned int getName() { return mName; }
		
		/// does this screen consume input, or allow it to also be sent to screens below it?
		virtual bool consumesInput() { return true; }
		
		virtual void notifyTransitionPhaseChange(const unsigned int transitionPhase) {}

		bool isPoppable()
		{
			return mPoppable;
		}

        /*!
        Accessor to if the screen is paused
        @return true if the screen is paused
        */
        bool isPaused()
        {
            return mIsPaused;
        }
		
		bool shouldUpdateScreensBelow()
		{
			return mUpdateBelow;
		}
        
		virtual bool shouldLoadAsync()
		{
			return false;
		}
        
		// Return this screen's widget manager
		WidgetManager* getWidgetManager() { return mWidgetMgr; }
		
	protected:
		
		unsigned int		mName;
		
		// track back key press to make sure it isn't processed more than once
		bool				mBackKeyPressed;
		bool				mPoppable;
		bool				mUpdateBelow;
        bool                mIsPaused;
		
		// Every screen has a widget manager
		WidgetManager*		mWidgetMgr;
        
        // TRUE if this screen takes multiple frames to finish loading.
        bool                mPropertiesLoadedOverMultipleFrames;
        
        // TRUE if this screen has completed a multi frame load.
        bool                mMultiFrameLoadComplete;

        ActionConcurrent    mLoadingActions;
        ActionTimeline      mActionTimeline;
    };
}


#endif	// _GAME_SCREEN_H_

