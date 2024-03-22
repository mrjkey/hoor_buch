package audio_manager

import (
	"fmt"
	"path/filepath"
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
	Files           []AudioFile   `json:"files"`             // list of audio files in the audiobook
	library         *Library
}

func (b *Audiobook) GetFileTimeAsPosition() int {
	format := GetFormat()
	return int(b.CurrentFileTime.Seconds()) * int(format.SampleRate)
}

func (b *Audiobook) SetFileTimeFromPosition(position int) {
	format := GetFormat()
	b.CurrentFileTime = time.Duration(position/int(format.SampleRate)) * time.Second
}

func (b *Audiobook) GetBookPath() string {
	if b == nil {
		fmt.Println("Audiobook is nil")
		panic("Audiobook is nil")
	}
	if b.library == nil {
		fmt.Println("Library is nil")
		panic("Library is nil")
	}
	fmt.Println("BaseFilePath: ", b.library.BaseFilePath)
	return filepath.Join(b.library.BaseFilePath, b.Title)
}

type Library struct {
	BaseFilePath string      `json:"base_file_path"`
	Audiobooks   []Audiobook `json:"audiobooks"`
}

func (l *Library) AddAudiobook(book Audiobook) {
	l.Audiobooks = append(l.Audiobooks, book)
}

type AudioFile struct {
	Filename string
	Length   time.Duration
	book     *Audiobook
}

func (f *AudioFile) GetFilePath() string {
	if f.book == nil {
		fmt.Println("Book is nil")
		panic("Book is nil")
	}
	return filepath.Join(f.book.GetBookPath(), f.Filename)
}

type Bookmark struct {
	index    int
	book     *Audiobook
	gui_init bool
}

type ServerLibrary struct {
	Audiobooks []ServerAudiobook `json:"audiobooks"`
	Clients    []ClientInfo      `json:"clients"`
}

type ClientInfo struct {
	ipv4 string
	ipv6 string
	id   string
}

type ServerAudiobook struct {
	Title     string `json:"title"`
	Author    string `json:"author"`
	TotalTime int    `json:"total_time"`
	Files     []struct {
		Filename string `json:"filename"`
		Length   int    `json:"length"`
	} `json:"files"`
	CurrentTime     int `json:"current_time"`
	CurrentFile     int `json:"current_file"`
	CurrentFileTime int `json:"current_file_time"`
	hostClientId    string
}
