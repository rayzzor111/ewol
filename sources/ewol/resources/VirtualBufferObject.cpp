/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license BSD v3 (see license file)
 */

#include <etk/types.h>
#include <ewol/debug.h>
#include <ewol/resources/ResourceManager.h>
#include <ewol/resources/VirtualBufferObject.h>
#include <ewol/renderer/eContext.h>

#undef __class__
#define __class__	"VirtualBufferObject"

ewol::VirtualBufferObject::VirtualBufferObject(int32_t _number) :
	m_exist(false)
{
	m_nbVBO = etk_avg(1, _number, NB_VBO_MAX);
	for (int32_t iii=0; iii<NB_VBO_MAX; iii++) {
		m_vbo[iii]=0;
		m_vboUsed[iii]=false;
	}
	m_resourceLevel = 3;
	EWOL_DEBUG("OGL : load VBO count=\"" << _number << "\"");
}


ewol::VirtualBufferObject::~VirtualBufferObject(void)
{
	removeContext();
}


void ewol::VirtualBufferObject::retreiveData(void)
{
	EWOL_ERROR("TODO ... ");
}


void ewol::VirtualBufferObject::updateContext(void)
{
	if (false == m_exist) {
		// Allocate and assign a Vertex Array Object to our handle
		glGenBuffers(m_nbVBO, m_vbo);
	}
	m_exist = true;
	for (int32_t iii=0; iii<m_nbVBO; iii++) {
		EWOL_INFO("VBO    : add [" << m_uniqueId << "]=" << m_buffer[iii].size() << "*sizeof(float) OGl_Id=" << m_vbo[iii]);
		if (true == m_vboUsed[iii]) {
			// select the buffer to set data inside it ...
			if (m_buffer[iii].size()>0) {
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo[iii]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_buffer[iii].size(), &((m_buffer[iii])[0]), GL_STATIC_DRAW);
			}
		}
	}
	// un-bind it to permet to have no erreor in the next display ...
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ewol::VirtualBufferObject::removeContext(void)
{
	if (true == m_exist) {
		EWOL_INFO("VBO: remove [" << m_uniqueId << "] OGl_Id=" << m_vbo[0]
		                                                << "/" << m_vbo[1]
		                                                << "/" << m_vbo[2]
		                                                << "/" << m_vbo[3]);
		glDeleteBuffers(m_nbVBO, m_vbo);
		m_exist = false;
		for (int32_t iii=0; iii<NB_VBO_MAX; iii++) {
			m_vbo[iii] = 0;
		}
	}
}

void ewol::VirtualBufferObject::removeContextToLate(void)
{
	m_exist = false;
	for (int32_t iii=0; iii<NB_VBO_MAX; iii++) {
		m_vbo[iii] = 0;
	}
}

void ewol::VirtualBufferObject::reload(void)
{
	removeContext();
	updateContext();
}


void ewol::VirtualBufferObject::flush(void)
{
	// request to the manager to be call at the next update ...
	ewol::getContext().getResourcesManager().update(this);
}

void ewol::VirtualBufferObject::pushOnBuffer(int32_t id, const vec3& data)
{
	m_vboUsed[id] = true;
	m_buffer[id].pushBack(data.x());
	m_buffer[id].pushBack(data.y());
	m_buffer[id].pushBack(data.z());
}

vec3 ewol::VirtualBufferObject::getOnBufferVec3(int32_t id, int32_t elementID)
{
	if (elementID*3>m_buffer[id].size()) {
		return vec3(0,0,0);
	}
	return vec3(m_buffer[id][3*elementID],
	            m_buffer[id][3*elementID+1],
	            m_buffer[id][3*elementID+2]);
}
int32_t ewol::VirtualBufferObject::sizeOnBufferVec3(int32_t id)
{
	return m_buffer[id].size()/3;
}

void ewol::VirtualBufferObject::pushOnBuffer(int32_t id, const vec2& data)
{
	m_vboUsed[id] = true;
	m_buffer[id].pushBack(data.x());
	m_buffer[id].pushBack(data.y());
}

vec2 ewol::VirtualBufferObject::getOnBufferVec2(int32_t id, int32_t elementID)
{
	if (elementID*2>m_buffer[id].size()) {
		return vec2(0,0);
	}
	return vec2(m_buffer[id][2*elementID],
	            m_buffer[id][2*elementID+1]);
}

int32_t ewol::VirtualBufferObject::sizeOnBufferVec2(int32_t id)
{
	return m_buffer[id].size()/2;
}



ewol::VirtualBufferObject* ewol::VirtualBufferObject::keep(int32_t _number)
{
	// this element create a new one every time ....
	ewol::VirtualBufferObject* object = new ewol::VirtualBufferObject(_number);
	if (NULL == object) {
		EWOL_ERROR("allocation error of a resource : ??VBO??");
		return NULL;
	}
	getManager().localAdd(object);
	return object;
}

void ewol::VirtualBufferObject::release(ewol::VirtualBufferObject*& _object)
{
	if (NULL == _object) {
		return;
	}
	ewol::Resource* object2 = static_cast<ewol::Resource*>(_object);
	getManager().release(object2);
	_object = NULL;
}
