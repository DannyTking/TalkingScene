//
//  SkeletonActorManager.h
//  WaterConcept
//
//  Created by Jason Pecho on 12/21/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _SKELETON_ACTOR_MANAGER_H_
#define _SKELETON_ACTOR_MANAGER_H_

#include <map>
#include <SkeletonActor.h>

namespace Walaber
{
	class SkeletonActorManager
	{
    private:
        
        struct NamedSkeletonActor
        {
            std::string         name;
            SkeletonActor*      actor;
        };
        
	public:
		//===========--------------=================-------------------==============----------
		//-----------==============------------------=================---------------==========
		static SkeletonActorManager* getInstancePtr()
		{
			static SkeletonActorManager instance;
			return &instance;
		}
        
        enum CreationCollisionBehavior
        {
            CreationCollisionBehavior_UseExisting,
            CreationCollisionBehavior_Override
        };
        
        enum ChannelBehavior
        {
            ChannelBehavior_Clear,
            ChannelBehavior_Append
        };
		
        typedef std::vector< NamedSkeletonActor >                       NamedSkeletonActorArray;
        typedef std::map< std::string, NamedSkeletonActorArray >            ChannelToNamedActor;
        
		~SkeletonActorManager()
		{
			ChannelToNamedActor::iterator it = mSkeletonActors.begin();
			ChannelToNamedActor::iterator itEnd = mSkeletonActors.end();
			
			while(it != itEnd)
			{
                NamedSkeletonActorArray& actorArray = it->second;
				for( unsigned int i = 0; i < actorArray.size(); ++i )
                {
                    delete it->second[i].actor;
                }
                actorArray.clear();
                
				++it;
			}
			mSkeletonActors.clear();
		}
		
		SkeletonActor* getSkeletonActorWithID( const std::string& channel,
                                               const std::string& actorID,
                                               CreationCollisionBehavior collisionBehavior,
                                               ChannelBehavior channelBehavior )
		{
			SkeletonActor* skeletonActor = 0;
			
            ChannelToNamedActor::iterator it = mSkeletonActors.find( channel );
            if( it != mSkeletonActors.end() )
            {
                NamedSkeletonActorArray& actorArray = it->second;
				for( unsigned int i = 0; i < actorArray.size(); ++i )
                {
                    bool actorExistsInChannel = false;
                    NamedSkeletonActorArray& actorArray = it->second;
                    for( unsigned int i = 0; i < actorArray.size(); ++i )
                    {
                        if( actorArray[i].name == actorID )
                        {
                            actorExistsInChannel = true;
                            // skeleton actor exists with this id
                            switch( collisionBehavior )
                            {
                                case CreationCollisionBehavior_UseExisting:
                                    skeletonActor = actorArray[i].actor;
                                    break;
                                
                                case CreationCollisionBehavior_Override:
                                    delete actorArray[i].actor;
                                    skeletonActor = new SkeletonActor;
                                    actorArray[i].actor = skeletonActor;
                            }
                        }
                    }
                    
                    if( !actorExistsInChannel )
                    {
                        switch( channelBehavior )
                        {
                            case ChannelBehavior_Clear:
                            {
                                // delete currently loaded skeleton actors
                                NamedSkeletonActorArray& actorArray = it->second;
                                for( unsigned int i = 0; i < actorArray.size(); ++i )
                                {
                                    delete it->second[i].actor;
                                }
                                actorArray.clear();
                                
                                skeletonActor = new SkeletonActor;
                                
                                NamedSkeletonActor namedActor;
                                namedActor.name = actorID;
                                namedActor.actor = skeletonActor;
                                
                                actorArray.push_back(namedActor);
                            }
                                break;
                                
                            case ChannelBehavior_Append:
                            {
                                NamedSkeletonActorArray& actorArray = it->second;
                                
                                skeletonActor = new SkeletonActor;
                                
                                NamedSkeletonActor namedActor;
                                namedActor.name = actorID;
                                namedActor.actor = skeletonActor;
                                
                                actorArray.push_back(namedActor);
                            }
                        }
                    }
                }
            }
            else
            {
                mSkeletonActors.insert( std::make_pair( channel, NamedSkeletonActorArray() ) );
                NamedSkeletonActorArray& actorArray = mSkeletonActors[channel];
                actorArray.push_back( NamedSkeletonActor() );
                NamedSkeletonActor& namedActor = actorArray.back();
                
                namedActor.name = actorID;
                namedActor.actor = new SkeletonActor;
                skeletonActor = namedActor.actor;
            }
			
			return skeletonActor;			
		}
		
		bool clearChannel( const std::string& channel )
		{
			bool ret = false;
			
            ChannelToNamedActor::iterator it = mSkeletonActors.find( channel );
            if( it != mSkeletonActors.end() )
            {
                NamedSkeletonActorArray& actorArray = it->second;
				for( unsigned int i = 0; i < actorArray.size(); ++i )
                {
                    delete it->second[i].actor;
                }
                actorArray.clear();
                
                mSkeletonActors.erase( it );
                
                ret = true;
            }
            
			return ret;
		}
        
	private:
		//===========--------------=================-------------------==============----------
		//-----------==============------------------=================---------------==========
		SkeletonActorManager():
		mSkeletonActors()
		{}

		
		ChannelToNamedActor                 mSkeletonActors;
	};
}

#endif
