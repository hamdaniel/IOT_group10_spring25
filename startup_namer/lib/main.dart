import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'dart:typed_data';
import 'package:permission_handler/permission_handler.dart';
import 'settingsScreen.dart';
import 'dart:math';
import 'Maze.dart';
import 'dart:collection';
import 'compass.dart';


void main() => runApp(MaterialApp(home: MyApp()));

class MyApp extends StatefulWidget {
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  List<List<int>>? lastMaze;
  Point<int>? playerPosition;
  BluetoothConnection? connection;
  String status = "Not connected";
  late BuildContext menuContext;

  int timeToCompleteMaze = 90; 
  int visionAtMaze = 1;

  // Removed initState and auto-connect

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
  void _sendMessage(String msg) {
    if (connection != null && connection!.isConnected) {
      connection!.output.add(Uint8List.fromList(msg.codeUnits));
    }
  }

void _startGame() {
  final int size = 16;
  final rand = Random();

  // Maze with walls (1) everywhere
  List<List<int>> maze = List.generate(size, (_) => List.generate(size, (_) => 1));

  // Helper to get a random edge cell (odd coordinates for best maze structure)
  List<Point<int>> edgeCells = [];
  for (int i = 1; i < size; i += 2) {
    edgeCells.add(Point(i, 0));
    edgeCells.add(Point(i, size - 1));
    edgeCells.add(Point(0, i));
    edgeCells.add(Point(size - 1, i));
  }

  // Pick random start edge cell
  Point<int> start = edgeCells[rand.nextInt(edgeCells.length)];

  // Carve maze using DFS from start
  void carve(int x, int y) {
    maze[x][y] = 0;
    var dirs = [
      [0, 2],
      [0, -2],
      [2, 0],
      [-2, 0],
    ]..shuffle(rand);
    for (var d in dirs) {
      int nx = x + d[0], ny = y + d[1];
      if (nx > 0 && nx < size && ny > 0 && ny < size && maze[nx][ny] == 1) {
        maze[x + d[0] ~/ 2][y + d[1] ~/ 2] = 0; // Remove wall between
        carve(nx, ny);
      }
    }
  }

  carve(start.x, start.y);

  // Find the farthest reachable edge cell from start
  Point<int> end = start;
  int maxDist = -1;
  List<List<int>> dist = List.generate(size, (_) => List.filled(size, -1));
  Queue<Point<int>> queue = Queue();
  queue.add(start);
  dist[start.x][start.y] = 0;

  while (queue.isNotEmpty) {
    Point<int> p = queue.removeFirst();
    for (var d in [
      Point(0, 1),
      Point(1, 0),
      Point(0, -1),
      Point(-1, 0),
    ]) {
      int nx = p.x + d.x, ny = p.y + d.y;
      if (nx >= 0 && nx < size && ny >= 0 && ny < size &&
          maze[nx][ny] == 0 && dist[nx][ny] == -1) {
        dist[nx][ny] = dist[p.x][p.y] + 1;
        queue.add(Point(nx, ny));
        // If this is an edge cell, check if it's the farthest so far
        if ((nx == 0 || nx == size - 1 || ny == 0 || ny == size - 1) &&
            dist[nx][ny] > maxDist) {
          maxDist = dist[nx][ny];
          end = Point(nx, ny);
        }
      }
    }
  }

  // Ensure start and end are open
  maze[start.x][start.y] = 2;
  maze[end.x][end.y] = 3;

  // Store the maze for display
  setState(() {
    lastMaze = maze;
  });

  // Convert maze to string

String mazeString = maze.expand((row) => row).join();

// Send maze string (just 0s and 1s, no prefix, no newline)
_sendMessage("${mazeString}\n");

String time_send = "${timeToCompleteMaze.toString().padLeft(3, '0')}";
String vision_send = "${visionAtMaze.toString()}";

_sendMessage("${time_send}\n");
print ("Time to complete maze: $time_send");
_sendMessage("${vision_send}\n");
print ("Vision at maze: $vision_send");
  // Show the maze in a dialog
  showDialog(
    context: menuContext,
    barrierDismissible: false,
    builder: (context) => CompassDialog(
      connection: connection,
      end: end,
      start:start
    ),
  );
}

  void _openSettings() async {
    final result = await Navigator.push(
      menuContext,
      MaterialPageRoute(
        builder: (context) => SettingsScreen(
          initialTime: timeToCompleteMaze,
          initialVision: visionAtMaze,
        ),
      ),
    );
    if (result != null && result is Map<String, int>) {
      setState(() {
        timeToCompleteMaze = result['time']!;
        visionAtMaze = result['vision']!;
      });
  
    }
  }

  @override
  Widget build(BuildContext context) {
    return Builder(
      builder: (BuildContext innerContext) {
        menuContext = innerContext;
        return MainMenu(
          status: status,
          onConnect: _connectToESP32,
          onStartGame: _startGame,
          onChangeSettings: _openSettings,
          timeToCompleteMaze: timeToCompleteMaze,
          visionAtMaze: visionAtMaze,
        );
      },
    );
  }
}

class MainMenu extends StatelessWidget {
  final String status;
  final VoidCallback onConnect;
  final VoidCallback onStartGame;
  final VoidCallback onChangeSettings;
  final int timeToCompleteMaze;
  final int visionAtMaze;

  const MainMenu({
    required this.status,
    required this.onConnect,
    required this.onStartGame,
    required this.onChangeSettings,
    required this.timeToCompleteMaze,
    required this.visionAtMaze,
  });

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("ESP32 Main Menu")),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(status),
            SizedBox(height: 20),
            ElevatedButton(
              child: Text("Connect to ESP32"),
              onPressed: onConnect,
            ),
            SizedBox(height: 20),
            ElevatedButton(
              child: Text("Start Game"),
              onPressed: onStartGame,
            ),
            SizedBox(height: 10),
            ElevatedButton(
              child: Text("Change Settings"),
              onPressed: onChangeSettings,
            ),
            SizedBox(height: 30),
            Text("Current Settings:"),
            Text("Time To Complete Maze: $timeToCompleteMaze seconds"),
            Text("Vision At Maze: $visionAtMaze pixels"),
          ],
        ),
      ),
    );
  }
}