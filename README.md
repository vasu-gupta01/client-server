## FILES

- **man/client.man** - Source file for client man page.
- **man/server.man** - Source file for server man page.
- **util/hash_table.h** - Header for hash table implementation.
- **util/hash_table.c** - Source file for hash table implementation.
- **client.c** - Source file for client implementation.
- **server.c** - Source file for server implementation.
- **server.h** - Header file for global defines and function declarations.
- **tands.c** - Source file containing Trans and Sleep implementations.
- **makefile** - Makefile to generate server and client executables and also generate PDFs for the man files of client and server.
- **README.md** - This file.

## ASSUMPTIONS

1. The input provided to client is always correct (See INSTRUCTIONS for input format).
2. Arguments provided to server and client are correct.
3. The server is terminated by waiting for the 30-second timer. Terminating manually with keyboard interrupt does not display transaction summary.

## SUMMARY

This is an implementation of a single threaded client-server architecture. Refer to **server.pdf** and **client.pdf** for more details.

_Note:_

I started by implementing a simple client-server architecture model based on binarytides.com/server-client-example-c-sockets-linux. The rest of the implementation was built around this.
As I was developing, I tested on my machine using the localhost ip-address (127.0.0.1).

## INSTRUCTIONS

There may be multiple ways to run and compile, the following steps outline the simplest way:

- In the root directory `a3/`, run `make`. This should compile and generate the required executables, including 2 PDFs for the client and server man pages.
- Run the server using `./server [PORT]`
- Run clients on the same machine or different using `./client [PORT] [SERVER-IP] <[INPUT-FILE]`.
- **NOTE:** `[INPUT-FILE]` is a file containing a series of `T<n>` and `S<n>` separated by a new-line.
