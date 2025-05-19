import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'dart:typed_data';
import 'package:permission_handler/permission_handler.dart';

void main() => runApp(MyApp());

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  BluetoothConnection? connection;
  String status = "Disconnected";

  @override
  void initState() {
    super.initState();
    _connectToESP32();
  }
void _connectToESP32() async {
  // Ask for Bluetooth and location permissions
  await [
    Permission.bluetooth,
    Permission.bluetoothConnect,
    Permission.bluetoothScan,
    Permission.location,
  ].request();

  // Check if we have permission
  if (!await Permission.bluetoothConnect.isGranted) {
    setState(() {
      status = "Bluetooth connect permission not granted.";
    });
    return;
  }

  // Now try getting bonded devices
  try {
    List<BluetoothDevice> devices = await FlutterBluetoothSerial.instance.getBondedDevices();
    BluetoothDevice? esp32;

    for (var d in devices) {
      if (d.name != null && d.name!.startsWith("ESP32")) {
        esp32 = d;
        break;
      }
    }

    if (esp32 != null) {
      BluetoothConnection.toAddress(esp32.address).then((conn) {
        connection = conn;
        setState(() {
          status = "Connected to ESP32!";
        });

        conn.input!.listen((data) {
          print("Received: ${String.fromCharCodes(data)}");
        });
      }).catchError((error) {
        setState(() {
          status = "Connection failed: $error";
        });
      });
    } else {
      setState(() {
        status = "ESP32 not found in paired devices.";
      });
    }
  } catch (e) {
    setState(() {
      status = "Failed to get bonded devices: $e";
    });
  }
}


  void _sendMessage(String msg) {
    if (connection != null && connection!.isConnected) {
      print("Sending: $msg");
      connection!.output.add(Uint8List.fromList(msg.codeUnits));
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(title: Text("ESP32 Bluetooth")),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(status),
              ElevatedButton(
                child: Text("Send Hello"),
                onPressed: () => _sendMessage("2\n"),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
