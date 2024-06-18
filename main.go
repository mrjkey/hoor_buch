package main

import (
	"flag"
	"fmt"

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

// set the library base path for server or client
func getMode() bool {
	mode := flag.String("mode", "", "Operation mode of the application (server, fetch, update)")
	serverURL := flag.String("server-url", "", "URL of the server to fetch from or update to")
	serverPort := flag.String("port", "", "Port the server should listen on")

	flag.Parse() // Parse the command-line arguments provided by the user

	switch *mode {
	case "server":
		audio_manager.SetLibraryPath("../hb_server_files")
		if *serverPort == "" {
			fmt.Println("Server URL is required in fetch mode")
			return false
		}
		// Start the application in server mode
		fmt.Printf("Starting server from %s...\n", *serverPort)
		audio_manager.StartServer(*serverPort) // Assuming startServer is a function that starts the HTTP server
		return false
	case "fetch":
		audio_manager.SetLibraryPath("../hb_client_files")
		if *serverURL == "" {
			fmt.Println("Server URL is required in fetch mode")
			return false
		}
		// Fetch the library data from the server
		fmt.Printf("Fetching library from %s...\n", *serverURL)
		err := audio_manager.FetchLibrary(*serverURL) // Assuming fetchLibrary is a function to fetch data
		if err != nil {
			fmt.Printf("Error fetching library: %v\n", err)
		}
		return false
	case "update":
		audio_manager.SetLibraryPath("../hb_client_files")
		if *serverURL == "" {
			fmt.Println("Server URL is required in update mode")
			return false
		}
		// Update the server with local library data
		fmt.Printf("Updating library on %s with data from %s...\n", *serverURL, audio_manager.GetLibraryFilePath())
		err := audio_manager.UpdateLibrary(*serverURL, audio_manager.GetLibraryFilePath()) // Assuming updateLibrary is a function to send data
		if err != nil {
			fmt.Printf("Error updating library: %v\n", err)
		}
		return false
	default:
		audio_manager.SetLibraryPath("../hb_client_files")
		fmt.Println("Starting GUI mode as no flags were provided..")
		return true
	}
}
