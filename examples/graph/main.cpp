/******************************************************************************
* guacamole - delicious VR *
* *
* Copyright: (c) 2011-2013 Bauhaus-Universität Weimar *
* Contact: felix.lauer@uni-weimar.de / simon.schneegans@uni-weimar.de *
* *
* This program is free software: you can redistribute it and/or modify it *
* under the terms of the GNU General Public License as published by the Free *
* Software Foundation, either version 3 of the License, or (at your option) *
* any later version. *
* *
* This program is distributed in the hope that it will be useful, but *
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License *
* for more details. *
* *
* You should have received a copy of the GNU General Public License along *
* with this program. If not, see <http://www.gnu.org/licenses/>. *
* *
******************************************************************************/

#include <gua/guacamole.hpp>
#include <gua/renderer/GraphLoader.hpp>

#include <thread>
#include <chrono>

int main(int argc, char** argv)
{
  gua::init(argc, argv);

  gua::ShadingModelDatabase::load_shading_models_from("data/materials/");
  gua::MaterialDatabase::load_materials_from("data/materials/");

  gua::SceneGraph scene_graph("main_scene");

  gua::GraphLoader loader;
  auto graph_geometry(loader.create("graph_visoalization","data/materials/Red.gmd"));

  auto graph = scene_graph.add_node("/",graph_geometry);

  auto light = scene_graph.add_node<gua::PointLightNode>("/","light");
  light->scale(5.f);
  light->translate(0, 1.f, 1.f);

  auto screen = scene_graph.add_node<gua::ScreenNode>("/","screen");
  screen->data.set_size(gua::math::vec2(1.6f,0.9f));
	screen->translate(0.0f, 0.0f, 5.0f);

  auto eye = scene_graph.add_node<gua::TransformNode>("/screen", "eye");
  eye->translate(0, 0, 7.0);

  auto pipe = new gua::Pipeline();
  pipe->config.set_camera(gua::Camera("/screen/eye","/screen/eye",
																			"/screen","/screen",
																			"main_scene"));
  pipe->config.set_enable_fps_display(true);

  pipe->config.set_enable_backface_culling(false);
	pipe->config.set_enable_preview_display(true);

  pipe->set_window(new gua::Window());
	pipe->get_window()->config.set_left_resolution(gua::math::vec2ui(1600, 1200));
  pipe->get_window()->config.set_size(gua::math::vec2ui(1600, 1200));
  pipe->config.set_left_resolution(gua::math::vec2ui(1600, 1200));

  gua::Renderer renderer({pipe});

  while (true)
	{
    std::this_thread::sleep_for(std::chrono::milliseconds(100/6));
    renderer.queue_draw({&scene_graph});
  }

  return 0;
}

