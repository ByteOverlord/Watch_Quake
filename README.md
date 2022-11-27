![Logo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Watch_Quake_Logo_01.png)
18.11.2022

Watch_Quake is a Quake 1 port for Apple Watches that uses software rendering and has working audio playback. Runs shareware and registered versions of the game with optional music tracks.

<https://www.youtube.com/watch?v=89TAt72eYt4> (Series 5 gameplay video)

This port started from the original Quake Watch port by Tomas "MyOwnClone" Vymazal.
 
## About

We where intrigued by MyOwnClone’s quake_watch port and saw the potential of the Apple Watch. The controls needed be tweaked a bit and adjust the game to run in fullscreen. We envisioned a port that would feel closer to a commercial release but have the same menu structure and feel of the original 1996 Quake release. The initial spark was given for the project and ByteOverlord started refining the various aspects about this game port.

![Photo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/WQ_Photo_2.jpg)

## Changes by ByteOverlord

* Sound Mixer separated to a second thread from the drawing pipeline to give the Mixer more time for processing.
* “cd” audio support added. Audio file format “Mono, 22050 Hz, WAV, IMA ADPCM” preferred for lower processing and minimal space required.
* Timer changed from “ns_timer” to “dispatch_source” (Possibly more accurate).
* Main menu custom texts are drawn on the fly from the original texture so no extra textures needed (Maps, Cheats).
* Maps menu added with custom level screenshots and selectable difficulty.
* Cheats menu added with toggles that stay active in level changes.
* Saves game config (Options) and save games (.sav) in “Application Support” folder. Game config saves automatically when the app goes to background.
* Resolution setting is automatic with hud, menu and console notification placements affected by resolution.
* "Run Benchmark" mode with selectable Demo1, Demo2 and Demo3 added in the options menu.
* DefragAllocator and DefragCache. Replaces sound cache.

![Photo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/WQ_Photo_3.jpg)

## Features

* PAK0 and PAK1 loading
* Menu screen
* Save and load game
* Autosaving of options
* Map quick select screen
* Cheat activation screen
* Tweaked controls for watch interface
* Underwater audio effect
* Position and animation interpolation
* Music playback
* Benchmark mode
* Automatic native resolution (Series 4 40mm -> Ultra)

![Controls](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Menu_Controls.png)

![Controls](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/In_Game_Controls.png)

## Changelog

**27.11.2022</em>**  

* **<em>Benchmark section added</em>**
* **<em>Series 4 updated as supported in requirements</em>**
* **<em>DefragAllocator added in the project</em>**
* **<em>Sound fixes</em>**
* **<em>"Run Benchmark" added in the options menu</em>**
* **<em>Menu controls tweaked</em>**

19.11.2022  

* <em>SOUNDFIX! Fixed possible crash with audio cache</em>

## Game size 
 
Complete game with optional music takes ~ 99,4 MB on the watch.

```
PAK0.PAK is 18,7 MB
PAK1.PAK is 34,3 MB
Music Tracks are 39,3 MB (converted with Audacity (Mono, 22050 Hz, WAV, IMA ADPCM))
```


## Benchmark

#### Reference table:  

Good  | Moderate  | Bad
------------- | ------------- |-------------
≤ 5 ms  | 6-12 ms  | > 16 ms  |

#### Results: 

DEMO1  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 4.19  | 2.01  | 11.85  |
S4 44mm  | -  | -  | -  |
S5 40mm  | 4.17  | 2.12  | 11.08  |
S5 44mm  | 4.76  | 2.41  | 13.17  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | -  | -  | -  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

DEMO2  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 3.75  | 1.91  | 12.29  |
S4 44mm  | -  | -  | -  |
S5 40mm  | 3.72  | 1.80  | 10.67  |
S5 44mm  | 4.15  | 1.92  | 11.21  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | -  | -  | -  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

DEMO3  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 4.54  | 1.88  | 12.62  |
S4 44mm  | -  | -  | -  |
S5 40mm  | 4.37  | 1.77  | 12.84  |
S5 44mm  | 5.06  | 2.15  | 15.78  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | -  | -  | -  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

Benchmarks are only indicative.

## Known issues

* Audio will not transmit through Bluetooth
* Camera rotates suddenly on some occasions
* Smooth animation on, torches flicker on the edges of the view
* R_AliasDrawModel trivial_accept disabled (Incorrect bounding box?)
* Sound does not playback on app start (Put the game to the dock and open it from there again.)
* App does not start anymore (Build the app to the watch from Xcode. Force quit the app on the watch while Xcode is still running. Close Xcode and disconnect the iPhone from the computer. Start the app from the watch)

## Building Requirements

* Apple Watch Series 4 or later with watchOS 9.1 or later
* iPhone 8 or later with iOS 16 or later
* Xcode 14.1 or later with command-line tools
* Homebrew
* Innoextract
* bchunk (only for music)
* Audacity (only for music)


## Building the Watch_Quake

Install Xcode through Apple AppStore and install command-line tools when Xcode asks for it.

Download the Watch Quake from GitHub. (<https://github.com/ByteOverlord/Watch_Quake>)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00.png)

Get Quake through gog.com (if you just want to use the shareware jump to “Copy PAK...” and only use the shareware’s PAK0.PAK)

Download the quake installer from your gog.com profile:  
 `"gog.com Profile" -> Games -> Quake -> View Downloads -> Download Offline Backup Game Installers -> Quake`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_01_A.png)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_01.png)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_02.png)

Install Homebrew from terminal:   
`ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/homebrew/go/install)"`
	
Install innoextract from Homebrew:

```
brew update
brew install innoextract
```

In terminal extract the Quake game files with innoextract:   
`innoextract “downloaded setup_quake_the_offering_2.0.0.6.exe file location”`

(innoextract will extract an “app” and "tmp" folders in current working directory.)	
"app" folder has the needed PAK files and optional audio.

Copy PAK0.PAK and PAK1.PAK files to WatchQuake project (PAK names have to be uppercase (case sensitive)):   
`/WatchQuake Watch App/Resources/id1/`


Open the GitHub WatchQuake Xcode project.


In WatchQuake project file go to:  
`WatchQuake -> Targets -> WatchQuake -> Signing & Capabilities`

Select your "Team" and write a unique “Bundle Identifier”.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_03.png)
Then in WatchQuake project file go to:  
 `WatchQuake -> Targets -> WatchQuake Watch App -> Signing & Capabilities`

Select your "Team" and use the same “Bundle Identifier” as previously but with “.watchkitapp” added at the end.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_04.png)

If the project “Frameworks” folder shows the framework texts in red:


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_05A.png)
	
In WatchQuake project file go to:  
`WatchQuake -> Targets -> WatchQuake -> Build Phases -> Link Binary With Libraries`


Add these frameworks:

```
AVFoundation.framework
CoreGraphics.framework
Foundation.framework
SwiftUI.framework
UIKit.framework
```


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_05B.png)
And after adding them delete the frameworks with red text.

Check that the project is on release setting.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_06.png)


Test the build first with a simulator (Series 5 Simulator or up (watchOS Simulator))


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_06A.png)


After testing connect iPhone to the Mac and make sure Apple Watch is connected to it.

Set the iPhone and Apple Watch in to "Developer" mode.

On iPhone:  
`Settings -> Privacy & Security -> Developer Mode (SECURITY) -> Developer Mode`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_A.png)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_B.png)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_C.png)

On Apple Watch:  
`Settings -> Privacy & Security -> Developer Mode (SECURITY) -> Developer Mode`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_D.png)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_E.png)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_F.png)

After the devices have restarted.  

Select the Apple Watch as target.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_07.png)

From Xcode build the Watch_Quake to the watch.  


Transfer takes:  
5-20 min (iPhone X)  
<1 min (iPhone 12) 

## Adding music to the game (Optional)

Install bchunk from terminal:  
`brew install bchunk`


Extract game music from the gog game files:  
`bchunk -w “game.gog file location” “game.cue file location” track`


(Music tracks will extract in to current working directory (track02 - track11.wav).)


Open Audacity and drag the tracks to it. Edit the tracks one by one to Mono.  
`Tracks -> Mix -> Mix Stereo Down to Mono`


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_00.png)

Set the “Project Rate (Hz)” to 22050.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_01.png)


Export tracks with “Export Multiple” in “WAV” format with “IMA ADPCM” encoding.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_02.png)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_03.png)


Place the exported audio files (track02 - track11.wav) inside the Watch Quake project:   
`“WatchQuake Watch App/Resources/id1/music/”`


From Xcode build the Watch_Quake to the watch.  


Transfer takes:  
5-20 min (iPhone X)  
<1 min (iPhone 12)

![Photo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/WQ_Photo_1.jpg)

## Credits

quake\_watch <https://github.com/MyOwnClone/quake_watch>  
vkquake (Underwater sound filter, snd_dma init) <https://github.com/Novum/vkQuake>  
Quakespasm (Position and animation interpolation) <https://github.com/sezero/quakespasm>  
Quake\_For\_OSX <https://github.com/Izhido/Quake\_For\_OSX>  
Quake-iOS <https://github.com/tomkidd/Quake-iOS>  
Quake source code by Id Software. <https://github.com/id-Software/Quake>  
App icon, documentation and video by IdeaVoid


## License
This software is released under the terms of the GNU General Public License v2.
