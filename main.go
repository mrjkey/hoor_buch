package main

import (
	"fmt"
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"github.com/faiface/beep"
	// "github.com/faiface/beep/flac"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
	// "github.com/faiface/beep/wav"
	// "github.com/hajimehoshi/oto"
	"os"
	"time"
)

// This application will be an audiobook player that is build using go and the fyne library
// The application will be able to manage a library of audiobooks and play them

var currentPos int = 0

func main() {
	// create a new window
	// test out audio stuff
	// open an audio file
	f, err := os.Open("test.mp3")
	if err != nil {
		fmt.Println("Error opening file")
		fmt.Println(err)
		return
	}
	defer f.Close()

	// decode the audio file
	streamer, format, err := mp3.Decode(f)
	if err != nil {
		fmt.Println("Error decoding file")
		fmt.Println(err)
		return
	}
	defer streamer.Close()

	// initialize the speaker
	err = speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))
	if err != nil {
		fmt.Println("Error initializing speaker")
		fmt.Println(err)
		return
	}

	var ctrl *beep.Ctrl
	ctrl = &beep.Ctrl{Streamer: beep.Loop(-1, streamer), Paused: false}

	// play the audio
	// done := make(chan bool)
	speaker.Play(ctrl)

	// start the main loop
	fmt.Println("Hello, World!")
	myApp := app.New()
	myWindow := myApp.NewWindow("Audiobook App")

	// Play button
	playBtn := widget.NewButton("Play", func() {
		go func() {
			speaker.Lock()
			fmt.Println("Playing audio")
			if seeker, ok := ctrl.Streamer.(beep.StreamSeeker); ok && currentPos != 0 {
				fmt.Println("Seeking to position: ", currentPos)
				seeker.Seek(currentPos)
			}

			ctrl.Paused = false
			speaker.Unlock()
			fmt.Println("end of play button function")
		}()
	})

	// Pause button
	pauseBtn := widget.NewButton("Pause", func() {
		go func() {
			speaker.Lock()
			fmt.Println("Pausing audio")
			// speaker.Lock()
			// speaker.Play(ctrl)
			speaker.Unlock()
			fmt.Println("end of pause button function")
		}()
	})

	content := container.NewVBox(
		widget.NewLabel("Hello, Fyne!"),
		playBtn,
		pauseBtn,
	)

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
