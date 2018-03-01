/*
	3D Camera View widget
*/

#include "CameraView.h"

#include <cmath>

#include <QMouseEvent>
#include <QDebug>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CameraView::CameraView(VolumeRender* render, QWidget* parent) :
	m_render(render),
	m_width(300),
	m_height(300),
	QWidget(parent)
{
	Q_ASSERT(m_render != nullptr);

	m_image.setAlignment(Qt::AlignCenter);

	//Setup image widget
	m_layout.addWidget(&m_image);
	QWidget::setLayout(&m_layout);
	QWidget::setFixedSize(m_width, m_height);

	//default direction
	m_viewMatrix = QMatrix4x4();
	m_viewMatrix.rotate(90, 1, 0);

	redraw();
}

/*
	Redraw event
*/
void CameraView::redraw()
{
	m_image.setPixmap(
		m_render->draw3D(m_width, m_height, m_viewMatrix)
	);
}

QVector3D CameraView::mapToSphere(const QPointF& point)
{
	float hw = (float)m_width / 2;
	float hh = (float)m_height / 2;

	//compute intersection vector
	
	const float x = point.x() - hw;
	const float y = point.y() - hh;
	const float z = sqrtf(hh*hh - x*x - y*y);

	QVector3D vec;
	vec.setX(x);
	vec.setY(y);
	//NaN occurs when point is outside sphere
	//Clamp z value to 0 if outside
	vec.setZ((isnan(z)) ? 0.0f : z);

	return vec.normalized();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Input events
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CameraView::mousePressEvent(QMouseEvent* event)
{
	//On mouse press, update current point
	m_curPoint = mapToSphere(event->localPos());
}

void CameraView::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		//p0/p1 arcball vectors
		const QVector3D& p0 = m_curPoint;
		QVector3D p1 = mapToSphere(event->localPos());

		//Compute angle between p0 and p1
		float angle = acosf(QVector3D::dotProduct(p1, p0));
		angle *= (180.0f / 3.14159f);

		//Compute axis of rotation between p0 and p1
		QVector3D axis = QVector3D::crossProduct(p1, p0);

		//m_viewMatrix = QMatrix4x4();
		m_viewMatrix.rotate(angle, axis);

		//Update current point
		m_curPoint = p1;

		redraw();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
