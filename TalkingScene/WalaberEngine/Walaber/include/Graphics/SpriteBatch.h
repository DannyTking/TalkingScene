/*
 *  SpriteBatch.h
 *  JellyCar3
 *
 *  Created by Jason Pecho on 1/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SPRITE_BATCH_H_
#define _SPRITE_BATCH_H_

#include <VertexTypes.h>
#include <TextureManager.h>
#include <BitmapFont.h>
#include <VectorTools.h>

#include <map>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch
	{
	//public enums, structs, and constants
	public:
		
		///--------------------------------------------------------------------
		// text justification options
		///--------------------------------------------------------------------
		enum Justification 
		{
			JUSTIFICATION_CENTER,
			JUSTIFICATION_RIGHT,
			JUSTIFICATION_LEFT,
		};
		
		///--------------------------------------------------------------------
		// BatchMode describes how to draw
		///--------------------------------------------------------------------
		enum BatchMode 
		{
			BM_NONE,
            BM_IMMEDIATE, // FIFO drawing
            BM_LAYERS_IMMEDIATE, // use layers but per layer just appened verts to end of list
            BM_LAYERS, // batch together textures on a layers
			BM_OPTIMAL, // make the least amount of draw calls and texture switches
            BM_DEPTH, // sort on the depth of a draw action
			BM_DEFAULT = BM_LAYERS
		};
		
		
		///--------------------------------------------------------------------
		// blend mode setting, and how many verts to draw at that setting.
		///--------------------------------------------------------------------
		struct BlendSection
		{
			BlendSection() { mVertBlendMode = VCBM_Multiply; mVertCount = 0; }
			BlendSection( VertexColorBlendMode bm ) { mVertBlendMode = bm; mVertCount = 0; }
			
			VertexColorBlendMode mVertBlendMode;
			unsigned int mVertCount;
		};
        
		///--------------------------------------------------------------------
		// Drawmode setting
		///--------------------------------------------------------------------
        enum DrawMode
        {
            DM_NONE,
            DM_TRIANGLE_STRIP,
            DM_TRIANGLE_FAN,
            DM_DEFAULT = DM_TRIANGLE_STRIP
        };
		
		///--------------------------------------------------------------------
		// Struct of per draw data
		///--------------------------------------------------------------------
        typedef std::vector< PositionTextureColorVert >     VertList;
		typedef std::vector< BlendSection >					BlendSectionList;
		
        struct DrawAction
		{
            unsigned int mTextureName;
            int mLayer;
            float mDepth;
            VertList mVerts;
			BlendSectionList mBlendSections;
            DrawMode            mDrawMode;

            DrawAction():
            mTextureName( 0 ),
            mLayer( 0 ),
            mDepth( 0.0f ),
            mVerts(),
            mBlendSections(),
            mDrawMode( DM_DEFAULT )
            {}
            
		};
		
		// sorted by layer then by textures in a RB tree, the STL impl of a map
		typedef std::vector< DrawAction* >                  DrawActionList;
		typedef std::map< int, DrawActionList >     LayeredTextureBatchMap;		
		
	// interface
	public:
		///--------------------------------------------------------------------
		// ctor
		///--------------------------------------------------------------------
		SpriteBatch();

		///--------------------------------------------------------------------
		// copy ctor
		///--------------------------------------------------------------------
		SpriteBatch(const SpriteBatch& sprBch);
		
		///--------------------------------------------------------------------
		// assignment
		///--------------------------------------------------------------------
		SpriteBatch& operator=(const SpriteBatch& sprBch);
		
		///--------------------------------------------------------------------
		// dtor
		///--------------------------------------------------------------------
		~SpriteBatch();
		
		///--------------------------------------------------------------------
		// Signal to start batching draw calls with default setting
		///--------------------------------------------------------------------
		void start();
		
		///--------------------------------------------------------------------
		// Signal to start batching draw calls with user specifed setting
		///--------------------------------------------------------------------
		void start(BatchMode bm);
		
		///--------------------------------------------------------------------
		// draw sprite batch and clear batch settings
		///--------------------------------------------------------------------
		void flush();
		
		///--------------------------------------------------------------------
		// check to see if the sprite batch has been started
		///--------------------------------------------------------------------
		bool isStarted(){ return mBatchMode != BM_NONE; }
        
        ///--------------------------------------------------------------------
		// return the current mode the batch is in
		///--------------------------------------------------------------------
		BatchMode getBatchMode(){ return mBatchMode; }
		
		///--------------------------------------------------------------------
		// draw functions
		///--------------------------------------------------------------------
		void drawQuad(const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Color& c = Color::White, const VertexColorBlendMode bm = VCBM_Multiply );
		void drawQuad(const int layer, const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Color& c = Color::White, const VertexColorBlendMode bm = VCBM_Multiply );
		
		void drawQuad(const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color& c = Color::White, const VertexColorBlendMode bm = VCBM_Multiply );
		void drawQuad(const int layer, const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color& c = Color::White, const VertexColorBlendMode bm = VCBM_Multiply );
		
		// soecify vertex-specific colors... order is 0:[-x,y] 1:[x,y] 2:[-x,-y] 3:[x,-y]
		void drawQuad(const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color* colors, const VertexColorBlendMode bm = VCBM_Multiply );
		void drawQuad(const int layer, const TexturePtr textureName, const Vector2& pos, const float angle, const Vector2& size, const Rect& uvs, const Color* colors, const VertexColorBlendMode bm = VCBM_Multiply );
		
		void drawQuadColored(const Vector2& pos, const float angle, const Vector2& size, const Color& c);
		void drawQuadColored(const int layer, const Vector2& pos, const float angle, const Vector2& size, const Color& c);
        
        void drawCircle(const TexturePtr textureName, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const VertexColorBlendMode bm = VCBM_Multiply );
        void drawCircle(const int layer, const TexturePtr textureName, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const VertexColorBlendMode bm = VCBM_Multiply );
        void drawSweptCircle(const TexturePtr textureName, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const float percentOfCircle, const VertexColorBlendMode bm = VCBM_Multiply );

        void drawCircleColored(const Vector2& pos, const float radius, const Color& c);
        
		void drawLineColored(const int layer, const Vector2& ptA, const Vector2& ptB, const float width, const Color& c);
		
		// draw an arbitrary triangle (textured).  expects arrays of 3 items for each. if 'colors' is NULL, Color::White is used for each vertex color.
		void drawTri(const TexturePtr textureName, const Vector2* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );
		void drawTri(const int layer, const TexturePtr textureName, const Vector2* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );
		
		// draw an arbitrary set of verts (textured).  expects arrays of 'count' items for each. if 'colors' is NULL, Color::White is used for each vertex color.
		void drawVerts(const TexturePtr textureName, const unsigned int count, const bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );
		void drawVerts(const int layer, const TexturePtr textureName, const unsigned int count, const bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );
		
        // draw an arbitrary set of verts (textured).  expects arrays of 'count' items for each. if 'colors' is NULL, Color::White is used for each vertex color.
		void drawVertsWithDepth(const float depth, const int layer, const TexturePtr textureName, const unsigned int count, const bool addDegenerates, const Vector3* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );

        inline void drawTeardrop(const int layer, const TexturePtr textureName, const Vector2& pos, const Vector2& tipPos, const float radius, const Color& c, const VertexColorBlendMode bm = VCBM_Multiply, const bool alphaFade = false )
        {
            switch(mBatchMode)
            {
                case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = layer;
                    drawAction->mTextureName = textureName->getName();
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
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
                    drawAction->mLayer = layer;
                    drawAction->mTextureName = textureName->getName();
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
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
                    DrawAction* drawAction;
                    if(textureName->getName() == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = textureName->getName();
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                        mLastDrawAction = drawAction;
                    }
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }
                    break;
                    
                case BM_OPTIMAL:
                    // draw it..
                {
                    DrawAction* drawAction;
                    if(textureName->getName() == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = textureName->getName();
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                        mLastDrawAction = drawAction;
                    }
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }
                    break;
                    
                default:
                    printf("ERROR: draw has been called before a call to start\n");
                    assert(0);
            }
        }
        
        inline void drawTeardrop(const TexturePtr textureName, const Vector2& pos, const Vector2& tipPos, const float radius, const Color& c, const VertexColorBlendMode bm = VCBM_Multiply, bool alphaFade = false )
        {
            switch(mBatchMode)
            {
                case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = mCurrentLayer;
                    drawAction->mTextureName = textureName->getName();
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                    drawActionList.push_back(drawAction);
                }  
                    break;
                    
                case BM_LAYERS_IMMEDIATE:
                    
                    mCurrentLayer = mTopLayer++;
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = mCurrentLayer;
                    drawAction->mTextureName = textureName->getName();
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                    drawActionList.push_back(drawAction);
                }  
                    break;
                    
                case BM_LAYERS: // OPTIMAL per layer
                    
                    mCurrentLayer = mTopLayer++;
                    // draw it..
                {
                    DrawAction* drawAction;
                    if(textureName->getName() == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = textureName->getName();
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                        mLastDrawAction = drawAction;
                    }
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }
                    break;
                    
                case BM_OPTIMAL:				
                    
                    // draw it..
                {
                    DrawAction* drawAction;
                    if(textureName->getName() == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = textureName->getName();
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, textureName->getName());
                        mLastDrawAction = drawAction;
                    }
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }
                    break;
                    
                default:
                    printf("ERROR: draw has been called before a call to start\n");
                    assert(0);
            } 
        }
		
		inline void drawTeardropColored(const int layer, const Vector2& pos, const Vector2& tipPos, const float radius, const Color& c, const VertexColorBlendMode bm = VCBM_Multiply, bool alphaFade = false )
        {
            switch(mBatchMode)
            {
                case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = layer;
                    drawAction->mTextureName = 0;
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                    drawActionList.push_back(drawAction);
                }
                    break;
                    
                case BM_LAYERS_IMMEDIATE:
                    
					mCurrentLayer = layer;
                    if (layer >= mTopLayer) 
                    {
                        mTopLayer = layer + 1;
                    }
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = layer;
                    drawAction->mTextureName = 0;
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
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
                    DrawAction* drawAction;
                    if(0 == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = 0;
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                        mLastDrawAction = drawAction;
                    }
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }	
                    
                    break;
                    
                case BM_OPTIMAL:
                    
                    // draw it..
                {
                    DrawAction* drawAction;
                    if(0 == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = 0;
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                        mLastDrawAction = drawAction;
                    }
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }
                    
                    break;
                    
                default:
                    printf("ERROR: draw has been called before a call to start\n");
                    assert(0);
            }
        }
		
        
        inline void drawTeardropColored(const Vector2& pos, const Vector2& tipPos, const float radius, const Color& c, const VertexColorBlendMode bm = VCBM_Multiply, bool alphaFade = false )
        {
            switch(mBatchMode)
            {
                case BM_IMMEDIATE:
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = mCurrentLayer;
                    drawAction->mTextureName = 0;
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                    drawActionList.push_back(drawAction);
                }
                    break;
                    
                case BM_LAYERS_IMMEDIATE:
                    
                    mCurrentLayer = mTopLayer++;
                {
                    DrawActionList& drawActionList = mLayeredBatches[mCurrentLayer];
                    DrawAction* drawAction = _getDrawAction();
                    drawAction->mLayer = mCurrentLayer;
                    drawAction->mTextureName = 0;
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                    drawActionList.push_back(drawAction);
                }
                    break;
                    
                case BM_LAYERS:
                    
                    mCurrentLayer = mTopLayer++;
                    
                    // draw it..
                {
                    DrawAction* drawAction;
                    if(0 == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = 0;
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                        mLastDrawAction = drawAction;
                    }
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }	
                    
                    break;
                    
                case BM_OPTIMAL:
                    
                    // draw it..
                {
                    DrawAction* drawAction;
                    if(0 == mLastTexture && mCurrentLayer == mLastLayer)
                    {
                        drawAction = mLastDrawAction;
                    }
                    else
                    {     
                        mLastTexture = 0;
                        mLastLayer = mCurrentLayer;
                        drawAction = _getDrawActionForTextureOnLayer(mCurrentLayer, 0);
                        mLastDrawAction = drawAction;
                    }
                    
                    _addTeardropToBatch(drawAction->mVerts, drawAction->mBlendSections, pos, tipPos, radius, c, bm, alphaFade);
                    
                }
                    
                    break;
                    
                default:
                    printf("ERROR: draw has been called before a call to start\n");
                    assert(0);
            }
        }
		
        
		// Depending on the justification, pos means different things
		// for LEFT, pos = left margin
		// for CENTER, pos = position to center around
		// for RIGHT, pos = right margin
		// pos.Y is always the top!!!
		void drawString(BitmapFont* font, const std::string text, const Vector2& pos, float angle, 
						float scaleFactor, const Color& c, const Justification just = JUSTIFICATION_LEFT);
		void drawString(const int layer, BitmapFont* font, const std::string text, const Vector2& pos, 
						float angle, float scaleFactor, const Color& c, const Justification just = JUSTIFICATION_LEFT);
		
		///--------------------------------------------------------------------
		// these functions will clip a string to fit into a box, drawing only the part of the
		// string that fits into the box
		///--------------------------------------------------------------------
		void drawStringClipped(BitmapFont* font, const std::string text, const Vector2& pos, const Rect& rClip,
							   float angle, float scaleFactor, const Color& c, const Justification just = JUSTIFICATION_LEFT);
		void drawStringClipped(const int layer, BitmapFont* font, const std::string text, const Vector2& pos, const Rect& rClip,
							   float angle, float scaleFactor, const Color& c, const Justification just = JUSTIFICATION_LEFT);
		
		
        
        // projection matrix helper functions...
        // this one sets a standard pixel-space projection matrix, for common UI / 2D drawing.
        void setDefaultProjectionMatrix();
        
        void setCameraProjectionMatrix( Vector2& camPos, Vector2& camSize );
        
        
	// data and helper functions
	private:
        
        // creates a new draw action if an unused draw action does not exist
        DrawAction* _getDrawAction();

        // this function encapsulates the check for the the last draw action to see if we can append
        // other wise it calls _getDrawAction() to get an unused draw action
        DrawAction* _getDrawActionForTextureAtDepth( const float depth, unsigned int textureName, DrawMode drawMode = DM_DEFAULT );
        
        // searches a given layer for a DrawAction for that texture, returns first one it finds - linear search
        // if it doesn't find one, it returns new one and adds it to the layer draw list
        DrawAction* _getDrawActionForTextureOnLayer(int layer, unsigned int textureName, DrawMode drawMode = DM_DEFAULT);
		
		// draw strings normally
		void _drawString(BitmapFont* font, const std::string str, const Vector2& pos, float radAngle, float scaleFactor, 
						 const Color& c, const Justification just = JUSTIFICATION_LEFT, const VertexColorBlendMode bm = VCBM_Multiply);
		void _drawStringLeft(BitmapFont* font, const std::string& str, const Vector2& pos, float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm);
		void _drawStringRight(BitmapFont* font, const std::string& str, const Vector2& pos, float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm);
		void _drawStringCenter(BitmapFont* font, const std::string& str, const Vector2& pos, float radAngle, float scaleFactor, const Color& c, const VertexColorBlendMode bm);
		Vector2 _drawGlyph( const BitmapFont::GlyphInfo& g, VertList& vertBuffer, BlendSectionList& blendList, const Vector2& drawPos, const float scaleFactor, const float cosAngle, 
						    const float sinAngle, unsigned int color, const VertexColorBlendMode bm );
		
		// draw clipped string
		void _drawStringClip(BitmapFont* font, const std::string str, const Vector2& pos, const Rect& rect, float radAngle, 
							 float scaleFactor, const Color& c, const Justification just = JUSTIFICATION_LEFT, const VertexColorBlendMode bm = VCBM_Multiply);
		void _drawStringLeftClip(BitmapFont* font, const std::string& str, const Vector2& pos, const Rect& rect, float radAngle, 
								 float scaleFactor, const Color& c, const VertexColorBlendMode bm);
		void _drawStringRightClip(BitmapFont* font, const std::string& str, const Vector2& pos, const Rect& rect, float radAngle, 
								  float scaleFactor, const Color& c, const VertexColorBlendMode bm);
		void _drawStringCenterClip(BitmapFont* font, const std::string& str, const Vector2& pos, const Rect& rect, float radAngle, 
								   float scaleFactor, const Color& c, const VertexColorBlendMode bm);
		Vector2 _drawGlyphClip( const BitmapFont::GlyphInfo& g, VertList& vertBuffer, BlendSectionList& blendList, const Vector2& drawPos, const Rect& rect,
							   const float scaleFactor, const float cosAngle, const float sinAngle, unsigned int color, const VertexColorBlendMode bm );
		
        // quads added on the current layer in immediate mode
		inline void _addQuadToBatchImmediate(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Color&c, const VertexColorBlendMode bm);
		inline void _addQuadToBatchImmediate(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color&c, const VertexColorBlendMode bm);
		inline void _addQuadToBatchImmediate(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color* cols, const VertexColorBlendMode bm);
		
        // quads added on the current layer in optimal mode
        inline void _addQuadToBatchOptimal(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Color&c, const VertexColorBlendMode bm);
		inline void _addQuadToBatchOptimal(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color&c, const VertexColorBlendMode bm);
		inline void _addQuadToBatchOptimal(const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color* cols, const VertexColorBlendMode bm);
        
        inline void _addQuadToDepthBatch(const float depth, const int layer, const TexturePtr textureName, const Vector2& pos, const float radAngle, const Vector2& size, const Rect& uvs, const Color& color, const VertexColorBlendMode bm);

		void _addQuadToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Rect& rect, const Vector2& pos, const float cosAngle, const float sinAngle, const Vector2& size, const unsigned int c, const VertexColorBlendMode bm);
		void _addQuadToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Rect& rect, const Vector2& pos, const float cosAngle, const float sinAngle, const Vector2& size, const Color* cols, const VertexColorBlendMode bm);
		
        
        // circles
        inline void _addCircleToBatch(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const float radius, const Color& c, const VertexColorBlendMode bm );
        inline void _addCircleToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const VertexColorBlendMode bm );
        inline void _addCircleToBatchUVRect(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const float radius, const Rect& uvs, const Color& c, const int segmentCount, const float percentOfCircle, const VertexColorBlendMode bm );

		inline void _addTeardropToBatch(VertList& vertBuffer, BlendSectionList& blendList, const Vector2& pos, const Vector2& tipPos, const float radius, const Color& c, const VertexColorBlendMode bm, bool alphaFade)
        {
            // this is done as an octogon shape, using a single triangle strip. (6 triangles)
            unsigned int pc = packColor(c);
            unsigned int tc = packColor(Color(c.R,c.G,c.B,0));  //transparent 
            
			
			_updateBlendSectionList( blendList, 10, bm );
			
            
            Vector2 toTip = tipPos - pos;
            float sqL = toTip.lengthSquared();
            float sqR = (radius * radius);
            bool drawTip = (sqL > sqR);
            float angle = (drawTip ? (atan2f(toTip.Y, toTip.X) - 1.57079633f) : 0.0f);
            
            float cosAngle = cosf(angle);
            float sinAngle = sinf(angle);
            
            
            if(!alphaFade) //put the if statement outside of the for loop for efficiency
            {
                for (int i = 0; i < 8; i++)
                {
                    const Vector2& offsetSrc = mTeardropOffsets[i];
                    
                    const Vector2& uv = mTeardropUVs[i];
                    
                    Vector2 offset = VectorTools::rotateVector(offsetSrc, cosAngle, sinAngle);
                    
                    if ((drawTip) && (i < 3))
                    {
                        vertBuffer.push_back( PositionTextureColorVert( Vector3( tipPos + (offset * radius * 0.3f) ), uv.X, uv.Y, pc) );
                    }
                    else
                    {
                        vertBuffer.push_back( PositionTextureColorVert( Vector3( pos + (offset * radius) ), uv.X, uv.Y, pc) );
                    }
                    
                    
                    if (i==0)
                    {
                        // duplicate first vert
                        vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);            
                    }
                    
                }
            }
            else
            {
                for (int i = 0; i < 8; i++)
                {
                    const Vector2& offsetSrc = mTeardropOffsets[i];
                    
                    const Vector2& uv = mTeardropUVs[i];
                    
                    Vector2 offset = VectorTools::rotateVector(offsetSrc, cosAngle, sinAngle);
                    
                    if ((drawTip) && (i < 3))
                    {
                        vertBuffer.push_back( PositionTextureColorVert( Vector3( tipPos + (offset * radius * 0.3f) ), uv.X, uv.Y, tc) );
                    }
                    else
                    {
                        vertBuffer.push_back( PositionTextureColorVert( Vector3( pos + (offset * radius) ), uv.X, uv.Y, pc) );
                    }
                    
                    
                    if (i==0)
                    {
                        // duplicate first vert
                        vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);            
                    }
                    
                }

            }          
            
            // duplicate last vert
            vertBuffer.push_back(vertBuffer[vertBuffer.size()-1]);
            
        }
		
		// triangles
		inline void _addVertsToBatchImmediate(const TexturePtr textureName, const unsigned int count, bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );
		inline void _addVertsToBatchOptimal(const TexturePtr textureName,  const unsigned int count, bool addDegenerates, const Vector2* positions, const Vector2* uvs, const Color* colors = NULL, const VertexColorBlendMode bm = VCBM_Multiply );
		
		
		
		inline void _updateBlendSectionList(BlendSectionList& blendList, unsigned int vertCount, const VertexColorBlendMode bm )
		{
			if ((blendList.size() == 0) || (blendList.back().mVertBlendMode != bm))
			{
				blendList.push_back( BlendSection( bm ) );
			}
			BlendSection& bs = blendList.back();
			bs.mVertCount += vertCount;
		}
		
		// merge the verts in draw action "filler" to the end of "toBeFilled"'s verts.
		inline void _mergeDrawActions( DrawAction* toBeFilled, DrawAction* filler )
		{
			// copy the verts from "filler" into "toBeFilled"
			toBeFilled->mVerts.insert(toBeFilled->mVerts.end(), filler->mVerts.begin(), filler->mVerts.end());
			
			// merge the BlendSectionLists.
			for (BlendSectionList::iterator it = filler->mBlendSections.begin(); it != filler->mBlendSections.end(); it++)
				_updateBlendSectionList(toBeFilled->mBlendSections, (*it).mVertCount, (*it).mVertBlendMode);
		}
		
		
		void _reset();
        void _flushImmediate();
		void _flushOptimal();
        void _flushDepth();
        void _flushToGL(unsigned int textureName, VertList& vertBuffer, BlendSectionList& blendList, DrawMode & drawMode);
		
    private:// members
                    
		LayeredTextureBatchMap							mLayeredBatches;
        DrawActionList                                  mDepthBatch;
		BatchMode										mBatchMode;
		int                                             mTopLayer; // highest layer
		int                                             mCurrentLayer; // curent layer to draw to
        int                                             mLastLayer;
        int                                             mLastTexture;
        float                                           mLastDepth;
        DrawAction*                                     mLastDrawAction;
                    

        std::vector< DrawAction* >                      mDrawActionPool;
        // limit us to 65000 draw actions - hopefully this won't become a problem
        std::vector<unsigned short>                     mUnusedDrawActions;
        
        static const float                              mTeardropAngles[];
        static const Vector2                            mTeardropOffsets[];
        static const Vector2                            mTeardropUVs[];
	};
}

#endif
