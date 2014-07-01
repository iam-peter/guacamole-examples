
#include <gua/guacamole.hpp>
#include <gua/renderer/GraphLoader.hpp>

std::shared_ptr<gua::Node> 

init_graph(gua::SceneGraph * scene_graph)
{
  gua::GraphLoader loader;

  std::shared_ptr<gua::Node>

  graph_node(loader.create("graph","data/materials/Red.gmd"));

  return scene_graph->add_node(scene_graph->get_root(),graph_node);
}

std::shared_ptr<gua::PointLightNode> 

init_light(gua::SceneGraph * scene_graph)
{
  return scene_graph->

  add_node<gua::PointLightNode>(scene_graph->get_root(),"light");
}

std::shared_ptr<gua::ScreenNode> 

init_screen(gua::SceneGraph * scene_graph)
{
  std::shared_ptr<gua::ScreenNode> screen = 

  scene_graph->add_node<gua::ScreenNode>(scene_graph->get_root(),"screen");

  screen->data.set_size(gua::math::vec2(1.6f,0.9f));
  screen->translate(0.0f,0.0f,1.0f);

  return screen;
}

std::shared_ptr<gua::TransformNode>

init_eye(gua::SceneGraph * scene_graph)
{
  std::shared_ptr<gua::TransformNode> eye =

  scene_graph->add_node<gua::TransformNode>(scene_graph->get_root(),"eye");
  eye->translate(0.0f,0.0f,2.5f);

  return eye;
}

void pipe_config(gua::Pipeline * pipe)
{
  unsigned const width = 1600 , height = 900;

  gua::Camera camera("eye","eye","screen","screen","scene");
  pipe->config.set_camera(camera);

  pipe->config.set_left_resolution(gua::math::vec2ui(width,height));
  pipe->config.set_enable_fps_display(true);
  pipe->config.set_enable_preview_display(true);

  gua::Window * window = new gua::Window();
  window->config.set_left_resolution(gua::math::vec2ui(width,height));
  window->config.set_size(gua::math::vec2ui(width,height));
  pipe->set_window(window);
}

int main(int argc,char ** argv)
{
  gua::init(argc,argv);

  gua::SceneGraph * scene_graph = new gua::SceneGraph("scene");

  auto graph_node(init_graph(scene_graph));
  auto light_node(init_light(scene_graph));
  auto screen_node(init_screen(scene_graph));
  auto eye_node(init_eye(scene_graph));

  gua::Pipeline * pipe(new gua::Pipeline);
  pipe_config(pipe);
  std::vector<gua::Pipeline *> pipes;
  pipes.insert(pipes.begin(),pipe);
  gua::Renderer renderer(pipes);


  double const frame_time = 1.0f / 60.0f;

  gua::events::MainLoop main_loop;
  gua::events::Ticker ticker(main_loop,frame_time);

  std::vector<gua::SceneGraph const*> entire_scene;
  entire_scene.insert(entire_scene.begin(),scene_graph);

  std::function<void(void)> draw = 

  [&]()
  {
    renderer.queue_draw(entire_scene);
  };

  ticker.on_tick.connect(draw);

  main_loop.start(); 

  return 0;
}

