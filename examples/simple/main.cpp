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
#include <gua/renderer/TriMeshLoader.hpp>

#include <gua/utils/DataColumn.hpp>
#include <gua/utils/DataSet.hpp>
#include <gua/utils/string_utils.hpp>

#include <gua/renderer/ScatterPlotLoader.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

int main(int argc, char** argv) {

  // initialize guacamole
  gua::init(argc, argv);

  // setup scene
  gua::SceneGraph graph("main_scenegraph");

  gua::ScatterPlotLoader scatterPlotLoader;
  std::vector<float> xdata, ydata;
  for (float x(0.0); x < 1.0; x += 0.05)
  {
    xdata.push_back(x);
    ydata.push_back(0.5 + 0.5 * std::sin(x * 2 * M_PI));

  }
  auto scatterplot_geometry(scatterPlotLoader.create(
      "scatterplot"
    , "data/materials/Red.gmd"
    , xdata
    , ydata
    , ydata
  ));
  auto scatterplot = graph.add_node("/", scatterplot_geometry);
  scatterplot->scale(0.5f);

  auto light = graph.add_node<gua::node::PointLightNode>("/", "light");
  light->scale(5.f);
  light->translate(0, 1.f, 1.f);

  auto screen = graph.add_node<gua::node::ScreenNode>("/", "screen");
  screen->data.set_size(gua::math::vec2(1.6f, 0.9f));

  auto eye = graph.add_node<gua::node::TransformNode>("/screen", "eye");
  eye->translate(0, 0, 5);

  unsigned int width = 1920;
  unsigned int height = (9.0 / 16.0) * width;

  auto pipe = new gua::Pipeline();
  pipe->config.set_camera(gua::Camera("/screen/eye", "/screen/eye",
                                      "/screen", "/screen",
                                      "main_scenegraph"));
  pipe->config.set_left_resolution(gua::math::vec2ui(width, height));
  pipe->config.set_enable_fps_display(true);
  pipe->config.set_enable_frustum_culling(true);
  pipe->config.set_enable_preview_display(true);

  auto window(new gua::Window());
  window->config.set_size(gua::math::vec2ui(width, height));
  window->config.set_left_resolution(gua::math::vec2ui(width, height));
  window->config.set_enable_vsync(true);

  pipe->set_window(window);

  gua::Renderer renderer({pipe});

  // application loop
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100/6));

	  scatterplot->rotate(0.1, 0, 1, 0);

    renderer.queue_draw({&graph});
  }

  return 0;
}
