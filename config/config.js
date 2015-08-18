// start cppCMS application using this config, *after* starting the database- and web-server (see lighttpd.conf/fastcgi.conf for latter)
{    
    "service" : {
        "api" : "fastcgi",
         "socket" : "/tmp/todos-fastcgi-socket" 
    },
    "logging" : {
      "level" : "error", // change to debug (and use BOOSTER_DEBUG) if you need more output
    },
    "todo-backend" : {  
      // application specific data (change if your postgres srv is running on another machine/port) 
      "connection_string" : "postgresql:host=localhost;port=5432;dbname=todos;user=todosapp", 
      "base_url": "http://localhost:8080",
    },
}
