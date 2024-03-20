package audio_manager

import (
	"fmt"
	"time"
)

// audiobook library type
type Audiobook struct {
	Title           string        `json:"title"`
	Author          string        `json:"author"`
	TotalTime       time.Duration `json:"total_time"`        // length of the audiobook in seconds
	CurrentTime     time.Duration `json:"current_time"`      // current position in the audiobook in seconds
	CurrentFile     *AudioFile    `json:"current_file"`      // index of the current file being played
	CurrentFileTime time.Duration `json:"current_file_time"` // current position in the current file in seconds
	Path            string        `json:"path"`              // path to the directory containing the audiobook
	Files           []AudioFile   `json:"files"`             // list of audio files in the audiobook
}

func (b *Audiobook) GetFileTimeAsPosition() int {
	format := GetFormat()
	return int(b.CurrentFileTime.Seconds()) * int(format.SampleRate)
}

func (b *Audiobook) SetFileTimeFromPosition(position int) {
	format := GetFormat()
	b.CurrentFileTime = time.Duration(position/int(format.SampleRate)) * time.Second
	fmt.Println("Setting file time: ", b.CurrentFileTime)
}

type Library struct {
	Audiobooks []Audiobook `json:"audiobooks"`
}

func (l *Library) AddAudiobook(book Audiobook) {
	l.Audiobooks = append(l.Audiobooks, book)
}

type AudioFile struct {
	Path   string
	Length time.Duration
}

type Bookmark struct {
	index int
	book  *Audiobook
}
