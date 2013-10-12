//
//  ActionLoad.h
//  MickeyGame
//
//  Created by Christopher Gulliver on 01/26/13.
//  Copyright 2013 Disney Mobile. All rights reserved.
//

#ifndef _WALABER_ACTION_LOAD_H_
#define _WALABER_ACTION_LOAD_H_

// Standard Library Includes
#include <string>
#include <vector>

// Walaber Includes
#include <Action.h>
#include <ParticleEmitter.h>
#include <Sprite.h>

namespace Walaber
{
    /*!
    Boilerplate class for loading actions. This is because most loading actions are asynchronous
    and require a callback to inform when the action has been completed
    */
    class Action_Load : public IAction
    {
    public:
        /*!

        @param elapsedTime the time change since the last time execute was called
        @return true if the callback has been called
        */
        virtual bool execute( float elapsedTime )
        {
            if ( !mLoading )
            {
                executeLoad();
                mLoading = true;
            }

            return mCallbackReceived;
        }
        
    protected:
        Action_Load() :
            mLoading( false ),
            mCallbackReceived( false )
        {
            _setcallback();
        }
    
        virtual void executeLoad() = 0;

        /*!
        Setup the callback to point to _completedCallback
        */
        void _setcallback()
        {
            MemberCallbackPtr<Action_Load>::type memberCallback( \
                new MemberCallback<Action_Load>( this, &Action_Load::_completedCallback) );
            CallbackPtr mCallback = static_pointer_cast<Callback>( memberCallback );
        }

        /*!
        A callback method that will be called when the action is completed
        */
        void _completedCallback(void* data)
        {
            mCallbackReceived = true;
        }

        bool        mCallbackReceived;
        CallbackPtr mCallback;
        bool        mLoading;
    };

    /*!
    A specific Action to facilitate loading curves
    */
    class Action_LoadCurve : public Action_Load
    {
    public:
        Action_LoadCurve( CurvePtr & curve, const std::string& filename ) :
            mCurve( curve ),
            mFilename( filename )
        {
        }
        
    private:
        Action_LoadCurve();
        
        virtual void executeLoad()
        {
            CurveManager::getManager().getCurve( &mCurve, mFilename, mCallback );
        }

        CurvePtr    mCurve;
        std::string mFilename;
    };

    /*!
    A specific Action to facilitate loading particle emitters
    */
    class Action_LoadParticleEmitter : public Action_Load
    {
    public:
        Action_LoadParticleEmitter( ParticleEmitter & particleEmitter, const std::string& filename ) :
            mParticleEmitter( particleEmitter ),
            mFilename( filename )
        {
        }
        
    private:
        Action_LoadParticleEmitter();
        
        virtual void executeLoad()
        {
            mParticleEmitter.loadFromXML( mFilename, mCallback);
        }

        ParticleEmitter &   mParticleEmitter;
        std::string         mFilename;
    };

    /*!
    A specific Action to facilitate loading sprites
    */
    class Action_LoadSprite : public Action_Load
    {
    public:
        Action_LoadSprite( Sprite & sprite, const std::string& filename ) :
            mSprite( sprite ),
            mFilename( filename )
        {
        }
        
    private:
        Action_LoadSprite();
        
        virtual void executeLoad()
        {
            mSprite.loadFromXML( mFilename, mCallback );
        }

        Sprite &    mSprite;
        std::string mFilename;
    };

    /*!
    A specific Action to facilitate loading a texture atlas and handling its callback
    */
    class Action_LoadTextureAtlas : public Action_Load
    {
    public:
        Action_LoadTextureAtlas( const std::string& filename ) :
            mFilename( filename )
        {
        }
        
    private:
        Action_LoadTextureAtlas();
        
        virtual void executeLoad()
        {
            TextureManager::getManager().loadTextureAtlas( mFilename, mCallback );
        }

        std::string         mFilename;
    };

    /*!
    A specific Action to facilitate loading a texture atlas and handling its callback
    */
    class Action_ReloadTextureAtlas : public Action_Load
    {
    public:
        Action_ReloadTextureAtlas( const std::string& filename ) :
            mFilename( filename )
        {
        }
        
    private:
        Action_ReloadTextureAtlas();
        
        virtual void executeLoad()
        {
            TextureManager::getManager().reloadTextureAtlas( mFilename, mCallback );
        }

        std::string         mFilename;
    };

    /*!
    A specific Action to facilitate loading a texture atlas and handling its callback
    */
    class Action_LoadWidgetsXML : public Action_Load
    {
    public:
        Action_LoadWidgetsXML( const std::string& filename, WidgetManager* widgetMgr ) :
            mFilename( filename ),
            mWidgetMgr( *widgetMgr )
        {
        }
        
    private:
        Action_LoadWidgetsXML();
        
        virtual void executeLoad()
        {
            WidgetHelper::loadWidgetsXML( mFilename, &mWidgetMgr, mCallback );
        }

        std::string         mFilename;
        WidgetManager &     mWidgetMgr;
    };

    /*!
    */
    class Action_LoadMetaData : public Action_Load
    {
    };

    class Action_FileExists : public Action_Load
    {
    /*
		MemberCallbackPtr<Screen_PuppetShow>::type memberCallback( new MemberCallback<Screen_PuppetShow>(this, &Screen_PuppetShow::_fileExists) );
		CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
		FileManager::getInstancePtr()->fileExists( filename, callback, plist );
    */
    }

}

#endif // _WALABER_ACTION_LOAD_H_