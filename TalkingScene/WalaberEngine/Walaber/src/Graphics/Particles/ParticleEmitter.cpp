//
//  ParticleEmitter.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 1/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ParticleEmitter.h"

#include <SpriteBatch.h>
#include <FileManager.h>
#include <MemberCallback.h>
#include <XML.h>

namespace Walaber
{

	//---------==========-----------========----------==========-----------============-------------
	//---------==========-----------========----------==========-----------============-------------
	ParticleEmitter::ParticleEmitter(const int maxParticles):
	Node(),
	mParticleTextures(),		
	mEmitterZones(),
	mSizeRange( std::make_pair( Vector2::One, Vector2::One ) ),
    mStartSizeRange( std::make_pair( Vector2::One, Vector2::One ) ),    
	mGrowToRange( std::make_pair( Vector2::One, Vector2::One ) ),
	mDirectionRanges( ),
	mSpeedRange( std::make_pair( 0.0f, 0.0f ) ),
	mParticlesPerSecondRange( std::make_pair( 0.0f, 0.0f ) ),
	mMassRange( std::make_pair( 1.0f, 1.0f ) ),
	mAngleRange( std::make_pair( 0.0f, 0.0f ) ),
	mOmegaRange( std::make_pair( 0.0f, 0.0f ) ),
	mFadeTimeRange( std::make_pair( 0.5f, 0.5f ) ),
	mGravityForce( Vector2( 0.0f, 0.0f ) ),
    mApexTimeSpan( std::make_pair( 0.0f, 0.0f ) ),
    mSizeSlerpStart( false ),
    mSizeSlerpEnd( false ),    
	mFinishedLoadingCallback(),
	mCurrentTimePerParticle( 0.0f ),
	mParticleTimer( 0.0f ),
	mMultiplier( 1.0f ),
	mNumParticles( -1 ),
	mParticleSet(maxParticles),
	mParticlesUseGravity( false ),
	mActive( false ),
	mIsScreenSpace( false )
	{
	}
	
	
	//---------==========-----------========----------==========-----------============-------------
	//---------==========-----------========----------==========-----------============-------------
	ParticleEmitter::~ParticleEmitter()
	{
		if(mParent)
		{
			mParent->removeChild(this);
		}
	}
	
	//---------==========-----------========----------==========-----------============-------------
	//---------==========-----------========----------==========-----------============-------------
	void ParticleEmitter::loadFromXML(const std::string& xmlFile, CallbackPtr callback )
	{		
		mFinishedLoadingCallback = callback;
		
		MemberCallbackPtr<ParticleEmitter>::type memcallback( new MemberCallback<ParticleEmitter>(this, &ParticleEmitter::_xmlFileLoaded ));
		CallbackPtr loadingCallback = static_pointer_cast<Callback>(memcallback);
												
		FileManager::getInstancePtr()->readFile(xmlFile, loadingCallback, PropertyList());
	}
	
	//---------==========-----------========----------==========-----------============-------------
	//---------==========-----------========----------==========-----------============-------------
	void ParticleEmitter::update( float deltaTime )
	{
		if( mActive && mNumParticles && mCurrentTimePerParticle)
		{
			mParticleTimer += deltaTime;
			float numParts = mParticleTimer / mCurrentTimePerParticle;
			if( numParts >= 1 )
			{				
				// create numParts particles
				int iNumParts = (int)numParts;
				mParticleTimer -= (float)iNumParts * mCurrentTimePerParticle;
				
				for( unsigned int i = 0; i < iNumParts; ++i )
				{
					Vector2 partPos;
                    EmitterZone ez;
                                       
					// get an emitter zone
					if(!mEmitterZones.empty())
					{
						unsigned int emitterIndex = rand() % mEmitterZones.size();
						ez = mEmitterZones[emitterIndex];  //copy in emitter zone data and override the defaults
                        

                        // I think the right way to do this is to scale the radius by however much the length of the position vector scales, but for
                        // now I just scale it by the World Scales X component
                        /*
                        float range = randomRange( 0.0f, (getWorldScale().X * ez.mRadius) );
                        float rotRange = randomRange( 0.0f, Walaber::TWO_PI );
                        partPos = getWorldPosition2D() + (getWorldScale2D()*ez.mPosition) + VectorTools::rotateVector( Vector2( range, 0.0f ), rotRange );
            			*/
                        
                        //changed to do what i think the comment says ---
                        //hopefully, it doesn't screw anything up
                        float r = randomRange( 0.0f, ez.mRadius );
                        float a = randomRange( 0.0f, Walaber::TWO_PI);
                        partPos = getWorldPosition2D() + getWorldScale2D()*(ez.mPosition + Vector2( r*cosf(a), r*sinf(a) ));
                        
            
            
                        //figure for rectangular zones as well
                        Vector2 halfRect = ez.mRect / 2;
                        partPos += getWorldScale2D() * Vector2( randomRange(-halfRect.X, halfRect.X), randomRange(-halfRect.Y, halfRect.Y) );
					}
					else
					{
						// otherwise use the emiter position
						partPos = getWorldPosition2D();
					}                        
                        
                       
                    // TODO: make this relative to the world position of the node
                    // now get the direction and speed so we can give it a velocity
                    float rotRange = 0.0f; 
                    if( !mDirectionRanges.empty() )
                    {
                        int range = rand() % mDirectionRanges.size();
                        
                        rotRange = mDirectionRanges[range].first + randomRange( 0.0f, mDirectionRanges[range].second - mDirectionRanges[range].first );
                    }
                    

                    
                    //for ring spawns, we might spawn a ring of particles each time. 
                    for( int j=0; j<ez.mRingParticles ; j++)
                    {          
                    
                        float speed = 0.0f;
                        
                        if(!mIsScreenSpace)
                        {
                            speed = mSpeedRange.first + randomRange( 0.0f, mSpeedRange.second - mSpeedRange.first);
                        }
                        else
                        {
                            speed = mScreenSpeedRange.first + randomRange( 0.0f, mScreenSpeedRange.second - mScreenSpeedRange.first);
                        }
                        

                        
                        Vector2 size;
                        size.X = mSizeRange.first.X + randomRange( 0.0f, mSizeRange.second.X - mSizeRange.first.X );
                        size.Y = mSizeRange.first.Y + randomRange( 0.0f, mSizeRange.second.Y - mSizeRange.first.Y );
                        size *= getWorldScale2D();
                        
                        Vector2 growSize;
                        growSize.X = mGrowToRange.first.X + randomRange( 0.0f, mGrowToRange.second.X - mGrowToRange.first.X );
                        growSize.Y = mGrowToRange.first.Y + randomRange( 0.0f, mGrowToRange.second.Y - mGrowToRange.first.Y );
                        growSize *= getWorldScale2D();
                        
                        Vector2 startSize;
                        startSize.X = mStartSizeRange.first.X + randomRange( 0.0f, mStartSizeRange.second.X - mStartSizeRange.first.X );
                        startSize.Y = mStartSizeRange.first.Y + randomRange( 0.0f, mStartSizeRange.second.Y - mStartSizeRange.first.Y );   
                        startSize *= getWorldScale2D();
                        
                        float mass = mMassRange.first + randomRange( 0.0f, mMassRange.second - mMassRange.first );
                        
                        float angle = mAngleRange.first + randomRange( 0.0f, mAngleRange.second - mAngleRange.first );
                        
                        float omega = mOmegaRange.first + randomRange( 0.0f, mOmegaRange.second - mOmegaRange.first );
                        
                        float lifeTime = mLifeTimeRange.first + randomRange( 0.0f, mLifeTimeRange.second - mLifeTimeRange.first );
                        
                        float fadeTime = mFadeTimeRange.first + randomRange( 0.0f, mFadeTimeRange.second - mFadeTimeRange.first );
                        
                        int texIndex = rand() % mParticleTextures.size();
                        
                        int outIndex = -1;            
                                              
                                                              
                        float rotRangeOffset = (Walaber::TWO_PI * j)/ez.mRingParticles;                     
                        Vector2 actualPos = partPos;
                        Vector2 velocity =  transformVector(VectorTools::rotateVector( Vector2( 1.0f , 0.0f ), rotRange+rotRangeOffset ) ).normalisedCopy() * speed;     
                        
                                                                        //in radial mode, position them in a ring
                        if( ez.mRingRadius > 0.0f )
                        {
                            float r = ez.mRingRadius;
                            if(ez.mRingParticles > 1)
	                            actualPos += getWorldScale2D() * Vector2(r*cosf(rotRangeOffset), r*sinf(rotRangeOffset));
                            else //they must've meant, always spawn a certain distance away from an origin point
                            	actualPos += velocity.normalisedCopy()*ez.mRingRadius;
                        }                            
                        
                        Vector2 oldPos = actualPos - velocity;                          
                        
                        mParticleSet.addParticle( mass, actualPos, 
                                              oldPos, 
                                              angle, size, 
                                              Vector2::Zero, Vector2::Zero, 
                                              lifeTime, 
                                              omega, 
                                              texIndex, 
                                              outIndex );
                                              
                                              
                        if ( outIndex != -1 )
                        {
                            mParticleSet.mParticles[outIndex].mUseGravity = mParticlesUseGravity;
                            mParticleSet.mParticles[outIndex].mStartSize = startSize;                        
                            mParticleSet.mParticles[outIndex].mEndSize = growSize;
                            mParticleSet.mParticles[outIndex].mFadeLength = fadeTime;
                            mParticleSet.mParticles[outIndex].mSizeApexStart = mApexTimeSpan.first;
                            mParticleSet.mParticles[outIndex].mSizeApexEnd   = mApexTimeSpan.second;     
                            mParticleSet.mParticles[outIndex].mSizeSlerpUp   = mSizeSlerpStart;
                            mParticleSet.mParticles[outIndex].mSizeSlerpDown = mSizeSlerpEnd;
                        }                                              
                    }
                    
                    


                    float rand =  mParticlesPerSecondRange.first + randomRange( 0.0f, mParticlesPerSecondRange.second - mParticlesPerSecondRange.first );
                    
                    if(rand)
                    {
                        mCurrentTimePerParticle = 1.0f / rand;
                    }
                    else
                    {
                        mCurrentTimePerParticle = 0.0f;
                    }
                    
                    if( mNumParticles > 0 )
                    {
                        --mNumParticles;
                        
                        if( 0 == mNumParticles )
                        {
                            break;
                        }						
                    }                         
                        
				}         
                
			}
		}

		mParticleSet.updateParticles( deltaTime );
		
		for (int i = 0; i <= mParticleSet.getHighestLiveIndex(); i++) 
		{
			if ((mParticleSet.mParticles[i].mAlive) && (mParticleSet.mParticles[i].mUseGravity))
			{
				mParticleSet.mParticles[i].mForceAccum += mGravityForce*mMultiplier;
			}
		}
	}
	
	//---------==========-----------========----------==========-----------============-------------
	//---------==========-----------========----------==========-----------============-------------
	void ParticleEmitter::draw( SpriteBatch* sb )
	{
		// TODO: Make the layer definable
		// Draw the particle set
		mParticleSet.drawParticles( (*sb), mParticleTextures, Color(255, 255, 255), 100 );
	}
	
	//---------==========-----------========----------==========-----------============-------------
	//---------==========-----------========----------==========-----------============-------------
	void ParticleEmitter::_xmlFileLoaded(void* data)
	{
		FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
		ParticleEmitterLoadedParameters callbackParams(ResultCode_OK);
		
		if( params->result == ResultCode_OK )
		{
			XMLDocument xmlDoc( params->buffer, params->length, "Emitter" );
			
			// delete the raw file buffer now...
			delete[] params->buffer;
			
			
			if( xmlDoc )
			{
				Property prop;
				
				// load the emitter data from the doc
				XMLDocument::Iterator xmlIt = xmlDoc.getRootIterator();
				if( xmlIt.getAttribute( "name", prop ) )
				{
					setName( prop.asString() );
				}
				
				if( xmlIt.getAttribute( "maxParticles", prop ) )
				{
					mParticleSet.initParticles( prop.asInt() );
				}
				
				if( xmlIt.getAttribute( "active", prop ) )
				{
					mActive = prop.asInt() ? true : false ;
				}
				
				XMLDocument::Iterator childIt = xmlIt.getChildIterator();
				while( childIt )
				{
					if( "EmitterZones" == childIt.getNodeName() )
					{
						XMLDocument::NamedIterator emitIt = childIt.getChildNamedIterator( "EmitterZone" );
						
						while( emitIt )
						{
                            EmitterZone ez;
							// get zone data - don't need all data to add a zone anymore
							if( emitIt.getAttribute( "position", prop ) )
                            	ez.mPosition = prop.asVector2();
                                
                            if( emitIt.getAttribute( "radius", prop ) )
								ez.mRadius = prop.asFloat();
									
                            if( emitIt.getAttribute( "rect", prop ) )
                               	ez.mRect = prop.asVector2();
                            
                            if( emitIt.getAttribute( "ringparticles", prop ) )
                            	ez.mRingParticles = prop.asInt();
                            
                            if( emitIt.getAttribute( "ringradius", prop ) )
                            	ez.mRingRadius = prop.asFloat();
                                    
	                        addEmitterZone( ez );
							
							++emitIt;
						}
					}
					else if( "Textures" == childIt.getNodeName() )
					{
						XMLDocument::NamedIterator texIt = childIt.getChildNamedIterator( "Texture" );
						
						while( texIt )
						{
							if( texIt.getAttribute( "path", prop ) )
							{
								TexturePtr tex = TextureManager::getManager().getTexture( prop.asString() );
								mParticleTextures.push_back( tex );
							}
							
							++texIt;
						}
					}
					else if( "Gravity" == childIt.getNodeName() )
					{
						if( childIt.getAttribute( "force", prop ) )
						{
							mParticlesUseGravity = true;
							
							mGravityForce = prop.asVector2();
						}
					}
					else if( "Particles" == childIt.getNodeName() )
					{
						if( childIt.getAttribute( "number", prop ) )
						{
							mNumParticles = prop.asInt();
						}
						
						// now go through the rest of the particle properties
						XMLDocument::Iterator partIt = childIt.getChildIterator();
						while( partIt )
						{
							if( "Size" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									Vector2 min = prop.asVector2();
									Vector2 max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asVector2();
									}
									else
									{
										max = min;
									}
									
									setSizeRange( min, max );
									setGrowToRange( min, max );
								}								
							}
							else if( "StartSize" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									Vector2 min = prop.asVector2();
									Vector2 max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asVector2();
									}
									else
									{
										max = min;
									}
									
									setStartSizeRange( min, max );
								}
							}                            
							else if( "GrowToSize" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									Vector2 min = prop.asVector2();
									Vector2 max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asVector2();
									}
									else
									{
										max = min;
									}
									
									setGrowToRange( min, max );
								}
							}
							else if( "Directions" == partIt.getNodeName() )
							{
								XMLDocument::NamedIterator directionIt = partIt.getChildNamedIterator("Direction");
								
								while(directionIt)
								{
									if( directionIt.getAttribute( "min", prop ) )
									{
										float min = prop.asFloat();
										float max;
										if( directionIt.getAttribute( "max", prop ) )
										{
											max = prop.asFloat();
										}
										else
										{
											max = min;
										}
										
										addDirectionRangeDegrees( min, max );
									}
									
									++directionIt;
								}
							}
							else if( "Speed" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setSpeedRange( min, max );
								}
							}
							else if( "ScreenSpeed" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setScreenSpeedRange( min, max );
								}
							}
							else if( "ParticlesPerSecond" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setParticlesPerSecond( min, max );
									float rand =  mParticlesPerSecondRange.first + randomRange( 0.0f, mParticlesPerSecondRange.second - mParticlesPerSecondRange.first );
									
									if(rand)
									{
										mCurrentTimePerParticle = 1.0f / rand;
									}
									else
									{
										mCurrentTimePerParticle = 0.0f;
									}
								}
							}
							else if( "Mass" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setMassRange( min, max );
								}
							}
							else if( "Angle" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setAngleRange( min, max );
								}
							}
							else if ( "Omega" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setOmegaRange( min, max );
								}
							}
							else if( "LifeTime" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setLifeTimeRange( min, max );
								}
							}
							else if( "FadeTime" == partIt.getNodeName() )
							{
								if( partIt.getAttribute( "min", prop ) )
								{
									float min = prop.asFloat();
									float max;
									if( partIt.getAttribute( "max", prop ) )
									{
										max = prop.asFloat();
									}
									else
									{
										max = min;
									}
									
									setFadeTimeRange( min, max );
								}
							}
							else if( "Apex" == partIt.getNodeName() )
							{
                            	//given in percent of lifetime
                            	//using "min" and "max" here would be misleading, as it's a time range, not a random range like the rest
								if( partIt.getAttribute( "start", prop ) )
								{
									float start = prop.asFloat();
									float end;
									if( partIt.getAttribute( "end", prop ) )
									{
										end = prop.asFloat();
									}
									else
									{
										end = start;
									}
									
									setApexTimeSpan( start, end );
								}								
							}
							else if( "SizeSlerp" == partIt.getNodeName() )
							{
                            	//should we slerp into/out of the size?
								if( partIt.getAttribute( "start", prop ) )
								{
									bool start = prop.asInt();
									bool end;
									if( partIt.getAttribute( "end", prop ) )
									{
										end = prop.asInt();
									}
									else
									{
										end = start;
									}
									
									setSizeSlerp( start, end );
								}								
							}                            
							
							++partIt;
						}
					}
					
					++childIt;
				}
			}
			else
			{
				callbackParams.result = ResultCode_ErrorXMLParseError;
			}
		}
		else
		{
			callbackParams.result = ResultCode_ErrorFileNotFound;
		}
		
		
		
		if(mFinishedLoadingCallback)
		{
			mFinishedLoadingCallback->invoke(&callbackParams);
		}
	}
}
