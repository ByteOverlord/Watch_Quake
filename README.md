![Logo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Watch_Quake_Logo_01.png)
18.11.2022

Watch_Quake is a Quake 1 port for Apple Watches that uses software rendering and has working audio playback. Runs shareware and registered versions of the game with optional music tracks.

<https://www.youtube.com/watch?v=89TAt72eYt4> (Series 5 gameplay video)

This port started from the original Quake Watch port by Tomas "MyOwnClone" Vymazal.

## Contents

[**About**](#about)  
[**Changes by ByteOverlord**](#changes)  
[**Features**](#features)  
[**Changelog**](#changelog)  
[**Game Size**](#size)  
[**Benchmark**](#benchmark)  
[**Known Issues**](#issues)  
[**Building Requirements**](#requirements)  
[**Building the Watch_Quake**](#building)  
[**Adding music to Watch_Quake (Optional)**](#music)  
[**Credits**](#credits)  
[**License**](#license)  

## <a name="about"></a>About

We where intrigued by MyOwnClone’s quake_watch port and saw the potential of the Apple Watch. The controls needed be tweaked a bit and adjust the game to run in fullscreen. We envisioned a port that would feel closer to a commercial release but have the same menu structure and feel of the original 1996 Quake release. The initial spark was given for the project and ByteOverlord started refining the various aspects about this game port.

![Photo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/WQ_Photo_2.jpg)

## <a name="changes"></a>Changes by ByteOverlord

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

## <a name="features"></a>Features

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

![Controls](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Menu_Controls.jpg)

![Controls](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/In_Game_Controls.jpg)

## <a name="changelog"></a>Changelog

**04.12.2022**  

* **<em>Cameralook fix (Better input handling)</em>**
* **<em>Sound fix (Stabilized performance on newer watches (tested on SE 2nd Gen))</em>**

<em>27.11.2022</em> 

* <em>Benchmark section added</em>
* <em>Series 4 updated as supported in requirements</em>
* <em>DefragAllocator added in the project</em>
* <em>Sound fixes</em>
* <em>"Run Benchmark" added in the options menu</em>
* <em>Menu controls tweaked</em>

<em>19.11.2022</em>   

* <em>Sound fix (Fixed possible crash with audio cache)</em>

## <a name="size"></a>Game size 
 
Complete game with optional music takes ~ 99,4 MB on the watch.

```
PAK0.PAK is 18,7 MB
PAK1.PAK is 34,3 MB
Music Tracks are 39,3 MB (converted with Audacity (Mono, 22050 Hz, WAV, IMA ADPCM))
```

## <a name="benchmark"></a>Benchmark

#### Reference table:  

Good  | Moderate  | Bad
------------- | ------------- |-------------
≤ 5 ms  | 6-12 ms  | > 16 ms  |

#### Results: 

DEMO1  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 5.94  | 2.63  | 13.41  |
S4 44mm  | -  | -  | -  |
S5 40mm  | 5.32  | 2.44  | 13.84  |
S5 44mm  | 6.13  | 2.71  | 13.96  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | 7.35  | 2.61  | 14.98  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

DEMO2  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 5.50  | 1.95  | 14.79  |
S4 44mm  | -  | -  | -  |
S5 40mm  | 5.19  | 2.12  | 13.43  |
S5 44mm  | 5.94  | 2.51  | 15.36  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | 6.77  | 2.12  | 15.20  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

DEMO3  | Avg  | Low  | High  |
-------- | ------------- | ------------- | ------------- |
S4 40mm  | 6.21  | 1.99  | 14.21  |
S4 44mm  | -  | -  | -  |
S5 40mm  | 5.38  | 2.07  | 13.75  |
S5 44mm  | 6.41  | 2.78  | 14.01  |
SE (1st Gen) 40mm  | -  | -  | -  |
SE (1st Gen) 44mm  | -  | -  | -  |
S6 40mm  | -  | -  | -  |
S6 44mm  | -  | -  | -  |
S7 41mm  | -  | -  | -  |
S7 45mm  | -  | -  | -  |
SE (2nd Gen) 40mm  | -  | -  | -  |
SE (2nd Gen) 44mm  | 7.47  | 2.06  | 14.59  |
S8 41mm  | -  | -  | -  |
S8 45mm  | -  | -  | -  |
Ultra    | -  | -  | -  |

<em>Benchmarks are only indicative.</em> 

## <a name="issues"></a>Known Issues

* Audio will not transmit through Bluetooth
* Shareware version (using only PAK0) softlocks when going through a episode 2-4 portals
* Smooth animation on, torches flicker on the edges of the view
* R_AliasDrawModel trivial_accept disabled (Incorrect bounding box?)
* Sound does not playback on app start (Put the game to the dock and open it from there again.)
* App does not start anymore (Build the app to the watch from Xcode. Force quit the app on the watch while Xcode is still running. Close Xcode and disconnect the iPhone from the computer. Start the app from the watch)

## <a name="requirements"></a>Building Requirements

* Apple Watch Series 4 or later with watchOS 9.1 or later
* iPhone 8 or later with iOS 16 or later
* Mac with macOS 12.5 or later
* Xcode 14.1 or later with command-line tools
* Homebrew
* Innoextract
* bchunk (only for music)
* Audacity (only for music)

## <a name="building"></a>Building the Watch_Quake

1) Install Xcode through Apple AppStore and install command-line tools when Xcode asks for it.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_0.jpg)

2) Download the Watch Quake from GitHub. (<https://github.com/ByteOverlord/Watch_Quake>)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00.jpg)

3) Get Quake through gog.com (if you just want to use the shareware jump to step “8. Copy PAK...” and only use the shareware’s PAK0.PAK)

4) Download the quake installer from your gog.com profile:  
 `"gog.com Profile" -> Games -> Quake -> View Downloads -> Download Offline Backup Game Installers -> Quake`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_01_A.jpg)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_01.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_02.jpg)

5) Install Homebrew from terminal:   
`ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/homebrew/go/install)"`
	
6) Install innoextract from Homebrew:

```
brew update
brew install innoextract
```

7) In terminal extract the Quake game files with innoextract:   
`innoextract “downloaded setup_quake_the_offering_2.0.0.6.exe file location”`

(innoextract will extract an “app” and "tmp" folders in current working directory.)	
"app" folder has the needed PAK files and optional audio.

8) Copy PAK0.PAK and PAK1.PAK files to WatchQuake project (PAK names have to be uppercase (case sensitive)):   
`/WatchQuake Watch App/Resources/id1/`

9) Open the GitHub WatchQuake Xcode project.

10) In WatchQuake project file go to:  
`WatchQuake -> Targets -> WatchQuake -> Signing & Capabilities`

11) Select your "Team" and write a unique “Bundle Identifier”.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_03.jpg)

12) Then in WatchQuake project file go to:  
 `WatchQuake -> Targets -> WatchQuake Watch App -> Signing & Capabilities`

13) Select your "Team" and use the same “Bundle Identifier” as previously but with “.watchkitapp” added at the end.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_04.jpg)

If the project “Frameworks” folder shows the framework texts in red continue with step 14.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_05A.jpg)

If the project “Frameworks” folder shows the framework texts in white jump to step 17.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_05AB.jpg)
	
14) In WatchQuake project file go to:  
`WatchQuake -> Targets -> WatchQuake -> Build Phases -> Link Binary With Libraries`

15) Add these frameworks:

```
AVFoundation.framework
CoreGraphics.framework
Foundation.framework
SwiftUI.framework
UIKit.framework
```

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_05B.jpg)

16)  And after adding them delete the frameworks with red text.

17) Check that the project is on release setting.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_06.jpg)

18) Test the build first with a simulator (Series 5 Simulator or up (watchOS Simulator))

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_06A.jpg)

19) After testing connect iPhone to the Mac with a cable and make sure Apple Watch has a connection to iPhone.

20) Set the iPhone and Apple Watch in to "Developer" mode.

On iPhone:  
`Settings -> Privacy & Security -> Developer Mode (SECURITY) -> Developer Mode`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_A.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_B.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_C.jpg)

On Apple Watch:  
`Settings -> Privacy & Security -> Developer Mode (SECURITY) -> Developer Mode`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_D.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_E.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00_F.jpg)

After the devices have restarted.  

21) Select the Apple Watch as target.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_07.jpg)

22) From Xcode build the Watch_Quake to the watch.  

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_08.jpg)

Transfer times:  
<1 min (iPhone X & S4 40mm)  
5-20 min (iPhone X & S5 44mm)  
<1 min (iPhone 12 mini & S5 40mm)  
<1 min (iPhone 12 mini & SE 2nd Gen 44mm)

## <a name="music"></a>Adding music to Watch_Quake (Optional)

1) Install bchunk from terminal:  
`brew install bchunk`

2) Extract game music from the gog game files:  
`bchunk -w “game.gog file location” “game.cue file location” track`

3) (Music tracks will extract in to current working directory (track02 - track11.wav).)

4) Open Audacity and drag the tracks to it. Edit the tracks one by one to Mono.  
`Tracks -> Mix -> Mix Stereo Down to Mono`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_00.jpg)

5) Set the “Project Rate (Hz)” to 22050.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_01.jpg)

6) Export tracks with “Export Multiple” in “WAV” format with “IMA ADPCM” encoding.

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_02.jpg)
  
![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_03.jpg)

7) Place the exported audio files (track02 - track11.wav) inside the Watch Quake project:   
`“WatchQuake Watch App/Resources/id1/music/”`

8) From Xcode build the Watch_Quake to the watch.  

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_08.jpg)

Transfer times:  
<1 min (iPhone X & S4 40mm)  
5-20 min (iPhone X & S5 44mm)  
<1 min (iPhone 12 mini & S5 40mm)  
<1 min (iPhone 12 mini & SE 2nd Gen 44mm)

![Photo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/WQ_Photo_1.jpg)

## <a name="credits"></a>Credits

quake\_watch <https://github.com/MyOwnClone/quake_watch>  
vkquake (Underwater sound filter, snd_dma init) <https://github.com/Novum/vkQuake>  
Quakespasm (Position and animation interpolation) <https://github.com/sezero/quakespasm>  
Quake\_For\_OSX <https://github.com/Izhido/Quake\_For\_OSX>  
Quake-iOS <https://github.com/tomkidd/Quake-iOS>  
Quake source code by Id Software. <https://github.com/id-Software/Quake>  
App icon, documentation and video by IdeaVoid

## <a name="license"></a>License
This software is released under the terms of the GNU General Public License v2.
