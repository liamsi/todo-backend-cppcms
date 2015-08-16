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

void todos::todo(std::string num)
{
    prepend_cors_headers();
    int uid = std::stoi(num);  
    if(request().request_method()=="OPTIONS") {
       response().out(); 
       return;
    } 
    if(request().request_method()=="GET") {
      try { 
        TodoItem todo = TodoItem::find_by_id(uid, m_base_url, sql);
        response().set_content_header("application/json");
        cppcms::json::value result;
        result = todo;
        response().out() << result;
      } catch (std::string e) {        
        response().status(cppcms::http::response::not_found, "TodoItem with requested id does not exist");
        response().out();
        return;
      }
      
    }
    else if(request().request_method()=="PATCH") {
      TodoItem orig = TodoItem::find_by_id(uid, m_base_url, sql); 
      // get modifications from request, 
      // merge them into the object and save
    } else if(request().request_method()=="DELETE") {
      TodoItem::delete_by_id(uid, sql);
    } 
}

void todos::todos_noarg()
{
    prepend_cors_headers();
    if(request().request_method()=="OPTIONS") {
      response().out(); 
      return;
    } 
    if(request().request_method()=="GET") {
      try { 
        std::vector<TodoItem> all_todos = TodoItem::all(m_base_url, sql);
        response().set_content_header("application/json");
        cppcms::json::value result;
        result = all_todos;
        response().out() << result;
      } catch (std::string e) {        
        response().status(cppcms::http::response::not_found, "TodoItem with requested id does not exist");
        response().out();
        return;
      }      
    }
    else if(request().request_method()=="POST") {
      response().set_content_header("application/json");
      // TODO create new TodoItem from data in request and save it 
      // convert back to JSON, store in result
      
      cppcms::json::value result;
      response().out() << result;
    }
    else if(request().request_method()=="DELETE") {
      TodoItem::delete_all(sql);
      response().set_content_header("application/json");
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