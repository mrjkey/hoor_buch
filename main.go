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
	myApp := app.New()
	myWindow := myApp.NewWindow("Audiobook App")
	audio_manager.Init(myApp, myWindow)
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

	content, err := audio_manager.SetupAudioPlayerGui(ctrl)
	if err != nil {
		fmt.Println("Error setting up audio player gui")
		fmt.Println(err)
		return
	}

	myWindow.SetContent(content)
	myWindow.Resize(fyne.NewSize(800, 600))
	myWindow.ShowAndRun()
}
