# todo-backend-cppcms

This is yet another (quickly hacked) implementation of [todo-backend](http://www.todobackend.com/).
It fullfills the [specs](https://github.com/TodoBackend/todo-backend-js-spec) but wasn't published on 
a public server (heroku etc). You can test it on your local machine though. (Expect a detailed 
description soon).

This implementation is fully written in C++ using [cppCMS](http://cppcms.com/) running with
[lighttpd](http://www.lighttpd.net/) and storing data using
[cppDB](http://cppcms.com/sql/cppdb/) with a [postgresql](http://www.postgresql.org/) backend. 
(At the time beeing, there is no implementation in C++ on http://www.todobackend.com). 
The main purpose of this project is to present some web development example using C++/cppCMS in at a local 
user group [meetup](http://www.meetup.com/Meeting-C-Dusseldorf/events/224069170/) (here are the [slides](http://slides.com/ismailkhoffi/cppcms-intro/live#/) for that event).

### TODOs 
 - [ ] add a step by step description on howto run/test it locally
 - [x] link slides
 - [ ] deploy to some cloud service (heroku/yellow-circle), test 
 - [ ] add this implementation to this [list](https://github.com/TodoBackend/todo-backend-site/blob/master/data/implementations.yaml) and create a pull request
