# Open Timing Project

This project targets to provide an easy to use system for timing of parkour speedruns and any similar events.

## How it works.
- You need one server and one gate node.
- The server node creates a `OpenTimingProject` WiFi with a captive portal hosting simple website showing data a current and previsous run.
- After gate node is connected you can start a new run by pressing the server node.
- The gate node connects to the WiFi and at press stops active run.

## Node colors
- **Green** - Open, ready to start a new run.
- **Red** - Run in progress.
- **Orange center ring** - Gate node not connected to the server wifi.
- **Blue loading animation** - Gate node starting up.
- **Purple loading animation** - Server node starting up.