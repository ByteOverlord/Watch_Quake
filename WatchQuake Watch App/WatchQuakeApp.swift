//
//  WatchQuakeApp.swift
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

import SwiftUI
import AVFoundation
import UIKit

var playbackCancelled = false
var scheduleRunning = true

func OnDataNeeded(avPlayer: AVAudioPlayerNode, avBuffer: AVAudioPCMBuffer, bits: uint, type: Int32)
{
    var ch0 = UnsafeMutableRawPointer?(nil);
    var ch1 = UnsafeMutableRawPointer?(nil);
    if type == 0 {
        if bits == 32
        {
            ch0 = UnsafeMutableRawPointer(avBuffer.int32ChannelData![0])
            ch1 = UnsafeMutableRawPointer(avBuffer.int32ChannelData![1])
        }
        else if bits == 16
        {
            ch0 = UnsafeMutableRawPointer(avBuffer.int16ChannelData![0])
            ch1 = UnsafeMutableRawPointer(avBuffer.int16ChannelData![1])
        }
    }
    else if type == 1 {
        if bits == 32
        {
            ch0 = UnsafeMutableRawPointer(avBuffer.floatChannelData![0])
            ch1 = UnsafeMutableRawPointer(avBuffer.floatChannelData![1])
        }
    }
    WQAudioCallback([ch0,ch1],avBuffer.frameCapacity,avBuffer.format.channelCount,bits)
    if playbackCancelled == false {
        avPlayer.scheduleBuffer(avBuffer, at:nil, options: AVAudioPlayerNodeBufferOptions.interrupts, completionCallbackType: AVAudioPlayerNodeCompletionCallbackType.dataConsumed, completionHandler:{_ in
            OnDataNeeded(avPlayer: avPlayer, avBuffer: avBuffer, bits: bits, type: type)
        })
        scheduleRunning = true
    }
    else
    {
        scheduleRunning = false
    }
}

var gameMixerVolume = Float(0.1)//Float(0.25)

// https://stackoverflow.com/questions/57696436/swiftui-get-screen-size-in-multiplatform
class SGConvenience{
    #if os(watchOS)
    static var deviceScreenSize:CGSize = WKInterfaceDevice.current().screenBounds.size
    static var deviceScreenWidth:CGFloat = WKInterfaceDevice.current().screenBounds.width
    static var deviceScreenHeight:CGFloat = WKInterfaceDevice.current().screenBounds.height
    #elseif os(iOS)
    static var deviceScreenSize:CGSize = UIScreen.main.bounds.size
    static var deviceScreenWidth:CGFloat = UIScreen.main.bounds.width
    static var deviceScreenHeight:CGFloat = UIScreen.main.bounds.height
    #elseif os(macOS)
    static var deviceScreenSize:CGSize = NSScreen.main?.visibleFrame.size
    static var deviceScreenWidth:CGFloat = NSScreen.main?.visibleFrame.width
    static var deviceScreenHeight:CGFloat = NSScreen.main?.visibleFrame.height
    #endif
}

class NotificationWrapper {
    @State var headphonesConnected = false
    required init() {
        setupNotifications()
    }
    @objc func handleRouteChange(notification: Notification) {
        guard let userInfo = notification.userInfo,
            let reasonValue = userInfo[AVAudioSessionRouteChangeReasonKey] as? UInt,
            let reason = AVAudioSession.RouteChangeReason(rawValue: reasonValue) else {
                return
        }
        
        // Switch over the route change reason.
        switch reason {

        case .newDeviceAvailable: // New device found.
            let session = AVAudioSession.sharedInstance()
            var hadHeadhones = headphonesConnected
            headphonesConnected = hasHeadphones(in: session.currentRoute)
            if headphonesConnected && !hadHeadhones {
                print("headphones connected")
            }
        
        case .oldDeviceUnavailable: // Old device removed.
            if let previousRoute =
                userInfo[AVAudioSessionRouteChangePreviousRouteKey] as? AVAudioSessionRouteDescription {
                var hadHeadhones = headphonesConnected
                headphonesConnected = hasHeadphones(in: previousRoute)
                if !headphonesConnected && hadHeadhones {
                    print("headphones disconnected")
                }
            }
        
        default: ()
        }
    }
    func hasHeadphones(in routeDescription: AVAudioSessionRouteDescription) -> Bool {
        // Filter the outputs to only those with a port type of headphones.
        return !routeDescription.outputs.filter({$0.portType == .headphones}).isEmpty
    }
    func setupNotifications() {
        // Get the default notification center instance.
        let nc = NotificationCenter.default
        nc.addObserver(self,
                       selector: #selector(handleRouteChange),
                       name: AVAudioSession.routeChangeNotification,
                       object: nil)
    }
}

@main
struct WatchQuake_Watch_AppApp: App {
    @Environment(\.scenePhase) var scenePhase
    @State var scrollAmount = 0.0
    var audioEngine : AVAudioEngine? = nil
    var audioMixer : AVAudioMixerNode? = nil
    var audioPlayer : AVAudioPlayerNode? = nil
    var audioBuffer : AVAudioPCMBuffer? = nil
    var audioIsFloatType = Int32(0)
    var audioBits = uint(0)
    var audioType = Int32(0)
    var notificationHandler = NotificationWrapper()
    init() {
        let screenSize = SGConvenience.deviceScreenSize
        let sess = AVAudioSession.sharedInstance()
        try! sess.setCategory(AVAudioSession.Category.playAndRecord)
        do
        {
            //try sess.setPreferredSampleRate(44100.0)
        }
        try! sess.setActive(true)

        let pixelsPerDot = WKInterfaceDevice.current().screenScale
        WKApplication.shared().isAutorotating = true
        audioEngine = AVAudioEngine()
        audioMixer = audioEngine!.mainMixerNode
        let outFormat = audioEngine!.outputNode.outputFormat(forBus: 0)

        print("mixer")
        var bits = uint(0)
        var type = Int32(0)
        switch outFormat.commonFormat {
        case AVAudioCommonFormat.pcmFormatFloat64:
            print("64bit float")
            bits = uint(64)
            type = Int32(1)
        case AVAudioCommonFormat.pcmFormatFloat32:
            print("32bit float")
            bits = uint(32)
            type = Int32(1)
        case AVAudioCommonFormat.pcmFormatInt32:
            print("32bit int")
            bits = uint(32)
        case AVAudioCommonFormat.pcmFormatInt16:
            print("16bit int")
            bits = uint(16)
        default:
            print("unknown format")
        }
        audioBits = bits
        audioType = type
        
        print("samplerate Hz",outFormat.sampleRate)
        print("channels",outFormat.channelCount)
        print("interleaved",outFormat.isInterleaved)
        print("screen size",Int32(screenSize.width),Int32(screenSize.height))
        print("pixels per dot",pixelsPerDot)

        WQSetScreenSize(Int32(screenSize.width),Int32(screenSize.height),Float(pixelsPerDot))// hardware screen size in pixels
        WQSetAudioFormat(Int32(outFormat.sampleRate),bits,Int32(outFormat.channelCount),outFormat.isInterleaved ? 1 : 0,type)

        WQInit()
        WQSetLoop()

        let fbLength = AVAudioFrameCount(WQGetFrameBufferLength())
        audioPlayer = AVAudioPlayerNode()
        audioBuffer = AVAudioPCMBuffer(pcmFormat: outFormat, frameCapacity: fbLength)
        audioBuffer!.frameLength = fbLength
        audioEngine!.attach(audioPlayer!)
        audioEngine!.connect(audioPlayer!, to: audioMixer!, format: outFormat)
        print("stride",audioBuffer!.stride)
        print("buffersize",audioBuffer!.frameCapacity)

        try! audioEngine!.start()
        audioPlayer?.volume = gameMixerVolume
        audioPlayer?.play()
        OnDataNeeded(avPlayer: audioPlayer!, avBuffer: audioBuffer!, bits: uint(bits), type: type)
    }
    var body: some Scene {
        WindowGroup {
            ContentView()
                .onChange(of: scenePhase) { newPhase in
                    if newPhase == .active {
                        print("Active")
                        WQNotifyActive(1)
                        WQRequestState(1)
                        audioEngine!.attach(audioPlayer!)
                        let outFormat = audioEngine!.outputNode.outputFormat(forBus: 0)
                        audioEngine!.connect(audioPlayer!, to: audioMixer!, format: outFormat)
                        try! audioEngine?.start()
                        if audioPlayer?.isPlaying == false {
                            playbackCancelled = false
                            audioPlayer?.volume = gameMixerVolume
                            audioPlayer?.play()
                            if scheduleRunning == false {
                                print("audio callback scheduled")
                                OnDataNeeded(avPlayer: audioPlayer!, avBuffer: audioBuffer!, bits: uint(audioBits), type: audioType)
                            }
                        }
                    } else if newPhase == .inactive {
                        print("Inactive")
                        WQNotifyActive(0)
                        if audioPlayer?.isPlaying == true {
                            audioPlayer?.volume = 0.0
                            audioPlayer?.pause()
                            playbackCancelled = true
                        }
                        if audioEngine?.isRunning == true {
                            audioEngine?.pause()
                            audioEngine?.disconnectNodeInput(audioMixer!)
                            audioEngine?.detach(audioPlayer!)
                        }
                    } else if newPhase == .background {
                        print("Background")
                        WQRequestState(0)
                    }
                }
                .focusable().digitalCrownRotation(detent: $scrollAmount, from:-1.0, through:1.0, by:0.1, sensitivity: .low, isContinuous: true, isHapticFeedbackEnabled: false, onChange: { (event: DigitalCrownEvent) in
                //let s = Float($scrollAmount.wrappedValue)
                    WQInputCrownRotate(Float(0.0),Float(event.velocity))
                },onIdle: {
                //scrollAmount = 0.0
            }).onLongPressGesture(minimumDuration: 1.0, maximumDistance: 1, perform: {
                WQInputLongPress()
            }).onTouchDownAndPanGesture { (p:CGPoint, type:Int) in
                WQInputTapAndPan(p,Int32(type))
            }
        }
    }
}
