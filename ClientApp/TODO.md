The list of stuff that I need to get done in this app

- [x] Add volume control
- [x] Progress bar updates with book progress
	- [x] Store all file durations on startup
	- [x] Update progress bar will all previous files + current file time
	- [x] Show progress in hours, minutes, seconds
	- [x] Show progress / duration instead of just duration
- [ ] Continuous play - load next file, ff, reverse
- [ ] Complete book -> move book to another category
- [ ] Register Client to Server -> Get info from server
- [ ] Setup book transfer between clients

- Continues play - play one file after another
	- If the end of a file is reached, go to the next file (if it exists)
	- If rewinding, go to previous file if < 0 and file exists
	- If forwarding, got to next file if > total duration and file exists
- If a book is complete, move the book to the completed books section
- Setup server application that can store library info
	- This should store 
		- the client id (client name)
		- ipv6 address
		- library .yaml
		- all book .yamls
		- list of books
	- keep track of clients 
	- when a client syncs to the server, reply with the other client info for progress. 
- Setup book transfer to other clientapp
	- copy entire directory file by file
	- add directory to library



