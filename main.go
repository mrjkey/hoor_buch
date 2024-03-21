package main

import (
	"flag"
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"

	// import the audio_manager package from src/audio_manager.go
	"hoor_buch/src/audio_manager"
)

// This application will be an audiobook player that is build using go and the fyne library
// The application will be able to manage a library of audiobooks and play them
func main() {
	run_gui := getMode()

	if run_gui {
		myApp := app.New()
		myWindow := myApp.NewWindow("Audiobook App")
		audio_manager.Init(myApp, myWindow)
		content, err := audio_manager.SetupAudioPlayerGui()
		if err != nil {
			fmt.Println("Error setting up audio player gui")
			fmt.Println(err)
			return
		}

		myWindow.SetContent(content)
		myWindow.Resize(fyne.NewSize(800, 600))
		myWindow.ShowAndRun()
	}
}

func getMode() bool {
	mode := flag.String("mode", "", "Operation mode of the application (server, fetch, update)")
	serverURL := flag.String("server-url", "", "URL of the server to fetch from or update to")
	libraryPath := flag.String("library-path", "library.json", "Path to the library.json file")

	flag.Parse() // Parse the command-line arguments provided by the user

	switch *mode {
	case "server":
		// Start the application in server mode
		fmt.Println("Starting server...")
		startServer() // Assuming startServer is a function that starts the HTTP server
		return false
	case "fetch":
		if *serverURL == "" {
			fmt.Println("Server URL is required in fetch mode")
			return false
		}
		// Fetch the library data from the server
		fmt.Printf("Fetching library from %s...\n", *serverURL)
		err := fetchLibrary(*serverURL) // Assuming fetchLibrary is a function to fetch data
		if err != nil {
			fmt.Printf("Error fetching library: %v\n", err)
		}
		return false
	case "update":
		if *serverURL == "" {
			fmt.Println("Server URL is required in update mode")
			return false
		}
		// Update the server with local library data
		fmt.Printf("Updating library on %s with data from %s...\n", *serverURL, *libraryPath)
		err := updateLibrary(*serverURL, *libraryPath) // Assuming updateLibrary is a function to send data
		if err != nil {
			fmt.Printf("Error updating library: %v\n", err)
		}
		return false
	default:
		fmt.Println("Starting GUI mode as no flags were provided..")
		return true
	}
}

func ensureURLScheme(url string) string {
	if !strings.HasPrefix(url, "http://") && !strings.HasPrefix(url, "https://") {
		url = "http://" + url // Default to http if no scheme is provided
	}
	return url
}

func startServer() {
	http.HandleFunc("/library", libraryHandler)
	http.ListenAndServe(":8080", nil)
}

func libraryHandler(w http.ResponseWriter, r *http.Request) {
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

func fetchLibrary(serverURL string) error {
	serverURL = ensureURLScheme(serverURL)
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

func updateLibrary(serverURL string, libraryPath string) error {
	libraryFile, err := os.Open(libraryPath)
	if err != nil {
		return err
	}
	defer libraryFile.Close()

	serverURL = ensureURLScheme(serverURL)
	_, err = http.Post(serverURL+"/library", "application/json", libraryFile)
	return err
}
