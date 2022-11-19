![Logo](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Watch_Quake_Logo_00.png)
18.11.2022

Quake 1 port for Apple Watches that uses software rendering and has mostly working audio playback. Runs shareware and registered versions of the game with optional “cd” audio.

<https://www.youtube.com/watch?v=89TAt72eYt4> (Series 5 gameplay video)

 This port started from the original Quake Watch port by Tomas "MyOwnClone" Vymazal.
 
Complete game with optional music takes 97,9 MB on the watch.

```
PAK0.PAK is 18,7 MB
PAK1.PAK is 34,3 MB
Music Tracks are 39,3 MB (converted with Audacity (22050 Hz, WAV, IMA ADPCM))

```

**Changes by ByteOverlord**

* Save and load game on watchOS
* Music playback ("cd" audio)
* Camera look and tweaked controls
* Autosaving options
* Map quick select and cheats screens
* Automatic native resolution on watches  

### Features

* PAK0 and PAK1 loading
* Menu screen
* Save and load game support
* Autosaving options
* Map quick select screen
* Cheat activation screen
* Tweaked controls for watch interface
* Config.cfg with settings saving support
* Underwater audio effect
* Position and animation interpolation support
* Music playback (tracks have to be converted to a specific WAV format)
* Automatic native resolution on watch models (Series 5 40mm -> Ultra)

![Controls](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Menu_Controls.png)

![Controls](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/In_Game_Controls.png)


### Known issues

* Audio will not transmit through Bluetooth
* Audio stutters occasionally
* Camera rotates suddenly on some occasions
* Smooth animation on, torches flicker on the edges of the view
* R_AliasDrawModel trivial_accept disabled (Incorrect bounding box?)
* Sound does not playback on app start (Put the game to the dock and open it from there again.)
* App does not start anymore (Build the app to the watch from Xcode. Force quit the app on the watch while Xcode is still running. Close Xcode and disconnect the iPhone from the computer. Start the app from the watch) 

## Building Requirements

* Apple Watch Series 5 or later with watchOS 9.1 or later
* iPhone 8 or later with iOS 16 or later
* Xcode 14.1 or later with command-line tools
* Homebrew
* Innoextract
* bchunk (only for music)
* Audacity (only for music)


## Building the Watch Quake

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

Install Xcode through Apple AppStore and install command-line tools when Xcode asks for it.

  Download the Watch Quake from GitHub. (<https://github.com/ByteOverlord/Watch_Quake>)

 ![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_00.png)

 Get Quake through gog.com (if you just want to use the shareware jump to “Copy PAK...” and only use the shareware’s PAK0.PAK)

 Download the quake installer from your gog.com profile:  
 `"gog.com Profile" -> Games -> Quake -> View Downloads -> Download Offline Backup Game Installers -> Quake`

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_01.png) ![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_02.png)

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


  After testing connect iPhone to the Mac and make sure Apple Watch is connected to it. Select the Apple Watch as target.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Game_Install_Guide_07.png)

Build it to the watch. (Depending on the game files that were added. Transfer might take from 5-20 min) 

## Adding music to the game (Optional)

Install bchunk from terminal:  
`brew install bchunk`


Extract game music from the gog game files:  
`bchunk -w “game.gog file location” “game.cue file location” track`


(Music tracks will extract in to current working directory (track02 - track11.wav).)


Open Audacity and drag the tracks in it. Edit tracks one by one by selecting one and from the “Tracks” -> “Mix” apply “Mix Stereo Down to Mono” (recommended “Project Rate (Hz)” is 22050).


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_00.png)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_01.png)


Export tracks with “Export Multiple” in “WAV” format with “IMA ADPCM” encoding.


![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_02.png)

![Screenshot](https://github.com/ByteOverlord/Watch_Quake/raw/main/README_images/Music_Install_Guide_03.png)


Place the exported audio files (track02 - track11.wav) inside the Watch Quake project:   
`“WatchQuake Watch App/Resources/id1/music/”`


From Xcode build the Watch_Quake to the watch. (Transfer might take from 5-20 min)

## Credits

quake\_watch <https://github.com/MyOwnClone/quake_watch>  
vkquake (Underwater sound filter. snd_dma init) <https://github.com/Novum/vkQuake>  
Quakespasm (Animation and movement interpolation.) <https://github.com/sezero/quakespasm>  
Quake\_For\_OSX <https://github.com/Izhido/Quake\_For\_OSX>  
Quake-iOS <https://github.com/tomkidd/Quake-iOS>  
Quake source code by Id Software. <https://github.com/id-Software/Quake>  
App icon, documentation and video by IdeaVoid


## License
This software is released under the terms of the GNU General Public License v2.
