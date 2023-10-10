//
//  ContentView.swift
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

import SwiftUI
import UIKit

class Model : ObservableObject {
    @Published var mapTxt : String!
    @Published var txt = String("")
    @Published var benchmarkTxt = String("")
    @Published var img : CGImage!
    init() {
        mapTxt = nil
        img = nil
        txt = String("")
        benchmarkTxt = String("")
    }
}

let myModel = Model()

@_cdecl("refresh_screen")
func refreshScreen(ptr: UnsafeRawPointer) -> () {
    myModel.img = WQCreateGameImage()!.takeRetainedValue()
    myModel.benchmarkTxt = String(cString: WQGetBenchmarkString())
    if (WQShowFPS() != 0)
    {
        myModel.txt = String(cString: WQGetStatsString())
    }
    else
    {
        myModel.txt = String("")
    }
}

@_cdecl("refresh_mapselect")
func refreshMapselect(ptr: UnsafeRawPointer) -> () {
    let mapName = WQGetSelectedMapName()
    if mapName == nil {
        myModel.mapTxt = nil
    }
    else {
        myModel.mapTxt = String(cString: mapName!)
    }
}

struct ImageOverlay: View {
    @ObservedObject var model: Model
    init() {
        model = myModel
    }
    var body: some View {
        // stuttering audio when text is visible for the first time?
        Text(model.txt).font(.system(size: 6)).fixedSize().padding(Edge.Set(Edge.top), Double(-8.0))
        Text(model.benchmarkTxt).font(.system(size: 6)).fixedSize().padding(Edge.Set(Edge.top), Double(8.0))
    }
}

struct MapsOverlay: View {
    @ObservedObject var model: Model
    init() {
        model = myModel
    }
    var body: some View {
        if model.mapTxt != nil
        {
            let stats = WQGetStats()
            let gif = UIImage(named: String("mapselect/") + model.mapTxt + String(".gif"), in: Bundle.main, with: nil)
            if gif != nil {
                let scaleX = 256.0 / Double(stats.devPixelsPerDot)
                let scaleY = 246.0 / Double(stats.devPixelsPerDot)
                let offsetY = Double(96.0 / stats.devPixelsPerDot)
                Image.init(uiImage: gif!).resizable().scaledToFit().frame(width: scaleX, height: scaleY).padding(Edge.Set(Edge.top), offsetY)
            }
        }
    }
}

struct NullView: View {
    init() {
    }
    var body: some View {
        Text("").frame(width:0, height: 0)//.hidden()
    }
}

struct ContentView: View {
    @State var isTapped = false
    @ObservedObject var model: Model
    init()
    {
        model = myModel
    }
    var body: some View {
        let stats = WQGetStats()
        //let scaleX = (Float(stats.width) / Float(stats.devWidth))
        //let scaleY = (Float(stats.height) / Float(stats.devHeight))
        let scale = stats.devPixelsPerDot//scaleY > scaleX ? scaleY : scaleX
        if (model.img != nil)
        {
            //let offset = CGSize(width: 0.0, height: 128.0)
            Image.init(model.img, scale: Double(scale), label: Text("Game"))
                //.scaledToFit()
                .overlay(MapsOverlay(),alignment: .top)
                .overlay(ImageOverlay(), alignment: .top)
        }
    }
}
