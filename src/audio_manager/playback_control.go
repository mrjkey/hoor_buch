package audio_manager

import (
	"os"
	"time"

	"github.com/faiface/beep"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
)

func SetupAudioPlayer(filename string) (*beep.Ctrl, error) {
	// open an audio file
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	// defer file.Close()

	// decode the audio file
	streamer, format, err := mp3.Decode(file)
	if err != nil {
		return nil, err
	}
	// defer streamer.Close()

	// initialize the speaker	// initialize the speaker
	err = speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))
	if err != nil {
		return nil, err
	}

	ctrl := &beep.Ctrl{Streamer: beep.Loop(-1, streamer), Paused: true}
	speaker.Play(ctrl)
	return ctrl, nil
}
