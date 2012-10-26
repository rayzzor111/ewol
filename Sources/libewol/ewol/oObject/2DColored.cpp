/**
 *******************************************************************************
 * @file ewol/oObject/2DColored.cpp
 * @brief ewol OpenGl Object system (Sources)
 * @author Edouard DUPIN
 * @date 09/11/2011
 * @par Project
 * ewol
 *
 * @par Copyright
 * Copyright 2011 Edouard DUPIN, all right reserved
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY.
 *
 * Licence summary : 
 *    You can modify and redistribute the sources code and binaries.
 *    You can send me the bug-fix
 *
 * Term of the licence in in the file licence.txt.
 *
 *******************************************************************************
 */

#include <ewol/oObject/2DColored.h>
#include <ewol/openGL/openGL.h>
#include <math.h>


#undef __class__
#define __class__	"ewol::OObject2DColored"


ewol::OObject2DColored::OObject2DColored(void)
{
	m_triElement = 0;
	SetColor(1.0, 1.0, 1.0, 1.0);
	#ifdef __VIDEO__OPENGL_ES_2
		etk::UString tmpString("color.prog");
		// get the shader resource :
		m_GLPosition = 0;
		if (true == ewol::resource::Keep(tmpString, m_GLprogram) ) {
			m_GLPosition = m_GLprogram->GetAttribute("EW_coord2d");
			m_GLColor    = m_GLprogram->GetAttribute("EW_color");
			m_GLMatrix   = m_GLprogram->GetUniform("EW_MatrixTransformation");
		}
	#endif
}


ewol::OObject2DColored::~OObject2DColored(void)
{
	m_coord.Clear();
	m_coordColor.Clear();
	#ifdef __VIDEO__OPENGL_ES_2
		ewol::resource::Release(m_GLprogram);
	#endif
}


void ewol::OObject2DColored::Draw(void)
{
	if (m_coord.Size()<=0) {
		return;
	}
	#ifdef __VIDEO__OPENGL_ES_2
		if (m_GLprogram==NULL) {
			EWOL_ERROR("No shader ...");
			return;
		}
		//glScalef(m_scaling.x, m_scaling.y, 1.0);
		m_GLprogram->Use();
		// set Matrix : translation/positionMatrix
		etk::Matrix4 tmpMatrix = ewol::openGL::GetMatrix();
		m_GLprogram->UniformMatrix4fv(m_GLMatrix, 1, tmpMatrix.m_mat);
		// position :
		m_GLprogram->SendAttribute(m_GLPosition, 2/*x,y*/, &m_coord[0]);
		// color :
		m_GLprogram->SendAttribute(m_GLColor, 4/*r,g,b,a*/, &m_coordColor[0]);
		// Request the draw of the elements : 
		glDrawArrays(GL_TRIANGLES, 0, m_coord.Size());
		m_GLprogram->UnUse();
	#else
		glPushMatrix();
		// Enable Pointers
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		
		glScalef(m_scaling.x, m_scaling.y, 1.0);
		
		// Set the vertex pointer to our vertex data
		glVertexPointer(2, GL_FLOAT, 0, &m_coord[0] );
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &m_coordColor[0] );
		// Render : draw all of the triangles at once
		glDrawArrays( GL_TRIANGLES, 0, m_coord.Size());
		//glDrawElements( GL_TRIANGLES, 0, m_coord.Size());
		//EWOL_DEBUG("Draw ..." << m_coord.Size()/3 << " triangle(s)");
		// Disable Pointers
		glDisableClientState( GL_COLOR_ARRAY );
		glDisableClientState( GL_VERTEX_ARRAY );
		glPopMatrix();
	#endif

}

void ewol::OObject2DColored::Clear(void)
{
	m_coord.Clear();
	m_coordColor.Clear();
}


void generatePolyGone(etk::Vector<etk::Vector2D<float> > & input, etk::Vector<etk::Vector2D<float> > & output )
{
	if (input.Size()<3) {
		return;
	}
	// TODO : Regenerate a linear poligone generation
	for (int32_t iii=1; iii<input.Size()-1; iii++) {
		output.PushBack(input[0]);
		output.PushBack(input[iii]);
		output.PushBack(input[iii+1]);
	}
	//EWOL_DEBUG("generate Plygone : " << input.Size() << " ==> " << output.Size() );
}

void SutherlandHodgman(etk::Vector<etk::Vector2D<float> > & input, etk::Vector<etk::Vector2D<float> > & output, float sx, float sy, float ex, float ey)
{
	// with Sutherland-Hodgman-Algorithm
	if (input.Size() <0) {
		return;
	}
	//int32_t sizeInit=input.Size();
	// last element :
	etk::Vector2D<float> destPoint;
	etk::Vector2D<float> lastElement = input[input.Size()-1];
	bool inside = true;
	if (lastElement.x < sx) {
		inside = false;
	}
	//EWOL_DEBUG("generate an crop : ");
	for(int32_t iii=0; iii<input.Size(); iii++) {
		if(input[iii].x < sx) {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> OUT ");
				//new point intersection ...
				//y=aaax+bbb
				float aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				float bbb = lastElement.y - (aaa*lastElement.x);
				destPoint.y = aaa*sx + bbb;
				destPoint.x = sx;
				output.PushBack(destPoint);
			} else {
				//EWOL_DEBUG("element OUT ==> OUT ");
			}
			inside = false;
		} else {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> IN ");
				output.PushBack(input[iii]);
			} else {
				//EWOL_DEBUG("element OUT ==> IN ");
				//new point intersection ...
				//y=aaax+bbb
				float aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				float bbb = lastElement.y - (aaa*lastElement.x);
				destPoint.y = aaa*sx + bbb;
				destPoint.x = sx;
				output.PushBack(destPoint);
				output.PushBack(input[iii]);
			}
			inside = true;
		}
		// update the last point position :
		lastElement.x = input[iii].x;
		lastElement.y = input[iii].y;
	}
	
	//EWOL_DEBUG("generate an crop on element : " << sizeInit << " ==> " << output.Size() << "intermediate (1)");
	input = output;
	output.Clear();
	lastElement = input[input.Size()-1];
	inside = true;
	if (lastElement.y < sy) {
		inside = false;
	}
	for(int32_t iii=0; iii<input.Size(); iii++) {
		if(input[iii].y < sy) {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> OUT ");
				//new point intersection ...
				//x=aaay+bbb
				float aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				float bbb = lastElement.x - (aaa*lastElement.y);
				destPoint.y = sy;
				destPoint.x = sy*aaa + bbb;
				output.PushBack(destPoint);
			} else {
				//EWOL_DEBUG("element OUT ==> OUT ");
			}
			inside = false;
		} else {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> IN ");
				output.PushBack(input[iii]);
			} else {
				//EWOL_DEBUG("element OUT ==> IN ");
				//new point intersection ...
				//y=aaax+bbb
				float aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				float bbb = lastElement.x - (aaa*lastElement.y);
				destPoint.y = sy;
				destPoint.x = sy*aaa + bbb;
				output.PushBack(destPoint);
				output.PushBack(input[iii]);
			}
			inside = true;
		}
		// update the last point position :
		lastElement.x = input[iii].x;
		lastElement.y = input[iii].y;
	}
	
	input = output;
	output.Clear();
	lastElement = input[input.Size()-1];
	inside = true;
	if (lastElement.x > ex) {
		inside = false;
	}
	//EWOL_DEBUG("generate an crop : ");
	for(int32_t iii=0; iii<input.Size(); iii++) {
		if(input[iii].x > ex) {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> OUT ");
				//new point intersection ...
				//y=aaax+bbb
				float aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				float bbb = lastElement.y - (aaa*lastElement.x);
				destPoint.y = aaa*ex + bbb;
				destPoint.x = ex;
				output.PushBack(destPoint);
			} else {
				//EWOL_DEBUG("element OUT ==> OUT ");
			}
			inside = false;
		} else {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> IN ");
				output.PushBack(input[iii]);
			} else {
				//EWOL_DEBUG("element OUT ==> IN ");
				//new point intersection ...
				//y=aaax+bbb
				float aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				float bbb = lastElement.y - (aaa*lastElement.x);
				destPoint.y = aaa*ex + bbb;
				destPoint.x = ex;
				output.PushBack(destPoint);
				output.PushBack(input[iii]);
			}
			inside = true;
		}
		// update the last point position :
		lastElement.x = input[iii].x;
		lastElement.y = input[iii].y;
	}
	
	input = output;
	output.Clear();
	lastElement = input[input.Size()-1];
	inside = true;
	if (lastElement.y > ey) {
		inside = false;
	}
	for(int32_t iii=0; iii<input.Size(); iii++) {
		if(input[iii].y > ey) {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> OUT ");
				//new point intersection ...
				//x=aaay+bbb
				float aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				float bbb = lastElement.x - (aaa*lastElement.y);
				destPoint.y = ey;
				destPoint.x = ey*aaa + bbb;
				output.PushBack(destPoint);
			} else {
				//EWOL_DEBUG("element OUT ==> OUT ");
			}
			inside = false;
		} else {
			if(true == inside) {
				//EWOL_DEBUG("element IN ==> IN ");
				output.PushBack(input[iii]);
			} else {
				//EWOL_DEBUG("element OUT ==> IN ");
				//new point intersection ...
				//y=aaax+bbb
				float aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				float bbb = lastElement.x - (aaa*lastElement.y);
				destPoint.y = ey;
				destPoint.x = ey*aaa + bbb;
				output.PushBack(destPoint);
				output.PushBack(input[iii]);
			}
			inside = true;
		}
		// update the last point position :
		lastElement.x = input[iii].x;
		lastElement.y = input[iii].y;
	}
	
	
	//EWOL_DEBUG("generate an crop on element : " << sizeInit << " ==> " << output.Size() );
}


void ewol::OObject2DColored::GenerateTriangle(void)
{
	m_triElement = 0;
	
	m_coord.PushBack(m_triangle[0]);
	m_coordColor.PushBack(m_color[0]);
	m_coord.PushBack(m_triangle[1]);
	m_coordColor.PushBack(m_color[1]);
	m_coord.PushBack(m_triangle[2]);
	m_coordColor.PushBack(m_color[2]);
}


void ewol::OObject2DColored::SetColor(draw::Color color)
{
	if (m_triElement < 1) {
		m_color[0] = color;
	}
	if (m_triElement < 2) {
		m_color[1] = color;
	}
	if (m_triElement < 3) {
		m_color[2] = color;
	}
}


void ewol::OObject2DColored::SetColor(float red, float green, float blue, float alpha)
{
	if (m_triElement < 1) {
		m_color[0] = draw::Color(red, green, blue, alpha);
	}
	if (m_triElement < 2) {
		m_color[1] = draw::Color(red, green, blue, alpha);
	}
	if (m_triElement < 3) {
		m_color[2] = draw::Color(red, green, blue, alpha);
	}
	
}

void ewol::OObject2DColored::SetPoint(etk::Vector2D<float> point)
{
	// TODO : Clean this :
	etk::Vector2D<float> tmpElement;
	tmpElement.x = point.x;
	tmpElement.y = point.y;
	m_triangle[m_triElement] = tmpElement;
	m_triElement++;
	if (m_triElement>=3) {
		GenerateTriangle();
	}
}

void ewol::OObject2DColored::SetPoint(float x, float y)
{
	m_triangle[m_triElement].x = x;
	m_triangle[m_triElement].y = y;
	m_triElement++;
	if (m_triElement>=3) {
		GenerateTriangle();
	}
}


void ewol::OObject2DColored::ResetCount(void)
{
	m_triElement = 0;
	m_color[1] = m_color[0];
	m_color[2] = m_color[0];
}

void ewol::OObject2DColored::Line(float sx, float sy, float ex, float ey, float thickness)
{
	ResetCount();
	if (sx == ex && sy == ey) {
		EWOL_WARNING("Try to draw an line width 0");
		return;
	}
	//teta = tan-1(oposer/adjacent)
	float teta = 0;
	if (sx <= ex) {
		teta = atan((ey-sy)/(ex-sx));
	} else {
		teta = M_PI + atan((ey-sy)/(ex-sx));
	}
	if (teta < 0) {
		teta += 2*M_PI;
	} else if (teta > 2*M_PI) {
		teta -= 2*M_PI;
	}
	//EWOL_DEBUG("teta = " << (teta*180/(M_PI)) << " deg." );
	float offsety = sin(teta-M_PI/2) * (thickness/2);
	float offsetx = cos(teta-M_PI/2) * (thickness/2);

	SetPoint(sx - offsetx, sy - offsety);
	SetPoint(sx + offsetx, sy + offsety);
	SetPoint(ex + offsetx, ey + offsety);
	
	SetPoint(ex + offsetx, ey + offsety);
	SetPoint(ex - offsetx, ey - offsety);
	SetPoint(sx - offsetx, sy - offsety);
}

void ewol::OObject2DColored::Rectangle(float x, float y, float w, float h)
{
	ResetCount();
	/*
	x += 3;
	y += 3;
	w -= 6;
	h -= 6;
	*/
	/* Bitmap position
	 *      xA     xB
	 *   yC *------*
	 *      |      |
	 *      |      |
	 *   yD *------*
	 */
	float dxA = x;
	float dxB = x + w;
	float dyC = y;
	float dyD = y + h;
	
	if (true == m_hasClipping) {
		if (dxA < m_clipping.x) {
			dxA = m_clipping.x;
		}
		if (dxB > m_clipping.x + m_clipping.w) {
			dxB = m_clipping.x + m_clipping.w;
		}
		if (dyC < m_clipping.y) {
			dyC = m_clipping.y;
		}
		if (dyD > m_clipping.y + m_clipping.h) {
			dyD = m_clipping.y + m_clipping.h;
		}
	}
	if(    dyC >= dyD
	    || dxA >= dxB) {
		return;
	}
	SetPoint(dxA, dyD);
	SetPoint(dxA, dyC);
	SetPoint(dxB, dyC);

	SetPoint(dxB, dyC);
	SetPoint(dxB, dyD);
	SetPoint(dxA, dyD);
}

void ewol::OObject2DColored::RectangleBorder(float x, float y, float w, float h, float thickness)
{
	// TODO : This did not manage the thickness of the line ...
	Line(x,   y,   x+w, y,   thickness);
	Line(x+w, y,   x+w, y+h, thickness);
	Line(x+w, y+h, x,   y+h, thickness);
	Line(x,   y+h, x,   y,   thickness);
}

void ewol::OObject2DColored::Circle(float x, float y, float radius, float thickness)
{
	ResetCount();
	if (radius<0) {
		radius *= -1;
	}
	if (radius < thickness/2) {
		Disc(x, y, thickness/2 + radius);
	}
	int32_t nbOcurence = radius;
	if (nbOcurence < 10)
	{
		nbOcurence = 10;
	}
	for (int32_t iii=0; iii<nbOcurence; iii++) {
		
		float angleOne = 2*M_PI* iii     / nbOcurence ;
		float offsetExty = sin(angleOne) * (radius+thickness/2);
		float offsetExtx = cos(angleOne) * (radius+thickness/2);
		float offsetInty = sin(angleOne) * (radius-thickness/2);
		float offsetIntx = cos(angleOne) * (radius-thickness/2);
		
		float angleTwo = 2*M_PI* (iii+1) / nbOcurence ;
		float offsetExt2y = sin(angleTwo) * (radius+thickness/2);
		float offsetExt2x = cos(angleTwo) * (radius+thickness/2);
		float offsetInt2y = sin(angleTwo) * (radius-thickness/2);
		float offsetInt2x = cos(angleTwo) * (radius-thickness/2);
		
		SetPoint(x + offsetIntx,  y + offsetInty);
		SetPoint(x + offsetExtx,  y + offsetExty);
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		SetPoint(x + offsetInt2x, y + offsetInt2y);
		SetPoint(x + offsetIntx,  y + offsetInty);
	}
}

void ewol::OObject2DColored::CirclePart(float x, float y, float radius, float thickness, float angleStart, float angleStop)
{
	ResetCount();
	if (radius<0) {
		radius *= -1;
	}
	if (radius < thickness/2) {
		Disc(x, y, thickness/2 + radius);
	}
	
	angleStart -= 90;
	angleStop  -= 90;
	float AStart = angleStart * (M_PI)/180;
	//float AStop  = angleStop  * (M_PI)/180;
	float angleLinear = (angleStop-angleStart)* (M_PI)/180;
	
	int32_t nbOcurence = radius;
	if (nbOcurence < 10)
	{
		nbOcurence = 10;
	}
	for (int32_t iii=0; iii<nbOcurence; iii++) {
		
		float angleOne =  AStart + (angleLinear* iii     / nbOcurence) ;
		float offsetExty = sin(angleOne) * (radius+thickness/2);
		float offsetExtx = cos(angleOne) * (radius+thickness/2);
		float offsetInty = sin(angleOne) * (radius-thickness/2);
		float offsetIntx = cos(angleOne) * (radius-thickness/2);
		
		float angleTwo =  AStart + (angleLinear*  (iii+1) / nbOcurence );
		float offsetExt2y = sin(angleTwo) * (radius+thickness/2);
		float offsetExt2x = cos(angleTwo) * (radius+thickness/2);
		float offsetInt2y = sin(angleTwo) * (radius-thickness/2);
		float offsetInt2x = cos(angleTwo) * (radius-thickness/2);
		
		SetPoint(x + offsetIntx,  y + offsetInty);
		SetPoint(x + offsetExtx,  y + offsetExty);
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		SetPoint(x + offsetInt2x, y + offsetInt2y);
		SetPoint(x + offsetIntx,  y + offsetInty);
	}
}

void ewol::OObject2DColored::Disc(float x, float y, float radius)
{
	ResetCount();
	if (radius<0) {
		radius *= -1;
	}
	int32_t nbOcurence = radius*0.50;
	if (nbOcurence < 15)
	{
		nbOcurence = 15;
	}

	// TODO : Generate a poligone instead if this ...
	for (int32_t iii=0; iii<nbOcurence; iii++) {
		SetPoint(x, y);
		
		float angleOne = 2*M_PI* iii / nbOcurence ;
		float offsety = sin(angleOne) * radius;
		float offsetx = cos(angleOne) * radius;
		
		SetPoint(x + offsetx, y + offsety);
		
		float angleTwo = 2*M_PI* (iii+1) / nbOcurence ;
		offsety = sin(angleTwo) * radius;
		offsetx = cos(angleTwo) * radius;
		
		SetPoint(x + offsetx, y + offsety);
	}
}

void ewol::OObject2DColored::DiscPart(float x, float y, float radius, float angleStart, float angleStop)
{
	ResetCount();
	if (radius<0) {
		radius *= -1;
	}
	angleStart -= 90;
	angleStop  -= 90;
	float AStart = angleStart * (M_PI)/180;
	//float AStop  = angleStop  * (M_PI)/180;
	float angleLinear = (angleStop-angleStart)* (M_PI)/180;
	//EWOL_DEBUG("Write a part of disk " << angleStart << " -> " << angleStop << "  ocurence=" << (angleLinear*180/(M_PI)) );
	
	int32_t nbOcurence = radius*0.50;
	if (nbOcurence < 15)
	{
		nbOcurence = 15;
	}
	
	// TODO : Generate a poligone instead if this ...
	for (int32_t iii=0; iii<nbOcurence; iii++) {
		SetPoint(x, y);
		
		float angleOne = AStart + (angleLinear* iii / nbOcurence) ;
		float offsety = sin(angleOne) * radius;
		float offsetx = cos(angleOne) * radius;
		
		SetPoint(x + offsetx, y + offsety);
		
		float angleTwo = AStart + (angleLinear* (iii+1) / nbOcurence) ;
		offsety = sin(angleTwo) * radius;
		offsetx = cos(angleTwo) * radius;
		
		SetPoint(x + offsetx, y + offsety);
	}
}
