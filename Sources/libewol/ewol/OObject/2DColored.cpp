/**
 *******************************************************************************
 * @file ewol/OObject/2DColored.cpp
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

#include <ewol/OObject/2DColored.h>
#include <ewol/importgl.h>
#include <math.h>


#undef __class__
#define __class__	"ewol::OObject2DColored"


ewol::OObject2DColored::OObject2DColored(void)
{
	m_triElement = 0;
	SetColor(1.0, 1.0, 1.0, 1.0);
}


ewol::OObject2DColored::~OObject2DColored(void)
{
	m_coord.Clear();
	m_coordColor.Clear();
}


void ewol::OObject2DColored::Draw(void)
{
	if (m_coord.Size()<=0) {
		return;
	}
	glPushMatrix();
	// Enable Pointers
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
	glScalef(m_scaling.x, m_scaling.y, 1.0);
	
	// Set the vertex pointer to our vertex data
	glVertexPointer(2, oglTypeFloat_t, 0, &m_coord[0] );
	//glColorPointer(4, oglTypeFloat_t, 0, &m_coordColor[0] );
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &m_coordColor[0] );
	// Render : draw all of the triangles at once
	glDrawArrays( GL_TRIANGLES, 0, m_coord.Size());
	//glDrawElements( GL_TRIANGLES, 0, m_coord.Size());
	//EWOL_DEBUG("Draw ..." << m_coord.Size()/3 << " triangle(s)");

	// Disable Pointers
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
	glPopMatrix();

}

void ewol::OObject2DColored::Clear(void)
{
	m_coord.Clear();
	m_coordColor.Clear();
}


void generatePolyGone(etk::VectorType<Coord2D<oglt> > & input, etk::VectorType<Coord2D<oglt> > & output )
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

void SutherlandHodgman(etk::VectorType<Coord2D<oglt> > & input, etk::VectorType<Coord2D<oglt> > & output, etkFloat_t sx, etkFloat_t sy, etkFloat_t ex, etkFloat_t ey)
{
	// with Sutherland-Hodgman-Algorithm
	if (input.Size() <0) {
		return;
	}
	//int32_t sizeInit=input.Size();
	// last element :
	Coord2D<oglt> destPoint;
	Coord2D<oglt> lastElement = input[input.Size()-1];
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
				etkFloat_t aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				etkFloat_t bbb = lastElement.y - (aaa*lastElement.x);
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
				etkFloat_t aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				etkFloat_t bbb = lastElement.y - (aaa*lastElement.x);
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
				etkFloat_t aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				etkFloat_t bbb = lastElement.x - (aaa*lastElement.y);
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
				etkFloat_t aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				etkFloat_t bbb = lastElement.x - (aaa*lastElement.y);
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
				etkFloat_t aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				etkFloat_t bbb = lastElement.y - (aaa*lastElement.x);
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
				etkFloat_t aaa = (lastElement.y-input[iii].y) / (lastElement.x-input[iii].x);
				etkFloat_t bbb = lastElement.y - (aaa*lastElement.x);
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
				etkFloat_t aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				etkFloat_t bbb = lastElement.x - (aaa*lastElement.y);
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
				etkFloat_t aaa = (lastElement.x-input[iii].x) / (lastElement.y-input[iii].y);
				etkFloat_t bbb = lastElement.x - (aaa*lastElement.y);
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


void ewol::OObject2DColored::SetColor(color_ts color)
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


void ewol::OObject2DColored::SetColor(etkFloat_t red, etkFloat_t green, etkFloat_t blue, etkFloat_t alpha)
{
	if (m_triElement < 1) {
		m_color[0].red = red*255;
		m_color[0].green = green*255;
		m_color[0].blue = blue*255;
		m_color[0].alpha = alpha*255;
	}
	if (m_triElement < 2) {
		m_color[1].red = red*255;
		m_color[1].green = green*255;
		m_color[1].blue = blue*255;
		m_color[1].alpha = alpha*255;
	}
	if (m_triElement < 3) {
		m_color[2].red = red*255;
		m_color[2].green = green*255;
		m_color[2].blue = blue*255;
		m_color[2].alpha = alpha*255;
	}
	
}

void ewol::OObject2DColored::SetPoint(Vector2D<float> point)
{
	// TODO : Clean this :
	Coord2D<oglt> tmpElement;
	tmpElement.x = point.x;
	tmpElement.y = point.y;
	m_triangle[m_triElement] = tmpElement;
	m_triElement++;
	if (m_triElement>=3) {
		GenerateTriangle();
	}
}

void ewol::OObject2DColored::SetPoint(etkFloat_t x, etkFloat_t y)
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

void ewol::OObject2DColored::Line(etkFloat_t sx, etkFloat_t sy, etkFloat_t ex, etkFloat_t ey, etkFloat_t thickness)
{
	ResetCount();
	if (sx == ex && sy == ey) {
		EWOL_WARNING("Try to draw an line width 0");
		return;
	}
	//teta = tan-1(oposer/adjacent)
	etkFloat_t teta = 0;
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
	etkFloat_t offsety = sin(teta-M_PI/2) * (thickness/2);
	etkFloat_t offsetx = cos(teta-M_PI/2) * (thickness/2);

	SetPoint(sx - offsetx, sy - offsety);
	SetPoint(sx + offsetx, sy + offsety);
	SetPoint(ex + offsetx, ey + offsety);
	
	SetPoint(ex + offsetx, ey + offsety);
	SetPoint(ex - offsetx, ey - offsety);
	SetPoint(sx - offsetx, sy - offsety);
}

void ewol::OObject2DColored::Rectangle(etkFloat_t x, etkFloat_t y, etkFloat_t w, etkFloat_t h)
{
	ResetCount();
	
	/* Bitmap position
	 *      xA     xB
	 *   yC *------*
	 *      |      |
	 *      |      |
	 *   yD *------*
	 */
	etkFloat_t dxA = x;
	etkFloat_t dxB = x + w;
	etkFloat_t dyC = y;
	etkFloat_t dyD = y + h;
	
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

void ewol::OObject2DColored::RectangleBorder(etkFloat_t x, etkFloat_t y, etkFloat_t w, etkFloat_t h, etkFloat_t thickness)
{
	// TODO : This did not manage the thickness of the line ...
	Line(x,   y,   x+w, y,   thickness);
	Line(x+w, y,   x+w, y+h, thickness);
	Line(x+w, y+h, x,   y+h, thickness);
	Line(x,   y+h, x,   y,   thickness);
}

void ewol::OObject2DColored::Circle(etkFloat_t x, etkFloat_t y, etkFloat_t radius, etkFloat_t thickness)
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
		
		etkFloat_t angleOne = 2*M_PI* iii     / nbOcurence ;
		etkFloat_t offsetExty = sin(angleOne) * (radius+thickness/2);
		etkFloat_t offsetExtx = cos(angleOne) * (radius+thickness/2);
		etkFloat_t offsetInty = sin(angleOne) * (radius-thickness/2);
		etkFloat_t offsetIntx = cos(angleOne) * (radius-thickness/2);
		
		etkFloat_t angleTwo = 2*M_PI* (iii+1) / nbOcurence ;
		etkFloat_t offsetExt2y = sin(angleTwo) * (radius+thickness/2);
		etkFloat_t offsetExt2x = cos(angleTwo) * (radius+thickness/2);
		etkFloat_t offsetInt2y = sin(angleTwo) * (radius-thickness/2);
		etkFloat_t offsetInt2x = cos(angleTwo) * (radius-thickness/2);
		
		SetPoint(x + offsetIntx,  y + offsetInty);
		SetPoint(x + offsetExtx,  y + offsetExty);
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		SetPoint(x + offsetInt2x, y + offsetInt2y);
		SetPoint(x + offsetIntx,  y + offsetInty);
	}
}

void ewol::OObject2DColored::CirclePart(etkFloat_t x, etkFloat_t y, etkFloat_t radius, etkFloat_t thickness, etkFloat_t angleStart, etkFloat_t angleStop)
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
	etkFloat_t AStart = angleStart * (M_PI)/180;
	//etkFloat_t AStop  = angleStop  * (M_PI)/180;
	etkFloat_t angleLinear = (angleStop-angleStart)* (M_PI)/180;
	
	int32_t nbOcurence = radius;
	if (nbOcurence < 10)
	{
		nbOcurence = 10;
	}
	for (int32_t iii=0; iii<nbOcurence; iii++) {
		
		etkFloat_t angleOne =  AStart + (angleLinear* iii     / nbOcurence) ;
		etkFloat_t offsetExty = sin(angleOne) * (radius+thickness/2);
		etkFloat_t offsetExtx = cos(angleOne) * (radius+thickness/2);
		etkFloat_t offsetInty = sin(angleOne) * (radius-thickness/2);
		etkFloat_t offsetIntx = cos(angleOne) * (radius-thickness/2);
		
		etkFloat_t angleTwo =  AStart + (angleLinear*  (iii+1) / nbOcurence );
		etkFloat_t offsetExt2y = sin(angleTwo) * (radius+thickness/2);
		etkFloat_t offsetExt2x = cos(angleTwo) * (radius+thickness/2);
		etkFloat_t offsetInt2y = sin(angleTwo) * (radius-thickness/2);
		etkFloat_t offsetInt2x = cos(angleTwo) * (radius-thickness/2);
		
		SetPoint(x + offsetIntx,  y + offsetInty);
		SetPoint(x + offsetExtx,  y + offsetExty);
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		
		SetPoint(x + offsetExt2x, y + offsetExt2y);
		SetPoint(x + offsetInt2x, y + offsetInt2y);
		SetPoint(x + offsetIntx,  y + offsetInty);
	}
}

void ewol::OObject2DColored::Disc(etkFloat_t x, etkFloat_t y, etkFloat_t radius)
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
		
		etkFloat_t angleOne = 2*M_PI* iii / nbOcurence ;
		etkFloat_t offsety = sin(angleOne) * radius;
		etkFloat_t offsetx = cos(angleOne) * radius;
		
		SetPoint(x + offsetx, y + offsety);
		
		etkFloat_t angleTwo = 2*M_PI* (iii+1) / nbOcurence ;
		offsety = sin(angleTwo) * radius;
		offsetx = cos(angleTwo) * radius;
		
		SetPoint(x + offsetx, y + offsety);
	}
}

void ewol::OObject2DColored::DiscPart(etkFloat_t x, etkFloat_t y, etkFloat_t radius, etkFloat_t angleStart, etkFloat_t angleStop)
{
	ResetCount();
	if (radius<0) {
		radius *= -1;
	}
	angleStart -= 90;
	angleStop  -= 90;
	etkFloat_t AStart = angleStart * (M_PI)/180;
	//etkFloat_t AStop  = angleStop  * (M_PI)/180;
	etkFloat_t angleLinear = (angleStop-angleStart)* (M_PI)/180;
	//EWOL_DEBUG("Write a part of disk " << angleStart << " -> " << angleStop << "  ocurence=" << (angleLinear*180/(M_PI)) );
	
	int32_t nbOcurence = radius*0.50;
	if (nbOcurence < 15)
	{
		nbOcurence = 15;
	}
	
	// TODO : Generate a poligone instead if this ...
	for (int32_t iii=0; iii<nbOcurence; iii++) {
		SetPoint(x, y);
		
		etkFloat_t angleOne = AStart + (angleLinear* iii / nbOcurence) ;
		etkFloat_t offsety = sin(angleOne) * radius;
		etkFloat_t offsetx = cos(angleOne) * radius;
		
		SetPoint(x + offsetx, y + offsety);
		
		etkFloat_t angleTwo = AStart + (angleLinear* (iii+1) / nbOcurence) ;
		offsety = sin(angleTwo) * radius;
		offsetx = cos(angleTwo) * radius;
		
		SetPoint(x + offsetx, y + offsety);
	}
}
