#ifndef TODOS_H
#define TODOS_H

#include <cppcms/application.h>
#include <cppdb/frontend.h>

namespace apps {

class todos : public cppcms::application {
public:
  todos(cppcms::service &srv);
  void todo(std::string s_uid);
  void todos_noarg();
  virtual void init();
  virtual void clear();
  // virtual void main(std::string url);
protected:
  cppdb::session sql;

private:
  std::string m_conn_str;
  std::string m_base_url;
   
  void prepend_cors_headers();
};

} // end apps
#endif // TODOS_H
