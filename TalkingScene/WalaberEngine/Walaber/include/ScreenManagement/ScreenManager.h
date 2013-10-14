#ifndef _SCREEN_MANAGER_H_
#define _SCREEN_MANAGER_H_

#include <vector>
#include <map>


#include <Vector2.h>
#include <GameScreen.h>

#include <Transition.h>
#include <TransitionLoader.h>

#include <PropertyList.h>
#include <Callback.h>
#include <SharedPtr.h>

namespace Walaber
{
	class PropertyList;
	
	enum TouchEventType
	{
		TET_Down,
		TET_Move,
		TET_Up
	};
	
	// --------========--------========--------========--------========--------========--------========--------========
    class ScreenManager
    {
	public:
		// --------========--------========--------========--------========--------========--------========--------========
		enum DrawMode
		{
			DM_Screens =	(1 <<  0),
			DM_PreDraw =	(1 <<  1),
			DM_PostDraw =	(1 <<  2),
			DM_Transition = (1 <<  3),
			DM_All = (DM_Screens | DM_PreDraw | DM_PostDraw | DM_Transition)
		};
											  
        struct UpdateOperationCallbackParameters
        {
            float delta;
        };
        
        struct ScreenLoaderFunctionParameters
        {
            unsigned int inScreenID;
            GameScreen* outGameScreen;
        };		
		
		struct TouchFirstResponderParamameters
		{
			TouchEventType		touchEvent;
			Vector2				curPos;
			Vector2				lastPos;
			unsigned int		touchID;
			bool				outCanBeConsumed;
			
			TouchFirstResponderParamameters():
			touchEvent(TET_Down),
			curPos(),
			lastPos(),
			outCanBeConsumed(false)
			{}
		};
		
		// --------========--------========--------========--------========--------========--------========--------========
		enum ScreenChangeMethod
		{
			SCM_Immediate,		// means that all screen operations called for this frame will be executed in order at the next call to update()
			SCM_WaitForFlush,	// means that all screen operations will be cached, and flushed only when commitScreenChanges() is called.
		};
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		enum ScreenManagerPriority
		{
			SLP_HIGH = 10,
			SLP_NORMAL = 50,
			SLP_LOW = 100
		};
		
		// --------========--------========---- ----========--------========--------========--------========--------========
        static void initDebugInfoOverlay(unsigned int screenName);
        
		// --------========--------========---- ----========--------========--------========--------========--------========
        static void destroyDebugInfoOverlay();
        
		// --------========--------========---- ----========--------========--------========--------========--------========
        static void initLoadingOverlay(unsigned int screenName);
        
		// --------========--------========---- ----========--------========--------========--------========--------========
        static void requestDestroyLoadingOverlay();
        
		// --------========--------========---- ----========--------========--------========--------========--------========
		static void addScreenLoader( CallbackPtr slf, unsigned int priority = (int)SLP_NORMAL ) 
		{ 
			mLoaderFunctions.insert( std::make_pair( priority, slf) ); 
		}
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		static void removeScreenLoader( CallbackPtr slf );
        
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		static void beginScreenChanges( ScreenChangeMethod t = SCM_Immediate );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		static void clearScreenChangeQueue()
		{
			mPrepList.clear();
		}
        
        //------------------------------------------------------------------------------------------
        static bool isScreenInQueue( unsigned int screenName );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// push a new screen onto the stack.
        static void pushScreen( unsigned int screenName );
		static void pushScreen( unsigned int screenName, const PropertyList& plist );
		
        // --------========--------========---- ----========--------========--------========--------========--------========
		/// pop the top screen off the stack.
        static void popScreen(bool forcePop = false);
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// pop all screens off of the stack.
		static void popAllScreens();
		
        // --------========--------========---- ----========--------========--------========--------========--------========
		/// remove the top screen, and replace it with another.
        static void swapScreen( unsigned int screenName );
		static void swapScreen( unsigned int screenName, const PropertyList& plist );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		static void commitScreenChanges();
		
		static void commitScreenChanges( unsigned int transitionIndex, float duration );
		static void commitScreenChanges( std::vector< unsigned int > transitionIndexes, float duration );
		
		static void commitScreenChanges( unsigned int transitionIndex, float duration, const PropertyList& plist );
		static void commitScreenChanges( std::vector< unsigned int > transitionIndexes, float duration, const PropertyList& plist );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// update only the loading overlay by 1 frame (called from separate thread).
        static void loadingOverlayUpdate(float elapsedSec);

		// --------========--------========---- ----========--------========--------========--------========--------========
		/// update the game by 1 frame.
        static void update(float elapsedSec);
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// draw the loading overlay screens, once for each given layer (called from separate thread). [0, LayerCount)
        static void loadingOverlayDraw();

		// --------========--------========---- ----========--------========--------========--------========--------========
		/// draw all of the game screens, once for each given layer. [0, LayerCount)
        static void draw(DrawMode dm = DM_All);
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// raw input, passed onto the game screens.
		static void touchDown( int touchID, Vector2 pos );
		static void touchMoved( int touchID, Vector2 curPos, Vector2 lastPos );
		static void touchUp( int touchID, Vector2 pos );
		static void touchLost( int touchID );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// raw accelerometer input, passed onto the game screens.
		static void accelerometerChanged( float x, float y, float z );
		
        // --------========--------========---- ----========--------========--------========--------========--------========
		/// get the screen currently on top of the stack.		
        static GameScreen* peekTop();

		// --------========--------========---- ----========--------========--------========--------========--------========
		/// get a specific screen in the stack (0 = top, 1+ heads down the list)
        static GameScreen* getScreenAt(int i);

        /*!
        Get a pointer to the first found sreen with the specified "name"
        @param the screen "name", which is an enum starting with the prefix ST_
        @return a pointer to the generic pointer GameScreen to the screen if it exists, or NULL if not
        @see getSpecificScreenWithName
        */
		static GameScreen* getScreenWithName( unsigned int n );

        /*!
        Get a reference to the first found sreen with the specified "name"
        @param the screen "name", which is an enum starting with the prefix ST_
        @return a reference to the screen. Fails hard if it does not exist
        */
        template< class T >
        static T & getSpecificScreenWithName( unsigned int screenName )
        {
            GameScreen * screen = ScreenManager::getScreenWithName( screenName );
            assert( screen != NULL );
            return * static_cast< T * >( screen );
        }
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// get the current number of screens in the stack.
        static int getScreenCount() { return mScreenStack.size(); }	
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// get or set the number of draw layers required.
		static void setDrawLayerCount( unsigned int l ) { mLayerCount = l; }
		static unsigned int getDrawLayerCount() { return mLayerCount; }
        
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// determine if transition has finished
        static bool isTransitioning();
        
        static bool isShowingLoadingOverlay()
        {
            return (bool)mLoadingOverlay;
        }
        
        // --------========--------========---- ----========--------========--------========--------========--------========
		// pre draw callback setting
        static void setPreDrawCallback(CallbackPtr preDrawCallback)
        {
            mPreDrawCallback = preDrawCallback;
        }
        
        // --------========--------========---- ----========--------========--------========--------========--------========
		// post callback setting
        static void setPostDrawCallback(CallbackPtr postDrawCallback)
        {
            mPostDrawCallback = postDrawCallback;
        }
        
        // --------========--------========---- ----========--------========--------========--------========--------========
		// pre update callback setting
        static void setPreUpdateCallback(CallbackPtr preUpdateCallback)
        {
            mPreUpdateCallback = preUpdateCallback;
        }
        
        // --------========--------========---- ----========--------========--------========--------========--------========
		// post update callback setting
        static void setPostUpdateCallback(CallbackPtr postUpdateCallback)
        {
            mPostUpdateCallback = postUpdateCallback;
        }

		// --------========--------========---- ----========--------========--------========--------========--------========
		// post update callback setting
        static void setTouchFirstResponderCallback(CallbackPtr touchFirstResponderCallback)
        {
            mTouchFirstResponderCallback = touchFirstResponderCallback;
        }
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		// post update callback setting
        static void setBackKeyPressed()
        {
            mBackKeyPressed = true;
        }
        
        static void setSystemMusicPlaying( bool systemMusicPlaying )
        {
            mSystemMusicIsPlaying = systemMusicPlaying;
        }
		
        // --------========--------========---- ----========--------========--------========--------========--------========
		static void recreateGraphicsContext();
        
        static GameScreen* getDebugOverlay() { return mDebugInfoOverlay; }
        
	private:
		// --------========--------========---- ----========--------========--------========--------========--------========
		enum ScreenOperation
		{
			SO_Push,
			SO_Swap,
			SO_Pop,
			SO_ForcePop,
		};
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		struct ScreenOperationDetails 
		{
			ScreenOperation		_operation;
			int					_count;
			unsigned int		_name;
			PropertyList		_plist;
		};
		
		
		static void _pushScreen( GameScreen* s );
		static void _popScreen( bool forcePop );
		static void _swapScreen( GameScreen* s );
		static void _goFlush();
        
		// --------========--------========---- ----========--------========--------========--------========--------========
        static void _destroyLoadingOverlay();
        
		
		// --------========--------========--------========--------========--------========--------========--------========
		typedef std::vector<GameScreen*>				GameScreenList;
		typedef std::vector<ScreenOperationDetails>		ScreenOperationDetailsList;
		
		// --------========--------========--------========--------========--------========--------========--------========
		static unsigned int								mLayerCount;
		
        static GameScreen*                              mDebugInfoOverlay; ///<Always on top
        static GameScreen*                              mLoadingOverlay;
        static bool                                     mShouldDestroyLoadingOverlay;
        
        static GameScreenList							mScreenStack;
		static ScreenOperationDetailsList				mPrepList;
		
		static bool										mFlushRequested;
		static bool										mBackKeyPressed;
        static bool                                     mSystemMusicIsPlaying;
		static ScreenChangeMethod						mFlushMethod;
		
		static CallbackPtr                              mPreDrawCallback;	
		static CallbackPtr                              mPostDrawCallback;
		
		static CallbackPtr                              mPreUpdateCallback;
		static CallbackPtr                              mPostUpdateCallback;	
		
		static CallbackPtr								mTouchFirstResponderCallback;
		
		
		typedef std::multimap<unsigned int, CallbackPtr>	LoaderPriorityMap;
		
		static LoaderPriorityMap						mLoaderFunctions;
		
		
		static std::vector< Transition* >				mTransitions;
		static Transition::TransitionPhase				mCurrentTransitionPhase;	
    };
}

#endif	// _SCREEN_MANAGER_H_
