#include "ui/gl_widget.h"

#include <ork/render/FrameBuffer.h>

using namespace elevations::ui;

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

gl_widget::gl_widget(QWidget* parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
	, q_timer_(details::gl_timer(this))
	, time_(std::make_pair(0.0, 0.0))
	, damaged_(false)
	, view_manager_(nullptr)
{
	connect(&q_timer_, SIGNAL(timeout()), this, SLOT(updateGL()));
	q_timer_.start();
}

gl_widget::~gl_widget()
{
	view_manager_ = nullptr;
	Object::exit();
}

void gl_widget::init(const elevations::ui::view_manager* view_manager)
{
	view_manager_ = const_cast<ui::view_manager*>(view_manager);
}

void gl_widget::updateGL()
{	
	if (damaged_)
	{
		view_manager_->update_resources();
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

void gl_widget::keyPressEvent(QKeyEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr)
	{
		auto key = details::convert_key(event->key());
		if (key != 0)
		{
			view_handler->specialKey(key, static_cast<EventHandler::modifier>(0), 0, 0);
		}
	}
}

void gl_widget::keyReleaseEvent(QKeyEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr)
	{
		auto key = details::convert_key(event->key());
		if (key != 0)
		{
			view_handler->specialKeyReleased(key, static_cast<EventHandler::modifier>(0), 0, 0);
		}
	}
}

void gl_widget::mousePressEvent(QMouseEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr && event->buttons())
	{
		view_handler->mouseClick(details::convert_button(event->button()),
			EventHandler::DOWN,
			details::convert_modifier(event->modifiers()),
			event->x(),
			event->y());
	}
}

void gl_widget::mouseReleaseEvent(QMouseEvent* event)
{
	auto view_handler = get_view_handler();
	if (view_handler != nullptr && event->buttons())
	{
		view_handler->mouseClick(details::convert_button(event->button()),
			EventHandler::UP,
			details::convert_modifier(event->modifiers()),
			event->x(),
			event->y());
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
	if (view_handler != nullptr && event->buttons())
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
	return view_manager_ != nullptr ? view_manager_->get_view_handler() : nullptr;
}

EventHandler::key details::convert_key(int key)
{
	switch (key)
	{
	case Qt::Key_Up:
		return EventHandler::KEY_UP;
	case Qt::Key_Down:
		return EventHandler::KEY_DOWN;
	case Qt::Key_Left:
		return EventHandler::KEY_LEFT;
	case Qt::Key_Right:
		return EventHandler::KEY_RIGHT;
	case Qt::Key_PageUp:
		return EventHandler::KEY_PAGE_UP;
	case Qt::Key_PageDown:
		return EventHandler::KEY_PAGE_DOWN;
	default:
		return static_cast<EventHandler::key>(0);
	}
}

EventHandler::button details::convert_button(int button)
{
	switch (button)
	{
	case Qt::LeftButton:
		return EventHandler::LEFT_BUTTON;
	case Qt::RightButton:
		return EventHandler::RIGHT_BUTTON;
	default:
		return static_cast<EventHandler::button>(0);
	}
}

EventHandler::modifier details::convert_modifier(int modifier)
{
	switch (modifier)
	{
	case Qt::CTRL:
		return EventHandler::CTRL;
	default:
		return static_cast<EventHandler::modifier>(0);
	}
}