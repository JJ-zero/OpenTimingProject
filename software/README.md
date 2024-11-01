# Software
> Disclaimer: This whole project and the code especially is a work in progress and was created in a short time. I'm not fluent in cpp and most of this goes against the best practices. I'm aware and happy to aprove your PRs.

## Web app
OTP uses simple website that is served automatically after connecting to WiFi as a captive portal. You can found the non-minimalized version [here](./OpenTimingProject.html). The site has two main routines:
- Server checking that gets data from the server node and updates the site.
- Timer update that keeps time running for the user. 

The reason for *faking* the times is an attempt at optimalization and keeping server node free to recieve the more important requests from gates. When connected the site can detect new run and add show the history of previous runs. 

## API reference

### `/app/records`
Returns a JSON containing ID of current run, time since start of the run and time of the previous run. Times are in miliseconds.
#### Example:
This example is for run 12, that started 4.455 seconds ago and the previous run was 7.5 seconds long.
```json
{
    "current": 4455,
    "id": 12,
    "previous": 7500
}
```

### `/api/gate`
Ends the current run and returns simple text. Intended to be called by gates. Idealy should be a POST request in the future and accept the gate ID.

### `/api/state`
Returns the current state of the run.
- If run is in progress, returns status 202 and text `Running`.
- If run is not in progress, returns status 200 and text `None`.
This endpoint should be super light and fast to be called by gates. Gates use the status code to determine the state of the run.
