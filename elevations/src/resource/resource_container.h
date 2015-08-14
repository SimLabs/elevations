#pragma once

#include <ork/scenegraph/SceneManager.h>
#include <proland/ui/BasicViewHandler.h>
#include <proland/util/PlanetViewController.h>

#include "dem/cube_mapper.h"

namespace elevations
{
	namespace resource
	{
		class resource_container : public Object, public proland::ViewManager
		{
		public:
			explicit resource_container(float radius = 1.0f, float dr = 1.1f);

			void update_resources() const;

			ptr<proland::BasicViewHandler> get_view_handler() const;
			ptr<dem::cube_mapper> get_cube_mapper() const;

			ptr<SceneManager> getScene() override;
			ptr<proland::TerrainViewController> getViewController() override;
			vec3d getWorldCoordinates(int x, int y) override;

			static ptr<resource_container> load_resource_container(const std::string& path, const std::string& filename, const std::string& log_filename = "log.html", size_t cache_size = 8);

			static const double DISTANCE_INFINITY;

		protected:
			float radius_, dr_;

			ptr<SceneManager> scene_manager_;
			ptr<proland::BasicViewHandler> view_handler_;
			ptr<proland::PlanetViewController> view_controller_;

			ptr<dem::cube_mapper> cube_mapper_;

			void swap(ptr<resource_container> o);
		};

		namespace details
		{
			template <typename T>
			bool is_infinity(T value)
			{
				return value > resource_container::DISTANCE_INFINITY;
			}

			template <typename T>
			bool is_infinity(const vec3<T>& vector)
			{
				return is_infinity(vector.x) || is_infinity(vector.y) || is_infinity(vector.z);
			}
		} // utils
	} // resources management
} // project namespace
