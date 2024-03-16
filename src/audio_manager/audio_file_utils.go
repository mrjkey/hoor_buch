package audio_manager

import (
	"fmt"
	"os"
	"path/filepath"
	"time"

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
