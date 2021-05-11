package main

import (
	"fmt"
	"time"

	zmq "github.com/alecthomas/gozmq"
)

func main() {
	context, _ := zmq.NewContext()
	socket, _ := context.NewSocket(zmq.REP)
	defer context.Close()
	defer socket.Close()
	socket.Bind("tcp://*:5555")

	for {
		msg, _ := socket.Recv(0)
		println("Received ", string(msg))
		time.Sleep(time.Second)
		reply := fmt.Sprintf("World")
		socket.Send([]byte(reply), 0)
	}
}
