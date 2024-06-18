package audio_manager

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

var (
	serverLibrary ServerLibrary
)

func LoadServerLibrary() error {
	// check if the server libarary directory exists, i.e. ../hb_server_files
	// if it does not exist, create the directory
	_, err := os.Stat(library.BaseFilePath)
	if os.IsNotExist(err) {
		os.Mkdir(library.BaseFilePath, 0755)
	}

	// load the library from a json file
	result := LoadLibrary()
	if !result {
		fmt.Println("Failed to load library")
		return fmt.Errorf("failed to load library")
	}
	return nil
}

func EnsureURLScheme(url string) string {
	if !strings.HasPrefix(url, "http://") && !strings.HasPrefix(url, "https://") {
		url = "http://" + url // Default to http if no scheme is provided
	}
	return url
}

func StartServer(serverPort string) {
	err := LoadServerLibrary()
	if err != nil {
		fmt.Println("Failed to load server library")
		return
	}
	http.HandleFunc("/library", LibraryHandler)
	http.HandleFunc("/register", RegisterClient)
	http.HandleFunc("/upload", UploadHandler)
	// get the server to listen locally on the provided port
	http.ListenAndServe(":"+serverPort, nil)
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
		http.ServeFile(w, r, GetLibraryFilePath())
		fmt.Println("Served library.json")
	case "POST":
		// write to the library file
		CopyResponseToFile(r.Body)
		// reload the library with the new path
		ReloadLibraryWithNewPath(library.BaseFilePath)

		// After reloading, check for missing audio files
		missingFiles := CheckMissingAudioFiles()
		if len(missingFiles) > 0 {
			// notify client about missing files
			missingFilesJSON, _ := json.Marshal(missingFiles)
			w.Write(missingFilesJSON)
			fmt.Println("Missing files: ", missingFiles)
		} else {
			w.Write([]byte("No missing files"))
		}
		fmt.Println("Updated library.json")
	default:
		http.Error(w, "Unsupported HTTP method", http.StatusBadRequest)
	}
}

func FetchLibrary(serverURL string) error {
	serverURL = EnsureURLScheme(serverURL)

	// check if the client directory exists, i.e. ../hb_client_files
	_, err := os.Stat(library.BaseFilePath)
	if os.IsNotExist(err) {
		os.Mkdir(library.BaseFilePath, 0755)
	}

	// Fetch the library data from the server
	resp, err := http.Get(serverURL + "/library")
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// write to the library file
	CopyResponseToFile(resp.Body)
	// reload the library with the new path
	ReloadLibraryWithNewPath(library.BaseFilePath)
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

func UploadHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case "POST":
		// Parse the multipart form
		err := r.ParseMultipartForm(1000 << 20) // 1000MB limit
		if err != nil {
			http.Error(w, "Error parsing form data", http.StatusBadRequest)
			return
		}

		// Get the audiobook name from the form
		audiobookName := r.FormValue("audiobookName")
		if audiobookName == "" {
			http.Error(w, "Error retrieving the audiobook name", http.StatusBadRequest)
			return
		}

		// Get the file from the form
		file, handler, err := r.FormFile("file")
		if err != nil {
			http.Error(w, "Error retrieving the file", http.StatusBadRequest)
			return
		}
		defer file.Close()

		dstPath := filepath.Join(library.BaseFilePath, audiobookName)
		dstPath = filepath.Join(dstPath, handler.Filename)
		// Create a new file in the destination path
		dst, err := os.Create(dstPath)
		if err != nil {
			http.Error(w, "Error creating the file", http.StatusInternalServerError)
			return
		}
		defer dst.Close()

		// Copy the uploaded file to the destination
		_, err = io.Copy(dst, file)
		if err != nil {
			http.Error(w, "Error saving the file", http.StatusInternalServerError)
			return
		}

		w.Write([]byte("File uploaded successfully"))
	default:
		http.Error(w, "Unsupported HTTP method", http.StatusBadRequest)
	}
}
