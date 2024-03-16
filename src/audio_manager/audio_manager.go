package audio_manager

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/storage"
	"fyne.io/fyne/v2/widget"
	"github.com/faiface/beep"
)

var currentPos int = 0
var App fyne.App
var Window fyne.Window

var library Library

var isPlaying bool
var content *fyne.Container
var bookList *fyne.Container

func Init(main_app fyne.App, main_window fyne.Window) {
	App = main_app
	Window = main_window
}

func PlayAudio(ctrl *beep.Ctrl) {
	// speaker.Lock()
	fmt.Println("Playing audio")
	if seeker, ok := ctrl.Streamer.(beep.StreamSeeker); ok && currentPos != 0 {
		fmt.Println("Seeking to position: ", currentPos)
		seeker.Seek(currentPos)
	} else {
		fmt.Println("Seeker not found")
	}

	ctrl.Paused = false
	// speaker.Unlock()
	fmt.Println("end of play button function")
}

func SetupPlayBtn(ctrl *beep.Ctrl) *widget.Button {
	playBtn := widget.NewButton("Play", nil) // Temporarily no action
	playBtn.OnTapped = func() {
		go func() {
			togglePlayPause(ctrl, playBtn)
		}()
	}
	return playBtn
}

func PauseAudio(ctrl *beep.Ctrl) {
	fmt.Println("Pausing audio")
	ctrl.Paused = true
	if seeker, ok := ctrl.Streamer.(beep.StreamSeeker); ok {
		currentPos = seeker.Position()
	}
	fmt.Println("end of pause button function")
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

func durationToHHMMSS(d time.Duration) string {
	// Convert duration to total seconds
	totalSeconds := int(d.Seconds())
	hours := totalSeconds / 3600
	minutes := (totalSeconds % 3600) / 60
	seconds := totalSeconds % 60

	// Return formatted string
	return fmt.Sprintf("%02d:%02d:%02d", hours, minutes, seconds)
}

func displayProgress(currentTime, totalTime time.Duration) string {
	return fmt.Sprintf("%s / %s", durationToHHMMSS(currentTime), durationToHHMMSS(totalTime))
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

			label.SetText(fmt.Sprintf("%s\n%s", book.Title, displayProgress(book.CurrentTime, book.TotalTime)))
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
	}

	// Add the library content back into the main content
	content.Add(bookList)
}

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

func LoadLibrary() {
	// load the library from a json file
	file, err := os.Open("library.json")
	if err != nil {
		return
	}
	defer file.Close()

	decoder := json.NewDecoder(file)
	err = decoder.Decode(&library)
	if err != nil {
		return
	}
	fmt.Println("Library: ", library)

	// display the library
	DisplayLibrary()
}

func SaveLibrary() error {
	// save the library to a json file
	data, err := json.MarshalIndent(library, "", "    ")
	if err != nil {
		return fmt.Errorf("error marshalling library to json: %w", err)
	}

	file, err := os.Create("library.json")
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

// add an audiobook to the library
func AddAudiobookToLibrary(book_path string) {
	fmt.Println("Book path: ", book_path)

	// get list of all audio files in the directory
	audioFiles, err := listAudioFilesInDirectory(book_path)
	if err != nil {
		fmt.Println("Error getting audio files")
		return
	}

	filePaths := make([]string, len(audioFiles))
	for i, file := range audioFiles {
		filePaths[i] = file.Path
		fmt.Println("File: ", file.Path)
	}

	// get the title to be the name of the directory
	title := filepath.Base(book_path)

	// get the author
	author := "Test Author"

	// get the length in seconds of the audiobook by adding up the length of all the audio files
	length := 0
	for _, file := range audioFiles {
		duration, _ := getAudioFileDuration(file.Path)
		length += int(duration)
	}

	// convert length to time.Duration
	lengthDuration := time.Duration(length) * time.Second

	// add the audiobook to the library
	audiobook := Audiobook{
		Title:       title,
		Author:      author,
		TotalTime:   lengthDuration,
		CurrentTime: 0,
		Path:        book_path,
		Files:       filePaths,
	}
	library.AddAudiobook(audiobook)

	// save the library
	SaveLibrary()
}

func listAudioFilesInDirectory(directory string) ([]AudioFile, error) {
	var audioFiles []AudioFile

	// walk the directory
	err := filepath.Walk(directory, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err

		}
		if info.IsDir() {
			return nil
		}

		switch filepath.Ext(path) {
		case ".mp3", ".wav":
			audioFiles = append(audioFiles, AudioFile{Path: path, Info: info})
			// fmt.Println("Audio File: ", path)
		default:
			// fmt.Println("File is not an audio file: ", path)
		}
		return nil
	})
	if err != nil {
		return nil, err
	}

	// print length of audio files
	fmt.Println("Length of audio files: ", len(audioFiles))

	return audioFiles, nil
}

func openFileDialog(window fyne.Window, callback func(string)) {
	//fileFilter := storage.NewExtensionFileFilter([]string{".mp3", ".wav"})
	fileDialog := dialog.NewFolderOpen(func(uri fyne.ListableURI, err error) {
		if err != nil {
			dialog.ShowError(err, window)
			return
		}
		if uri == nil {
			// user cancelled
			return
		}

		// return the
		callback(uri.Path())
	}, window)
	//	fileDialog.SetFilter(fileFilter)
	defaultPath := "D:\\Torrents\\Books"
	convertedPath := storage.NewFileURI(defaultPath)

	listablePath, err := storage.ListerForURI(convertedPath)
	if err != nil {
		fmt.Println("Error making the URI listable:", err)
		// Handle the error, perhaps default to some other path or present an error to the user
	} else {
		fileDialog.SetLocation(listablePath)
	}
	fileDialog.Show()
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
