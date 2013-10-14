/*
 *  SpriteBatch.cpp
 *  JellyCar3
 *
 *  Created by Jason Pecho on 1/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SpriteBatch.h"

#include <StringHelper.h>

#include <assert.h>

namespace Walaber
{	
    //                                  { 0.0f, 45.0f, -45.0f, 90.0f, -90.0f, 135.0f, -135.0f, 180.0f }
    const float SpriteBatch::mTeardropAngles[] = { 0.0f, .785398163f, -.785398163f, 1.57079633f, -1.57079633f, 2.35619449f, -2.35619449f, 3.14159265f };
    
	const Vector2 SpriteBatch::mTeardropOffsets[] = { 
        Vector2(0.0f, 1.0f), 
        Vector2(-0.707106769f, 0.707106769f), 
        Vector2(0.707106769f, 0.707106769f), 
        Vector2(-1.0f, 0.0f), 
        Vector2(1.0f, 0.0f), 
        Vector2(-0.707106769f, -0.707106769f), 
        Vector2(0.707106769f, -0.707106769f), 
        Vector2(0.0f, -1.0f) 
    };
    
    const Vector2 SpriteBatch::mTeardropUVs[] = {
        Vector2(0.5f, 1.0f),
        Vector2(0.1464466155f,0.8535533845f), 
        Vector2(0.8535533845f,0.8535533845f), 
        Vector2(0.0f, 0.5f), 
        Vector2(1.0f, 0.5f), 
        Vector2(0.1464466155f,0.1464466155f), 
        Vector2(0.8535533845f,0.1464466155f), 
        Vector2(0.5f, 0.0f), 
    };
    
	///--------------------------------------------------------------------
	SpriteBatch::SpriteBatch():
	mLayeredBatches(),
	mBatchMode(BM_NONE),
	mTopLayer(0),
	mCurrentLayer(0),
    mLastLayer(-1),
    mLastTexture(-1),
    mLastDepth(0.0f),
    mLastDrawAction(0)
	{

    }

	///--------------------------------------------------------------------
	SpriteBatch::SpriteBatch(const SpriteBatch& sprBch):
	mLayeredBatches(sprBch.mLayeredBatches),
	mBatchMode(sprBch.mBatchMode),
	mTopLayer(sprBch.mTopLayer),
	mCurrentLayer(sprBch.mCurrentLayer),
    mLastLayer(sprBch.mLastLayer),
    mLastTexture(sprBch.mLastTexture),
    mLastDepth(sprBch.mLastDepth),
    mLastDrawAction(sprBch.mLastDrawAction)
	{}

	///--------------------------------------------------------------------
	SpriteBatch& SpriteBatch::operator=(const SpriteBatch& sprBch)
	{
		mLayeredBatches = sprBch.mLayeredBatches;
		mBatchMode = sprBch.mBatchMode;
		mTopLayer = sprBch.mTopLayer;
		mCurrentLayer = sprBch.mCurrentLayer;
        mLastLayer = sprBch.mLastLayer;
        mLastTexture = sprBch.mLastTexture;
        mLastDepth = sprBch.mLastDepth;
        mLastDrawAction = sprBch.mLastDrawAction;
		
		return *this;
	}

	///--------------------------------------------------------------------
	SpriteBatch::~SpriteBatch()
	{
        unsigned int size = mDrawActionPool.size();
        for( unsigned int i = 0; i < size; ++i )
		{
            delete mDrawActionPool[i];
		}
	}

	///--------------------------------------------------------------------
	void SpriteBatch::start()
	{
		start(BM_DEFAULT);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::start(BatchMode bm)
	{
		if(mBatchMode != BM_NONE)
		{
			printf("ERROR: start has been called before calling flush after the last call to start\n");
			assert(0);
		}
		mBatchMode = bm;
		mTopLayer = 0;
	}

	///--------------------------------------------------------------------
	void SpriteBatch::flush()
	{	
		switch(mBatchMode)
		{	
            case BM_IMMEDIATE:
                
            case BM_LAYERS_IMMEDIATE:
                
                _flushImmediate();
                _reset();
                
                break;
                
			case BM_LAYERS:
                
            case BM_OPTIMAL:
				
				_flushOptimal();
				_reset();
                
                break;
                
            case BM_DEPTH:
                
                _flushDepth();
                _reset();
                
				break;
			default:
				printf("ERROR: flush has been called before a call to start\n");
				assert(0);
		}
	}

	///--------------------------------------------------------------------
	void SpriteBatch::drawQuad(const TexturePtr textureName, const Vector2& pos, float angle, const Vector2& size, const Color& c, const VertexColorBlendMode bm)
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addQuadToBatchImmediate(textureName, pos, angle, size, c, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
				_addQuadToBatchImmediate(textureName, pos, angle, size, c, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
                mCurrentLayer = mTopLayer++;
				_addQuadToBatchOptimal(textureName, pos, angle, size, c, bm);
				
				break;
                
			case BM_OPTIMAL:
				
                _addQuadToBatchOptimal(textureName, pos, angle, size, c, bm);
                
				break;
                
            case BM_DEPTH:
                
                mCurrentLayer = mTopLayer++;
                _addQuadToDepthBatch(100.0f, mCurrentLayer, textureName, pos, angle, size,
                                     Rect::UnitRect, c, bm);
                
                break;
			
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}		
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawQuad(const int layer, const TexturePtr textureName, const Vector2& pos, float angle, const Vector2& size, const Color& c, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addQuadToBatchImmediate(textureName, pos, angle, size, c, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = layer;
                if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}	
				_addQuadToBatchImmediate(textureName, pos, angle, size, c, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}			
                _addQuadToBatchOptimal(textureName, pos, angle, size, c, bm);
				break;
                
			case BM_OPTIMAL:
				 
                _addQuadToBatchOptimal(textureName, pos, angle, size, c, bm);
				
				break;
                
            case BM_DEPTH:
                
                mCurrentLayer = layer;
				if (layer >= mTopLayer)
				{
					mTopLayer = layer + 1;
				}
                _addQuadToDepthBatch(100.0f, mCurrentLayer, textureName, pos, angle, size,
                                     Rect::UnitRect, c, bm);
    
                break;
                
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawQuad(const TexturePtr textureName, const Vector2& pos, float angle, const Vector2& size, const Rect& uvs, const Color& c, const VertexColorBlendMode bm)
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addQuadToBatchImmediate(textureName, pos, angle, size, uvs, c, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
				_addQuadToBatchImmediate(textureName, pos, angle, size, uvs, c, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = mTopLayer++;
                _addQuadToBatchOptimal(textureName, pos, angle, size, uvs, c, bm);
				
				break;
                
			case BM_OPTIMAL:
				
                _addQuadToBatchOptimal(textureName, pos, angle, size, uvs, c, bm);
				
				break;
                
            case BM_DEPTH:
                
                mCurrentLayer = mTopLayer++;
                _addQuadToDepthBatch(100.0f, mCurrentLayer, textureName, pos, angle, size,
                                     uvs, c, bm);
                
                break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}		
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawQuad(const int layer, const TexturePtr textureName, const Vector2& pos, float angle, 
							   const Vector2& size, const Rect& uvs, const Color& c, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addQuadToBatchImmediate(textureName, pos, angle, size, uvs, c, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = layer;
                if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}	
				_addQuadToBatchImmediate(textureName, pos, angle, size, uvs, c, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}			
                _addQuadToBatchOptimal(textureName, pos, angle, size, uvs, c, bm);

				break;
                
			case BM_OPTIMAL:
				
                _addQuadToBatchOptimal(textureName, pos, angle, size, uvs, c, bm);
				
				break;
                
            case BM_DEPTH:
                
                mCurrentLayer = layer;
				if (layer >= mTopLayer)
				{
					mTopLayer = layer + 1;
				}
                _addQuadToDepthBatch(100.0f, mCurrentLayer, textureName, pos, angle, size,
                                     Rect::UnitRect, c, bm);
                
                break;
        
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawQuad(const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addQuadToBatchImmediate(textureName, pos, angle, size, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
				_addQuadToBatchImmediate(textureName, pos, angle, size, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = mTopLayer++;
				_addQuadToBatchOptimal(textureName, pos, angle, size, uvs, colors, bm);
				
				break;
                
			case BM_OPTIMAL:				
				
				_addQuadToBatchOptimal(textureName, pos, angle, size, uvs, colors, bm);
				
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}	
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawQuad(const int layer, const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addQuadToBatchImmediate(textureName, pos, angle, size, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = layer;
                if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}	
				_addQuadToBatchImmediate(textureName, pos, angle, size, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS:
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}			
				_addQuadToBatchOptimal(textureName, pos, angle, size, uvs, colors, bm);
				break;
                
			case BM_OPTIMAL:
				
				_addQuadToBatchOptimal(textureName, pos, angle, size, uvs, colors, bm);
				
				break;
                
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
	
	///--------------------------------------------------------------------

	void SpriteBatch::drawQuadColored(const Vector2& pos, const float angle, const Vector2& size,
                                      const Color& c)
	{
		float radAngle = (angle);
		float cosAngle = (float)cosf(radAngle);
		float sinAngle = (float)sinf(radAngle);
		
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
            {
                DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                DrawAction* drawAction = _getDrawAction();
                assert( drawAction != NULL );
                drawAction->mTextureName = 0;
                
                _addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
                
                drawActionList.push_back(drawAction);
            }
                break;
                
            case BM_LAYERS_IMMEDIATE:

                mCurrentLayer = mTopLayer++;
            {
                DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                DrawAction* drawAction = _getDrawAction();
                assert( drawAction != NULL );
                drawAction->mTextureName = 0;
                
                _addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
                
                drawActionList.push_back(drawAction);
            }
                break;
                
            case BM_LAYERS:
				
				mCurrentLayer = mTopLayer++;
            
            {    
                DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                assert( drawAction != NULL );
				_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
			}	
                
                break;
                
			case BM_OPTIMAL:
				
            {    
                DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                assert( drawAction != NULL );
				_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
			}	
				
				break;

            case BM_DEPTH:
                
                mCurrentLayer = mTopLayer++;
                _addQuadToDepthBatch(100.0f, mCurrentLayer, TexturePtr(), pos, angle, size,
                                     Rect::UnitRect, c, VCBM_Multiply);
                
                break;
                
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawQuadColored(const int layer, const Vector2& pos, const float angle, const Vector2& size, const Color& c)
	{
		float radAngle = (angle);
		float cosAngle = (float)cosf(radAngle);
		float sinAngle = (float)sinf(radAngle);
		
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
            {                
                DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                DrawAction* drawAction = _getDrawAction();
                assert( drawAction != NULL );
                drawAction->mTextureName = 0;
                
                _addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
                
                drawActionList.push_back(drawAction);
            }
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
            {  
                DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                DrawAction* drawAction = _getDrawAction();
                assert( drawAction != NULL );
                drawAction->mTextureName = 0;
                
                _addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
                
                drawActionList.push_back(drawAction);
            }   
                break;
                
            case BM_LAYERS:
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}		
				
            {    
                DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                assert( drawAction != NULL );
				_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
			}	
                
                break;
                
			case BM_OPTIMAL:
				
            {    
                DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                assert( drawAction != NULL );
				_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, Rect::UnitRect, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), VCBM_Multiply);
			}	
				
				break;
                
            case BM_DEPTH:
                
                mCurrentLayer = layer;
				if (layer >= mTopLayer)
				{
					mTopLayer = layer + 1;
				}
                _addQuadToDepthBatch(100.0f, mCurrentLayer, TexturePtr(), pos, angle, size,
                                     Rect::UnitRect, c, VCBM_Multiply);
                
                break;
                
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
    
    ///--------------------------------------------------------------------

	void SpriteBatch::drawCircle(const int layer, const TexturePtr textureName, const Vector2& pos,
                                 const float radius, const Rect& uvs, const Color& c,
                                 const int segmentCount, const VertexColorBlendMode bm )
    {
        switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = textureName->getName();
                
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                    drawActionList.push_back(drawAction);
                }                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = layer;
                if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}	
				
                // draw it...
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = textureName->getName();
                    
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                    drawActionList.push_back(drawAction);
                }  
                break;
                
            case BM_LAYERS:
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}
                
                // draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                }
				break;
                
			case BM_OPTIMAL:
				// draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                }
				break;
                
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
    }
    
    ///--------------------------------------------------------------------
	void SpriteBatch::drawCircle(const TexturePtr textureName, const Vector2& pos, const float radius, const Rect & uvs, const Color& c, const int segmentCount, const VertexColorBlendMode bm )
    {
        switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = textureName->getName();
                    
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                    drawActionList.push_back(drawAction);
                }  
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = textureName->getName();
                    
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                    drawActionList.push_back(drawAction);
                }  
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = mTopLayer++;
				// draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                }
				break;
                
			case BM_OPTIMAL:				
				
				// draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, uvs, c, segmentCount, bm);
                    
                }
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}	
    }

    ///--------------------------------------------------------------------
    void SpriteBatch::drawSweptCircle(const TexturePtr textureName, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const float percentOfCircle, const VertexColorBlendMode bm )
    {
        Rect centeredUVs = uvs;
        centeredUVs.size *= 0.5f;
        centeredUVs.upper_left += centeredUVs.size;
    
        switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = textureName->getName();
                    drawAction->mDrawMode = DM_TRIANGLE_FAN;

                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, centeredUVs, c, segmentCount, percentOfCircle, bm);
                    
                    drawActionList.push_back(drawAction);
                }  
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = textureName->getName();
                    drawAction->mDrawMode = DM_TRIANGLE_FAN;
                    
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, centeredUVs, c, segmentCount, percentOfCircle, bm);
                    
                    drawActionList.push_back(drawAction);
                }  
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = mTopLayer++;
				// draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName(), DM_TRIANGLE_FAN);
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, centeredUVs, c, segmentCount, percentOfCircle, bm);
                    
                }
				break;
                
			case BM_OPTIMAL:				
				
				// draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName(), DM_TRIANGLE_FAN);
                    _addCircleToBatchUVRect(drawAction->mVerts, drawAction->mBlendSections, pos, radius, centeredUVs, c, segmentCount, percentOfCircle, bm);
                    
                }
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}	
    }
    
    ///--------------------------------------------------------------------
	void SpriteBatch::drawCircleColored(const Vector2& pos, const float radius, const Color& c)
    {
        switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = 0;
                    
                    _addCircleToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, radius, c, VCBM_Multiply);
                    
                    drawActionList.push_back(drawAction);
                }
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mTextureName = 0;
                    
                    _addCircleToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, radius, c, VCBM_Multiply);
                    
                    drawActionList.push_back(drawAction);
                }
                break;
                
            case BM_LAYERS:
				
				mCurrentLayer = mTopLayer++;
                
                // draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                    _addCircleToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, radius, c, VCBM_Multiply);
                    
                }	
                
                break;
                
			case BM_OPTIMAL:
				
                // draw it..
                {
                    DrawAction* drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                    _addCircleToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, radius, c, VCBM_Multiply);
                    
                }
				
				break;
                
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}

    }
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawLineColored(const int layer, const Vector2& ptA, const Vector2& ptB, const float width, const Color& c)
	{
		Vector2 AtoB = ptB - ptA;
		
		Vector2 Center = ptA.lerp(ptB, 0.5f);
		float angle = atan2(AtoB.Y, AtoB.X);
		
		drawQuadColored(layer, Center, angle, Vector2( AtoB.length(), width ), c );
	}
		
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawTri(const TexturePtr textureName, const Vector2* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addVertsToBatchImmediate(textureName, 3, true, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
				_addVertsToBatchImmediate(textureName, 3, true, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
                mCurrentLayer = mTopLayer++;
				_addVertsToBatchOptimal(textureName, 3, true, positions, uvs, colors, bm);
				
				break;
                
			case BM_OPTIMAL:
				
                _addVertsToBatchOptimal(textureName, 3, true, positions, uvs, colors, bm);
                
				break;				
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}		
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawTri(const int layer, const TexturePtr textureName, const Vector2* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addVertsToBatchImmediate(textureName, 3, true, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = layer;
                if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}	
				_addVertsToBatchImmediate(textureName, 3, true, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}			
                _addVertsToBatchOptimal(textureName, 3, true, positions, uvs, colors, bm);
				break;
                
			case BM_OPTIMAL:
				
                _addVertsToBatchOptimal(textureName, 3, true, positions, uvs, colors, bm);
				
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawVerts(const TexturePtr textureName, const unsigned int count, const bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addVertsToBatchImmediate(textureName, count, addDegenerates, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = mTopLayer++;
				_addVertsToBatchImmediate(textureName, count, addDegenerates, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
                mCurrentLayer = mTopLayer++;
				_addVertsToBatchOptimal(textureName, count, addDegenerates, positions, uvs, colors, bm);
				
				break;
                
			case BM_OPTIMAL:
				
                _addVertsToBatchOptimal(textureName, count, addDegenerates, positions, uvs, colors, bm);
                
				break;				
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}		
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawVerts(const int layer, const TexturePtr textureName, const unsigned int count, const bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _addVertsToBatchImmediate(textureName, count, addDegenerates, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
                
                mCurrentLayer = layer;
                if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}	
				_addVertsToBatchImmediate(textureName, count, addDegenerates, positions, uvs, colors, bm);
                
                break;
                
            case BM_LAYERS: // OPTIMAL per layer
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
				{
					mTopLayer = layer + 1;
				}			
                _addVertsToBatchOptimal(textureName, count, addDegenerates, positions, uvs, colors, bm);
				break;
                
			case BM_OPTIMAL:
				
                _addVertsToBatchOptimal(textureName, count, addDegenerates, positions, uvs, colors, bm);
				
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
    
    void SpriteBatch::drawVertsWithDepth(const float depth, const int layer, const TexturePtr textureName, const unsigned int count, const bool addDegenerates, const Vector3* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
    {        
        mCurrentLayer = layer;
        
        unsigned int uiTexName = textureName ? textureName->getName() : 0;
        
        // This is to maintain backwards compatibility, we should refactor this class
        DrawAction* drawAction = 0;
        if( mBatchMode == BM_DEPTH )
        {
            drawAction = _getDrawActionForTextureAtDepth( depth, uiTexName );
        }
        else
        {
            drawAction = _getDrawActionForTextureOnLayer( layer, uiTexName );
        }
		
		int totalCount = count + (addDegenerates ? 2 : 0);
		_updateBlendSectionList( drawAction->mBlendSections, totalCount, bm );
		
        
		for (unsigned int i = 0; i < count; i++)
		{
			unsigned int c = (colors ? packColor(colors[i].R, colors[i].G, colors[i].B, colors[i].A) : packColor(255, 255, 255, 255));
			
			PositionTextureColorVert vert( positions[i], uvs[i].X, uvs[i].Y, c);
			
			drawAction->mVerts.push_back( vert );
			
			
			if (addDegenerates && ((i == 0) || (i == (count-1))))
			{
				// duplicate this vert!
				drawAction->mVerts.push_back( vert );
			}
		}
    }
    
	///--------------------------------------------------------------------
	void SpriteBatch::drawString(BitmapFont* font, const std::string text, const Vector2& pos, float angle, float scaleFactor, const Color& c, const Justification just)
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:

                _drawString(font, text, pos, angle, scaleFactor, c, just, font->getColorBlendMode());

                break;
            
            case BM_LAYERS_IMMEDIATE:
            case BM_LAYERS:
				
				mCurrentLayer = mTopLayer++;
				_drawString(font, text, pos, angle, scaleFactor, c, just, font->getColorBlendMode());

                break;
            
			case BM_OPTIMAL:				
				
				_drawString(font, text, pos, angle, scaleFactor, c, just, font->getColorBlendMode());
				
				break;
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
		
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawString(int layer, BitmapFont* font, const std::string text, const Vector2& pos, float angle, float scaleFactor, const Color& c, const Justification just)
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _drawString(font, text, pos, angle, scaleFactor, c, just, font->getColorBlendMode());
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
            case BM_LAYERS:
				
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}
				
				_drawString(font, text, pos, angle, scaleFactor, c, just, font->getColorBlendMode());
				
				break;
                
			case BM_OPTIMAL:
				
				_drawString(font, text, pos, angle, scaleFactor, c, just, font->getColorBlendMode());
				
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::drawStringClipped(BitmapFont* font, const std::string text, const Vector2& pos, const Rect& rClip,
						  float angle, float scaleFactor, const Color& c, const Justification just)
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _drawStringClip(font, text, pos, rClip, angle, scaleFactor, c, just, font->getColorBlendMode());
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
            case BM_LAYERS:
			{
				mCurrentLayer = mTopLayer++;
				_drawStringClip(font, text, pos, rClip, angle, scaleFactor, c, just, font->getColorBlendMode());
			}
				break;
                
			case BM_OPTIMAL:				
			{
				_drawStringClip(font, text, pos, rClip, angle, scaleFactor, c, just, font->getColorBlendMode());
			}	
				break;
				
			
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}

	///--------------------------------------------------------------------
	void SpriteBatch::drawStringClipped(const int layer, BitmapFont* font, const std::string text, const Vector2& pos, 
						const Rect& rClip, float angle, float scaleFactor, const Color& c, const Justification just)
	{
		switch(mBatchMode)
		{
            case BM_IMMEDIATE:
                
                _drawStringClip(font, text, pos, rClip, angle, scaleFactor, c, just, font->getColorBlendMode());
                
                break;
                
            case BM_LAYERS_IMMEDIATE:
            case BM_LAYERS:
			{
				mCurrentLayer = layer;
				if (layer >= mTopLayer) 
                {
					mTopLayer = layer + 1;
				}
				_drawStringClip(font, text, pos, rClip, angle, scaleFactor, c, just, font->getColorBlendMode());
			}	
				break;
                
			case BM_OPTIMAL:
			{
				_drawStringClip(font, text, pos, rClip, angle, scaleFactor, c, just, font->getColorBlendMode());
			}	
				break;
				
			default:
				printf("ERROR: draw has been called before a call to start\n");
				assert(0);
		}
	}
    
    
    
    ///--------------------------------------------------------------------
	void SpriteBatch::setDefaultProjectionMatrix()
    {
		// PROJECTION
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);
        
		GraphicsGL::createOrthoProjection(0, ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y, 0, 0.1f, 1.0f);
										  
		// VIEW MATRIX
        GraphicsGL::setMatrixModeAndLoadIdentity(GL_MODELVIEW);
    }
    
    
    ///--------------------------------------------------------------------
	void SpriteBatch::setCameraProjectionMatrix( Vector2& camPos, Vector2& camSize )
    {
        // PROJECTION
		GraphicsGL::setMatrixModeAndLoadIdentity(GL_PROJECTION);
        
        Vector2 camHalfSize = camSize * 0.5f;
        GraphicsGL::createOrthoProjection( camPos.X - camHalfSize.X, camPos.X + camHalfSize.X, camPos.Y - camHalfSize.Y, camPos.Y + camHalfSize.Y, 0.1f, 1.0f );
      
		// VIEW MATRIX
        GraphicsGL::setMatrixModeAndLoadIdentity(GL_MODELVIEW);
    }
    
    ///--------------------------------------------------------------------
    SpriteBatch::DrawAction* SpriteBatch::_getDrawAction()
    {
        DrawAction* ret = 0;
        unsigned int index;
        
        if(mUnusedDrawActions.empty())
        {
            ret = new DrawAction();
            mDrawActionPool.push_back( ret );
            //index = mDrawActionPool.size()-1;
        }
        else
        {
            // get the first unused draw action
            index = mUnusedDrawActions.back();
            ret = mDrawActionPool[index];
            
            // clear the vert list
            ret->mVerts.clear();
			ret->mBlendSections.clear();
            mUnusedDrawActions.pop_back();
        }
        
        return ret;        
    }
    
    SpriteBatch::DrawAction* SpriteBatch::_getDrawActionForTextureAtDepth( const float depth,
                                                                           unsigned int textureName,
                                                                           DrawMode drawMode )
    {
        DrawAction* drawAction = 0;
        
        if( textureName == mLastTexture &&
           mCurrentLayer == mLastLayer &&
           isEqualf( depth, mLastDepth ) )
        {
            drawAction = mLastDrawAction;
        }
        else
        {
            mLastLayer = mCurrentLayer;
            mLastDepth = depth;
            drawAction = _getDrawAction();
            mLastDrawAction = drawAction;
        
            drawAction->mTextureName = textureName;
            mLastTexture = textureName;

            drawAction->mLayer = mCurrentLayer;
            drawAction->mDepth = depth;
            drawAction->mDrawMode = drawMode;
            
            DrawActionList::iterator insertIter = mDepthBatch.end();
            DrawActionList::iterator iter;
            for (iter = mDepthBatch.begin(); iter != mDepthBatch.end(); iter++)
            {
                DrawAction* currentActionInList = (*iter);
                
                bool insertHere = false;
                if( isEqualf( drawAction->mDepth, currentActionInList->mDepth ) )
                {
                    insertHere = ( drawAction->mLayer < currentActionInList->mLayer );
                }
                else
                {
                    insertHere = drawAction->mDepth < currentActionInList->mDepth;
                }
                
                if (insertHere)
                {
                    insertIter = iter;
                    break;
                }
            }
            
            mDepthBatch.insert(insertIter, drawAction);
            
//            mDepthBatch.push_back(drawAction);
//            std::sort( mDepthBatch.begin(), mDepthBatch.end(), DepthLayerSort() );
        }
        
        return drawAction;
    }
    
    SpriteBatch::DrawAction* SpriteBatch::_getDrawActionForTextureOnLayer( int layer, unsigned int textureName, DrawMode drawMode)
    {
        DrawAction* ret = 0;
        
        DrawActionList& drawActionList = mLayeredBatches[layer];
        unsigned int i = 0;
        unsigned int size = drawActionList.size();
        while(i < size )
        {
            if(drawActionList[i]->mTextureName == textureName &&
               drawActionList[i]->mDrawMode == drawMode)
            {
                break;
            }
            ++i;
        }
        
        if(i == size)
        {
            // didnt find it, so push a DrawAction and set buffer to its vert Buffer
            ret = _getDrawAction();
            ret->mTextureName = textureName;
            ret->mDrawMode = drawMode;
            ret->mLayer = layer;
            
            drawActionList.push_back(ret);
        }
        else
        {
            ret = drawActionList[i];
        }
        
        mLastTexture = textureName;
        mLastLayer = mCurrentLayer;
        
        return ret;
    }
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawString(BitmapFont* font, const std::string str, const Vector2& pos, 
					 float radAngle, float scaleFactor, const Color& c, const Justification just, const VertexColorBlendMode bm)
	{
		switch(just)
		{
			case JUSTIFICATION_CENTER:
				_drawStringCenter(font, str, pos, radAngle, scaleFactor, c, bm);
				break;
				
			case JUSTIFICATION_RIGHT:
				_drawStringRight(font, str, pos, radAngle, scaleFactor, c, bm);
				break;
				
			case JUSTIFICATION_LEFT:
			default:
				_drawStringLeft(font, str, pos, radAngle, scaleFactor, c, bm);
				break;				
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawStringLeft(BitmapFont* font, const std::string& str, const Vector2& pos, float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm)
	{
		float cosAngle = (float)cosf(radAngle);
		float sinAngle = (float)sinf(radAngle);
		
		Vector2 drawPos = pos;
		Vector2 initDrawPos = pos;
		float lineNums = 1.0f;
		
		unsigned int page=-1;
		DrawAction* dA =  NULL;
		
		// set vert colors once!
		unsigned int color = packColor(c.R, c.G, c.B, c.A);
		
		for (unsigned int i = 0; true; i++)
		{
			const BitmapFont::GlyphInfo& glyph = font->getGlyph(i,str);
			
			if ((glyph.id == '\0'))
				break;
			
			if ((glyph.id == '\n' || glyph.id == '\r' || glyph.id == 8232))
			{
				drawPos = initDrawPos + VectorTools::rotateVector(Vector2(0, font->getLineHeight()*scaleFactor*lineNums), cosAngle, sinAngle);
				lineNums += 1.0f;
				continue;
			}
			
			if(font->getTexture(glyph.page)->getName() != page)
			{
				page = font->getTexture(glyph.page)->getName();

                // get a draw action for the texture and the layer
                dA = _getDrawActionForTextureOnLayer(mCurrentLayer, page);
			}
            
			if (dA)
				drawPos = _drawGlyph( glyph, dA->mVerts, dA->mBlendSections, drawPos, scaleFactor, cosAngle, sinAngle, color, bm );
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawStringRight(BitmapFont* font, const std::string& str, const Vector2& pos, float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm)
	{
		Vector2 size = font->measureString(str, scaleFactor); 
		_drawStringLeft(font, str, pos - Vector2(size.X, 0.0f), radAngle, scaleFactor, c, bm);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawStringCenter(BitmapFont* font, const std::string& str, const Vector2& pos, float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm)
	{
        std::vector<std::string> lines;
        StringHelper::split(str, '\n', lines);
        if(lines.size())
        {
            unsigned int size = lines.size();
            float yOffset = 0;
            for(unsigned int i=0; i < size; ++i)
            {
                Vector2 strSize = font->measureString(lines[i], scaleFactor); 
                _drawStringLeft(font, lines[i], pos - Vector2(strSize.X*.5f, -yOffset), radAngle, scaleFactor, c, bm); 
                yOffset += strSize.Y;
            }
        }
        else
        {
            Vector2 size = font->measureString(str, scaleFactor); 
            _drawStringLeft(font, str, pos - Vector2(size.X*.5f, 0.0f), radAngle, scaleFactor, c, bm);
        }
	}
	
	///--------------------------------------------------------------------
	Vector2 SpriteBatch::_drawGlyph( const BitmapFont::GlyphInfo& g, VertList& vertBuffer, BlendSectionList& blendList, const Vector2& drawPos, const float scaleFactor, 
									 const float cosAngle, const float sinAngle, unsigned int color, const VertexColorBlendMode bm )
	{
		Vector2 modifiedPos = drawPos + (g.offset*scaleFactor) + (g.nativeSize*scaleFactor*.5f);
		_addQuadToBatchUVRect(vertBuffer, blendList, g.texCoords, modifiedPos, cosAngle, sinAngle, g.nativeSize*scaleFactor, color, bm);
		
		return drawPos + VectorTools::rotateVector(Vector2(g.xAdvance*scaleFactor, 0), cosAngle, sinAngle);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawStringClip(BitmapFont* font, const std::string str, const Vector2& pos, const Rect& rect,
								  float radAngle, float scaleFactor, const Color& c, const Justification just, const VertexColorBlendMode bm)
	{
		switch(just)
		{
			case JUSTIFICATION_CENTER:
				_drawStringCenterClip(font, str, pos, rect, radAngle, scaleFactor, c, bm);
				break;
				
			case JUSTIFICATION_RIGHT:
				_drawStringRightClip(font, str, pos, rect, radAngle, scaleFactor, c, bm);
				break;
				
			case JUSTIFICATION_LEFT:
			default:
				_drawStringLeftClip(font, str, pos, rect, radAngle, scaleFactor, c, bm);
				break;				
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawStringLeftClip(BitmapFont* font, const std::string& str, const Vector2& pos, const Rect& rect,
										  float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm)
	{
		float cosAngle = (float)cosf(radAngle);
		float sinAngle = (float)sinf(radAngle);
		
		Vector2 drawPos = pos;
		unsigned int page=-1;
		DrawAction* dA =  NULL;
		
		// set vert colors once!
		unsigned int color = packColor(c.R, c.G, c.B, c.A);
		
		for (unsigned int i = 0; true; i++)
		{
			const BitmapFont::GlyphInfo& glyph = font->getGlyph(i,str);
			
			if ((glyph.id == '\0'))
				break;
			
			if ((glyph.id == '\n' || glyph.id == '\r' || glyph.id == 8232)) // 8232 is a line separator in utf8
			{
				drawPos.X = pos.X;
				drawPos.Y += font->getBaseHeight()*scaleFactor;
				continue;
			}
			
			if(font->getTexture(glyph.page)->getName() != page)
			{
				page = font->getTexture(glyph.page)->getName();
                
                // get a draw action for the texture and the layer
                dA = _getDrawActionForTextureOnLayer(mCurrentLayer, page);
			}
			drawPos = _drawGlyphClip( glyph, dA->mVerts, dA->mBlendSections, drawPos, rect, scaleFactor, cosAngle, sinAngle, color, bm );
		}
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_drawStringRightClip(BitmapFont* font, const std::string& str, const Vector2& pos, const Rect& rect,
										   float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm)
	{
		Vector2 size = font->measureString(std::string(str), scaleFactor); 
		_drawStringLeftClip(font, str, pos - Vector2(size.X, 0.0f), rect, radAngle, scaleFactor, c, bm);
	}
	
	void SpriteBatch::_drawStringCenterClip(BitmapFont* font, const std::string& str, const Vector2& pos, const Rect& rect,
											float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm)
	{
		Vector2 size = font->measureString(std::string(str), scaleFactor); 
		_drawStringLeftClip(font, str, pos - Vector2(size.X*.5f, 0.0f), rect, radAngle, scaleFactor, c, bm);
	}
	
	Vector2 SpriteBatch::_drawGlyphClip( const BitmapFont::GlyphInfo& g, VertList& vertBuffer, BlendSectionList& blendList, const Vector2& drawPos, const Rect& rect, const float scaleFactor, const float cosAngle, const float sinAngle, unsigned int color, const VertexColorBlendMode bm )
	{
        //scaleOffset is the upper left of draw quad.
		Vector2 scaledOffset = g.offset*scaleFactor;
		Vector2 scaledSize = g.nativeSize*scaleFactor;
		Vector2 glyphEnd = scaledOffset + scaledSize;
        
        Rect modifiedTexCoords;
        Vector2 modifiedSize;
        Vector2 modifiedPos;
        bool toDraw = false;
		
        //clip along X axis
		if((drawPos.X+scaledOffset.X) < rect.upper_left.X)
		{
			// starting point is outside of the box
			// check if any part of it is in the box
			float xOverEdge = drawPos.X+glyphEnd.X;
			if(xOverEdge > rect.upper_left.X)
			{
				// part of the rect is on screen so clip the left side
				xOverEdge = xOverEdge - rect.upper_left.X; 
				modifiedPos = Vector2(drawPos + scaledOffset + (scaledSize*.5f));
				modifiedPos.X += ((scaledSize.X - xOverEdge)*.5f);
				
				Vector2 newUpperLeft(g.texCoords.upper_left);
				newUpperLeft.X += (g.texCoords.size.X - (g.texCoords.size.X*(xOverEdge/scaledSize.X)));
				modifiedTexCoords = Rect(newUpperLeft, Vector2(g.texCoords.getBottomRight().X - newUpperLeft.X, g.texCoords.size.Y));
				
                modifiedSize =  Vector2(xOverEdge, scaledSize.Y);
                toDraw = true;
			}
		}
		else if((drawPos.X+scaledOffset.X)  < rect.getBottomRight().X && drawPos.X + glyphEnd.X >= (rect.getBottomRight().X-1.0f))
		{
			// part of glyph is on screen but trim the right side
			float xOverEdge = (drawPos.X + glyphEnd.X) - (rect.getBottomRight().X-1.0f);
			
			modifiedSize = Vector2(scaledSize.X-xOverEdge, scaledSize.Y);
			modifiedPos = Vector2(drawPos.X + scaledOffset.X + (modifiedSize.X*.5f), drawPos.Y + scaledOffset.Y + (scaledSize.Y*.5f));
			modifiedTexCoords = Rect(g.texCoords.upper_left, Vector2(((modifiedSize.X/scaledSize.X)*(g.texCoords.size.X)), g.texCoords.size.Y));
            
            if (modifiedSize.Y<0) {
                modifiedSize.Y = 0;
                toDraw = false;
            }else
            {
                toDraw = true;
            }
			
		}
		else if((drawPos.X + glyphEnd.X) <= rect.getBottomRight().X)
		{
			modifiedPos = drawPos + scaledOffset + (scaledSize*.5f);
            modifiedSize = g.nativeSize*scaleFactor;
            modifiedTexCoords = g.texCoords;
            toDraw = true;
		}
        
        //clip along Y axis
        if((drawPos.Y+scaledOffset.Y) < rect.upper_left.Y)
		{
			// starting point is outside of the box
			// check if any part of it is in the box
			float YOverEdge = drawPos.Y+glyphEnd.Y;
			if(YOverEdge > rect.upper_left.Y)
			{
				// part of the rect is on screen so clip the left side
				YOverEdge = YOverEdge - rect.upper_left.Y;
				modifiedPos.Y += ((scaledSize.Y - YOverEdge)*.5f);
				modifiedTexCoords.upper_left.Y += (g.texCoords.size.Y - (g.texCoords.size.Y*YOverEdge/scaledSize.Y));		
                modifiedSize.Y = YOverEdge;
			}
            toDraw = true;
		}
		else if((drawPos.Y+scaledOffset.Y)  < rect.getBottomRight().Y && drawPos.Y + glyphEnd.Y >= (rect.getBottomRight().Y))
		{
			// part of glyph is on screen but trim the bottom side
			float YOverEdge = (drawPos.Y + glyphEnd.Y) - (rect.getBottomRight().Y);
			
			modifiedSize.Y = scaledSize.Y-YOverEdge;  
			modifiedPos.Y = drawPos.Y + scaledOffset.Y + (modifiedSize.Y*.5f);
			modifiedTexCoords.size.Y = (modifiedSize.Y/scaledSize.Y)*(g.texCoords.size.Y);
            
            if (modifiedSize.Y<0) {
                modifiedSize.Y = 0;
                toDraw = false;
            }else
            {
                toDraw = true;
            }
			
		}
		else if((drawPos.Y + glyphEnd.Y) <= rect.getBottomRight().Y)
		{
			modifiedPos.Y = drawPos.Y + scaledOffset.Y + (scaledSize.Y*.5f);
            modifiedSize.Y = g.nativeSize.Y*scaleFactor;
            modifiedTexCoords.size.Y = g.texCoords.size.Y;
            toDraw = true;
		}else
        {
            toDraw = false;
        }
        
        if (toDraw) {
            _addQuadToBatchUVRect(vertBuffer, blendList, modifiedTexCoords, modifiedPos, cosAngle, sinAngle, modifiedSize, color, bm);
        }
		
		return drawPos + VectorTools::rotateVector(Vector2(g.xAdvance*scaleFactor, 0), cosAngle, sinAngle);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchImmediate(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Color&c, const VertexColorBlendMode bm)
	{
		_addQuadToBatchImmediate(textureName, pos, radAngle, size, textureName->getTextureRect(), c, bm);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchImmediate(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color&c, const VertexColorBlendMode bm)
	{
		float cosAngle = cosf(radAngle);
		float sinAngle = sinf(radAngle);
		
        DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
        DrawAction* drawAction = _getDrawAction();
        assert( drawAction != NULL );
        drawAction->mTextureName = textureName->getName();
        
		_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, uvs, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), bm);
        
        drawActionList.push_back(drawAction);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchImmediate(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color* cols, const VertexColorBlendMode bm)
	{
		float cosAngle = cosf(radAngle);
		float sinAngle = sinf(radAngle);
        
        DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
        DrawAction* drawAction = _getDrawAction();
        assert( drawAction != NULL );
        drawAction->mTextureName = textureName->getName();
		
		_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, uvs, pos, cosAngle, sinAngle, size, cols, bm);
        
        drawActionList.push_back(drawAction);
	}
    
    ///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchOptimal(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Color&c, const VertexColorBlendMode bm)
	{
		_addQuadToBatchOptimal(textureName, pos, radAngle, size, textureName->getTextureRect(), c, bm);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchOptimal(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color&c, const VertexColorBlendMode bm)
	{
		float cosAngle = cosf(radAngle);
		float sinAngle = sinf(radAngle);
		
        DrawAction* drawAction;
        if(textureName->getName() == mLastTexture && mCurrentLayer == mLastLayer)
        {
            drawAction = mLastDrawAction;
        }
        else
        {
            drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
            mLastDrawAction = drawAction;
        }
        assert( drawAction != NULL );

		_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, uvs, pos, cosAngle, sinAngle, size, packColor((unsigned char)c.R,(unsigned char)c.G,(unsigned char)c.B,(unsigned char)c.A), bm);
	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchOptimal(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color* cols, const VertexColorBlendMode bm)
	{
		float cosAngle = cosf(radAngle);
		float sinAngle = sinf(radAngle);
        
        DrawAction* drawAction;
        if(textureName->getName() == mLastTexture && mCurrentLayer == mLastLayer)
        {
            drawAction = mLastDrawAction;
        }
        else
        {
            drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
            mLastDrawAction = drawAction;
        }
        assert( drawAction != NULL );

		_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, uvs, pos, cosAngle, sinAngle, size, cols, bm);
	}
    
    void SpriteBatch::_addQuadToDepthBatch(const float depth, const int layer, const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color& color, const VertexColorBlendMode bm)
    {
        unsigned int uiTexName = textureName ? textureName->getName() : 0;
        DrawAction* drawAction = _getDrawActionForTextureAtDepth( depth, uiTexName );
        assert( drawAction != NULL );
        drawAction->mLayer = layer;
        
        float cosAngle = cosf(radAngle);
		float sinAngle = sinf(radAngle);
        
		unsigned int uiColor = packColor( color );
        
		_addQuadToBatchUVRect( drawAction->mVerts, drawAction->mBlendSections, uvs, pos, cosAngle,
                                sinAngle, size, uiColor, bm );
		
    }
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Rect& rect, const Vector2& pos, const float cosAngle, const float sinAngle, const Vector2& size, unsigned int color, const VertexColorBlendMode bm)
	{	
        Vector2 halfSize = size*.5f;
		Vector2 posNegHalfSize = halfSize;
		posNegHalfSize.X = -posNegHalfSize.X;
		
		
		_updateBlendSectionList( blendList, 6, bm );
		
		
		// bottom left vert
		vertBuffer.push_back(                                    
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(posNegHalfSize, cosAngle, sinAngle)), rect.upper_left.X, rect.getBottomRight().Y, color)
                             );
        
        // duplicate bottom left
        vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);
        
		
		// bottom right 
		vertBuffer.push_back(				 
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(halfSize, cosAngle, sinAngle) ),rect.getBottomRight().X, rect.getBottomRight().Y,color)
                             );		
		// top left 
		vertBuffer.push_back(				 
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(-halfSize, cosAngle, sinAngle) ), rect.upper_left.X, rect.upper_left.Y,color)
                             );
		
		// top right 
		vertBuffer.push_back(				 
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(-posNegHalfSize, cosAngle, sinAngle) ), rect.getBottomRight().X, rect.upper_left.Y,color)
                             );
        
        // duplicate top right
        vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);
		

	}
	
	///--------------------------------------------------------------------
	void SpriteBatch::_addQuadToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Rect& rect, const Vector2& pos, const float cosAngle, const float sinAngle, const Vector2& size, const Color* cols, const VertexColorBlendMode bm)
	{	
		Vector2 halfSize = size*.5f;
		Vector2 posNegHalfSize = halfSize;
		posNegHalfSize.X = -posNegHalfSize.X;
		
		_updateBlendSectionList( blendList, 6, bm );
		
		
		// bottom left vert
		vertBuffer.push_back(                                    
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(posNegHalfSize, cosAngle, sinAngle) ),
								 rect.upper_left.X, rect.getBottomRight().Y,
								 packColor(cols[0]))
                            );
        
        // duplicate bottom left
        vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);
        
		
		// bottom right 
		vertBuffer.push_back(				 
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(halfSize, cosAngle, sinAngle) ),
								 rect.getBottomRight().X, rect.getBottomRight().Y,
								 packColor(cols[1]))
                             );		
		// top left 
		vertBuffer.push_back(				 
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(-halfSize, cosAngle, sinAngle) ),
								 rect.upper_left.X, rect.upper_left.Y,
								 packColor(cols[2]))
                             );
		
		// top right 
		vertBuffer.push_back(				 
                             PositionTextureColorVert( Vector3( pos + VectorTools::rotateVector(-posNegHalfSize, cosAngle, sinAngle) ),
								 rect.getBottomRight().X, rect.upper_left.Y,
								 packColor(cols[3]))
                             );

		 // duplicate top right
        vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);
	}
	
    ///--------------------------------------------------------------------
	inline void SpriteBatch::_addCircleToBatch(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const float radius, const Color& c, const VertexColorBlendMode bm )
    {
        _addCircleToBatchUVRect(vertBuffer, blendList, pos, radius, Rect( Vector2( 0.5f, 0.5f ), Vector2( 0.5f, 0.5f )), c, 8, bm);
    }    
    
    ///--------------------------------------------------------------------
	inline void SpriteBatch::_addCircleToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const float radius, const Rect & uvs, const Color& c, const int segmentCount, const VertexColorBlendMode bm )
    {
        // this is done as an circle shape, using a single triangle strip alternating between a high index and a low index.
		_updateBlendSectionList( blendList, segmentCount, bm );

        unsigned int pc = packColor(c);
        float angleStepRadians = degToRad( 360.0f / segmentCount );
        for (int i = 0; i < segmentCount; ++i)
        {
            Vector2 offset = VectorTools::rotateVector(Vector2(0.0f, -1.0f), angleStepRadians * (segmentCount - i) );
            Vector2 uv = (offset * uvs.size) + uvs.upper_left;
            vertBuffer.push_back( PositionTextureColorVert( Vector3( pos + (offset * radius) ), uv.X, uv.Y, pc) );

            offset = VectorTools::rotateVector(Vector2(0.0f, -1.0f), angleStepRadians * i);
            uv = (offset * uvs.size) + uvs.upper_left;
            vertBuffer.push_back( PositionTextureColorVert( Vector3( pos + (offset * radius) ), uv.X, uv.Y, pc) );
        }
    }    

    ///--------------------------------------------------------------------
    inline void SpriteBatch::_addCircleToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const float percentOfCircle, const VertexColorBlendMode bm )
    {
        // This is done using a triangle fan, with the number of segments + 2
		_updateBlendSectionList( blendList, segmentCount + 2, bm );

        unsigned int pc = packColor(c);
        
        // Set the center vertex as the first one
        Vector2 uv = uvs.upper_left;
        vertBuffer.push_back( PositionTextureColorVert( Vector3( pos ), uv.X, uv.Y, pc ) );

        float segmentAngle = degToRad( ( 360.0f * percentOfCircle ) / segmentCount );

        for (int i = 0; i < segmentCount + 1; ++i)
        {
            Vector2 offset = VectorTools::rotateVector(Vector2(0.0f, -1.0f), segmentAngle * i);

            Vector2 uv = (offset * uvs.size) + uvs.upper_left;
            vertBuffer.push_back( PositionTextureColorVert( Vector3( pos + (offset * radius) ), uv.X, uv.Y, pc) );
        }    
    }

	///--------------------------------------------------------------------
	inline void SpriteBatch::_addVertsToBatchImmediate(const TexturePtr textureName, const unsigned int count, bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
        DrawAction* drawAction = _getDrawAction();

        if( textureName.use_count() )
        {
            drawAction->mTextureName = textureName->getName();
        }
        else
        {
            drawAction->mTextureName = 0;
        }
		
		int totalCount = count + (addDegenerates ? 2 : 0);
		_updateBlendSectionList( drawAction->mBlendSections, totalCount, bm );
		
        
		for (unsigned int i = 0; i < count; i++)
		{
			unsigned int c = (colors ? packColor(colors[i].R, colors[i].G, colors[i].B, colors[i].A) : packColor(255, 255, 255, 255));
			
			PositionTextureColorVert vert( Vector3( positions[i] ), uvs[i].X, uvs[i].Y, c);
			
			drawAction->mVerts.push_back( vert );
			
			
			if (addDegenerates && ((i == 0) || (i == (count-1))))
			{
				// duplicate this vert!
				drawAction->mVerts.push_back( vert );
			}
		}
        
        drawActionList.push_back(drawAction);
	}
	
	
	///--------------------------------------------------------------------
	inline void SpriteBatch::_addVertsToBatchOptimal(const TexturePtr textureName,  const unsigned int count, bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors, const VertexColorBlendMode bm )
	{
		DrawAction* drawAction;
        
        unsigned int texName = 0;
        if( textureName.use_count() )
        {
            texName = textureName->getName();
        }
        
        if( texName == mLastTexture && mCurrentLayer == mLastLayer)
        {
            drawAction = mLastDrawAction;
        }
        else
        {
            drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, texName);
            mLastDrawAction = drawAction;
        }
		
		
		int totalCount = count + (addDegenerates ? 2 : 0);
		_updateBlendSectionList( drawAction->mBlendSections, totalCount, bm );
		
		
		
		for (unsigned int i = 0; i < count; i++)
		{
			unsigned int c = (colors ? packColor(colors[i].R, colors[i].G, colors[i].B, colors[i].A) : packColor(255, 255, 255, 255));
			
			PositionTextureColorVert vert( Vector3( positions[i] ), uvs[i].X, uvs[i].Y, c);
			
			drawAction->mVerts.push_back( vert );
			
			
			if (addDegenerates && ((i == 0) || (i == (count-1))))
			{
				// duplicate this vert!
				drawAction->mVerts.push_back( vert );
			}
		}
	}
	
	
	///--------------------------------------------------------------------
	void SpriteBatch::_reset()
	{
		mBatchMode = BM_NONE;
		mTopLayer = 0;
		mCurrentLayer = 0;
        
        // put all the draw actions in the unused pool
        mUnusedDrawActions.clear();
        mUnusedDrawActions.reserve( mDrawActionPool.size() );
        int size = mDrawActionPool.size();
        for( int i=size-1; i >= 0; --i )
        {
            mUnusedDrawActions.push_back( i );
        }
        
        mLayeredBatches.clear();
        mDepthBatch.clear();
		
		mLastLayer = -1;
        mLastTexture = -1;
        mLastDepth = 0.0f;
        mLastDrawAction = NULL;
	}
    
    ///--------------------------------------------------------------------
	void SpriteBatch::_flushImmediate()
    {
        LayeredTextureBatchMap::iterator it = mLayeredBatches.begin();
		LayeredTextureBatchMap::iterator itEnd = mLayeredBatches.end();
        
        while(it != itEnd)
        {
            DrawActionList& drawActions = (*it).second;
            if(!drawActions.empty())
            {
                unsigned int size = drawActions.size();
                unsigned int prevName = drawActions[0]->mTextureName;
                //VertList* vertBuf = &drawActions[0]->mVerts;
				DrawAction* dA = drawActions[0];
				
                for(unsigned int i = 1 ; i < size; ++i)
                {
                    if(prevName == drawActions[i]->mTextureName)
                    {
                        //the last draw action used the same texture so add these verts into that draw actions verts
                        //vertBuf->insert(vertBuf->end(), drawActions[i]->mVerts.begin(), drawActions[i]->mVerts.end());
						_mergeDrawActions(dA, drawActions[i]);
                    }
                    else
                    {
                        _flushToGL(prevName, dA->mVerts, dA->mBlendSections, dA->mDrawMode);
                        prevName = drawActions[i]->mTextureName;
                        dA = drawActions[i];
                    }                
                }
                
                _flushToGL(prevName, dA->mVerts, dA->mBlendSections, dA->mDrawMode);

            }
            ++it;
        }        
    }
    
    ///--------------------------------------------------------------------
	void SpriteBatch::_flushDepth()
    {
        DrawActionList::iterator it = mDepthBatch.begin();
        DrawActionList::iterator itEnd = mDepthBatch.end();
        
        while ( it != itEnd )
        {
            DrawAction* drawAction = *it;
            
            DrawActionList::iterator itNext = it+1;
            while ( itNext != itEnd )
            {
                DrawAction* nextDrawAction = *itNext;
                
                if( nextDrawAction->mTextureName == drawAction->mTextureName )
                {
                    _mergeDrawActions( drawAction, nextDrawAction );
                    ++itNext;
                }
                else
                {
                    break;
                }
            }
            
            _flushToGL( drawAction->mTextureName, drawAction->mVerts, drawAction->mBlendSections,
                        drawAction->mDrawMode);
            
            it = itNext;
        }
    }

    
    ///--------------------------------------------------------------------
	void SpriteBatch::_flushOptimal()
	{		
        LayeredTextureBatchMap::iterator it = mLayeredBatches.begin();
		LayeredTextureBatchMap::iterator itEnd = mLayeredBatches.end();
        
        while(it != itEnd)
        {
            DrawActionList& drawActions = (*it).second;
            if(!drawActions.empty())
            {
                unsigned int size = drawActions.size();
                for(unsigned int i = 0 ; i < size; ++i)
                {
					Walaber::Logger::printf("Walaber", Logger::SV_VERBOSE, "numVerts in DA: %d blends sections: %d\n", drawActions[i]->mVerts.size(), drawActions[i]->mBlendSections.size());
                    _flushToGL(drawActions[i]->mTextureName, drawActions[i]->mVerts, drawActions[i]->mBlendSections, drawActions[i]->mDrawMode);
                }
            }
            ++it;
        }   
	}
    
	///--------------------------------------------------------------------
	void SpriteBatch::_flushToGL(unsigned int textureName, VertList& vertBuffer, BlendSectionList& blendList, DrawMode & drawMode)
	{	
		Walaber::Logger::printf("Walaber", Logger::SV_VERBOSE, "Drawing with texture name: %d\n", textureName);
		// Set a blending function to use
		GraphicsGL::setBlending(true);
		
		bool en;
		unsigned int origSrc, origDest;
		
		GraphicsGL::getAlphaBlending(en, origSrc, origDest);
		
		
		// enable textures.
		if (textureName == 0)
		{
			GraphicsGL::setTextureEnabled(false);
			GraphicsGL::bindTexture(0);
			GraphicsGL::disableTextureClientState();
		}
		else
		{
			GraphicsGL::setTextureEnabled(true);
			GraphicsGL::bindTexture( textureName );		
			
			glTexCoordPointer(2, GL_FLOAT, sizeof(PositionTextureColorVert), &vertBuffer[0].tex);
			GraphicsGL::enableTextureClientState();
		}
			
		glVertexPointer(2, GL_FLOAT, sizeof(PositionTextureColorVert), &vertBuffer[0].pos);
        GraphicsGL::enableVertexClientState();
		
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(PositionTextureColorVert), &vertBuffer[0].col);
        GraphicsGL::enableColorClientState();
		
	
		unsigned startIndex = 0;
		
		for (BlendSectionList::iterator it = blendList.begin(); it != blendList.end(); it++)
		{
			// set vertex color blending mode here
			GraphicsGL::setTextureEnvironmentMode( (*it).mVertBlendMode );
			
			// additive blending does not work with pre-multiplied alpha, so use traditional alpha blending instead.
			if ((*it).mVertBlendMode == VCBM_Additive)
			{
				GraphicsGL::setAlphaBlending(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				// set back to whatever it was before.
				GraphicsGL::setAlphaBlending(en, origSrc, origDest);
			}
				
			unsigned int numVerts = (*it).mVertCount;
            if ( drawMode == DM_TRIANGLE_STRIP )
            {
                glDrawArrays(GL_TRIANGLE_STRIP, startIndex, numVerts);
            }
            else if ( drawMode == DM_TRIANGLE_FAN )
            {
                glDrawArrays(GL_TRIANGLE_FAN, startIndex, numVerts);
            }
			
			startIndex += numVerts;
		}
		
		
		// Client State Reset
		//GraphicsGL::disableVertexClientState();
		//GraphicsGL::disableTextureClientState();
		//GraphicsGL::disableColorClientState();
		GraphicsGL::setTextureEnvironmentMode( VCBM_Multiply );
		GraphicsGL::setAlphaBlending(en, origSrc, origDest);
		
	}
}
