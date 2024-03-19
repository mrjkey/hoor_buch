package audio_manager

import (
	"fmt"
	"os"
	"time"
)

// audiobook library type
type Audiobook struct {
	Title           string        `json:"title"`
	Author          string        `json:"author"`
	TotalTime       time.Duration `json:"total_time"`        // length of the audiobook in seconds
	CurrentTime     time.Duration `json:"current_time"`      // current position in the audiobook in seconds
	CurrentFile     string        `json:"current_file"`      // index of the current file being played
	CurrentFileTime time.Duration `json:"current_file_time"` // current position in the current file in seconds
	Path            string        `json:"path"`              // path to the directory containing the audiobook
	Files           []string      `json:"files"`             // list of file paths
}

func (b *Audiobook) GetFileTimeAsInt() int {
	return int(b.CurrentFileTime.Seconds())
}

func (b *Audiobook) SetFileTimeFromInt(length int) {
	b.CurrentFileTime = time.Duration(length) * time.Second
	fmt.Println("Setting file time: ", b.CurrentFileTime)
	fmt.Println("Setting file time as int: ", length)
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
