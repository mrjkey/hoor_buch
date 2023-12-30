### Basic Requirements:

1. **Client Application (Desktop)**:
   - Develop a desktop application that can send data to and receive data from a web server.
   - Enable the application to run multiple instances on the same PC for testing.
   - Allow a client to register with the server and receive a unique ID.
   - Implement a feature to discover other clients registered on the server.
2. **Server Application (Web Server)**:
   - Create a simple web server that can receive registration from clients and store their details.
   - Provide a mechanism for clients to query other registered clients.
   - Implement a way to notify clients when another client wants to start a chat.
3. **Communication Protocol**:
   - Define a simple communication protocol for message exchange between client and server.
   - Ensure the protocol supports initiating and accepting chat sessions.
4. **Direct Client-to-Client Communication**:
   - After initial handshaking via the server, enable direct P2P messaging between clients.
   - The server should not be involved in the direct message exchange after the connection is established.
5. **Testing Capability**:
   - Ensure the application can send messages to itself for testing purposes.
