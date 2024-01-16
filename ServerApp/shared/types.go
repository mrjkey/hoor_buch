package shared

type Audiobook struct {
	Path               string             `json:"path"`
	Title              string             `json:"title"`
	Author             string             `json:"author"`
	Progress           float64            `json:"progress"`
	Duration           float64            `json:"duration"`
	Files              []string           `json:"files"`
	LastPlayedFile     string             `json:"last_played_file"`
	LastPlayedPosition int                `json:"last_played_position"`
	FileDurations      map[string]float64 `json:"file_durations"`
}
