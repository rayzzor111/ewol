/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#ifndef __MESH_H__
#define __MESH_H__

#include <etk/types.h>
#include <etk/Hach.h>
#include <ewol/renderer/resources/Resource.h>
#include <ewol/renderer/resources/Image.h>
#include <ewol/renderer/resources/Shader.h>
#include <ewol/renderer/resources/Program.h>
#include <ewol/renderer/resources/VirtualBufferObject.h>
#include <ewol/renderer/Light.h>
#include <ewol/renderer/Material.h>
// 3 "float" elements
#define MESH_VBO_VERTICES  (0)
// 2 "float" elements
#define MESH_VBO_TEXTURE   (1)
// 3 "float" elements
#define MESH_VBO_VERTICES_NORMAL    (2)
// Face normal position :
#define MESH_VBO_FACE_NORMAL       (3)
// 4 "float" elements
#define MESH_VBO_COLOR     (4)

namespace ewol
{
	class DisplacementTable
	{
		private:
			ivec2 m_size;
		public:
			etk::Vector<float> m_data;
			DisplacementTable(const ivec2& size) :
				m_size(size),
				m_data(size.x()*size.y())
			{
				// TODO : Check input ...
				m_data.ReSize(m_size.x()*m_size.y(), 0);
				for(int32_t iii=0; iii<m_size.x()*m_size.y(); iii++) {
					m_data[iii] = 0;
				}
			}
			float Get(int32_t x, int32_t y) const
			{
				// We increment of the size to prevent the <0 result due to the "%" methode ...
				x %= m_size.x();
				y %= m_size.y();
				while (x<0) {
					x+= m_size.x();
				}
				while (y<0) {
					y+= m_size.y();
				}
				return m_data[x + y*m_size.x()];
			}
			float GetInterpolate(float x, float y) const
			{
				
				while (x<0) { x+= 1.0; }
				while (y<0) { y+= 1.0; }
				while (x>=1.0) { x-= 1.0; }
				while (y>=1.0) { y-= 1.0; }
				x *= m_size.x();
				y *= m_size.y();
				//get fractional part of x and y
				float fractX = x - (int32_t)x;
				float fractY = y - (int32_t)y;
				
				//wrap around
				int32_t x1 = (int32_t)x;
				int32_t y1 = (int32_t)y;
				
				//neighbor values
				int32_t x2 = x1 - 1;
				int32_t y2 = y1 - 1;
				
				//smooth the noise with bilinear interpolation
				float value = 0.0;
				value += fractX       * fractY       * Get(x1, y1);
				value += fractX       * (1 - fractY) * Get(x1, y2);
				value += (1 - fractX) * fractY       * Get(x2, y1);
				value += (1 - fractX) * (1 - fractY) * Get(x2, y2);
				return value;
			}
			void Set(int32_t x, int32_t y, float val)
			{
				// We increment of the size to prevent the <0 result due to the "%" methode ...
				x += m_size.x();
				y += m_size.y();
				x %= m_size.x();
				y %= m_size.y();
				m_data[x + y*m_size.x()] = val;
			}
			const ivec2& GetSize(void) const { return m_size; };
	};
	class Face
	{
		public:
			int32_t m_vertex[3];
			int32_t m_uv[3];
			int32_t m_normal[3];
		public:
			Face(void) {};
			Face(int32_t v1, int32_t t1,
			     int32_t v2, int32_t t2,
			     int32_t v3, int32_t t3)
			{
				m_vertex[0] = v1;
				m_vertex[1] = v2;
				m_vertex[2] = v3;
				m_uv[0] = t1;
				m_uv[1] = t2;
				m_uv[2] = t3;
				m_normal[0] = -1;
				m_normal[1] = -1;
				m_normal[2] = -1;
			};
			Face(int32_t v1, int32_t t1, int32_t n1,
			     int32_t v2, int32_t t2, int32_t n2,
			     int32_t v3, int32_t t3, int32_t n3)
			{
				m_vertex[0] = v1;
				m_vertex[1] = v2;
				m_vertex[2] = v3;
				m_uv[0] = t1;
				m_uv[1] = t2;
				m_uv[2] = t3;
				m_normal[0] = n1;
				m_normal[1] = n2;
				m_normal[2] = n3;
			};
	};
	class FaceIndexing
	{
		public:
			etk::Vector<Face> m_faces;
			etk::Vector<uint32_t> m_index;
	};
	class Mesh : public ewol::Resource
	{
		public:
			typedef enum {
				normalModeNone,
				normalModeFace,
				normalModeVertex,
			} normalMode_te;
		private:
			normalMode_te m_normalMode;
		protected:
			ewol::Program* m_GLprogram;
			int32_t        m_GLPosition;
			int32_t        m_GLMatrix;
			int32_t        m_GLMatrixPosition;
			int32_t        m_GLNormal;
			int32_t        m_GLNormalFace;
			int32_t        m_GLtexture;
			int32_t        m_bufferOfset;
			int32_t        m_numberOfElments;
			MaterialGlId   m_GLMaterial;
			ewol::Light    m_light;
		protected:
			etk::Vector<vec3> m_listVertex; //!< List of all vertex in the element
			etk::Vector<vec2> m_listUV; //!< List of all UV point in the mesh (for the specify texture)
			etk::Vector<vec3> m_listFacesNormal; //!< List of all Face normal, when calculated
			etk::Vector<vec3> m_listVertexNormal; //!< List of all Face normal, when calculated
			etk::Hash<FaceIndexing> m_listFaces; //!< List of all Face for the mesh
			etk::Hash<ewol::Material*> m_materials;
		protected:
			ewol::VirtualBufferObject* m_verticesVBO;
		public:
			Mesh(const etk::UString& _fileName, const etk::UString& _shaderName="DATA:textured3D2.prog");
			virtual ~Mesh(void);
			virtual const char* GetType(void) { return "ewol::Mesh"; };
			virtual void Draw(mat4& positionMatrix);
			virtual void Draw2(mat4& positionMatrix);
			void GenerateVBO(void);
		public:
			// some addition basic funtion that permit to create or overwrite some caracterstics :
			void SetTexture(const etk::UString& myTexture);
		protected:
			void InternalSubdivide(bool smooth);
		private:
			void CalculateNormaleFace(void);
			void CalculateNormaleEdge(void);
			/*
			 * Element modification area :
			 */
		public :
			void CreateCube(float size=1.0);
			void CreateViewBox(const etk::UString& _materialName,float _size=1.0);
			void Subdivide(int32_t numberOfTime, bool smooth);
			void DisplaceElement(const ewol::DisplacementTable& displacement);
		private:
			bool LoadOBJ(const etk::UString& _fileName);
			bool LoadEMF(const etk::UString& _fileName);
		public:
			void AddMaterial(const etk::UString& _name, ewol::Material* _data);
	};
};


#endif
