package main

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
	"serverapp/shared"
)

type ClientInfo struct {
	Name     string             `json:"name"`
	Library  []shared.Audiobook `json:"library"`
	IPv6Addr string             `json:"ipv6_addr"`
}

var clients []ClientInfo

func registerClient(w http.ResponseWriter, r *http.Request) {
	if r.Method != "POST" {
		http.Error(w, "Only POST method is allowed", http.StatusMethodNotAllowed)
		return
	}
	// print request body
	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Fatal(err)
	}
	log.Println(string(body))

	var newClient ClientInfo
	if err := json.NewDecoder(r.Body).Decode(&newClient); err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	// Add to clients list
	clients = append(clients, newClient)

	w.WriteHeader(http.StatusCreated)
	json.NewEncoder(w).Encode(newClient)
}

func main() {
	http.HandleFunc("/register", registerClient)
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		// Handle requests
	})

	log.Println("Starting server on :6970")
	log.Fatal(http.ListenAndServe(":6970", nil))
}
