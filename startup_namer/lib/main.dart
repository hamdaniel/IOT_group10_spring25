import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'dart:typed_data';
import 'package:permission_handler/permission_handler.dart';
import 'Maze/Maze.dart';
import 'CustomMenu.dart';

void main() => runApp(MaterialApp(home: MyApp()));

class MyApp extends StatefulWidget {
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  BluetoothConnection? connection;
  String status = "Not connected";
  late BuildContext menuContext;

  void _connectToESP32() async {
    setState(() {
      status = "Connecting...";
    });

    await [
      Permission.bluetooth,
      Permission.bluetoothConnect,
      Permission.bluetoothScan,
      Permission.location,
    ].request();

    if (!await Permission.bluetoothConnect.isGranted) {
      setState(() {
        status = "Bluetooth connect permission not granted.";
      });
      return;
    }

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
        try {
          BluetoothConnection conn = await BluetoothConnection.toAddress(esp32.address);
          connection = conn;
          setState(() {
            status = "Connected to ESP32!";
          });
        } catch (error) {
          setState(() {
            status = "Connection failed";
          });
        }
      } else {
        setState(() {
          status = "ESP32 not found in paired devices.";
        });
      }
    } catch (e) {
      setState(() {
        status = "Connection failed";
      });
    }
  }

  void _openCustomMenu() {
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => CustomGameMenu(
          status: status,
          connection: connection,
        ),
      ),
    );
  }

  void _showPlaceholder(String difficulty) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text('$difficulty mode coming soon!')),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Main Menu")),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            ElevatedButton(
              onPressed: () => _showPlaceholder("Easy"),
              child: Text("Easy"),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () => _showPlaceholder("Medium"),
              child: Text("Medium"),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () => _showPlaceholder("Hard"),
              child: Text("Hard"),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: _openCustomMenu,
              child: Text("Custom"),
            ),
            SizedBox(height: 40),
            Text("Bluetooth Status: $status"),
            ElevatedButton(
              onPressed: _connectToESP32,
              child: Text("Connect to ESP32"),
            ),
          ],
        ),
      ),
    );
  }
}