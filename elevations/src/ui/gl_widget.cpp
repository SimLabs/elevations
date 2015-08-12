#include "gl_widget.h"

#include <ork/render/FrameBuffer.h>

using namespace elevations::ui;

gl_widget::gl_widget(QWidget* parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
	, q_timer_(details::gl_timer(this))
	, time_(std::make_pair(0.0, 0.0))
	, damaged_(false)
	, resource_container_(nullptr)
	, elevation_cursor_(nullptr)
{
	connect(&q_timer_, SIGNAL(timeout()), this, SLOT(updateGL()));
	q_timer_.start();
}

gl_widget::~gl_widget()
{
	resource_container_ = nullptr;
	Object::exit();
}

void gl_widget::init(const elevations::resource_container* container, const elevations::elevation_cursor* cursor)
{
	resource_container_ = const_cast<resource_container*>(container);
	elevation_cursor_ = const_cast<elevation_cursor*>(cursor);
}

void gl_widget::updateGL()
{	
	if (damaged_)
	{
		resource_container_->update_resources();
		damaged_ = false;
	}
	QGLWidget::updateGL();
}

void gl_widget::initializeGL()
{
	makeCurrent();
	glewExperimental = GL_TRUE;
	auto glew_init = glewInit();
	assert(glew_init == GLEW_OK);
	assert(glGetError() == 0);

	qglClearColor(Qt::black);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void gl_widget::paintGL()
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr)
	{
		view_handler->redisplay(time_.first, time_.second);
		swapBuffers();
		time_ = q_timer_.end();

		if (Logger::ERROR_LOGGER != nullptr) {
			Logger::ERROR_LOGGER->flush();
		}
	}
}

void gl_widget::resizeGL(int width, int height)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr)
	{
		auto frame_buffer = FrameBuffer::getDefault();
		frame_buffer->setViewport(vec4<GLint>(0, 0, width, height));
		frame_buffer->setPolygonMode(FILL, CULL);
		frame_buffer->setDepthTest(true, LESS);

		view_handler->reshape(width, height);
		updateGL();
	}
}

void gl_widget::mousePressEvent(QMouseEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr && (event->buttons() & Qt::LeftButton))
	{
		view_handler->mouseClick(EventHandler::LEFT_BUTTON,
			EventHandler::DOWN,
			static_cast<EventHandler::modifier>(0),
			event->x(),
			event->y());
	}

	if (elevation_cursor_ != nullptr && event->buttons() & Qt::RightButton)
	{
		auto height = elevation_cursor_->get_current_height();
		assert(!std::isnan(height));
	}
}

void gl_widget::wheelEvent(QWheelEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr && event->orientation() == Qt::Vertical)
	{
		view_handler->mouseWheel(event->delta() > 0 ? EventHandler::WHEEL_UP : EventHandler::WHEEL_DOWN,
			static_cast<EventHandler::modifier>(0),
			event->x(),
			event->y());
	}
}

void gl_widget::mouseMoveEvent(QMouseEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr && (event->buttons() & Qt::LeftButton))
	{
		view_handler->mouseMotion(event->x(), event->y());
	}
}

void gl_widget::enterEvent(QEvent* event)
{
	damaged_ = true;
}

void gl_widget::leaveEvent(QEvent* event)
{
	damaged_ = false;
}

ptr<proland::BasicViewHandler> gl_widget::get_view_handler() const
{
	return resource_container_ != nullptr ? resource_container_->get_view_handler() : nullptr;
}

details::gl_timer::gl_timer(gl_widget* parent, int interval)
	: QTimer(parent)
	, timer_(Timer())
{
	setInterval(interval);
}

std::pair<double, double> details::gl_timer::end()
{
	auto t = timer_.end();
	dt_ = t - t_;
	t_ = t;
	return std::make_pair(t_, dt_);
}

void details::gl_timer::start(int msec)
{
	QTimer::start(msec);
	timer_.start();
}

void details::gl_timer::start()
{
	QTimer::start();
	timer_.start();
}