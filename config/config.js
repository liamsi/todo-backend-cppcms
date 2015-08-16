// start cppCMS application using this config, *after* starting the database- and web-server (see lighttpd.conf/fastcgi.conf for latter)
{    
    "service" : {
        "api" : "fastcgi",
         "socket" : "/tmp/todos-fastcgi-socket" 
    },
    "logging" : {
      "level" : "info",
    },
    "todo-backend" : {  
      // application specific data  
      "connection_string" : "postgresql:host=localhost;port=5432;dbname=todos;user=todosapp", //psql -h localhost -p 5432 -U todosapp todos
      "base_url": "http://localhost:8080",
    },
}
