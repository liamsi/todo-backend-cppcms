#ifndef TODOITEM_H
#define TODOITEM_H

#include <iostream>
#include <vector>

#include "cppdb/frontend.h"
#include "booster/log.h"

class TodoItem
{ 
public:
  // Constructor creates a new TodoItem, use for data coming from the client (ignore base_url/uid)
  // after saved and the base_url is set, 
  TodoItem(const std::string &title, int order, bool completed);  
  // create a TodoItem object from the Database (with uid) to pass it to the client
  TodoItem(int uid, const std::string &title, int order, bool completed, const std::string &base_url);  
  
  // getter:
  int uid() const;
  std::string title() const;
  int order() const;
  bool completed() const;
  std::string url() const; // generate URL from m_base_url and m_uid or return empty string
  // end getter
  // set base_url
  void set_base_url(const std::string &base_url);
  
  // merge all fields of one instance to another but the uid:
  void merge(const TodoItem &other); 
  // instance related DB methods:
  bool save(cppdb::session& sql);
  
  // static DB related methods  
  static TodoItem find_by_id(int uid, const std::string &base_url, cppdb::session& sql) throw (std::string);
  static bool delete_by_id(int uid, cppdb::session& sql);
  static std::vector<TodoItem> all(const std::string& base_url, cppdb::session& sql);
  static bool delete_all(cppdb::session& sql);
  // creates table if neccessary (in large app one would use an sql file for that)
  static void init_tables(cppdb::session& sql);
private:
  int m_uid;
  std::string m_title;
  int m_order;
  bool m_completed;
  std::string m_base_url; 
  // create a new TodoItem in DB:
  bool insert_new(cppdb::session& sql);  
};

// from and to JSON:
namespace cppcms {
    // specialize cppcms::json::traits structure to convert objects from and to json values
    namespace json {
    template<>
        struct traits<TodoItem> {
          
            static TodoItem get(value const &v)
            {
                if(v.type()!=is_object)
                    throw bad_value_cast();
                
                std::string title = v.get<std::string>("title");
                // optional!
                double order = v.get<double>("order");
                bool completed = v.get<bool>("completed");
                // uid <- comes as a post parameter
                return TodoItem(title, order, completed);
            }
            // covert automagically to JSON (make sure the TodoItem was saved before and has a uid and base_url):
            static void set(value &v, TodoItem const &in)
            {
                v.set("title",in.title());
                v.set("order",in.order());
                v.set("completed",in.completed());
                if(in.uid() > 0) 
                {
                  v.set("uid", in.uid());
                  v.set("url", in.url()); 
                } else {
                  BOOSTER_ERROR("Converting a TodoItem to JSON which wasn't saved beforehand.");
                }
            }
        };
      
    }  // json
} // cppcms



#endif // TODOITEM_H
