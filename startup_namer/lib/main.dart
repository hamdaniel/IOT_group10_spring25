import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:permission_handler/permission_handler.dart';
import 'CustomMenu.dart';
import 'GameInProgress.dart';
import 'leaderboard.dart';
import 'globals.dart';
String gameDifficulty="";
final GlobalKey<NavigatorState> navigatorKey = GlobalKey<NavigatorState>();

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await SystemChrome.setPreferredOrientations([
    DeviceOrientation.portraitUp,
    DeviceOrientation.portraitDown,
  ]);
  runApp(MaterialApp(
    debugShowCheckedModeBanner: false,
    theme: ThemeData.dark(),
    home: MyApp(),
    navigatorKey: navigatorKey,
  ));
}

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
          globalConnection = conn;
          setState(() {
            status = "Connected to ESP32!";
          });

      // After connecting:
      globalInputBroadcast = conn.input?.asBroadcastStream();

      globalInputBroadcast?.listen((_) {}, onDone: () {
        setState(() {
          status = "Disconnected";
          connection = null;
          globalConnection = null;
        });
        navigatorKey.currentState?.popUntil((route) => route.isFirst);
        showDialog(
          context: navigatorKey.currentContext!,
          builder: (context) => AlertDialog(
            title: Text("Bluetooth Disconnected"),
            content: Text("Connection to ESP32 was lost."),
            actions: [
              TextButton(
                onPressed: () => Navigator.of(context).pop(),
                child: Text("OK"),
              ),
            ],
          ),
        );
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

    if (connection == null || !(connection?.isConnected ?? false)) {
      showDialog(
        context: context,
        builder: (context) => AlertDialog(
          title: Text("No Bluetooth Connection"),
          content: Text("Please connect to the ESP32 before starting a game."),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text("OK"),
            ),
          ],
        ),
      );
      return;
    }
    gameDifficulty = "Custom";
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

  void _startDifficulty(String difficulty) {

    if (connection == null || !(connection?.isConnected ?? false)) {
      showDialog(
        context: context,
        builder: (context) => AlertDialog(
          title: Text("No Bluetooth Connection"),
          content: Text("Please connect to the ESP32 before starting a game."),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text("OK"),
            ),
          ],
        ),
      );
      return;
    }

    List<String> allGames = ["maze", "snake", "morse", "symbol", "wires"];
    int gameTimeMinutes,mazeTime, mazeVision, snakeScoreToBeat, morseWordLength, symbolTime, symbolLevels, wiresAttempts, wiresNumber;
    double snakeSpeed;
    gameDifficulty = difficulty;
    if (difficulty == "Easy") {
      gameTimeMinutes=15;
      mazeTime = 180;
      mazeVision = 3;
      snakeScoreToBeat = 5;
      snakeSpeed = 1;
      morseWordLength = 3;
      symbolTime = 90;
      symbolLevels = 1;
      wiresAttempts = 15;
      wiresNumber = 3;
      connection!.output.add(Uint8List.fromList('${allGames.length}\n'.codeUnits),);
      int gameTimeSeconds = (gameTimeMinutes * 60).round();
      connection!.output.add(Uint8List.fromList('$gameTimeSeconds\n'.codeUnits),);

    } else if (difficulty == "Medium") {
      gameTimeMinutes=10;
      mazeTime = 120;
      mazeVision = 2;
      snakeScoreToBeat = 10;
      snakeSpeed = 2;
      morseWordLength = 4;
      symbolTime = 60;
      symbolLevels = 2;
      wiresAttempts = 15;
      wiresNumber = 4;
      connection!.output.add(Uint8List.fromList('${allGames.length}\n'.codeUnits),);
      int gameTimeSeconds = (gameTimeMinutes * 60).round();
      connection!.output.add(Uint8List.fromList('$gameTimeSeconds\n'.codeUnits),);

    } else if (difficulty == "Hard") {
      gameTimeMinutes=5;
      mazeTime = 60;
      mazeVision = 1;
      snakeScoreToBeat = 15;
      snakeSpeed = 3;
      morseWordLength = 5;
      symbolTime = 30;
      symbolLevels = 3;
      wiresAttempts = 15;
      wiresNumber = 5;
      connection!.output.add(Uint8List.fromList('${allGames.length}\n'.codeUnits),);
      int gameTimeSeconds = (gameTimeMinutes * 60).round();
      connection!.output.add(Uint8List.fromList('$gameTimeSeconds\n'.codeUnits),);
    } else {
      // fallback to custom menu
      _openCustomMenu();
      return;
    }

    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => GameSelectionScreen(
          selectedGames: List<String>.from(allGames),
          connection: connection,
          mazeTime: mazeTime,
          mazeVision: mazeVision,
          snakeScoreToBeat: snakeScoreToBeat,
          snakeSpeed: snakeSpeed.toDouble(),
          morseWordLength: morseWordLength,
          symbolTime: symbolTime,
          symbolLevels: symbolLevels,
          wiresAttempts: wiresAttempts,
          wiresNumber: wiresNumber,
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      extendBodyBehindAppBar: true,
      appBar: AppBar(
        backgroundColor: Colors.transparent,
        elevation: 0,
        systemOverlayStyle: SystemUiOverlayStyle(
          statusBarColor: Colors.transparent,
          statusBarIconBrightness: Brightness.light,
        ),
        actions: [
          IconButton(
            icon: Icon(Icons.leaderboard, color: Colors.amberAccent, size: 32),
            tooltip: "Leaderboards",
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => LeaderboardScreen()),
              );
            },
          ),
          SizedBox(width: 12),
        ],
      ),
      body: Container(
        decoration: BoxDecoration(
          gradient: LinearGradient(
            colors: [Colors.deepPurple.shade800, Colors.blue.shade600, Colors.cyan.shade400],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
        ),
        child: SafeArea( 
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
                  _buildMenuButton("Easy", Icons.sentiment_satisfied, () => _startDifficulty("Easy")),
                  SizedBox(height: 20),
                  _buildMenuButton("Medium", Icons.sentiment_neutral, () => _startDifficulty("Medium")),
                  SizedBox(height: 20),
                  _buildMenuButton("Hard", Icons.sentiment_very_dissatisfied, () => _startDifficulty("Hard")),
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