#pragma once

#include "ui_main_window.h"

namespace elevations
{
	namespace ui
	{
		class main_window : public QMainWindow
		{
			Q_OBJECT

		public:
			explicit main_window(QWidget* parent = nullptr);

			gl_widget* get_gl_widget() const;

		private:
			Ui::main_windowClass ui_;
		};
	} // QT user interface
} // project namespace