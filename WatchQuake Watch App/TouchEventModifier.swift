//
//  TouchEventModifier.swift
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 30.10.2022.
//

import Foundation
import SwiftUI

extension View {
    func onTouchDownAndPanGesture(callback: @escaping (CGPoint,Int) -> Void) -> some View {
        modifier(TouchDownAndPanEventModifier(callback: callback))
    }
}

struct TouchDownAndPanEventModifier: ViewModifier {
    @GestureState private var touchState = false
    @State private var tapped = false
    @State private var time = UInt64(0)
    let callback: (CGPoint,Int) -> Void
    func body(content: Content) -> some View {
        content
            .simultaneousGesture(
                DragGesture(minimumDistance: 0.0)
                    .updating($touchState) {(value, gestureState, transaction) in
                        gestureState = true
                    }
                    .onChanged { value in
                        if !self.tapped {
                            self.tapped = true
                            self.time = GetTimeNanoSeconds()// thread safe?
                        }                        
                        self.callback(value.location,1)// pan changed
                    }
                    .onEnded { value in
                        var called = false
                        if self.tapped {
                            var curTime = GetTimeNanoSeconds()// thread safe?
                            var delta = GetDeltaTime(curTime - self.time)
                            var diffX = value.startLocation.x - value.location.x
                            var diffY = value.startLocation.y - value.location.y
                            var distSq = diffX * diffX + diffY * diffY
                            if (delta < 0.2 && distSq < 4.0) {
                                self.callback(value.location,2)// tap and pan
                                called = true
                            }
                        }
                        if !called {
                            self.callback(value.location,0)// pan end
                        }
                        self.tapped = false
                    }
            ).onChange(of: touchState) { _ in
                // gesture cancelled
                if touchState == false && self.tapped {
                    self.callback(CGPoint(x: 0.0, y: 0.0),-1)// pan cancel
                    self.tapped = false
                }
            }
    }
}
