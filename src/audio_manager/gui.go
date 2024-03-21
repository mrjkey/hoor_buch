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
	bookProgressLabel  *widget.Label  // Label to display the overall book progress
	fileProgressSlider *widget.Slider // Slider for the current file progress
	fileProgressLabel  *widget.Label  // Label to display the current file progress
	currentFileLabel   *widget.Label  // Label to display the current file name
	content            *fyne.Container
	volumeSlider       *widget.Slider
	volumeSliderLabel  *widget.Label
	progressLabelMap   map[int]*widget.Label
	bookList           *widget.List
	playPauseBtn       *widget.Button
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

func pausePlayBtn() {
	playIcon := theme.MediaPlayIcon()
	playPauseBtn.SetIcon(playIcon)
	isPlaying = false
}

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
	LoadLibrary()

	// playBtn := SetupPlayBtn()
	controlButtons := SetupControlButtons()

	// Initialize sliders and labels
	bookProgressSlider = widget.NewSlider(0, 100) // Assuming 100% as the max value for book progress
	bookProgressLabel = widget.NewLabel("Book Progress:")
	// bookProgressSlider.                  // Make it a progress bar (user cannot move it)
	fileProgressSlider = widget.NewSlider(0, 100) // Max value will be updated based on the file length
	fileProgressLabel = widget.NewLabel("File Progress:")
	currentFileLabel = widget.NewLabel("Current File: None")

	volumeSlider = widget.NewSlider(0, 100)
	volumeSlider.Value = starting_volume
	initialstring := fmt.Sprintf("Volume: %.0f", starting_volume)
	volumeSliderLabel = widget.NewLabel(initialstring)

	volumeSlider.OnChanged = func(value float64) {
		global_volume.Volume = SliderToVolume(value)
		volumeSliderLabel.SetText(fmt.Sprintf("Volume: %.0f", value))
	}

	if bookList == nil {
		progressLabelMap = make(map[widget.ListItemID]*widget.Label)
		init_booklist()
	}

	// Setup audio update goroutine
	go updateAudioProgress()

	addAudiobookBtn := widget.NewButton("Add Audiobook", func() {
		openFileDialog(Window, func(path string) {
			fmt.Println("Path selected: ", path)
			AddAudiobookToLibrary(path)
			bookList.Refresh()
		})
	})

	header := container.NewVBox(
		volumeSliderLabel,
		volumeSlider,
		currentFileLabel,
		controlButtons,
		bookProgressLabel,
		bookProgressSlider,
		fileProgressLabel,
		fileProgressSlider,
		addAudiobookBtn,
	)

	// Setup content with new widgets
	content = container.NewBorder(header, nil, nil, nil, bookList)

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
		loadAndPlayPreviousFile()
		bookmark := GetBookmark()
		bookmark.gui_init = false
	})
	rewindOneMinuteBtn := widget.NewButtonWithIcon("1 minute", rewindOneMinuteIcon, func() {
		// Implement the functionality for "Rewind One Minute"
		SeekAudio(-60)
	})
	rewindTenSecondsBtn := widget.NewButtonWithIcon("10 seconds", rewindTenSecondsIcon, func() {
		// Implement the functionality for "Rewind Ten Seconds"
		SeekAudio(-10)
	})
	playPauseBtn = SetupPlayBtn() // Your existing function for play/pause
	fastForwardTenSecondsBtn := widget.NewButtonWithIcon("10 seconds", fastForwardTenSecondsIcon, func() {
		// Implement the functionality for "Fast Forward Ten Seconds"
		SeekAudio(10)
	})
	fastForwardOneMinuteBtn := widget.NewButtonWithIcon("1 minute", fastForwardOneMinuteIcon, func() {
		// Implement the functionality for "Fast Forward One Minute"
		SeekAudio(60)
	})
	nextBtn := widget.NewButtonWithIcon("", nextIcon, func() {
		// Implement the functionality for "Next File"
		loadAndPlayNextFile()
		bookmark := GetBookmark()
		bookmark.gui_init = false
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

func init_booklist() {
	bookList = widget.NewList(
		func() int {
			return len(library.Audiobooks)
		},
		func() fyne.CanvasObject {
			label := widget.NewLabel("")
			// Add spacers above and below the label to increase the item's height.
			return container.NewVBox(widget.NewSeparator(), label, widget.NewLabel(""))
		},
		func(id widget.ListItemID, co fyne.CanvasObject) {
			book := &library.Audiobooks[id]
			container := co.(*fyne.Container)
			if label, exists := progressLabelMap[id]; exists {
				label.SetText(fmt.Sprintf("%s\n%s", book.Title, getProgressString(book.CurrentTime, book.TotalTime)))
			} else {
				label := container.Objects[1].(*widget.Label)
				label.SetText(fmt.Sprintf("%s\n%s", book.Title, getProgressString(book.CurrentTime, book.TotalTime)))
				progressLabelMap[id] = label
			}
		},
	)

	// Handle selection
	bookList.OnSelected = func(id widget.ListItemID) {
		pausePlayBtn()
		time.Sleep(10 * time.Millisecond)

		book := &library.Audiobooks[id]
		err := SetBookmarkByBook(book)
		if err != nil {
			fmt.Println("error setting bookmark")
			fmt.Println(err)
		}
		err = SetupAudioPlayer(book)
		if err != nil {
			fmt.Println("error setting up audio player")
			fmt.Println(err)
		}

		// fmt.Printf("Selected book: %s\n", book.Title)
		// fmt.Printf("gui id: %d\n", id)
	}

	bookmark := GetBookmark()
	if bookmark.book != nil {
		bookList.Select(bookmark.index)
	}

}

// Goroutine to update sliders and label based on audio progress
func updateAudioProgress() {
	for {
		time.Sleep(time.Second) // Update every second
		bookmark := GetBookmark()
		if bookmark.book != nil || !bookmark.gui_init {
			book := bookmark.book
			// Update book progress slider
			old_current_file_time := bookmark.book.CurrentFileTime
			// update the current file time
			if global_streamer != nil {
				book.SetFileTimeFromPosition(global_streamer.Position())
				if book.CurrentFileTime > old_current_file_time || !GetBookmark().gui_init {
					bookmark := GetBookmark()
					// calculate the book progress, update overall current time
					book.CurrentTime = GetSummedDurationUpToIndex() + book.CurrentFileTime
					totalDuration := book.TotalTime.Seconds()
					bookProgress := 0.0
					if totalDuration > 0 {
						bookProgress = (book.CurrentTime.Seconds() / totalDuration) * 100
						bookProgressSlider.SetValue(bookProgress)
					}

					// Update file progress slider
					fileProgress := 0.0
					if global_streamer != nil {
						fileDuration := global_streamer.Len()
						currentPosition := global_streamer.Position()
						if fileDuration > 0 {
							fileProgress = (float64(currentPosition) / float64(fileDuration)) * 100
							fileProgressSlider.SetValue(fileProgress)
						}
					}

					// Update labels
					_, fileName := filepath.Split(book.CurrentFile.Path)
					currentFileLabel.SetText(fmt.Sprintf("Current File: %s", fileName))
					bookProgressLabel.SetText(fmt.Sprintf("Book Progress: %.2f%%", bookProgress))
					fileProgressLabel.SetText(fmt.Sprintf("File Progress: %.2f%%", fileProgress))
					// fmt.print
					progressLabelMap[bookmark.index].SetText(fmt.Sprintf("%s\n%s", book.Title, getProgressString(book.CurrentTime, book.TotalTime)))

					SaveLibrary()

					bookmark.gui_init = true
				}
			}
		}

	}
}

func GetSummedDurationUpToIndex() time.Duration {
	sum := time.Duration(0)
	book := GetBookmark().book
	index := GetFileIndexByPath(book, book.CurrentFile.Path)
	for i := 0; i < index; i++ {
		sum += book.Files[i].Length
	}
	return sum

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
