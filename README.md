# riftDriverWin
Small USB HID driver to activate Oculus Rift CV1 as extended display on Windows

NOTE: Will not work in case the Oculus software/drivers are already installed on the machine.

<b>How to compile the driver</b>
- Start Microsoft Visual Studio
- Select solution configuration "Release" for solution platform "x86"
- Click Build -> Build Solution

<b>Enable the Oculus Rift CV1 screen</b>
- In order to activate the Oculus Rift CV1 screen, just execute the riftDriverWin.exe (VC++ Redistributables required)
- The driver will enable the screen and send the keep-alive message cyclically

```shell
./Release/riftDriverWin.exe
```
- You should see that the orange LED on the Rift turns white (only if also HDMI signal with correct resolution and refresh rate is activated as well)
- You should see the extended desktop through the Rift (but not nice since left and right eye see different parts)
- Test stereo view by watching a side-by-side video in full-screen on YouTube
  (https://www.youtube.com/results?search_query=3D+side+by+side)

Hint: Hitting return in the terminal will stop the application.
