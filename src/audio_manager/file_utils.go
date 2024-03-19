package audio_manager

import (
	"fmt"
	"os"
	"path/filepath"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/storage"
	"github.com/faiface/beep/mp3"
)

func getAudioFileDuration(filePath string) (float64, error) {
	file, err := os.Open(filePath)
	if err != nil {
		return 0, err
	}
	defer file.Close()

	if filepath.Ext(filePath) == ".mp3" {
		_, format, _ := mp3.Decode(file)
		return float64(format.SampleRate.N(time.Second / 10)), nil
	} else {
		return 0, fmt.Errorf("unsupported file type")
	}
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

func getProgressString(currentTime, totalTime time.Duration) string {
	return fmt.Sprintf("%s / %s", durationToHHMMSS(currentTime), durationToHHMMSS(totalTime))
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
