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

#include <gua/utils/DataColumn.hpp>
#include <gua/utils/DataSet.hpp>
#include <gua/utils/string_utils.hpp>

#include <gua/renderer/enums.hpp>
#include <gua/renderer/ScatterPlotLoader.hpp>

#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {

  // initialize guacamole
  gua::init(argc, argv);

  // setup scene
  gua::SceneGraph graph("main_scenegraph");

  gua::utils::DataSet dataset;
  dataset.load_from_csv("data/csv/cars.csv", ";");

  gua::ScatterPlotLoader scatterPlotLoader;
  auto scatterplot_geometry(scatterPlotLoader.create_from_dataset(
      "scatterplot"
    , "data/materials/Red.gmd"
    , dataset
    , "Gewicht(t)"
    , "PS"
    , "Vmax(km/h)"
  ));
  auto scatterplot = graph.add_node("/", scatterplot_geometry);
  scatterplot->scale(0.2f);

  auto light = graph.add_node<gua::PointLightNode>("/", "light");
  light->scale(5.f);
  light->translate(0, 1.f, 1.f);

  auto screen = graph.add_node<gua::ScreenNode>("/", "screen");
  screen->data.set_size(gua::math::vec2(0.45f, 0.253125));

  auto left_eye = graph.add_node<gua::TransformNode>("/screen", "left_eye");
  left_eye->translate(-0.032, 0, 0.6);

  auto right_eye = graph.add_node<gua::TransformNode>("/screen", "right_eye");
  right_eye->translate(0.032, 0, 0.6);

  unsigned int width = 1920;
  unsigned int height = (9.0 / 16.0) * width;

  auto pipe = new gua::Pipeline();
  pipe->config.set_camera(gua::Camera("/screen/left_eye", "/screen/right_eye",
                                      "/screen", "/screen",
                                      "main_scenegraph"));
  pipe->config.set_left_resolution(gua::math::vec2ui(width, height));
  pipe->config.set_right_resolution(gua::math::vec2ui(width, height));
  pipe->config.set_enable_fps_display(true);
  pipe->config.set_enable_frustum_culling(true);
  pipe->config.set_enable_preview_display(true);
  pipe->config.set_enable_stereo(true);

  auto window(new gua::Window());
  window->config.set_size(gua::math::vec2ui(width, height));
  window->config.set_left_resolution(gua::math::vec2ui(width, height));
  window->config.set_right_resolution(gua::math::vec2ui(width, height));
  window->config.set_enable_vsync(true);
  window->config.set_stereo_mode(gua::StereoMode::ANAGLYPH_RED_CYAN);

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
