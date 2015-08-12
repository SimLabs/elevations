#pragma once

#include <ork/scenegraph/SceneManager.h>
#include <proland/ui/BasicViewHandler.h>
#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	class resource_container : public Object, public proland::ViewManager
	{
	public:
		resource_container();

		void update_resources() const;

		ptr<proland::BasicViewHandler> get_view_handler() const;
		ptr<proland::CPUElevationProducer> get_elevations_producer() const;

		ptr<SceneManager> getScene() override;
		ptr<proland::TerrainViewController> getViewController() override;
		vec3d getWorldCoordinates(int x, int y) override;

		static ptr<resource_container> load_resource_container(const std::string& path, const std::string& filename, const std::string& log_filename = "log.html", size_t cache_size = 8);

		static const double DISTANCE_INFINITY;

	protected:
		float radius_;
		float distance_;
		float dr_;

		ptr<SceneManager> scene_manager_;
		ptr<proland::BasicViewHandler> view_handler_;
		ptr<proland::TerrainViewController> view_controller_;

		ptr<proland::CPUElevationProducer> elevation_producer_;

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
} // project namespace
