package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"strconv"
	"strings"
	"time"
)

const MAX = 1000

func getCurrentRunningProcess(selfPID int) {
	const path = "/proc"

	// Open the /proc directory
	folder, err := os.Open(path)
	if err != nil {
		fmt.Println("Unable to read directory:", err)
		return
	}
	defer folder.Close()

	// Read the directory entries
	entries, err := folder.Readdirnames(0)
	if err != nil {
		fmt.Println("Unable to read directory entries:", err)
		return
	}

	var PIDS []int

	// Filter out non-process entries and convert names to integers
	for _, entry := range entries {
		if pid, err := strconv.Atoi(entry); err == nil && pid != selfPID {
			PIDS = append(PIDS, pid)
		}
	}

	for _, pid := range PIDS {
		filename := fmt.Sprintf("/proc/%d/stat", pid)
		data, err := ioutil.ReadFile(filename)
		if err != nil {
			fmt.Println("Unable to read file:", err)
			continue
		}

		// Split the content of the file to get the required information
		fields := strings.Fields(string(data))
		if len(fields) < 4 {
			continue
		}

		comm := fields[1]
		state := fields[2]
		ppid := fields[3]

		if state == "R" {
			fmt.Println("               ", pid)
			fmt.Println("*******************************")
			fmt.Printf("Process ID = %d\n", pid)
			fmt.Printf("COMM = %s\n", comm)
			fmt.Printf("State = %s\n", state)
			fmt.Printf("Parent PID = %s\n", ppid)
			fmt.Println("*******************************")
		}
	}
}

func main() {
	// Get the PID of the running program
	selfPID := os.Getpid()

	for {
		time.Sleep(1 * time.Second)
		getCurrentRunningProcess(selfPID)
	}
}

