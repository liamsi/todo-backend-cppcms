#ifndef TODOITEM_H
#define TODOITEM_H

#include <iostream>
#include <vector>

#include <cppcms/json.h>
#include <cppdb/frontend.h>
#include <booster/log.h>

class TodoItem {
public:
  // Constructor creates a new TodoItem, use for data coming from the client
  // don't forget to save it, before returning it back to the client
  TodoItem(const cppcms::json::value &v);

  // getter:
  int uid() const;
  std::string title() const;
  int order() const;
  bool completed() const;
  std::string
  url() const; // generate URL from m_base_url and m_uid or return empty string
  // end getter

  // "patch" method     (compare to merge function in Rust implementation
  // (https://github.com/Ryman/nickel-todo-backend/blob/67323cad18dbd3700d122c3f3949bbbca20b37e8/src/todo.rs#L37=
  // )
  void patch_from_json(const cppcms::json::value &todo_json,
                       cppdb::session &sql);

  // instance related DB methods:
  void save(cppdb::session &sql, const std::string &base_url);

  // static DB related methods
  static TodoItem find_by_id(int uid, const std::string &base_url,
                             cppdb::session &sql) throw(std::string);
  static void delete_by_id(int uid, cppdb::session &sql);
  static std::vector<TodoItem> all(const std::string &base_url,
                                   cppdb::session &sql);
  static void delete_all(cppdb::session &sql);
  // creates table if neccessary (in a real world app, one should use an sql
  // file for that)
  static void init_tables(cppdb::session &sql);

private:
  int m_uid;
  std::string m_title;
  int m_order;
  bool m_completed;
  std::string m_base_url;
  // create a new TodoItem in DB:
  void save(cppdb::session &sql);
  void insert_new(cppdb::session &sql);
  // create a TodoItem object from the Database (with uid) to pass it to the
  // client
  TodoItem(int uid, const std::string &title, int order, bool completed,
           const std::string &base_url);
  static TodoItem from_row(cppdb::result &res, const std::string &base_url);
};

// from and to JSON:
namespace cppcms {
// specialize cppcms::json::traits structure to convert objects from and to json
// values
namespace json {
template <> struct traits<TodoItem> {

  // convert automagically to JSON (make sure the TodoItem was saved before and
  // has a uid and base_url):
  static void set(value &v, TodoItem const &in) {
    v.set("title", in.title());
    v.set("order", in.order());
    v.set("completed", in.completed());
    if (in.uid() > 0) {
      v.set("uid", in.uid());
      v.set("url", in.url());
    } else {
      BOOSTER_ERROR(
          "Converting a TodoItem to JSON which wasn't saved beforehand.");
    }
  }
};

} // json
} // cppcms

#endif // TODOITEM_H
