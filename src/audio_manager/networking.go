package audio_manager

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

var (
	serverLibrary ServerLibrary
)

func LoadServerLibrary() {
	// load the library from a json file
	file, err := os.Open("library.json")
	if err != nil {
		return
	}
	defer file.Close()

	decoder := json.NewDecoder(file)
	err = decoder.Decode(&serverLibrary)
	if err != nil {
		return
	}
}

func EnsureURLScheme(url string) string {
	if !strings.HasPrefix(url, "http://") && !strings.HasPrefix(url, "https://") {
		url = "http://" + url // Default to http if no scheme is provided
	}
	return url
}

func StartServer() {
	LoadServerLibrary()
	http.HandleFunc("/library", LibraryHandler)
	http.HandleFunc("/register", RegisterClient)
	http.ListenAndServe(":8080", nil)
}

func RegisterClient(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Client registered")
	// print the ip address of the client, ipv4 or ipv6, whichever is available
	fmt.Println("Client IP: ", r.RemoteAddr)
	// check if the client is in the list of clients using their ip address
	// if not, add the client to the list of clients, provided them a unique id
	clients := serverLibrary.Clients
	clientIP := r.RemoteAddr
	clientExists := false
	for _, client := range clients {
		if client.Address == clientIP {
			clientExists = true
			break
		}
	}
	if !clientExists {
		newClient := ClientInfo{clientIP, "unique_id"}
		serverLibrary.Clients = append(serverLibrary.Clients, newClient)
		SaveServerLibrary()
	}
}

func SaveServerLibrary() error {
	data, err := json.MarshalIndent(serverLibrary, "", "    ")
	if err != nil {
		return fmt.Errorf("error marshalling library to json: %w", err)
	}
	file, err := os.Create("server_library.json")
	if err != nil {
		return fmt.Errorf("error creating file: %w", err)
	}
	defer file.Close()

	_, err = file.Write(data)
	if err != nil {
		return fmt.Errorf("error writing to file: %w", err)
	}
	return nil
}

func LibraryHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case "GET":
		// Serve the current state of library.json
		http.ServeFile(w, r, "library.json")
		fmt.Println("Served library.json")
	case "POST":
		// Update library.json with the data from the request
		outFile, err := os.Create("library.json")
		if err != nil {
			http.Error(w, "Failed to open library.json for writing", http.StatusInternalServerError)
			return
		}
		defer outFile.Close()
		_, err = io.Copy(outFile, r.Body)
		if err != nil {
			http.Error(w, "Failed to write to library.json", http.StatusInternalServerError)
			return
		}
		fmt.Println("Updated library.json")
	default:
		http.Error(w, "Unsupported HTTP method", http.StatusBadRequest)
	}
}

func FetchLibrary(serverURL string) error {
	serverURL = EnsureURLScheme(serverURL)
	resp, err := http.Get(serverURL + "/library")
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// Write the fetched content to library.json
	outFile, err := os.Create("library.json")
	if err != nil {
		return err
	}
	defer outFile.Close()

	_, err = io.Copy(outFile, resp.Body)
	return err
}

func UpdateLibrary(serverURL string, libraryPath string) error {
	libraryFile, err := os.Open(libraryPath)
	if err != nil {
		return err
	}
	defer libraryFile.Close()

	serverURL = EnsureURLScheme(serverURL)
	_, err = http.Post(serverURL+"/library", "application/json", libraryFile)
	return err
}
