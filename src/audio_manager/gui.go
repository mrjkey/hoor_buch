package audio_manager

import (
	"fmt"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/widget"
	"github.com/faiface/beep"
)

func SetupPlayBtn(ctrl *beep.Ctrl) *widget.Button {
	playBtn := widget.NewButton("Play", nil) // Temporarily no action
	playBtn.OnTapped = func() {
		go func() {
			togglePlayPause(ctrl, playBtn)
		}()
	}
	return playBtn
}

func togglePlayPause(ctrl *beep.Ctrl, playBtn *widget.Button) {
	if isPlaying {
		PauseAudio(ctrl)
		playBtn.SetText("Play")
	} else {
		PlayAudio(ctrl)
		playBtn.SetText("Pause")
	}
	isPlaying = !isPlaying
}

func SetupAudioPlayerGui(ctrl *beep.Ctrl) (*fyne.Container, error) {
	playBtn := SetupPlayBtn(ctrl)
	// button to add an audiobook to the library
	addAudiobookBtn := widget.NewButton("Add Audiobook", func() {
		openFileDialog(Window, func(path string) {
			fmt.Println("Path selected: ", path)
			AddAudiobookToLibrary(path)
			DisplayLibrary()
		})
	})

	// setup content as a "column" container, which stacks its children vertically
	header := container.NewVBox(
		widget.NewLabel("Hello, Fyne!"),
		playBtn,
		addAudiobookBtn,
		// layout.NewSpacer(),
	)

	content = container.New(layout.NewBorderLayout(header, nil, nil, nil), header)
	// content = container.New(layout.NewBorderLayout(header, nil, nil, nil), header)

	LoadLibrary()

	return content, nil
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
		fmt.Printf("Selected book: %s\n", book.Title)
		fmt.Printf("gui id: %d\n", id)
	}

	if bookmark.book != nil {
		bookList.Select(bookmark.index)
	}

	// Add the library content back into the main content
	content.Add(bookList)
}

// func SetupAudioPlayerGui(ctrl *beep.Ctrl) (*fyne.Container, error) {
//     // Example of changing the Play button with an icon and primary importance
//     playIcon := theme.MediaPlayIcon()
//     playBtn := widget.NewButtonWithIcon("Play", playIcon, func() {
//         go PlayAudio(ctrl)
//     })
//     playBtn.Importance = widget.HighImportance // Make it a primary button

//     // Pause button with an icon
//     pauseIcon := theme.MediaPauseIcon()
//     pauseBtn := widget.NewButtonWithIcon("Pause", pauseIcon, func() {
//         go PauseAudio(ctrl)
//     })

//     // Stop button
//     stopIcon := theme.MediaStopIcon()
//     stopBtn := widget.NewButtonWithIcon("Stop", stopIcon, func() {
//         go StopAudio(ctrl) // Assuming you have a StopAudio function
//     })

//     // Adding an audiobook button
//     addAudiobookIcon := theme.ContentAddIcon()
//     addAudiobookBtn := widget.NewButtonWithIcon("Add Audiobook", addAudiobookIcon, func() {
//         openFileDialog(Window, func(path string) {
//             fmt.Println("Path selected: ", path)
//             AddAudiobookToLibrary(path)
//         })
//     })

//     // Adjusting button layout, using a grid for example
//     buttonLayout := container.NewGridWithColumns(2, playBtn, pauseBtn, stopBtn, addAudiobookBtn)

//     content := container.NewVBox(
//         widget.NewLabel("Hello, Fyne!"),
//         buttonLayout,
//     )

//     return content, nil
// }

// func DisplayLibrary() {
// 	// remove the previous library content
// 	content.Remove(libraryContent)
// 	// update the library content
// 	libraryContent = container.NewVBox()
// 	for _, book := range library.Audiobooks {
// 		libraryContent.Add(widget.NewLabel(book.Title))
// 	}

// 	// add the library content back into the main content
// 	content.Add(libraryContent)
// }
