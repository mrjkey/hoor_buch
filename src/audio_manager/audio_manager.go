package audio_manager

import (
	"fmt"
	"os"
	"path/filepath"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
	"github.com/faiface/beep"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
)

var currentPos int = 0
var App fyne.App
var Window fyne.Window

// audiobook library type
type Audiobook struct {
	Title  string
	Author string
	Length int
	Files  []string
}

type Library struct {
	Audiobooks []Audiobook
}

func (l *Library) AddAudiobook(book Audiobook) {
	l.Audiobooks = append(l.Audiobooks, book)
}

var library Library

type AudioFile struct {
	Path string
	Info os.FileInfo
}

func Init(main_app fyne.App, main_window fyne.Window) {
	App = main_app
	Window = main_window
}

func SetupAudioPlayer(filename string) (*beep.Ctrl, error) {
	// open an audio file
	file, err := os.Open(filename)
	if err != nil {
		fmt.Println("Error opening file")
		fmt.Println(err)
		return nil, err
		//weird formating

	}
	// defer file.Close()

	// decode the audio file
	streamer, format, err := mp3.Decode(file)
	if err != nil {
		fmt.Println("Error decoding file")
		fmt.Println(err)
		return nil, err
	}
	// defer streamer.Close()

	// initialize the speaker	// initialize the speaker
	err = speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))
	if err != nil {
		fmt.Println("Error initializing speaker")
		fmt.Println(err)
		return nil, err
	}

	// var ctrl *beep.Ctrl
	ctrl := &beep.Ctrl{Streamer: beep.Loop(-1, streamer), Paused: false}

	// play the audio
	// done := make(chan bool)
	speaker.Play(ctrl)
	return ctrl, nil
}

func SetupAudioPlayerGui(ctrl *beep.Ctrl) (*fyne.Container, error) {
	// Play button
	playBtn := widget.NewButton("Play", func() {
		go func() {
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
		}()
	})

	// Pause button
	pauseBtn := widget.NewButton("Pause", func() {
		go func() {
			// speaker.Lock()
			fmt.Println("Pausing audio")
			ctrl.Paused = true
			if seeker, ok := ctrl.Streamer.(beep.StreamSeeker); ok {
				currentPos = seeker.Position()
			}
			// speaker.Lock()
			// speaker.Play(ctrl)
			// speaker.Unlock()
			fmt.Println("end of pause button function")
		}()
	})

	// button to add an audiobook to the library
	addAudiobookBtn := widget.NewButton("Add Audiobook", func() {
		openFileDialog(Window, func(path string) {
			fmt.Println("Path selected: ", path)
		})
	})

	content := container.NewVBox(
		widget.NewLabel("Hello, Fyne!"),
		playBtn,
		pauseBtn,
		addAudiobookBtn,
	)

	return content, nil
}

// create a library of audiobooks.
// an audiobook can consist of 1 or many audio files
// an individual audio book's progress should be saved
// the library should be able to be saved and loaded from a file
// the library should be able to be sorted by title, author, and progress
// the library should be able to be searched by title, author, and progress
// the library should be auto-loaded when the application starts
// The user should have the option to add or remove audiobooks from the library
// The library should keep track if you have finished an audiobook or not
// This data should be saved in a format that can be easily loaded and saved, and uploaded to a cloud service

func LoadLibrary() {
	// load the library from a json file
}

func SaveLibrary() {
	// save the library to a json file

}

// add an audiobook to the library
func AddAudiobookToLibrary() (book_path string) {
	fmt.Println("Book path: ", book_path)

	// get list of all audio files in the directory
	audioFiles, err := listAudioFilesInDirectory(book_path)
	if err != nil {
		return
	}

	filePaths := make([]string, len(audioFiles))
	for i, file := range audioFiles {
		filePaths[i] = file.Path
	}

	// get the title to be the name of the directory
	title := filepath.Base(book_path)

	// get the author
	author := "Test Author"

	// get the length in seconds of the audiobook by adding up the length of all the audio files
	length := 0
	for _, file := range audioFiles {
		length += int(file.Info.Size())
	}

	// add the audiobook to the library
	audiobook := Audiobook{
		Title:  title,
		Author: author,
		Length: length,
		Files:  filePaths,
	}
	library.AddAudiobook(audiobook)

	// save the library
	SaveLibrary()

	// return
	return
}

func listAudioFilesInDirectory(directory string) ([]AudioFile, error) {
	var audioFiles []AudioFile

	// walk the directory
	err := filepath.Walk(directory, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err

		}
		if !info.IsDir() {
			return nil
		}
		// check if the file is an audio file
		// if it is, add it to the list
		switch filepath.Ext(path) {
		case ".mp3", ".wav":
			audioFiles = append(audioFiles, AudioFile{Path: path, Info: info})
		}
		return nil
	})
	if err != nil {
		return nil, err
	}

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
	fileDialog.Show()
}
