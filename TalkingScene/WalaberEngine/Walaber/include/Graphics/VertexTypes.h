#ifndef _WALABER_VERTEX_TYPES_H_
#define _WALABER_VERTEX_TYPES_H_

#include <Vector2.h>
#include <Vector3.h>
#include <GraphicsGL.h>
#include <Color.h>

namespace Walaber
{
	
	inline unsigned int packColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
	{
		return ((a << 24) | (b << 16) | (g << 8) | (r));
	}
	
	static inline unsigned int packColor( Color c)
	{
		return ((c.A << 24) | (c.B << 16) | (c.G << 8) | (c.R));
	}
	
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	struct PositionColorVert
	{
		GLfloat			pos[2];
		unsigned int	col;
		
		
		inline void setPos( const Vector2& p )
		{ 
			pos[0] = p.X; 
			pos[1] = p.Y; 
		}
		
		inline void setPos( const float x, const float y)
		{ 
			pos[0] = x; 
			pos[1] = y; 
		}
		
		inline void setColor( const Color& c )
		{ 
			col = packColor( (unsigned char)(c.R ), 
							(unsigned char)(c.G ), 
							(unsigned char)(c.B ), 
							(unsigned char)(c.A )); 
		}
				
		PositionColorVert(){};
		PositionColorVert(const Vector2& posV, unsigned int c)
		:col(c)
		{
			pos[0] = posV.X;
			pos[1] = posV.Y;
		}
		
	};
    
    // ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	struct PositionTextureVert
	{
		GLfloat			pos[2];
		GLfloat         tex[2];
		
		
		inline void setPos( const Vector2& p )
		{ 
			pos[0] = p.X; 
			pos[1] = p.Y; 
		}
		
		inline void setPos( const float x, const float y)
		{ 
			pos[0] = x; 
			pos[1] = y; 
		}
		
        inline void setUV( const Vector2& uv )
		{ 
			tex[0] = uv.X; 
			tex[1] = uv.Y; 
		}
		
		inline void setUV( const float x, const float y)
		{
			tex[0] = x; 
			tex[1] = y;
		}
        
		PositionTextureVert(){};
		PositionTextureVert(const Vector2& posV, const Vector2& texV)
		{
			pos[0] = posV.X;
			pos[1] = posV.Y;
            
            tex[0] = texV.X;
            tex[1] = texV.Y;
		}
		
	};
    
    // ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	struct PositionTextureColorVert
	{
		GLfloat			pos[3];
		GLfloat			tex[2];
		unsigned int	col;
		
		inline void setPos2D( const Vector2& p)
        {
			pos[0] = p.X;
			pos[1] = p.Y;
            pos[2] = 0.0f;
        }
        
		inline void setPos( const Vector3& p )
		{
			pos[0] = p.X;
			pos[1] = p.Y;
            pos[2] = p.Z;
		}
		
		inline void setPos( const float x, const float y, const float z )
		{
			pos[0] = x;
			pos[1] = y;
            pos[2] = z;
		}
		
		inline void setColor( const Color& c )
		{
			col = packColor( (unsigned char)(c.R ),
							(unsigned char)(c.G ),
							(unsigned char)(c.B ),
							(unsigned char)(c.A ));
		}
		
		inline void setUV( const Vector2& uv )
		{
			tex[0] = uv.X;
			tex[1] = uv.Y;
		}
		
		inline void setUV( const float x, const float y)
		{
			tex[0] = x;
			tex[1] = y;
		}
		
		PositionTextureColorVert(){};
		PositionTextureColorVert(const Vector3& posV, GLfloat texU, GLfloat texV, unsigned int c)
		:col(c)
		{
			pos[0] = posV.X;
			pos[1] = posV.Y;
            pos[2] = posV.Z;
			
			tex[0] = texU;
			tex[1] = texV;
		}
        
	};

	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	struct PositionTextureColorVert2D
	{
		GLfloat			pos[2];
		GLfloat			tex[2];
		unsigned int	col;
		
		
		inline void setPos( const Vector2& p )
		{ 
			pos[0] = p.X; 
			pos[1] = p.Y; 
		}
		
		inline void setPos( const float x, const float y)
		{ 
			pos[0] = x; 
			pos[1] = y; 
		}
		
		inline void setColor( const Color& c )
		{ 
			col = packColor( (unsigned char)(c.R ), 
							(unsigned char)(c.G ), 
							(unsigned char)(c.B ), 
							(unsigned char)(c.A )); 
		}
		
		inline void setUV( const Vector2& uv )
		{ 
			tex[0] = uv.X; 
			tex[1] = uv.Y; 
		}
		
		inline void setUV( const float x, const float y)
		{
			tex[0] = x; 
			tex[1] = y;
		}
		
		PositionTextureColorVert2D(){};
		PositionTextureColorVert2D(const Vector2& posV, GLfloat texU, GLfloat texV, unsigned int c)
		:col(c)
		{
			pos[0] = posV.X;
			pos[1] = posV.Y;
            
			tex[0] = texU;
			tex[1] = texV;
		}

	};
	
	
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	struct PositionTwoTextureColorVert
	{
		GLfloat			pos[2];
		GLfloat			tex0[2];
		GLfloat			tex1[2];
		unsigned int	col;
		
		
		inline void setPos( const Vector2& p )
		{ 
			pos[0] = p.X; 
			pos[1] = p.Y; 
		}
		
		inline void setPos( const float x, const float y)
		{ 
			pos[0] = x; 
			pos[1] = y; 
		}
		
		inline void setColor( const Color& c )
		{ 
			col = packColor( (unsigned char)(c.R ), 
							(unsigned char)(c.G ), 
							(unsigned char)(c.B ), 
							(unsigned char)(c.A )); 
		}
		
		inline void setUV( unsigned int index, const Vector2& uv )
		{ 
			if (index == 0)
			{
				tex0[0] = uv.X;
				tex0[1] = uv.Y;
			}
			else
			{
				tex1[0] = uv.X;
				tex1[1] = uv.Y;
			}
		}
		
		inline void setUV( unsigned int index, const float x, const float y)
		{
			if (index == 0)
			{
				tex0[0] = x;
				tex0[1] = y;
			}
			else
			{
				tex1[0] = x;
				tex1[1] = y;
			}
		}
		
		PositionTwoTextureColorVert(){};
		PositionTwoTextureColorVert(const Vector2& posV, GLfloat texU_0, GLfloat texV_0, GLfloat texU_1, GLfloat texV_1, unsigned int c)
		:col(c)
		{
			pos[0] = posV.X;
			pos[1] = posV.Y;
			
			tex0[0] = texU_0;
			tex0[1] = texV_0;
			
			tex1[0] = texU_1;
			tex1[1] = texV_1;
		}
		
		PositionTwoTextureColorVert(const Vector2& p, const Vector2& uv0, const Vector2& uv1, unsigned int c)
		:col(c)
		{
			pos[0] = p.X;
			pos[1] = p.Y;
			
			tex0[0] = uv0.X;
			tex0[1] = uv0.Y;
			
			tex1[0] = uv1.X;
			tex1[1] = uv1.Y;
		}
		
	};
    
    // ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	// ----------==----------==----------==----------==----------==----------==----------==----------==----------==----------==
	struct PositionNormalTangentTextureColorVert
	{
		GLfloat			pos[3];
		GLfloat			norm[3];
		GLfloat			tangent[3];
		GLfloat			tex[2];
		unsigned int	col;
		
		
		inline void setPos( const Vector3& p )
		{
			pos[0] = p.X;
			pos[1] = p.Y;
			pos[2] = p.Z;
		}
		
		inline void setPos( const float x, const float y, const float z)
		{
			pos[0] = x;
			pos[1] = y;
			pos[2] = z;
		}
		
		inline void setNorm( const Vector3& n )
		{
			norm[0] = n.X;
			norm[1] = n.Y;
			norm[2] = n.Z;
		}
		
		inline void setNorm( const float x, const float y, const float z)
		{
			norm[0] = x;
			norm[1] = y;
			norm[2] = z;
		}
		
		inline void setTangent( const Vector3& t )
		{
			tangent[0] = t.X;
			tangent[1] = t.Y;
			tangent[2] = t.Z;
		}
		
		inline void setTangent( const float x, const float y, const float z)
		{
			tangent[0] = x;
			tangent[1] = y;
			tangent[2] = z;
		}
		
		inline void setColor( const Color& c )
		{
			col = packColor( (unsigned char)(c.R ),
							(unsigned char)(c.G ),
							(unsigned char)(c.B ),
							(unsigned char)(c.A ));
		}
		
		inline void setUV( const Vector2& uv )
		{
			tex[0] = uv.X;
			tex[1] = uv.Y;
		}
		
		inline void setUV( const float x, const float y)
		{
			tex[0] = x;
			tex[1] = y;
		}
		
		PositionNormalTangentTextureColorVert(){};
		PositionNormalTangentTextureColorVert(const Vector3& posV, const Vector3& normV, const Vector3& tangentV, GLfloat texU, GLfloat texV, unsigned int c)
		:col(c)
		{
			pos[0] = posV.X;
			pos[1] = posV.Y;
			pos[2] = posV.Z;
			
			norm[0] = normV.X;
			norm[1] = normV.Y;
			norm[2] = normV.Z;
			
			tangent[0] = tangentV.X;
			tangent[1] = tangentV.Y;
			tangent[2] = tangentV.Z;
			
			tex[0] = texU;
			tex[1] = texV;
		}
		
	};
}

#endif	// _WALABER_VERTEX_TYPES_H_