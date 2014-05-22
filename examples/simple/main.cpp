/******************************************************************************
 * guacamole - delicious VR                                                   *
 *                                                                            *
 * Copyright: (c) 2011-2013 Bauhaus-Universität Weimar                        *
 * Contact:   felix.lauer@uni-weimar.de / simon.schneegans@uni-weimar.de      *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify it    *
 * under the terms of the GNU General Public License as published by the Free *
 * Software Foundation, either version 3 of the License, or (at your option)  *
 * any later version.                                                         *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program. If not, see <http://www.gnu.org/licenses/>.             *
 *                                                                            *
 ******************************************************************************/

#include <gua/guacamole.hpp>
#include <gua/renderer/ScatterPlotLoader.hpp>

#include <thread>
#include <chrono>

int main(int argc, char** argv) {

  // initialize guacamole
  gua::init(argc, argv);

  // setup scene
  gua::SceneGraph graph("main_scenegraph");

  gua::ScatterPlotLoader loader;
  auto teapot_geometry(loader.create_geometry_from_file("teapot", "data/objects/teapot.obj", "data/materials/Red.gmd", gua::ScatterPlotLoader::NORMALIZE_POSITION | gua::ScatterPlotLoader::NORMALIZE_SCALE));

  auto teapot = graph.add_node("/", teapot_geometry);

  teapot->scale(4.0f);

  auto light = graph.add_node<gua::PointLightNode>("/", "light");
  light->scale(5.f);
  light->translate(0, 1.f, 1.f);

  auto screen = graph.add_node<gua::ScreenNode>("/", "screen");
  screen->data.set_size(gua::math::vec2(1.6f, 0.9f));
  screen->translate(0.0f, 0.0f, 5.0f);

  auto eye = graph.add_node<gua::TransformNode>("/screen", "eye");
  eye->translate(0.0f, 0.0f, 7.0f);

  auto pipe = new gua::Pipeline();
  pipe->config.set_camera(gua::Camera("/screen/eye", "/screen/eye", "/screen", "/screen", "main_scenegraph"));
  pipe->config.set_enable_fps_display(true);

  pipe->config.set_enable_backface_culling(false);
  pipe->config.set_enable_preview_display(true);

  pipe->set_window(new gua::Window());
  pipe->get_window()->config.set_left_resolution(gua::math::vec2ui(1600, 1200));
  pipe->get_window()->config.set_size(gua::math::vec2ui(1600, 1200));
  pipe->config.set_left_resolution(gua::math::vec2ui(1600, 1200));

  gua::Renderer renderer({pipe});

  // application loop
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100/6));

	  teapot->rotate(0.1, 0, 1, 0);

    renderer.queue_draw({&graph});
  }

  return 0;
}
