package main

import (
	"bytes"
	"encoding/json"
	"log"
	"net/http"
	"serverapp/shared"
)

type ClientInfo struct {
	Name     string             `json:"name"`
	Library  []shared.Audiobook `json:"library"`
	IPv6Addr string             `json:"ipv6_addr"`
}

// Assuming Audiobook struct is defined similarly to the server

func main() {
	clientInfo := ClientInfo{
		Name:     "TestClient",
		Library:  []shared.Audiobook{}, // Populate as needed
		IPv6Addr: "::1",
	}

	requestBody, err := json.Marshal(clientInfo)
	if err != nil {
		log.Fatalf("Error marshaling JSON: %v", err)
	}

	resp, err := http.Post("http://localhost:6970/register", "application/json", bytes.NewBuffer(requestBody))
	if err != nil {
		log.Fatalf("Error making POST request: %v", err)
	}
	defer resp.Body.Close()

	// Read and print the response body
	// ...

	log.Println("Client registration successful")
}
