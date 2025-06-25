import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'dart:typed_data';
import 'package:permission_handler/permission_handler.dart';
import 'Maze/Maze.dart';
import 'CustomMenu.dart';

void main() => runApp(MaterialApp(
  debugShowCheckedModeBanner: false,
  theme: ThemeData.dark(),
  home: MyApp(),
));

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
      body: Container(
        decoration: BoxDecoration(
          gradient: LinearGradient(
            colors: [Colors.deepPurple.shade800, Colors.blue.shade600, Colors.cyan.shade400],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
        ),
        child: Center(
          child: SingleChildScrollView(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Icon(Icons.videogame_asset, size: 80, color: Colors.amberAccent),
                SizedBox(height: 20),
                Text(
                  "Escape Game",
                  style: TextStyle(
                    fontSize: 36,
                    fontWeight: FontWeight.bold,
                    color: Colors.amberAccent,
                    letterSpacing: 2,
                    shadows: [
                      Shadow(
                        blurRadius: 8,
                        color: Colors.black54,
                        offset: Offset(2, 2),
                      ),
                    ],
                  ),
                ),
                SizedBox(height: 40),
                _buildMenuButton("Easy", Icons.sentiment_satisfied, () => _showPlaceholder("Easy")),
                SizedBox(height: 20),
                _buildMenuButton("Medium", Icons.sentiment_neutral, () => _showPlaceholder("Medium")),
                SizedBox(height: 20),
                _buildMenuButton("Hard", Icons.sentiment_very_dissatisfied, () => _showPlaceholder("Hard")),
                SizedBox(height: 20),
                _buildMenuButton("Custom", Icons.tune, _openCustomMenu),
                SizedBox(height: 40),
                Card(
                  color: Colors.white.withOpacity(0.15),
                  shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                  elevation: 8,
                  child: Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 24, vertical: 16),
                    child: Column(
                      children: [
                        Text(
                          "Bluetooth Status:",
                          style: TextStyle(color: Colors.white70, fontSize: 16),
                        ),
                        SizedBox(height: 8),
                        Text(
                          status,
                          style: TextStyle(
                            color: status == "Connected to ESP32!" ? Colors.greenAccent : Colors.redAccent,
                            fontWeight: FontWeight.bold,
                            fontSize: 18,
                          ),
                        ),
                        SizedBox(height: 12),
                        ElevatedButton.icon(
                          onPressed: _connectToESP32,
                          icon: Icon(Icons.bluetooth),
                          label: Text("Connect to ESP32"),
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.blueAccent,
                            foregroundColor: Colors.white,
                            shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                            textStyle: TextStyle(fontWeight: FontWeight.bold),
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildMenuButton(String text, IconData icon, VoidCallback onPressed) {
    return SizedBox(
      width: 220,
      height: 56,
      child: ElevatedButton.icon(
        icon: Icon(icon, size: 28),
        label: Text(
          text,
          style: TextStyle(fontSize: 22, fontWeight: FontWeight.bold, letterSpacing: 1),
        ),
        onPressed: onPressed,
        style: ElevatedButton.styleFrom(
          backgroundColor: Colors.purpleAccent,
          foregroundColor: Colors.white,
          elevation: 6,
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
        ),
      ),
    );
  }
}