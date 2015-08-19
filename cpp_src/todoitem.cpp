#include <string>
#include <booster/log.h>

#include "todoitem.h"

using cppdb::session;

TodoItem::TodoItem(int uid, const std::string &title, int order, bool completed,
                   const std::string &base_url)
    : m_uid(uid), m_title(title), m_order(order), m_completed(completed),
      m_base_url(base_url) {}

TodoItem::TodoItem(const cppcms::json::value &v) : m_uid(0) {
  using cppcms::json::bad_value_cast;
  try {
    m_title = v.get<std::string>("title");
  } catch (bad_value_cast b) {
    m_title = "";
    BOOSTER_ERROR("Trying to create a TodoItem without a title");
  }
  try {
    m_order = v.get<int>("order");
  } catch (bad_value_cast b) {
    m_order = 0;
  }
  try {
    m_completed = v.get<bool>("completed");
  } catch (bad_value_cast b) {
    m_completed = false;
  }
}

int TodoItem::uid() const { return m_uid; }

std::string TodoItem::title() const { return m_title; }

int TodoItem::order() const { return m_order; }

bool TodoItem::completed() const { return m_completed; }


std::string TodoItem::url() const {
  if (m_base_url.empty() || m_uid == 0)
    return "";
  else
    return m_base_url + "/todos/" + std::to_string(m_uid);
}


void TodoItem::patch_from_json(const cppcms::json::value &todo_json,
                               cppdb::session &sql) {
  using cppcms::json::bad_value_cast;
  try {
    m_title = todo_json.get<std::string>("title");
  } catch (bad_value_cast b) { /* silently ignore title if non-existent */
  }
  try {
    m_order = todo_json.get<int>("order");
  } catch (bad_value_cast b) { /* silently ignore order if non-existent */
  }
  try {
    m_completed = todo_json.get<bool>("completed");
  } catch (bad_value_cast b) { /* silently ignore completed if non-existent */
  }

  save(sql);
}


/* --------------------------- */
/* --- DB related methods ---  */
/* --------------------------- */

void TodoItem::save(session &sql, const std::string &base_url) {
  m_base_url = base_url;
  save(sql);
}


void TodoItem::save(session &sql) {
  if (m_uid > 0) { // update
    cppdb::statement st = sql << "UPDATE todos SET title =     ?, "
                                 "order_idx = ?, "
                                 "completed = ? "
                                 "WHERE uid = ?" << title() << order()
                              << completed()
                              /* WHERE */ << m_uid << cppdb::exec;
  } else { // insert
    insert_new(sql);
  }
}

void TodoItem::insert_new(session &sql) {
  cppdb::statement st = sql
                        << "INSERT INTO todos(title, order_idx, completed) "
                           "VALUES( ? ,       ? ,            ?) RETURNING uid"
                        << title() << order() << completed();
  cppdb::result r = st.row();

  if (!r.empty())
    m_uid = r.get<int>(0);
}

// unexported helper:
TodoItem TodoItem::from_row(cppdb::result &res, const std::string &base_url) {
  int uid = res.get<int>(0);
  std::string title = res.get<std::string>(1);
  int order = res.get<int>(2);
  std::string s_completed = res.get<std::string>(
      3); // cannot get as bool! see pending feature request:
          // http://sourceforge.net/p/cppcms/feature-requests/4/
  bool completed = (s_completed == "t");

  return TodoItem(uid, title, order, completed, base_url);
}

TodoItem TodoItem::find_by_id(int uid, const std::string &base_url,
                              session &sql) throw(std::string) {
  cppdb::result res =
      sql << "SELECT uid, title, order_idx, completed FROM todos "
             "WHERE uid = ?" << uid << cppdb::row; // syntactic sugar (see:
  // http://cppcms.com/sql/cppdb/query.html)
  if (!res.empty()) {
    return from_row(res, base_url);
  } else {
    BOOSTER_ERROR("Could not find todo item with requested id");
    throw "Could not find todo item with id: " + std::to_string(uid);
  }
}

std::vector<TodoItem> TodoItem::all(const std::string &base_url, session &sql) {
  std::vector<TodoItem> all_todoItems;
  cppdb::result res =
      sql << "SELECT uid, title, order_idx, completed FROM todos";
  while (res.next()) {
    all_todoItems.push_back(from_row(res, base_url));
  }
  return all_todoItems;
}

void TodoItem::delete_all(session &sql) {
  cppdb::statement st = sql << "TRUNCATE todos";
  st.exec();
}

void TodoItem::delete_by_id(int uid, session &sql) {
  cppdb::statement st = sql << "DELETE FROM todos * WHERE uid = ?" << uid;
  st.exec();
}

void TodoItem::init_tables(session &sql) {
  sql << "CREATE TABLE IF NOT EXISTS todos ("
         "uid SERIAL PRIMARY KEY, "
         "title VARCHAR NOT NULL, "
         "order_idx INTEGER DEFAULT 0, "
         "completed BOOL DEFAULT FALSE)" << cppdb::exec;
}

/* --------------------------- */
/* --- END DB related methods  */
/* --------------------------- */
