# CornWeb

A simple http web server written in C, only accepts GET and HEAD requests. Aiming for simplicity, best for people who just want to host an informative website that does not require intense processing.

## Functionalities

- Use a configuration file to tweak settings
- User defined endpoints
- Accept basic GET, HEAD requests

## Build From Source

### Build

Under the project root directory

```
make
```

### Clean

Under the project root directory

```
make clean
```

## Configuration

**MAKE SURE THE CONFIGURATION FILE IS AT THE PARENT DIRECTORY OF THE ACTUAL PROGRAM**

There are a few thing you can configure in the server.conf file

```
listening_from=ADDRESS
port=PORT
root_static=/path/to/file
```

- 'listening_from' gets the address of the listening range you want.
- 'port' gets the port number you want this server to start on.
- 'root_static' gets the path to the static file you want to show when accessing the domain without any path.(e.g. index.html)

You can also set endpoints in a certain section of the configuration file.
Under the line '\[/endpoint=/path/to/file\]', you can define your endpoints as such

```
[/endpoint=/path/to/file]
/endpoint=/path/to/file
```

Here is an example

```
[/endpoint=/path/to/file]
/style.css=/home/name/project/style.css
```

## Limitations

- Only accept GET, HEAD requests
- No dynamic routing
- Serves static files only
- Single threaded
- No https support
- No logging
