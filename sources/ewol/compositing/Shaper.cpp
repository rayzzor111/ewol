/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#include <tinyXML/tinyxml.h>

#include <etk/os/FSNode.h>
#include <ewol/debug.h>
#include <ewol/compositing/Shaper.h>
#include <ewol/config.h>

#undef __class__
#define __class__	"ewol::Shaper"

ewol::Shaper::Shaper(etk::UString shaperName) :
	m_name(shaperName),
	m_config(NULL),
	m_confIdPaddingX(-1),
	m_confIdPaddingY(-1),
	m_confIdChangeTime(-1),
	m_confProgramFile(-1),
	m_GLprogram(NULL),
	m_GLPosition(-1),
	m_GLMatrix(-1),
	m_GLPropertySize(-1),
	m_GLPropertyInsidePos(-1),
	m_GLPropertyInsideSize(-1),
	m_GLStateOld(-1),
	m_GLStateNew(-1),
	m_GLStateTransition(-1),
	m_nextStatusRequested(-1),
	m_time(-1),
	m_stateOld(0),
	m_stateNew(0),
	m_stateTransition(1.0)
{
	LoadProgram();
	m_coord[0].x= 0;
	m_coord[0].y= m_propertySize.y;
	
	m_coord[1].x= 0;
	m_coord[1].y= 0;
	
	m_coord[2].x= m_propertySize.x;
	m_coord[2].y= 0;
	
	
	m_coord[3].x= m_propertySize.x;
	m_coord[3].y= 0;
	
	m_coord[4].x= m_propertySize.x;
	m_coord[4].y= m_propertySize.y;
	
	m_coord[5].x= 0;
	m_coord[5].y= m_propertySize.y;
}

ewol::Shaper::~Shaper(void)
{
	if (NULL != m_GLprogram) {
		ewol::resource::Release(m_GLprogram);
		m_GLprogram = NULL;
	}
	if (NULL != m_config) {
		ewol::resource::Release(m_config);
		m_config = NULL;
	}
}

void ewol::Shaper::LoadProgram(void)
{
	if (m_name=="") {
		EWOL_DEBUG("no Shaper set for loading resources ...");
		return;
	}
	if (true == ewol::resource::Keep(m_name, m_config) ) {
		m_confIdPaddingX   = m_config->Request("PaddingX");
		m_confIdPaddingY   = m_config->Request("PaddingY");
		m_confIdChangeTime = m_config->Request("ChangeTime");
		m_confProgramFile  = m_config->Request("program");
	}
	etk::UString basicShaderFile = m_config->GetString(m_confProgramFile);
	// Get the relative position of the current file ...
	etk::FSNode file(m_name);
	etk::UString tmpFilename = file.GetRelativeFolder() + basicShaderFile;
	EWOL_DEBUG("Shaper try load shader : " << tmpFilename << " with base : " << basicShaderFile);
	// get the shader resource :
	m_GLPosition = 0;
	if (true == ewol::resource::Keep(tmpFilename, m_GLprogram) ) {
		m_GLPosition        = m_GLprogram->GetAttribute("EW_coord2d");
		m_GLMatrix          = m_GLprogram->GetUniform("EW_MatrixTransformation");
		// Widget property ==> for the Vertex shader
		m_GLPropertySize       = m_GLprogram->GetUniform("EW_widgetProperty.size");
		m_GLPropertyInsidePos  = m_GLprogram->GetUniform("EW_widgetProperty.insidePos");
		m_GLPropertyInsideSize = m_GLprogram->GetUniform("EW_widgetProperty.insideSize");
		// status property ==> for the fragment shader
		m_GLStateOld           = m_GLprogram->GetUniform("EW_status.stateOld");
		m_GLStateNew           = m_GLprogram->GetUniform("EW_status.stateNew");
		m_GLStateTransition    = m_GLprogram->GetUniform("EW_status.transition");
	}
}


void ewol::Shaper::Draw(void)
{
	if (m_config == NULL) {
		// this is a normale case ... the user can choice to have no config basic file ...
		return;
	}
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
	// Note : Must be all the time a [-1..1] square ...  
	// TODO : plop ...
	m_GLprogram->SendAttribute(m_GLPosition, 2/*x,y*/, m_coord);
	// all entry parameters :
	m_GLprogram->Uniform2fv(m_GLPropertySize,       1, &m_propertySize.x);
	m_GLprogram->Uniform2fv(m_GLPropertyInsidePos,  1, &m_propertyInsidePosition.x);
	m_GLprogram->Uniform2fv(m_GLPropertyInsideSize, 1, &m_propertyInsideSize.x);
	m_GLprogram->Uniform1i(m_GLStateOld,        m_stateOld);
	m_GLprogram->Uniform1i(m_GLStateNew,        m_stateNew);
	m_GLprogram->Uniform1f(m_GLStateTransition, m_stateTransition);
	
	// Request the draw of the elements : 
	glDrawArrays(GL_TRIANGLES, 0, 6);
	m_GLprogram->UnUse();
}

void ewol::Shaper::Clear(void)
{
	// nothing to do ...
}




bool ewol::Shaper::ChangeStatusIn(int32_t newStatusId)
{
	m_nextStatusRequested = newStatusId;
	return true;
}

bool ewol::Shaper::PeriodicCall(int64_t localTime)
{
	// start :
	if (m_time == -1) {
		m_time = localTime;
		m_stateOld = m_stateNew;
		m_stateNew = m_nextStatusRequested;
		m_nextStatusRequested = -1;
		m_stateTransition = 0.0;
		EWOL_VERBOSE("     ##### START #####  ");
	}
	int64_t offset = localTime - m_time;
	float timeRelativity = m_config->GetFloat(m_confIdChangeTime)*1000.0;
	if (offset > timeRelativity) {
		// check if no new state requested:
		if (m_nextStatusRequested != -1) {
			m_time = localTime;
			m_stateOld = m_stateNew;
			m_stateNew = m_nextStatusRequested;
			m_nextStatusRequested = -1;
			m_stateTransition = 0.0;
		} else {
			m_stateTransition = 1.0;
			EWOL_VERBOSE("     ##### STOP #####  ");
			return false;
			m_time = -1;
		}
	} else {
		m_stateTransition = (float)offset / timeRelativity;
		EWOL_VERBOSE("time=" << offset << " in " << timeRelativity << " Transition : " << m_stateTransition);
	}
	return true;
}


void ewol::Shaper::SetSize(etk::Vector2D<float> newSize)
{
	if (m_propertySize != newSize) {
		m_propertySize = newSize;
		// set coord ==> must be a static VBO ...
		m_coord[0].x= 0;
		m_coord[0].y= m_propertySize.y;
		m_coord[1].x= 0;
		m_coord[1].y= 0;
		m_coord[2].x= m_propertySize.x;
		m_coord[2].y= 0;
		
		m_coord[3].x= m_propertySize.x;
		m_coord[3].y= 0;
		m_coord[4].x= m_propertySize.x;
		m_coord[4].y= m_propertySize.y;
		m_coord[5].x= 0;
		m_coord[5].y= m_propertySize.y;
	}
}

void ewol::Shaper::SetInsideSize(etk::Vector2D<float> newInsideSize)
{
	m_propertyInsideSize = newInsideSize;
}

void ewol::Shaper::SetInsidePos(etk::Vector2D<float> newInsidePos)
{
	m_propertyInsidePosition = newInsidePos;
}


etk::Vector2D<float> ewol::Shaper::GetPadding(void)
{
	etk::Vector2D<float> padding;
	padding.x = m_config->GetFloat(m_confIdPaddingX);
	padding.y = m_config->GetFloat(m_confIdPaddingY);
	return padding;
}


void ewol::Shaper::SetSource(etk::UString newFile)
{
	Clear();
	if (NULL != m_GLprogram) {
		ewol::resource::Release(m_GLprogram);
		m_GLprogram = NULL;
	}
	if (NULL != m_config) {
		ewol::resource::Release(m_config);
		m_config = NULL;
	}
	LoadProgram();
}

bool ewol::Shaper::HasSources(void)
{
	return m_GLprogram!=NULL;
}
