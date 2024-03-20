package audio_manager

import (
	"fmt"
	"path/filepath"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
)

var (
	bookProgressSlider *widget.Slider // Slider for the overall book progress
	fileProgressSlider *widget.Slider // Slider for the current file progress
	currentFileLabel   *widget.Label  // Label to display the current file name
	content            *fyne.Container
	volumeSlider       *widget.Slider
	volumeSliderLabel  *widget.Label
)

func SetupPlayBtn() *widget.Button {
	playIcon := theme.MediaPlayIcon()
	playBtn := widget.NewButtonWithIcon("", playIcon, nil)
	playBtn.OnTapped = func() {
		go func() {
			togglePlayPause(playBtn)
		}()
	}
	return playBtn
}

func togglePlayPause(playBtn *widget.Button) {
	playIcon := theme.MediaPlayIcon()
	pauseIcon := theme.MediaPauseIcon()

	if isPlaying {
		PauseAudio()
		playBtn.SetIcon(playIcon)
	} else {
		PlayAudio()
		playBtn.SetIcon(pauseIcon)
	}
	isPlaying = !isPlaying
}

// func SetupAudioPlayerGuiOld() (*fyne.Container, error) {
// 	playBtn := SetupPlayBtn()
// 	// button to add an audiobook to the library

// 	// setup content as a "column" container, which stacks its children vertically
// 	header := container.NewVBox(
// 		widget.NewLabel("Hello, Fyne!"),
// 		playBtn,
// 		addAudiobookBtn,
// 		// layout.NewSpacer(),
// 	)

// 	content = container.New(layout.NewBorderLayout(header, nil, nil, nil), header)
// 	// content = container.New(layout.NewBorderLayout(header, nil, nil, nil), header)

// 	LoadLibrary()

// 	return content, nil
// }

// SliderToVolume converts a slider position (0-100) to a volume control value.
func SliderToVolume(sliderPos float64) float64 {
	if sliderPos <= 0 {
		// Mute condition, handle accordingly in your application
		return -100000 // Returning negative infinity to represent muting
	} else if sliderPos >= 100 {
		return 0 // Maximum volume
	} else {
		// Convert slider position to a dB scale linearly, then convert dB to power ratio
		db := (sliderPos/100.0)*(maxDB-minDB) + minDB
		// Assuming the base is 2, convert dB to the power ratio (volume)
		volume := db / 10.0
		return volume
	}
}

// Update GUI setup to include sliders and current file label
func SetupAudioPlayerGui() (*fyne.Container, error) {
	// playBtn := SetupPlayBtn()
	controlButtons := SetupControlButtons()

	// Initialize sliders and labels
	bookProgressSlider = widget.NewSlider(0, 100) // Assuming 100% as the max value for book progress
	// bookProgressSlider.                  // Make it a progress bar (user cannot move it)
	fileProgressSlider = widget.NewSlider(0, 100) // Max value will be updated based on the file length
	currentFileLabel = widget.NewLabel("Current File: None")

	volumeSlider = widget.NewSlider(0, 100)
	volumeSlider.Value = starting_volume
	initialstring := fmt.Sprintf("Volume: %.0f", starting_volume)
	volumeSliderLabel = widget.NewLabel(initialstring)

	volumeSlider.OnChanged = func(value float64) {
		global_volume.Volume = SliderToVolume(value)
		volumeSliderLabel.SetText(fmt.Sprintf("Volume: %.0f", value))
	}

	// Setup audio update goroutine
	go updateAudioProgress()

	addAudiobookBtn := widget.NewButton("Add Audiobook", func() {
		openFileDialog(Window, func(path string) {
			fmt.Println("Path selected: ", path)
			AddAudiobookToLibrary(path)
			DisplayLibrary()
		})
	})

	// Setup content with new widgets
	content = container.NewVBox(
		volumeSliderLabel,
		volumeSlider,
		currentFileLabel,
		controlButtons,
		widget.NewLabel("Book Progress:"),
		bookProgressSlider,
		widget.NewLabel("File Progress:"),
		fileProgressSlider,
		addAudiobookBtn,
	)

	// Your existing setup logic...
	LoadLibrary()

	return content, nil
}

// Add the additional button setup functions here
func SetupControlButtons() *fyne.Container {
	// Icons for the control buttons
	prevIcon := theme.MediaSkipPreviousIcon()
	rewindOneMinuteIcon := theme.MediaFastRewindIcon()
	rewindTenSecondsIcon := theme.MediaFastRewindIcon()
	fastForwardTenSecondsIcon := theme.MediaFastForwardIcon()
	fastForwardOneMinuteIcon := theme.MediaFastForwardIcon()
	nextIcon := theme.MediaSkipNextIcon()

	// Initialize buttons with icons
	prevBtn := widget.NewButtonWithIcon("", prevIcon, func() {
		// Implement the functionality for "Previous File"
	})
	rewindOneMinuteBtn := widget.NewButtonWithIcon("1 minute", rewindOneMinuteIcon, func() {
		// Implement the functionality for "Rewind One Minute"
	})
	rewindTenSecondsBtn := widget.NewButtonWithIcon("10 seconds", rewindTenSecondsIcon, func() {
		// Implement the functionality for "Rewind Ten Seconds"
	})
	playPauseBtn := SetupPlayBtn() // Your existing function for play/pause
	fastForwardTenSecondsBtn := widget.NewButtonWithIcon("10 seconds", fastForwardTenSecondsIcon, func() {
		// Implement the functionality for "Fast Forward Ten Seconds"
	})
	fastForwardOneMinuteBtn := widget.NewButtonWithIcon("1 minute", fastForwardOneMinuteIcon, func() {
		// Implement the functionality for "Fast Forward One Minute"
	})
	nextBtn := widget.NewButtonWithIcon("", nextIcon, func() {
		// Implement the functionality for "Next File"
	})

	// Arrange buttons in a horizontal layout
	controlButtons := container.NewHBox(
		prevBtn,
		rewindOneMinuteBtn,
		rewindTenSecondsBtn,
		playPauseBtn,
		fastForwardTenSecondsBtn,
		fastForwardOneMinuteBtn,
		nextBtn,
	)

	return controlButtons
}

func DisplayLibrary() {
	// Remove the previous library content
	content.Remove(bookList)

	// Use a List widget for selectable books
	bookList := widget.NewList(
		func() int {
			return len(library.Audiobooks)
		},
		func() fyne.CanvasObject {
			label := widget.NewLabel("")
			slider := widget.NewSlider(0, 1) // Initial min and max values; adjust based on actual audiobook durations
			slider.Step = 1                  // Set step value to 1 for fine-grained control, adjust as needed
			return container.NewVBox(label, slider)
		},
		func(id widget.ListItemID, co fyne.CanvasObject) {
			book := library.Audiobooks[id]
			container := co.(*fyne.Container)
			label := container.Objects[0].(*widget.Label)
			slider := container.Objects[1].(*widget.Slider)

			label.SetText(fmt.Sprintf("%s\n%s", book.Title, getProgressString(book.CurrentTime, book.TotalTime)))
			slider.Min = 0
			slider.Max = book.TotalTime.Seconds()
			slider.Value = book.CurrentTime.Seconds()
			slider.OnChanged = func(value float64) {
				// Update the book's current time based on slider value
				// You'll need to implement this to actually seek in the audio playback
				book.CurrentTime = time.Duration(value) * time.Second
			}
			slider.Refresh()
		},
	)

	// Handle selection
	bookList.OnSelected = func(id widget.ListItemID) {
		book := library.Audiobooks[id]
		err := SetupAudioPlayer(&book)
		if err != nil {
			fmt.Println("error setting up audio player")
			fmt.Println(err)
		}

		fmt.Printf("Selected book: %s\n", book.Title)
		fmt.Printf("gui id: %d\n", id)
	}

	bookmark := GetBookmark()
	if bookmark.book != nil {
		bookList.Select(bookmark.index)
	}

	// Add the library content back into the main content
	content.Add(bookList)
}

// Goroutine to update sliders and label based on audio progress
func updateAudioProgress() {
	for {
		time.Sleep(time.Second) // Update every second

		if GetBookmark().book != nil {
			// Update book progress slider
			totalDuration := GetBookmark().book.TotalTime.Seconds()
			currentDuration := GetBookmark().book.CurrentTime.Seconds()
			if totalDuration > 0 {
				bookProgress := (currentDuration / totalDuration) * 100
				bookProgressSlider.SetValue(bookProgress)
			}

			// Update file progress slider
			if global_streamer != nil {
				fileDuration := global_streamer.Len()
				currentPosition := global_streamer.Position()
				if fileDuration > 0 {
					fileProgress := (float64(currentPosition) / float64(fileDuration)) * 100
					fileProgressSlider.SetValue(fileProgress)
				}
			}

			// Update current file label
			_, fileName := filepath.Split(GetBookmark().book.CurrentFile.Path)
			currentFileLabel.SetText(fmt.Sprintf("Current File: %s", fileName))
		}

	}
}

// Example on changing the file slider value manually by the user
// This should seek within the current playing file accordingly
func setupFileProgressSlider() {
	fileProgressSlider.OnChanged = func(value float64) {
		if global_streamer != nil && GetBookmark().book != nil {
			fileDuration := global_streamer.Len()
			newPosition := int(value / 100 * float64(fileDuration))
			global_streamer.Seek(newPosition)
			GetBookmark().book.SetFileTimeFromPosition(newPosition)
		}
	}
}
