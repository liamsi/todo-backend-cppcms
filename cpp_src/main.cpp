#include <iostream>

#include <cppcms/applications_pool.h>
#include <cppcms/service.h>

#include "todos.h"

int main(int argc, char **argv) {
  try {
    cppcms::service srv(argc, argv);
    srv.applications_pool().mount(cppcms::applications_factory<apps::todos>());
    srv.run();
  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }
}
