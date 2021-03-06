/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <etk/math/Matrix4.h>

#include <ewol/debug.h>
#include <ewol/compositing/Compositing.h>


ewol::Compositing::Compositing() {
	// nothing to do
}


ewol::Compositing::~Compositing() {
	// nothing to do
}


void ewol::Compositing::resetMatrix() {
	m_matrixApply.identity();
}


void ewol::Compositing::translate(const vec3& _vect) {
	m_matrixApply *= etk::matTranslate(_vect);
}


void ewol::Compositing::rotate(const vec3& _vect, float _angle) {
	m_matrixApply *= etk::matRotate(_vect, _angle);
}


void ewol::Compositing::scale(const vec3& _vect) {
	m_matrixApply *= etk::matScale(_vect);
}


void ewol::Compositing::clear() {
	m_matrixApply.identity();
}


void ewol::Compositing::setMatrix(const mat4& _mat) {
	m_matrixApply = _mat;
}
