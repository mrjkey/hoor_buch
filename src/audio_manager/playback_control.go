package audio_manager

import (
	"fmt"
	"os"
	"time"

	"github.com/faiface/beep"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
)

var global_ctrl *beep.Ctrl
var global_streamer *beep.StreamSeekCloser

func GetCtrl() *beep.Ctrl {
	return global_ctrl
}

func GetStreamer() *beep.StreamSeekCloser {
	return global_streamer
}

func SetupAudioPlayer(book *Audiobook) error {
	if global_ctrl != nil {
		PauseAudio()
	}
	// open an audio file
	file, err := os.Open(book.CurrentFile)
	if err != nil {
		return err
	}

	// decode the audio file
	streamer, format, err := mp3.Decode(file)
	if err != nil {
		return err
	}
	global_streamer = &streamer

	// initialize the speaker
	err = speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))
	if err != nil {
		return err
	}

	global_ctrl = &beep.Ctrl{Streamer: beep.Loop(-1, streamer), Paused: true}

	fmt.Println(streamer.Position())

	// init book position
	fmt.Println("Seeking to position: ", book.CurrentFileTime)
	streamer.Seek(book.GetFileTimeAsInt())

	speaker.Play(global_ctrl)
	return nil
}

func PlayAudio() {
	ctrl := GetCtrl()
	book := GetBookmark().book
	// speaker.Lock()
	fmt.Println("Playing audio")
	fmt.Println("Seeking to position: ", book.CurrentFileTime)
	streamer := *GetStreamer()
	streamer.Seek(book.GetFileTimeAsInt())
	// book.GetFileTimeAsInt())

	ctrl.Paused = false
	// speaker.Unlock()
	fmt.Println("end of play button function")
}

func PauseAudio() {
	ctrl := GetCtrl()
	book := GetBookmark().book
	streamer := *GetStreamer()
	fmt.Println("Pausing audio")
	ctrl.Paused = true
	book.SetFileTimeFromInt(streamer.Position())
	fmt.Println("end of pause button function")
	fmt.Println("current file time: ", book.CurrentFileTime)
	SaveLibrary()
}
