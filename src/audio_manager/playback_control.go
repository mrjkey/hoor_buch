package audio_manager

import (
	"fmt"
	"os"
	"time"

	"github.com/faiface/beep"
	"github.com/faiface/beep/effects"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
)

var (
	global_ctrl     *beep.Ctrl
	global_streamer beep.StreamSeekCloser
	global_format   beep.Format
	global_volume   *effects.Volume
	minDB           float64 = -60.0
	maxDB           float64 = 0.0
	starting_volume float64 = 70 // 70%
)

func GetCtrl() *beep.Ctrl {
	return global_ctrl
}

func GetStreamer() beep.StreamSeekCloser {
	return global_streamer
}

func GetFormat() beep.Format {
	return global_format
}

// Updated to handle transitioning between audio files
func SetupAudioPlayer(book *Audiobook) error {
	// currentBook = book // Set the current book
	return loadAndPlayCurrentFile()
}

// Load and play from the current file position
func loadAndPlayCurrentFile() error {
	book := GetBookmark().book
	if global_ctrl != nil {
		PauseAudio()
	}

	file, err := os.Open(book.CurrentFile.Path)
	if err != nil {
		return err
	}

	streamer, format, err := mp3.Decode(file)
	if err != nil {
		return err
	}
	global_streamer = streamer
	global_format = format

	err = speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))
	if err != nil {
		return err
	}

	global_ctrl = &beep.Ctrl{Streamer: beep.Seq(streamer, beep.Callback(func() {
		// Callback function to handle end of current file
		go handleEndOfFile()
	})), Paused: true}

	global_volume = &effects.Volume{
		Streamer: global_ctrl,
		Base:     2,
		Volume:   0,
		Silent:   false,
	}

	global_volume.Volume = SliderToVolume(70)
	fmt.Println("Current volume: ", global_volume.Volume)

	streamer.Seek(book.GetFileTimeAsPosition())
	speaker.Play(global_volume)

	return nil
}

func handleEndOfFile() {
	book := GetBookmark().book
	nextFileIndex := findNextFileIndex()
	if nextFileIndex < len(book.Files) {
		book.CurrentFile = &book.Files[nextFileIndex]
		book.CurrentFileTime = 0
		loadAndPlayCurrentFile() // Load and play the next file
	} else {
		// End of book reached, handle accordingly
		fmt.Println("End of book reached")
	}
}

// Find the index of the current file in the book's file list
func findNextFileIndex() int {
	book := GetBookmark().book
	for i, file := range book.Files {
		if file == *book.CurrentFile {
			return i + 1 // Return the index of the next file
		}
	}
	return len(book.Files) // Return length if current file not found, indicating end of book
}

// func PlayAudio() {
// 	ctrl := GetCtrl()
// 	book := GetBookmark().book
// 	// speaker.Lock()
// 	fmt.Println("Playing audio")
// 	fmt.Println("Seeking to position: ", book.CurrentFileTime)
// 	streamer := *GetStreamer()
// 	streamer.Seek(book.GetFileTimeAsPosition())
// 	// book.GetFileTimeAsInt())

// 	ctrl.Paused = false
// 	// speaker.Unlock()
// 	fmt.Println("end of play button function")
// }

func PlayAudio() {
	// global_volume.Volume -= 0.5
	fmt.Println("Current volume: ", global_volume.Volume)
	// Ensure audio setup is done before play
	if global_ctrl == nil {
		SetupAudioPlayer(GetBookmark().book)
	}
	global_ctrl.Paused = false
}

// func PauseAudio() {
// 	ctrl := GetCtrl()
// 	book := GetBookmark().book
// 	streamer := *GetStreamer()
// 	fmt.Println("Pausing audio")
// 	ctrl.Paused = true
// 	book.SetFileTimeFromPosition(streamer.Position())
// 	SaveLibrary()
// }

func PauseAudio() {
	if global_ctrl != nil {
		global_ctrl.Paused = true
		if global_streamer != nil {
			book := GetBookmark().book
			// position := global_streamer.Position()
			book.SetFileTimeFromPosition(global_streamer.Position())
			SaveLibrary()
		}
	}
}
