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
#include <gua/renderer/ConeTreeLoader.hpp>
#include <gua/renderer/TriMeshLoader.hpp>

#include <thread>
#include <chrono>

const std::string geometry("data/objects/monkey.obj");

void setup_scene(gua::SceneGraph& graph,
                 std::shared_ptr<gua::Node> const& root_monkey,
                 int depth_count) {

  if (depth_count > 0) {
    gua::TriMeshLoader loader;

    float offset(2.f);
    std::vector<gua::math::vec3> directions = {
      gua::math::vec3(0, offset, 0),
      gua::math::vec3(0, -offset, 0),
      gua::math::vec3(offset, 0, 0),
      gua::math::vec3(-offset, 0, 0),
      gua::math::vec3(0, 0, offset),
      gua::math::vec3(0, 0, -offset)
    };

    for (auto direction: directions) {
      auto monkey_geometry(loader.create_geometry_from_file(
        "monkey",
        geometry,
        "data/materials/Stones.gmd"
      ));

      auto monkey = root_monkey->add_child(monkey_geometry);
      monkey->scale(0.5, 0.5, 0.5);
      monkey->translate(direction[0], direction[1], direction[2]);

      setup_scene(graph, monkey, depth_count - 1);
    }
  }
}


int main(int argc, char** argv) {

  // initialize guacamole
  gua::init(argc, argv);

  // setup sample scenegraph
  gua::SceneGraph graph("main_scenegraph");

  gua::TriMeshLoader loader;

  auto monkey_geometry(loader.create_geometry_from_file(
    "root_ape",
    geometry,
    "data/materials/Stones.gmd"
  ));

  auto root_monkey = graph.add_node("/", monkey_geometry);
  root_monkey->scale(0.5, 0.5, 0.5);

  // depth    monkey    cube          car
  // 1        14.084      56    3.619.000 Vertices  /      7 draw calls
  // 2        74.444     296   19.129.000 Vertices  /     37 draw calls
  // 3       436.604   1.736  112.189.000 Vertices  /    217 draw calls
  // 4     2.609.564  10.376              Vertices  /  1.297 draw calls
  // 5    15.647.324  62.216              Vertices  /  7.777 draw calls
  // 6    93.873.884 373.256              Vertices  / 46.657 draw calls
  //setup_scene(graph, root_monkey, 2);

  auto sun_light1 = graph.add_node<gua::SunLightNode>("/", "sun_light1");
  auto sun_light2 = graph.add_node<gua::SunLightNode>("/", "sun_light2");
  auto sun_light3 = graph.add_node<gua::SunLightNode>("/", "sun_light3");
  auto sun_light4 = graph.add_node<gua::SunLightNode>("/", "sun_light4");

  auto sun_light11 = graph.add_node<gua::SunLightNode>("/sun_light1/", "sun_light11");
  auto sun_light12 = graph.add_node<gua::SunLightNode>("/sun_light1/", "sun_light12");
  auto sun_light13 = graph.add_node<gua::SunLightNode>("/sun_light1/", "sun_light13");


  // displayed scenegraph
  gua::SceneGraph CT_graph("CT_scenegraph");

  auto sun_light = CT_graph.add_node<gua::SunLightNode>("/", "sun_light");

  auto screen = CT_graph.add_node<gua::ScreenNode>("/", "screen");
  screen->data.set_size(gua::math::vec2(1.6f, 1.2));
  screen->translate(0.0f, 0.0f, 3.0f);

  auto eye = CT_graph.add_node<gua::TransformNode>("/screen", "eye");
  eye->translate(0.0f, 1.0f, 5.0f);

  //auto teapot_geometry(loader.create_geometry_from_file("teapot", "data/objects/teapot.obj", "data/materials/Red.gmd", gua::ScatterPlotLoader::NORMALIZE_POSITION | gua::ScatterPlotLoader::NORMALIZE_SCALE));
  gua::ConeTreeLoader CT_loader;
  auto conetree_node(CT_loader.create("ConeTree", "data/materials/Red.gmd", graph));
  auto conetree = CT_graph.add_node("/", conetree_node);
  conetree->scale(0.2f);
  
  auto pipe = new gua::Pipeline();
  pipe->config.set_camera(gua::Camera("/screen/eye", "/screen/eye", "/screen", "/screen", "CT_scenegraph"));
  pipe->config.set_enable_fps_display(true);

  pipe->config.set_enable_backface_culling(false);
  //pipe->config.set_enable_preview_display(true);

  pipe->set_window(new gua::Window());
  pipe->get_window()->config.set_left_resolution(gua::math::vec2ui(1600, 1200));
  pipe->get_window()->config.set_size(gua::math::vec2ui(1600, 1200));
  pipe->config.set_left_resolution(gua::math::vec2ui(1600, 1200));

  gua::Renderer renderer({pipe});

  // application loop
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100/6));

	  //conetree->rotate(0.1, 0, 1, 0);

    renderer.queue_draw({&CT_graph});
  }

  return 0;
}
