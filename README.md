# c_networking

[![makefile CI](https://github.com/kbrault/c_networking/actions/workflows/makefile.yml/badge.svg)](https://github.com/kbrault/c_networking/actions/workflows/makefile.yml)
[![cppcheck](https://github.com/kbrault/c_networking/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/kbrault/c_networking/actions/workflows/cppcheck.yml)

<details>
  <summary>basic_http_server.c</summary>
    
```mermaid
sequenceDiagram
    participant Server
    participant Client

    Server ->> Server: Create socket (socket())
    Server ->> Server: Set socket options (setsockopt())
    Server ->> Server: Bind socket to address and port (bind())
    Server ->> Server: Listen for incoming connections (listen())

    loop Continuously listen for connections
        Server ->> Server: Accept incoming connection (accept())
        Client -->> Server: New connection accepted
        Client ->> Server: HTTP request
        Server -->> Client: HTTP response (write())
        Client ->> Server: Close connection
        Server ->> Server: Close client socket (close())
    end
```
</details>
<details>
  <summary>multithread_http_server.c</summary>

```mermaid
sequenceDiagram
    participant Client
    participant Server
    participant Thread

    Server ->> Server: Create socket (socket())
    Server ->> Server: Set socket options (setsockopt())
    Server ->> Server: Bind socket to address and port (bind())
    Server ->> Server: Listen for incoming connections (listen())
    loop Handling Connections
        Server ->> Server: Accept incoming connection (accept())
        Client -->> Server: New connection accepted
        Client ->> Server: HTTP request
        Server ->> Thread: Create thread
        Thread -->> Server: Handle request
        Server -->> Client: HTTP response (write())
        Client ->> Server: Close connection
        Server ->> Server: Close client socket (close())
    end

```
</details>