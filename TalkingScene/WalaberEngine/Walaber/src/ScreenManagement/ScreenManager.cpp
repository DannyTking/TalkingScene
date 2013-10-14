#include "ScreenManager.h"

#include "WalaberGame.h"
#include "SoundManager.h"

#include "Logger.h"

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	unsigned int ScreenManager::mLayerCount = 1;
	
    GameScreen* ScreenManager::mDebugInfoOverlay = 0;
    GameScreen* ScreenManager::mLoadingOverlay = 0;
    bool        ScreenManager::mShouldDestroyLoadingOverlay = false;
    
	ScreenManager::GameScreenList ScreenManager::mScreenStack;
	
	ScreenManager::ScreenOperationDetailsList ScreenManager::mPrepList;
	
	bool ScreenManager::mFlushRequested = false;
	bool ScreenManager::mBackKeyPressed = false;
	ScreenManager::ScreenChangeMethod ScreenManager::mFlushMethod = SCM_Immediate;
	
	CallbackPtr ScreenManager::mPreDrawCallback;	
	CallbackPtr ScreenManager::mPostDrawCallback;

	CallbackPtr ScreenManager::mPreUpdateCallback;
	CallbackPtr ScreenManager::mPostUpdateCallback;
	
	CallbackPtr ScreenManager::mTouchFirstResponderCallback;
	
	ScreenManager::LoaderPriorityMap ScreenManager::mLoaderFunctions;
	
	std::vector< Transition* > ScreenManager::mTransitions;
	Transition::TransitionPhase ScreenManager::mCurrentTransitionPhase = Transition::TP_NONE;
    
    bool ScreenManager::mSystemMusicIsPlaying = true;
	
	
	// --------========--------========--------========--------========--------========--------========--------========
    void ScreenManager::initDebugInfoOverlay(unsigned int screenName)
    {
        if (mDebugInfoOverlay == 0)
        {
            ScreenLoaderFunctionParameters params;
            params.inScreenID = screenName;
            
            for (LoaderPriorityMap::iterator it = mLoaderFunctions.begin(); it != mLoaderFunctions.end(); ++it)
            {
                it->second->invoke( &params );
                
                // did it load a screen?
                if (params.outGameScreen)
                    break;
            }
            
            if (params.outGameScreen)
            {
                mDebugInfoOverlay = params.outGameScreen;
                
                mDebugInfoOverlay->enter();
            }
            else
            {
                Walaber::Logger::printf("Walaber",
                                        Walaber::Logger::SV_FATAL,
                                        "ERROR loading debug info overlay, loader returned NULL.");
            }
        }
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
    void ScreenManager::destroyDebugInfoOverlay()
    {
        if (mDebugInfoOverlay)
        {
            mDebugInfoOverlay->exit();
            
            delete mDebugInfoOverlay;
            mDebugInfoOverlay = NULL;
        }
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
    void ScreenManager::initLoadingOverlay(unsigned int screenName)
    {
        mShouldDestroyLoadingOverlay = false;
        
        if (mLoadingOverlay == 0)
        {
            ScreenLoaderFunctionParameters params;
            params.inScreenID = screenName;
            
            for (LoaderPriorityMap::iterator it = mLoaderFunctions.begin(); it != mLoaderFunctions.end(); ++it)
            {
                it->second->invoke( &params );
                
                // did it load a screen?
                if (params.outGameScreen)
                    break;
            }
            
            if (params.outGameScreen)
            {
                mLoadingOverlay = params.outGameScreen;
                
                mLoadingOverlay->enter();
            }
            else
            {
                Walaber::Logger::printf("Walaber",
                                        Walaber::Logger::SV_FATAL,
                                        "ERROR loading loading overlay, loader returned NULL.");
            }
        }
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
    void ScreenManager::requestDestroyLoadingOverlay()
    {
        mShouldDestroyLoadingOverlay = true;
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
    void ScreenManager::_destroyLoadingOverlay()
    {
        if (mLoadingOverlay)
        {
            recreateGraphicsContext();  // some async loads could invalide certain graphics, be safe.
            
            mLoadingOverlay->exit();
            
            delete mLoadingOverlay;
            mLoadingOverlay = NULL;
        }
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::removeScreenLoader( CallbackPtr slf )
	{
		for (LoaderPriorityMap::iterator it = mLoaderFunctions.begin(); it != mLoaderFunctions.end(); ++it)
		{
			if (it->second.get() == slf.get())
			{
				// remove this!
				mLoaderFunctions.erase(it);
				break;
			}
		}
	}
	
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::beginScreenChanges( ScreenChangeMethod t )
	{
		mFlushMethod = t;
	}
	
    //------------------------------------------------------------------------------------------
    bool ScreenManager::isScreenInQueue( unsigned int screenName )
    {
		for (unsigned int i = 0; i < mPrepList.size(); i++)
		{
			// execute each operation in the order that it came in...Walaber::ScreenManager::SO_Pop
			ScreenOperationDetails sd = mPrepList[i];
			
			if (sd._name == screenName)
            {
                return true;
            }
        }
        
        return false;
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::pushScreen(unsigned int screenName)
	{
		if (mLoaderFunctions.empty())
			return;
		
		//GameScreen* s = mLoaderFunction(screenName);
		
		ScreenOperationDetails sd;
		sd._operation = SO_Push;
		sd._count = 1;
		//sd._screen = s;
		sd._name = screenName;
		
		mPrepList.push_back(sd);
		
		//s->enter();
		
		//return s;
	
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::pushScreen(unsigned int screenName, const PropertyList& plist)
	{
		if (mLoaderFunctions.empty())
			return;
		
		//GameScreen* s = mLoaderFunction(screenName);
		
		ScreenOperationDetails sd;
		sd._operation = SO_Push;
		sd._count = 1;
		sd._name = screenName;
		sd._plist = plist;
		//sd._screen = s;
		
		mPrepList.push_back(sd);
		
		//s->loadPropertyList(plist);
		//s->enter();
		
		//return s;
		
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::swapScreen(unsigned int screenName)
	{
		if (mLoaderFunctions.empty())
			return;
		
		//GameScreen* s = mLoaderFunction(screenName);
		
		ScreenOperationDetails sd;
		sd._operation = SO_Swap;
		sd._count = 1;
		//sd._screen = s;
		sd._name = screenName;
		
		mPrepList.push_back(sd);
		
		//s->enter();
		
		//return s;
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::swapScreen(unsigned int screenName, const PropertyList& plist)
	{
		if (mLoaderFunctions.empty())
			return;
		
		//GameScreen* s = mLoaderFunction(screenName);
		
		ScreenOperationDetails sd;
		sd._operation = SO_Swap;
		sd._count = 1;
		//sd._screen = s;
		sd._name = screenName;
		sd._plist = plist;
		
		mPrepList.push_back(sd);
		
		//s->loadPropertyList(plist);
		//s->enter();
		
		//return s;
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::popScreen(bool forcePop)
	{
		ScreenOperationDetails sd;
		if( forcePop )
		{
			sd._operation = SO_ForcePop;
		}
		else
		{
			sd._operation = SO_Pop;
		}
		sd._count = 1;
		//sd._screen = NULL;
		
		mPrepList.push_back(sd);
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::popAllScreens()
	{
		ScreenOperationDetails sd;
		sd._operation = SO_ForcePop;
		sd._count = mScreenStack.size();
		//sd._screen = NULL;
		
		mPrepList.push_back(sd);
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::commitScreenChanges()
	{
		mFlushMethod = SCM_Immediate;
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::commitScreenChanges( unsigned int transitionIndex, float duration )
	{
		std::vector< unsigned int > temp;
		temp.push_back(transitionIndex);
		
		commitScreenChanges( temp, duration );
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::commitScreenChanges( std::vector< unsigned int > transitionIndexes, float duration )
	{
		for (int i = 0; i < transitionIndexes.size(); i++)
		{
			Transition* tempTrans = TransitionLoader::createTransition(transitionIndexes[i]);
			if (tempTrans)
			{
				mTransitions.push_back(tempTrans);
				mTransitions[i]->setup( duration );
			}
		}
        
        // transition phase has started, tell the screens
        mCurrentTransitionPhase = (Transition::TransitionPhase)Transition::TP_Started;
        for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
        {
            mScreenStack[i]->notifyTransitionPhaseChange(mCurrentTransitionPhase);
        }
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::commitScreenChanges( unsigned int transitionIndex, float duration, const PropertyList& plist )
	{
		std::vector< unsigned int > temp;
		temp.push_back(transitionIndex);
		
		commitScreenChanges( temp, duration, plist );
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::commitScreenChanges( std::vector< unsigned int > transitionIndexes, float duration, const PropertyList& plist )
	{
		for (int i = 0; i < transitionIndexes.size(); i++)
		{
			Transition* tempTrans = TransitionLoader::createTransition(transitionIndexes[i]);
			if (tempTrans)
			{
				mTransitions.push_back(tempTrans);
				mTransitions[i]->setup( duration, plist );
			}
		}
        
        // transition phase has started, tell the screens
        mCurrentTransitionPhase = (Transition::TransitionPhase)Transition::TP_Started;
        for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
        {
            mScreenStack[i]->notifyTransitionPhaseChange(mCurrentTransitionPhase);
        }
	}
	
	// ---------------------------------------------------------------------------------------------
    void ScreenManager::loadingOverlayUpdate(float elapsedSec)
    {
        //--- Update DebugInfoOverlay
        if (mLoadingOverlay)
        {
            if (mLoadingOverlay->isLoading())
            {
                mLoadingOverlay->loading( elapsedSec );
            }
            else
            {
                mLoadingOverlay->update(elapsedSec, true);
            }
        }
    }
    
	// ---------------------------------------------------------------------------------------------
	void ScreenManager::update(float elapsedSec)
	{
		if ((mPrepList.size() > 0) && (mFlushMethod == SCM_Immediate))
		{
			_goFlush();
            
            if ((mPrepList.size() == 0) ||
                WalaberGame::IsLoadingThreadRunning() == false)
            {
                mFlushMethod = SCM_WaitForFlush;
            }
		}
		
        // continually ping soundmanager about system music status so that we can try to resume our in-game music
        if ( mSystemMusicIsPlaying )
        {
            bool muted = Walaber::SoundManager::getInstancePtr()->updateMusicMuteStatus();
            if( !muted )
            {
                Walaber::SoundManager::getInstancePtr()->setAllPaused( false );
                mSystemMusicIsPlaying = false;
            }
        }
        
        if (mShouldDestroyLoadingOverlay)
        {
            _destroyLoadingOverlay();
        }
        
        if (WalaberGame::IsLoadingThreadRunning() == false)
        {
            if (mPreUpdateCallback)
            {
                UpdateOperationCallbackParameters params;
                params.delta = elapsedSec;
                
                mPreUpdateCallback->invoke( &params );
            }
            
            GameScreen* topScreen = peekTop();
            
            if(mBackKeyPressed)
            {
                mBackKeyPressed = false;
                
                if (topScreen)
                {
                    topScreen->backKeyPressed();
                }
            }
            
            //--- Update DebugInfoOverlay
            if (mDebugInfoOverlay)
            {
                if (mDebugInfoOverlay->isLoading())
                {
                    mDebugInfoOverlay->loading( elapsedSec );
                }
                else
                {
                    mDebugInfoOverlay->update(elapsedSec, true);
                }
            }
            
            //--- Update DebugInfoOverlay
            if (mLoadingOverlay)
            {
                if (mLoadingOverlay->isLoading())
                {
                    mLoadingOverlay->loading( elapsedSec );
                }
                else
                {
                    mLoadingOverlay->update(elapsedSec, true);
                }
            }
            
            //---
            
            for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
            {
                bool updatedScreen = false;
                if ( mScreenStack[i]->isLoading() )
                {
                    mScreenStack[i]->loading( elapsedSec );
                }
                else
                {
                    if ( !mScreenStack[i]->isPaused() )
                    {
                        mScreenStack[i]->update(elapsedSec, (i == (mScreenStack.size()-1)) );
                        updatedScreen = true;
                    }
                }
                
                if( updatedScreen && !mScreenStack[i]->shouldUpdateScreensBelow() )
                {
                    break;
                }
            }
            
            if (mPostUpdateCallback)
            {
                UpdateOperationCallbackParameters params;
                params.delta = elapsedSec;
                
                mPostUpdateCallback->invoke( &params );
            }
            
            
            if (!mTransitions.empty() &&
                (!topScreen || topScreen->isMultiFrameLoadComplete()))
            {
                // The state is based on the first transition (index 0)
                unsigned int currentState = mTransitions[0]->update( elapsedSec );
                
                // But, we still need to update everyone
                for (int i = 1; i < mTransitions.size(); i++)
                {
                    mTransitions[i]->update(elapsedSec);
                }
                
                if (currentState != mCurrentTransitionPhase)
                {
                    // transition phase has changed, tell the screens
                    for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
                    {		
                        mScreenStack[i]->notifyTransitionPhaseChange(currentState);
                    }
                }
                
                // Push the screen if it's time to flush
                if (currentState == Transition::TP_Flush) 
                {
                    mFlushMethod = SCM_Immediate;
                }
                // Otherwise, the transition is finished and clear the refernce
                else if (currentState == Transition::TP_Finished) 
                {
                    for (int i = mTransitions.size() - 1; i >= 0; i--)
                    {
                        delete mTransitions[i];
                    }
                    
                    mTransitions.clear();
                }
                
                mCurrentTransitionPhase = (Transition::TransitionPhase)currentState;
            }
        }
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
    void ScreenManager::loadingOverlayDraw()
    {
		if ( !mTransitions.empty())
		{
			for (int i = mTransitions.size() - 1; i >= 0; i--)
			{
				mTransitions[i]->draw();
			}
		}
        
        for (unsigned int l = 0; l < mLayerCount; l++)
        {
            if (mLoadingOverlay)
            {
                mLoadingOverlay->draw(l);
            }
        }
    }
    
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::draw(DrawMode dm)
	{		
		if ( (dm & DM_PreDraw ) && mPreDrawCallback)
        {
			mPreDrawCallback->invoke(0);
		}
		
		if (dm & DM_Screens)
		{
			for (unsigned int l = 0; l < mLayerCount; l++)
			{
				for (unsigned int i = 0; i < mScreenStack.size(); i++)
                {
					mScreenStack[i]->draw(l);
                }
                
                if (mDebugInfoOverlay)
                {
                    mDebugInfoOverlay->draw(l);
                }
                
                if (mLoadingOverlay)
                {
                    mLoadingOverlay->draw(l);
                }
			}
		}
		
		// Draw the transitions (starting from the back)
		if ( (dm & DM_Transition ) && !mTransitions.empty()) 
		{
			for (int i = mTransitions.size() - 1; i >= 0; i--)
			{
				mTransitions[i]->draw();
			}
		}
		
		if ( (dm & DM_PostDraw ) && mPostDrawCallback)
        {
			mPostDrawCallback->invoke( 0 );
        }		
	}
	
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::touchDown( int touchID, Vector2 pos )
	{
        //print the position so it's easier to position HUD elements
        //Walaber::Logger::printf("wmw", Walaber::Logger::SV_ERROR, "Normalized screen coordinates: (%f, %f)\n", pos.X, pos.Y);
        
        
		// expand to screen coordinates from the normalized coordinates we received.
		pos *= ScreenCoord::getScreenResolution();
		
		//Walaber::Logger::printf("wmw", Walaber::Logger::SV_ERROR, 
		//						"touchDown: %d,%d\n", 
		//						(int)pos.X, (int)pos.Y);
		bool firstResponderClaimedTouch = false;
		
		if ( mTouchFirstResponderCallback ) 
		{
			TouchFirstResponderParamameters params;
			params.touchEvent = TET_Down;
			params.curPos = pos;
			params.touchID = touchID;
			
			mTouchFirstResponderCallback->invoke( &params );
			
			firstResponderClaimedTouch = params.outCanBeConsumed;
		}
		
		if( firstResponderClaimedTouch )
		{
			// the first responder wants that touch!
			return;
		}

		// No touches can go through when transitions are running
		if (mTransitions.empty())
		{
			if (mScreenStack.size() > 0)
			{
				for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
				{
					GameScreen* s = mScreenStack[i];
					
					s->touchDown(touchID, pos);
					
					if (s->consumesInput())
						return;
				}
			}
		}
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::touchMoved( int touchID, Vector2 curPos, Vector2 lastPos )
	{
		// expand to screen coordinates from the normalized coordinates we received.
		curPos *= ScreenCoord::getScreenResolution();
		lastPos *= ScreenCoord::getScreenResolution();
		
		
		//Walaber::Logger::printf("wmw", Walaber::Logger::SV_ERROR, 
		//						"touchMoved: %d,%d --> %d,%d\n", 
		//						(int)lastPos.X, (int)lastPos.Y, (int)curPos.X, (int)curPos.Y);
		
		bool firstResponderClaimedTouch = false;
		
		if ( mTouchFirstResponderCallback ) 
		{
			TouchFirstResponderParamameters params;
			params.touchEvent = TET_Move;
			params.curPos = curPos;
			params.lastPos = lastPos;
			params.touchID = touchID;
			mTouchFirstResponderCallback->invoke( &params );
			
			firstResponderClaimedTouch = params.outCanBeConsumed;
		}
		
		if( firstResponderClaimedTouch )
		{
			// the first responder wants that touch!
			return;
		}
		
		
		// No touches can go through when transitions are running
		if (mTransitions.empty())
		{
			if (mScreenStack.size() > 0)
			{
				for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
				{
					GameScreen* s = mScreenStack[i];
					
					s->touchMoved(touchID, curPos, lastPos);
					
					if (s->consumesInput())
						return;
				}
			}
		}
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::accelerometerChanged( float x, float y, float z )
	{
		if (mScreenStack.size() > 0)
		{
			for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
			{
				GameScreen* s = mScreenStack[i];
				
				s->accelerometerChanged(x, y, z);
				
				if (s->consumesInput())
					return;
			}
		}
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::touchUp( int touchID, Vector2 pos )
	{
		// expand to screen coordinates from the normalized coordinates we received.
		pos *= ScreenCoord::getScreenResolution();
		
		//Walaber::Logger::printf("wmw", Walaber::Logger::SV_ERROR, 
		//						"touchUp: %d,%d\n", 
		//						(int)pos.X, (int)pos.Y);
		bool firstResponderClaimedTouch = false;
		
		if ( mTouchFirstResponderCallback ) 
		{
			TouchFirstResponderParamameters params;
			params.touchEvent = TET_Up;
			params.curPos = pos;
			params.touchID = touchID;
			mTouchFirstResponderCallback->invoke( &params );
			
			firstResponderClaimedTouch = params.outCanBeConsumed;
		}
		
		if( firstResponderClaimedTouch )
		{
			// the first responder wants that touch!
			return;
		}
		
		
		// No touches can go through when transitions are running
		if (mTransitions.empty())
		{
			if (mScreenStack.size() > 0)
			{
				for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
				{
					GameScreen* s = mScreenStack[i];
					
					s->touchUp(touchID, pos);
					
					if (s->consumesInput())
						return;
				}
			}
		}
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::touchLost( int touchID )
	{
		// No touches can go through when transitions are running
		if (mTransitions.empty())
		{
			if (mScreenStack.size() > 0)
			{
				for (int i = (int)mScreenStack.size()-1; i >= 0; i--)
				{
					GameScreen* s = mScreenStack[i];
					
					s->touchLost(touchID);
				}
			}
		}
	}
	
	
	// --------========--------========--------========--------========--------========--------========--------========
	GameScreen* ScreenManager::peekTop()
	{
		if (mScreenStack.size() > 0)
			return mScreenStack.back();
		else
			return NULL;
	}
    
    // --------========--------========--------========--------========--------========--------========--------========
	bool ScreenManager::isTransitioning()
	{
		if (mTransitions.empty())
			return false;
		else
			return true;
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	GameScreen* ScreenManager::getScreenAt(int i)
	{
		assert(i < mScreenStack.size());
		
		return mScreenStack[i];
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	GameScreen* ScreenManager::getScreenWithName( unsigned int n )
	{
		for (unsigned int i = 0; i < mScreenStack.size(); i++)
		{
			if (mScreenStack[i]->getName() == n)
				return mScreenStack[i];
		}
		
		return NULL;
	}

	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::_pushScreen( GameScreen* s )
	{
#ifdef _DEBUG
		Walaber::Logger::printf("Walaber", Walaber::Logger::SV_FATAL, "ScreenManager::_pushScreen()");
#endif
		
		s->enter();
		
		if (mScreenStack.size() > 0)
			mScreenStack.back()->beingStacked();
		
		mScreenStack.push_back(s);
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::_popScreen(bool forcePop)
	{
		Logger::printf("Walaber", Logger::SV_DEBUG, "ScreenManager::_popScreen\n");
		
		if (mScreenStack.size() > 0 && (forcePop || mScreenStack.back()->isPoppable()) )
		{
			GameScreen* back = mScreenStack.back();
			back->exit();
			delete back;
			
			mScreenStack.pop_back();
			
			// clear out any unused textures...
			TextureManager::getManager().memoryWarning(false);	// non-critical, so "stayInMemory" textures are not killed.
			
			// and sounds
			SoundManager::getInstancePtr()->optimize(false);
		}
		
		if (mScreenStack.size() > 0)
			mScreenStack.back()->regainedTop();
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::_swapScreen( GameScreen* s )
	{
		Logger::printf("Walaber", Logger::SV_DEBUG, "ScreenManager::_swapScreen - new screen: %d\n", s->getName());
		
		s->enter();
		
		if (mScreenStack.size() > 0)
		{
			GameScreen* back = mScreenStack.back();
			back->exit();
			delete back;
			
			mScreenStack.pop_back();
			
			// clear out any unused textures...
			TextureManager::getManager().memoryWarning(false);	// non-critical, so "stayInMemory" textures are not killed.
			
			// and sounds
			SoundManager::getInstancePtr()->optimize(false);
		}
		
		_pushScreen(s);
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
	void ScreenManager::_goFlush()
	{
		// a flush was requested, execute it now!
		for (unsigned int i = 0; i < mPrepList.size(); i++)
		{
			// execute each operation in the order that it came in...Walaber::ScreenManager::SO_Pop
			ScreenOperationDetails sd = mPrepList[i];
			
			if (sd._operation == SO_Pop)
			{
				for (unsigned int p = 0; p < sd._count; p++)
					_popScreen(false); // false, don't force pop
			}
			if (sd._operation == SO_ForcePop)
			{
				for (unsigned int p = 0; p < sd._count; p++)
					_popScreen(true); // true, force pop
			}
			else if (sd._operation == SO_Push)
			{
				ScreenLoaderFunctionParameters params;
                params.inScreenID = sd._name;
				
				
				for (LoaderPriorityMap::iterator it = mLoaderFunctions.begin(); it != mLoaderFunctions.end(); ++it)
				{
					it->second->invoke( &params );
					
					// did it load a screen?
					if (params.outGameScreen)
						break;
				}
				
				
				if (params.outGameScreen)
				{
                    if (params.outGameScreen->shouldLoadAsync() == false)
                    {
                        params.outGameScreen->loadPropertyList( sd._plist );
                        
                        _pushScreen( params.outGameScreen );
                    }
                    else
                    {
                        params.outGameScreen->beginAsyncEnter( sd._plist );
                        
                        if (mScreenStack.size() > 0)
                            mScreenStack.back()->beingStacked();
                        
                        mScreenStack.push_back(params.outGameScreen);
                        
                        if (WalaberGame::IsLoadingThreadRunning())
                        {
                            mPrepList.erase(mPrepList.begin(), mPrepList.begin() + i + 1);
                            return;
                        }
                    }
				}
				else
				{
					Walaber::Logger::printf("Walaber", Walaber::Logger::SV_FATAL, "ERROR loading screen with name [%d], loader returned NULL.", sd._name);
				}
			}
			else if (sd._operation == SO_Swap)
			{
				ScreenLoaderFunctionParameters params;
                params.inScreenID = sd._name;
				
				for (LoaderPriorityMap::iterator it = mLoaderFunctions.begin(); it != mLoaderFunctions.end(); ++it)
				{
					it->second->invoke( &params );
					
					// did it load a screen?
					if (params.outGameScreen)
						break;
				}
				
				if (params.outGameScreen)
				{
					params.outGameScreen->loadPropertyList( sd._plist );
					
					_swapScreen( params.outGameScreen );
				}
				else
				{
					Walaber::Logger::printf("Walaber", Walaber::Logger::SV_FATAL, "ERROR loading screen with name [%d], loader returned NULL.", sd._name);
				}
			}
		}
		
		// clear the prep list.
		mPrepList.clear();	
		
		mFlushMethod = SCM_Immediate;
	}
	
	void ScreenManager::recreateGraphicsContext()
	{
		for (unsigned int i = 0; i < mScreenStack.size(); i++)
			mScreenStack[i]->recreateGraphicsContext();
	}

   
}
