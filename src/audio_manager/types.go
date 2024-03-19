package audio_manager

import (
	"os"
	"time"
)

// audiobook library type
type Audiobook struct {
	Title           string        `json:"title"`
	Author          string        `json:"author"`
	TotalTime       time.Duration `json:"total_time"`        // length of the audiobook in seconds
	CurrentTime     time.Duration `json:"current_time"`      // current position in the audiobook in seconds
	CurrentFile     int           `json:"current_file"`      // index of the current file being played
	CurrentFileTime time.Duration `json:"current_file_time"` // current position in the current file in seconds
	Path            string        `json:"path"`              // path to the directory containing the audiobook
	Files           []string      `json:"files"`             // list of file paths
}

type Library struct {
	Audiobooks []Audiobook `json:"audiobooks"`
}

func (l *Library) AddAudiobook(book Audiobook) {
	l.Audiobooks = append(l.Audiobooks, book)
}

type AudioFile struct {
	Path string
	Info os.FileInfo
}

type Bookmark struct {
	index int
	book  *Audiobook
}
