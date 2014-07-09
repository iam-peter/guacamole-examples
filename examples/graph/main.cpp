
#include <gua/guacamole.hpp>
#include <gua/renderer/GraphLoader.hpp>

std::shared_ptr<gua::Node> 

init_graph(gua::SceneGraph * scene_graph)
{
  gua::GraphLoader loader;

  std::shared_ptr<gua::Node>

  graph(loader.generate("graph","data/materials/Red.gmd",10,10));

  graph->translate(0.0f,0.0f,-400.0f);

  return scene_graph->add_node(scene_graph->get_root(),graph);
}

std::shared_ptr<gua::SunLightNode> 

init_light(gua::SceneGraph * scene_graph)
{
  std::shared_ptr<gua::SunLightNode> light = 

  scene_graph->add_node<gua::SunLightNode>(scene_graph->get_root(),"light");

  light->translate(0.0f,0.0f,0.0f);

  return light;
}

std::shared_ptr<gua::ScreenNode> 

init_screen(gua::SceneGraph * scene_graph)
{
  std::shared_ptr<gua::ScreenNode> screen = 

  scene_graph->add_node<gua::ScreenNode>(scene_graph->get_root(),"screen");

  screen->data.set_size(gua::math::vec2(1.6f,0.9f));
  screen->translate(0.0f,0.0f,0.0f);

  return screen;
}

std::shared_ptr<gua::TransformNode>

init_eye(gua::SceneGraph * scene_graph)
{
  std::shared_ptr<gua::TransformNode> eye =

  scene_graph->add_node<gua::TransformNode>("screen/","eye");

  eye->translate(0.0f,0.0f,2.0f);

  return eye;
}

void pipe_config(gua::Pipeline * pipe)
{
  unsigned const width = 1600 , height = 900;

  gua::Camera camera("screen/eye","screen/eye","screen","screen","scene");
  pipe->config.set_camera(camera);

  pipe->config.set_left_resolution(gua::math::vec2ui(width,height));
  pipe->config.set_enable_fps_display(true);
  pipe->config.set_enable_preview_display(true);
  pipe->config.set_enable_backface_culling(false);

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
    //graph_node->rotate(0.2,0.0,0.1,0.0);
    renderer.queue_draw(entire_scene);
  };

  ticker.on_tick.connect(draw);

  main_loop.start(); 

  return 0;
}

