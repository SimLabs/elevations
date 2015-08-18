#pragma once

#include <ork/scenegraph/SceneManager.h>
#include <proland/ui/BasicViewHandler.h>
#include <proland/util/PlanetViewController.h>

namespace elevations
{
	namespace ui
	{
		class view_manager : public Object, public proland::ViewManager
		{
		public:
			explicit view_manager(float radius = 1.0f, float dr = 1.1f);

			void update_resources() const;

			ptr<proland::BasicViewHandler> get_view_handler() const;

			ptr<SceneManager> getScene() override;
			ptr<proland::TerrainViewController> getViewController() override;
			vec3d getWorldCoordinates(int x, int y) override;

			static const double DISTANCE_INFINITY;

		protected:
			float radius_, dr_;

			ptr<SceneManager> scene_manager_;
			ptr<proland::BasicViewHandler> view_handler_;
			ptr<proland::PlanetViewController> view_controller_;

			void swap(ptr<view_manager> other);
		};

		namespace details
		{
			bool is_infinity(double value);

			bool is_infinity(const vec3<double>& vector);
		} // utils
	} //  QT user interface
} // project namespace
