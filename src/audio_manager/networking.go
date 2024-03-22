package audio_manager

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
)

var (
	serverLibrary ServerLibrary
)

func EnsureURLScheme(url string) string {
	if !strings.HasPrefix(url, "http://") && !strings.HasPrefix(url, "https://") {
		url = "http://" + url // Default to http if no scheme is provided
	}
	return url
}

func StartServer() {
	http.HandleFunc("/library", LibraryHandler)
	http.HandleFunc("/register", RegisterClient)
	http.ListenAndServe(":8080", nil)
}

func RegisterClient(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Client registered")

	// print the ip address of the client, ipv4 or ipv6, whichever is available
	fmt.Println("Client IP: ", r.RemoteAddr)
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
