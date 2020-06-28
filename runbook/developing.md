# Development environment setup

## Web portal
Web portal is designed to run in Docker container and collaborate with VSCode Remote Development.
Make sure you have `docker-compose` on your machine. VSCode will ask to open the project in the container.

To start backend web server, run:
```
$ cargo run
```

To start frontend webpack dev server, run:
```
$ cd manarimo
$ npm start
```