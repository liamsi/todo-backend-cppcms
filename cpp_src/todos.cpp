#include <iostream>

#include <cppcms/applications_pool.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/url_dispatcher.h> 
#include <cppcms/url_mapper.h>
#include <cppcms/json.h>
#include <booster/log.h>

#include "todos.h"
#include "todoitem.h"

namespace apps {
todos::todos(cppcms::service& srv) : cppcms::application(srv)
{
  m_conn_str = settings().get<std::string>("todo-backend.connection_string");
  m_base_url = settings().get<std::string>("todo-backend.base_url");
  sql.open(m_conn_str);
  BOOSTER_INFO("Connected to db");
  TodoItem::init_tables(sql); 
  
  dispatcher().assign("/todos/(\\d+)",&todos::todo,this,1);
  mapper().assign("todo","/todos/{1}");
  
  dispatcher().assign("/todos",&todos::todos_noarg,this);
  mapper().assign("todos","/todos");
  
  mapper().root("/todos");
}

void todos::init()
{
  sql.open(m_conn_str);
  cppcms::application::init();
}

void todos::clear()
{
  sql.close();
  cppcms::application::clear();
}

// unexported helper to convert from raw POST data to cppcms::json::value
static cppcms::json::value from_raw_post_data(const std::pair<void *,size_t> &post_data) throw(booster::invalid_argument) {
  using cppcms::json::value;
  std::istringstream ss(std::string(reinterpret_cast<char const *>(post_data.first),post_data.second));
  value request;
  if(!request.load(ss,true))
          throw booster::invalid_argument("Invalid JSON");
  return request;
}

void todos::todo(std::string num)
{
    using cppcms::json::value;
    prepend_cors_headers();
    int uid = std::stoi(num);  // change to atoi if you have a very old compiler or don't not want --std=c++11 
    if(request().request_method()=="OPTIONS") {
       response().out(); 
       return;
    } 
    response().set_content_header("application/json");
    if(request().request_method()=="GET") {
      try { 
        TodoItem todo = TodoItem::find_by_id(uid, m_base_url, sql);
        value result = todo;
        response().out() << result;
      } catch (std::string e) {        
        response().status(cppcms::http::response::not_found, "TodoItem with requested id does not exist");
        response().out();
        return;
      }
      
    }
    else if(request().request_method()=="PATCH") {
      // HTTPs PATCH isn't supposed to be used like this, but we want to follow the specs of www.todo-backend.com
      // more details: http://williamdurand.fr/2014/02/14/please-do-not-patch-like-an-idiot/ 
      // or https://tools.ietf.org/html/rfc5789
      value json_todo = from_raw_post_data(request().raw_post_data());
      TodoItem orig = TodoItem::find_by_id(uid, m_base_url, sql);
      orig.patch_from_json(json_todo);
      
      value patched  = orig;
      response().out() << patched;  
    } else if(request().request_method()=="DELETE") {
      TodoItem::delete_by_id(uid, sql);
    } 
}

void todos::todos_noarg()
{
    using cppcms::json::value;
    prepend_cors_headers();
    if(request().request_method()=="OPTIONS") {
      response().out(); 
      return;
    } 
    response().set_content_header("application/json");
    if(request().request_method()=="GET") {
      try { 
        std::vector<TodoItem> all_todos = TodoItem::all(m_base_url, sql);        
        value result = all_todos;
        response().out() << result;
      } catch (std::string e) {        
        response().status(cppcms::http::response::not_found, "TodoItem with requested id does not exist");
        response().out();
        return;
      }      
    }
    else if(request().request_method()=="POST") {
      value json_todo = from_raw_post_data(request().raw_post_data());
      TodoItem new_item = json_todo.get_value<TodoItem>();
      new_item.save(sql, m_base_url);

      cppcms::json::value result = new_item;
      response().out() << result;
    }
    else if(request().request_method()=="DELETE") {
      TodoItem::delete_all(sql);
      cppcms::json::value empty;
      response().out() << empty;
    } 
}


void todos::prepend_cors_headers()
{
    response().set_header("Access-Control-Allow-Origin","*");
    response().set_header("Access-Control-Allow-Headers","accept,Content-Type");
    response().set_header("Access-Control-Allow-Methods", "GET,POST,PATCH,DELETE");
    if(request().request_method()=="OPTIONS") {
        response().out();
	return;
     }    
}
} // end apps