package audio_manager

import (
	"bufio"
	"encoding/json"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"time"

	"fyne.io/fyne/v2"
)

var App fyne.App
var Window fyne.Window
var library Library
var isPlaying bool
var global_bookmark = &Bookmark{-1, nil, true}

func GetBookmark() *Bookmark {
	return global_bookmark
}

func Init(main_app fyne.App, main_window fyne.Window) {
	App = main_app
	Window = main_window
}

func SetLibraryPath(path string) {
	library.BaseFilePath = path
}

func GetLibraryFilePath() string {
	return library.BaseFilePath + "/library.json"
}

func ReloadLibraryWithNewPath(newPath string) error {
	// Load the library from the file
	result := LoadLibrary()
	if !result {
		fmt.Println("Failed to load library")
		return fmt.Errorf("failed to load library")
	}

	// Update the base file path to the current base path
	SetLibraryPath(newPath)

	// Save the library to the file
	err := SaveLibrary()
	return err
}

func CopyResponseToFile(body io.Reader) error {
	// Write the fetched content to the library file
	outFile, err := os.Create(GetLibraryFilePath())
	if err != nil {
		return err
	}
	defer outFile.Close()
	_, err = io.Copy(outFile, body)
	if err != nil {
		return err
	}
	return nil
}

func LoadLibrary() bool {
	// load the library from a json file
	// basepath must be set before calling this function
	file, err := os.Open(GetLibraryFilePath())
	if err != nil {
		// if the library can't be loaded, ask the user if a new file should be created
		reader := bufio.NewReader(os.Stdin)
		fmt.Println("Library file not found, would you like to create a new library file? (y/n)")
		text, _ := reader.ReadString('\n')
		text = strings.TrimSpace(text)

		if strings.ToLower(text) == "y" {
			fmt.Println("Creating new library file")
			library = Library{BaseFilePath: library.BaseFilePath}
			SaveLibrary()
			return true
		}
		return false
	}
	defer file.Close()

	decoder := json.NewDecoder(file)
	err = decoder.Decode(&library)
	if err != nil {
		return false
	}

	ensureReferences()

	if global_bookmark.book == nil {
		// if the size of the library is greater than 0, set the current book to the first book in the library
		if len(library.Audiobooks) > 0 {
			_ = SetBookmarkByIndex(0)
		}
	}
	return true
}

func SaveLibrary() error {
	fmt.Println("library base file path: ", library.BaseFilePath)
	// save the library to a json file
	data, err := json.MarshalIndent(library, "", "    ")
	if err != nil {
		return fmt.Errorf("error marshalling library to json: %w", err)
	}

	file, err := os.Create(GetLibraryFilePath())
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

func SetBookmarkByTitle(title string) error {
	index, book, err := GetBookByTitle(title)
	if err != nil {
		return err
	}
	global_bookmark = &Bookmark{index, book, false}
	return nil
}

func SetBookmarkByIndex(index int) error {
	if index < len(library.Audiobooks) {
		book := &library.Audiobooks[index]
		global_bookmark = &Bookmark{index, book, false}
		return nil
	} else {
		return fmt.Errorf("provided index is greater than libarary length %d", index)
	}
}

func SetBookmarkByBook(book *Audiobook) error {
	index := GetIndexByBook(book)
	if index == -1 {
		return fmt.Errorf("no index for book found: %s", book.Title)
	}
	global_bookmark = &Bookmark{index, book, false}
	return nil
}

func GetIndexByBook(book *Audiobook) int {
	for key, value := range library.Audiobooks {
		if value.Title == book.Title {
			return key
		}
	}
	return -1
}

func GetBookByTitle(title string) (int, *Audiobook, error) {
	for key, book := range library.Audiobooks {
		if book.Title == title {
			return key, &book, nil
		}
	}
	return -1, nil, fmt.Errorf("no book by that name: %s", title)
}

func GetFileIndexByName(book *Audiobook, name string) int {
	for i, file := range book.Files {
		if file.Filename == name {
			return i
		}
	}
	return -1
}

// add an audiobook to the library
func AddAudiobookToLibrary(book_path string) {
	prev_book := BookAlreadyInLibrary(book_path)
	if prev_book != nil {
		fmt.Println("Book already in library")
	}

	// create a directory to copy the book into
	directory := filepath.Join(library.BaseFilePath, filepath.Base(book_path))
	_, err := os.Stat(directory)
	if os.IsNotExist(err) {
		os.Mkdir(directory, 0755)
	}

	audiobook := Audiobook{}
	audiobook.library = &library
	audiobook.Title = filepath.Base(book_path)
	audiobook.Author = "Test Author"

	// get list of all audio files in the directory
	audioFiles, err := listAudioFilesInDirectory(book_path)
	if err != nil {
		fmt.Println("Error getting audio files")
		return
	}

	// copy the audio files to the directory if they are not already there
	for _, file := range audioFiles {
		_, err := os.Stat(filepath.Join(directory, file.Filename))
		if os.IsNotExist(err) {
			CopyFile(filepath.Join(book_path, file.Filename), filepath.Join(directory, file.Filename))
		}
	}

	// get list of all audio files in the directory
	audioFiles, err = listAudioFilesInDirectory(directory)
	if err != nil {
		fmt.Println("Error getting audio files")
		return
	}

	// get the length in seconds of the audiobook by adding up the length of all the audio files
	length := 0
	for key, file := range audioFiles {
		file.book = &audiobook
		duration, _ := getAudioFileDuration(file.GetFilePath())
		fmt.Println("Duration: ", duration)
		audioFiles[key].Length = time.Duration(duration) * time.Second
		length += int(duration)
	}
	audiobook.Files = audioFiles
	audiobook.TotalTime = time.Duration(length) * time.Second
	audiobook.CurrentTime = 0
	audiobook.CurrentFile = &audioFiles[0]
	audiobook.CurrentFileTime = 0

	if prev_book != nil {
		*prev_book = audiobook
	} else {
		library.AddAudiobook(audiobook)
		ensureReferences()
	}

	SaveLibrary()
}

func ensureReferences() {
	// setup references in the library
	for i := range library.Audiobooks {
		library.Audiobooks[i].library = &library
		for j := range library.Audiobooks[i].Files {
			library.Audiobooks[i].Files[j].book = &library.Audiobooks[i]
			if library.Audiobooks[i].CurrentFile.Filename == library.Audiobooks[i].Files[j].Filename {
				library.Audiobooks[i].CurrentFile = &library.Audiobooks[i].Files[j]
			}
		}
	}
}

func BookAlreadyInLibrary(book_path string) *Audiobook {
	for i, book := range library.Audiobooks {
		if book.GetBookPath() == book_path {
			return &library.Audiobooks[i]
		}
	}
	return nil
}

func CheckMissingAudioFiles() []MissingFile {
	var missingFiles []MissingFile

	for _, audiobook := range library.Audiobooks {
		for _, file := range audiobook.Files {
			if _, err := os.Stat(file.GetFilePath()); os.IsNotExist(err) {
				missingFiles = append(missingFiles, MissingFile{audiobook.Title, file.Filename})
			}
		}
	}
	return missingFiles
}
