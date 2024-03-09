package main

import (
	"fmt"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"

	// import the audio_manager package from src/audio_manager.go
	"hoor_buch/src/audio_manager"
)

// This application will be an audiobook player that is build using go and the fyne library
// The application will be able to manage a library of audiobooks and play them
func main() {
	// create a new window
	// test out audio stuff

	filename := "test.mp3"
	ctrl, err := audio_manager.SetupAudioPlayer(filename)
	if err != nil {
		fmt.Println("Error setting up audio player")
		fmt.Println(err)
		return
	}
	// start the main loop
	fmt.Println("Hello, World!")
	myApp := app.New()
	myWindow := myApp.NewWindow("Audiobook App")

	content, err := audio_manager.SetupAudioPlayerGui(ctrl)
	if err != nil {
		fmt.Println("Error setting up audio player gui")
		fmt.Println(err)
		return
	}

	// Set the content of the window.
	// For now, we'll just use a simple label as a placeholder.
	myWindow.SetContent(content)

	// Set the window size.
	myWindow.Resize(fyne.NewSize(400, 300))

	// Show and run the application.
	myWindow.ShowAndRun()
	// wait for the audio to finish
	// <-done
}
