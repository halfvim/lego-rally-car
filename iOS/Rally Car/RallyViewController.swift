import UIKit
import Alamofire
import Socket

class RallyViewController: UIViewController {
    @IBOutlet weak var speedDot: UIView!
    @IBOutlet weak var orientDot: UIView!
    
    let socket = try! Socket.create(family: .inet, type: .datagram, proto: .udp)
    let address = Socket.createAddress(for: "192.168.4.1", on: 10002)!
    
    var lastSent = Date().timeIntervalSince1970
    
    func sendRequest(_ cmd: String) {
        if Date().timeIntervalSince1970 - lastSent > 0.02 || cmd == "B" || cmd == "W0" || cmd == "S0" || cmd == "W0" || cmd == "S0" {
            try! socket.write(from: cmd, to: address)
            lastSent = Date().timeIntervalSince1970
        }
    }
    
    @IBAction func lightChanged(_ sender: UISwitch) {
        if sender.isOn {
            sendRequest("H")
        } else {
            sendRequest("L")
        }
    }
    
    @IBAction func breakTouched(_ sender: UIButton) {
        sendRequest("B")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        speedDot.isUserInteractionEnabled = true
        speedDot.addGestureRecognizer(UIPanGestureRecognizer(target: self, action: #selector(speedPan(gesture:))))
        
        orientDot.isUserInteractionEnabled = true
        orientDot.addGestureRecognizer(UIPanGestureRecognizer(target: self, action: #selector(orientPan(gesture:))))
    }
    
    @objc func sendSpeedRequest(speed: Int) {
        print(speed)
        if speed > 0 {
            sendRequest("W\(speed)")
        } else {
            sendRequest("S\(-speed)")
        }
    }
    
    @objc func sendOrientRequest(orient: Int) {
        print(orient)
        if orient < 0 {
            sendRequest("A\(-orient)")
        } else {
            sendRequest("D\(orient)")
        }
    }
    
    @objc func setOrient(orient: Int) {
        sendOrientRequest(orient: orient)
    }
    
    @objc func resetOrient() {
        sendOrientRequest(orient: 0)
    }
    
    @objc func setSpeed(speed: Int) {
        sendSpeedRequest(speed: speed)
    }
    
    @objc func resetSpeed() {
        sendSpeedRequest(speed: 0)
    }
    
    @objc func orientPan(gesture: UIPanGestureRecognizer) {
        if gesture.state == .changed {
            let translation = gesture.translation(in: speedDot)
            var orient = translation.x / 130
            if orient > 1 {
                orient = 1
            }
            if orient < -1 {
                orient = -1
            }
            orient = orient * 255
            setOrient(orient: Int(orient))
        }
        
        if gesture.state == .ended {
            setOrient(orient: 0)
            Timer.scheduledTimer(timeInterval: 0.02, target: self,   selector: #selector(resetOrient), userInfo: nil, repeats: false)
            Timer.scheduledTimer(timeInterval: 0.5, target: self,   selector: #selector(resetOrient), userInfo: nil, repeats: false)
        }
    }
    
    @objc func speedPan(gesture: UIPanGestureRecognizer) {
        if gesture.state == .changed {
            let translation = gesture.translation(in: speedDot)
            let y = translation.y
            let screenHeight = view.frame.height
            var speed = y / (screenHeight / 2 * 0.7)
            if speed > 1 {
                speed = 1
            }
            if speed < -1 {
                speed = -1
            }
            speed = speed * (255 - 85)
            if speed > 0 {
                speed += 85
            } else if speed < 0 {
                speed -= 85
            }
            speed *= -1
            setSpeed(speed: Int(speed))
        }
        
        if gesture.state == .ended {
            setSpeed(speed: 0)
            Timer.scheduledTimer(timeInterval: 0.02, target: self,   selector: #selector(resetSpeed), userInfo: nil, repeats: false)
            Timer.scheduledTimer(timeInterval: 0.5, target: self,   selector: #selector(resetSpeed), userInfo: nil, repeats: false)
        }
    }
}
