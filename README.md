# arduino-lamp-server
A simple http server for my train signal lamp

# What is it?
Arduino Lamp Server is a simple HTTP server that controls a relay board. It is designed to work with an Arduino Uno and a Seed Studio Relay Shield.

It listens for HTTP POST and GET requests.

# Features
- Turning on Red and Green Lamps
- Getting the status of the lamp relays
- HTML page to turn on and off the lamp

# Turning on the Lamp using POST
Make a POST request to the `on` resource of the Lamp colour on the IP address of the UNO:

`curl -X POST http://192.168.1.177/red/on` or `curl -X POST http://192.168.1.177/green/on`

The server will respond with a JSON payload, indicating that the lamp is now on:
```json
{
  "name":"red-main",
  "colour":"red",
  "state":"on"
}
```

# Turning off the Lamp using POST
Make a POST request to the `off` resource of the Lamp colour on the IP address of the UNO:

`curl -X POST http://192.168.1.177/red/off` or `curl -X POST http://192.168.1.177/green/off`

The server will respond with a JSON payload, indicating that the lamp is now off:
```json
{
  "name":"red-main",
  "colour":"red",
  "state":"off"
}
```


# Getting the status of the lamp using JSON
Make a GET request to the IP address of the UNO:

`curl -X GET http://192.168.1.177/status`

The server will respond with an array of objects, where each object represents the status of one controllable lamp:

```json
[
  {
    "name":"red-main",
    "colour":"red",
    "state":"off"
  },
  {
    "name":"green-main",
    "colour":"green",
    "state":"off"
  }
]
```
# Getting the status of the lamp using HTML
Make a GET request to the IP address of the UNO:

`curl -X GET http://192.168.1.177`

This will return a HTML page with the status of each lamp, and a button to either turn it on or off depending the state of the lamp

# Heritage
This Arduino sketch is based on the official [Ethernet Web Server demo](https://www.arduino.cc/en/Tutorial/LibraryExamples/WebServer)

