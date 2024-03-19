package audio_manager

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"time"

	"fyne.io/fyne/v2"
)

var currentPos int = 0
var App fyne.App
var Window fyne.Window
var library Library
var isPlaying bool
var content *fyne.Container
var bookList *fyne.Container
var currentBook *Audiobook = nil

func Init(main_app fyne.App, main_window fyne.Window) {
	App = main_app
	Window = main_window
}

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

	if currentBook == nil {
		// if the size of the library is greater than 0, set the current book to the first book in the library
		if len(library.Audiobooks) > 0 {
			SetCurrentBook(&library.Audiobooks[0])
		}
	}
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

func SetCurrentBook(book *Audiobook) {
	currentBook = book
}

func GetBookByTitle(title string) *Audiobook {
	for _, book := range library.Audiobooks {
		if book.Title == title {
			return &book
		}
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
