#pragma once

#include <GL/glew.h>

#include <QTimer>
#include <QGLWidget>
#include <QWheelEvent>

#include <ork/core/Timer.h>
#include <proland/ui/BasicViewHandler.h>

#include "resource/resource_container.h"
#include "dem/elevation_cursor.h"

namespace elevations
{
	namespace ui
	{
		class gl_widget;

		namespace details
		{
			class gl_timer : public QTimer
			{
				Q_OBJECT

			public:
				explicit gl_timer(gl_widget* parent, int interval = 0);

				std::pair<double, double> end();

			public slots:
				void start(int msec);
				void start();

			private:
				Timer timer_;
				double t_;
				double dt_;
			};
		} // utils

		class gl_widget : public QGLWidget
		{
			Q_OBJECT

		public:
			explicit gl_widget(QWidget* parent = nullptr);
			virtual ~gl_widget();

			void init(const resource::resource_container* container, const dem::elevation_cursor* cursor = nullptr);

		public Q_SLOTS:
			void updateGL() override;

		protected:
			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int width, int height) override;

			void mousePressEvent(QMouseEvent* event) override;
			void wheelEvent(QWheelEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;

			void enterEvent(QEvent* event) override;
			void leaveEvent(QEvent* event) override;

		private:
			details::gl_timer q_timer_;
			std::pair<double, double> time_;			
			bool damaged_;

			resource::resource_container* resource_container_;
			dem::elevation_cursor* elevation_cursor_;

			ptr<proland::BasicViewHandler> get_view_handler() const;
		};
	} // QT user interface
} // project namespace

