import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'Maze/Maze.dart';
import 'dart:typed_data';
import 'GameInProgress.dart';
import 'Snake/SnakeSettingsScreen.dart';
import 'Maze/MazeSettingsScreen.dart';

class CustomGameMenu extends StatefulWidget {
  final String status;
  final BluetoothConnection? connection;

  const CustomGameMenu({
    required this.status,
    required this.connection,
  });

  @override
  State<CustomGameMenu> createState() => _CustomGameMenuState();
}

class _CustomGameMenuState extends State<CustomGameMenu> {
  final List<String> selectedGames = [];
  double _gameTimeMinutes = 0.5;
  int? mazeTime = 90;
  int? mazeVision = 1;
  int? snakeScoreToBeat = 20;
  double? snakeSpeed = 1.0;

  void _onGameTap(String game) async {
    if (selectedGames.contains(game)) {
      setState(() {
        selectedGames.remove(game);
      });
      return;
    }

    if (game == "maze") {
      final result = await Navigator.push<Map<String, int>>(
        context,
        MaterialPageRoute(
          builder: (context) => MazeSettingsScreen(),
        ),
      );
      if (result != null) {
        setState(() {
          if (!selectedGames.contains("maze")) {
            selectedGames.add("maze");
          }
          mazeTime = result['time'];
          mazeVision = result['vision'];
        });
      }
    } else if (game == "snake") {
      final result = await Navigator.push<Map<String, dynamic>>(
        context,
        MaterialPageRoute(
          builder: (context) => SnakeSettingsScreen(),
        ),
      );
      if (result != null) {
        setState(() {
          if (!selectedGames.contains("snake")) {
            selectedGames.add("snake");
            snakeScoreToBeat = result['scoreToBeat'];
            snakeSpeed = result['speed'];
          }
        });
      }
    } else if (game == "wires") {
      setState(() {
        selectedGames.add(game);
      });
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Wires game coming soon!')),
      );
    } else if (game == "morse") {
      setState(() {
        selectedGames.add(game);
      });
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Morse Code game coming soon!')),
      );
    }
  }

  void _startGames() {
    if (widget.connection == null || !widget.connection!.isConnected) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('ESP32 not connected!')),
      );
      return;
    }
    if (selectedGames.isEmpty) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Please select at least one game.')),
      );
      return;
    }
    widget.connection!.output.add(
      Uint8List.fromList('${selectedGames.length}\n'.codeUnits),
    );
    int gameTimeSeconds = (_gameTimeMinutes * 60).round();
    widget.connection!.output.add(
      Uint8List.fromList('$gameTimeSeconds\n'.codeUnits),
    );

    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => GameSelectionScreen(
          selectedGames: List<String>.from(selectedGames),
          connection: widget.connection,
          mazeTime: mazeTime ?? 90,
          mazeVision: mazeVision ?? 1,
          snakeScoreToBeat: snakeScoreToBeat ?? 100,
          snakeSpeed: snakeSpeed ?? 1.0,
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final mazeImage = 'assets/maze.png';
    final snakeImage = 'assets/snake.png';
    final wiresImage = 'assets/wires.png'; 
    final morseImage = 'assets/morse.png'; 

    Widget gameTile(String label, String asset, String key) {
      final isSelected = selectedGames.contains(key);
      return GestureDetector(
        onTap: () => _onGameTap(key),
        child: AnimatedContainer(
          duration: Duration(milliseconds: 200),
          decoration: BoxDecoration(
            gradient: isSelected
                ? LinearGradient(
                    colors: [Colors.purpleAccent, Colors.blueAccent],
                    begin: Alignment.topLeft,
                    end: Alignment.bottomRight,
                  )
                : LinearGradient(
                    colors: [Colors.white10, Colors.white12],
                    begin: Alignment.topLeft,
                    end: Alignment.bottomRight,
                  ),
            border: Border.all(
              color: isSelected ? Colors.amberAccent : Colors.transparent,
              width: 4,
            ),
            borderRadius: BorderRadius.circular(18),
            boxShadow: isSelected
                ? [
                    BoxShadow(
                      color: Colors.amberAccent.withOpacity(0.4),
                      blurRadius: 16,
                      offset: Offset(0, 8),
                    ),
                  ]
                : [],
          ),
          padding: EdgeInsets.all(8),
          child: Column(
            children: [
              Image.asset(asset, width: 90, height: 90),
              SizedBox(height: 8),
              Text(
                label,
                style: TextStyle(
                  color: isSelected ? Colors.amberAccent : Colors.white,
                  fontWeight: FontWeight.bold,
                  fontSize: 18,
                  letterSpacing: 1,
                  shadows: [
                    Shadow(
                      blurRadius: 4,
                      color: Colors.black45,
                      offset: Offset(1, 2),
                    ),
                  ],
                ),
              ),
            ],
          ),
        ),
      );
    }

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
            child: Card(
              color: Colors.white.withOpacity(0.13),
              shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(28)),
              elevation: 12,
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 32, vertical: 32),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Icon(Icons.tune, size: 60, color: Colors.amberAccent),
                    SizedBox(height: 16),
                    Text(
                      "Custom Game",
                      style: TextStyle(
                        fontSize: 32,
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
                    SizedBox(height: 32),
                    Text(
                      "Game time: ${_gameTimeMinutes.toStringAsFixed(1)} min",
                      style: TextStyle(
                        color: Colors.white,
                        fontSize: 18,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                    Slider(
                      value: _gameTimeMinutes,
                      min: 0.5,
                      max: 10,
                      divisions: 19,
                      label: "${_gameTimeMinutes.toStringAsFixed(1)} min",
                      activeColor: Colors.purpleAccent,
                      inactiveColor: Colors.white24,
                      onChanged: (val) {
                        setState(() {
                          _gameTimeMinutes = val;
                        });
                      },
                    ),
                    SizedBox(height: 24),
                    GridView.count(
                      crossAxisCount: 2,
                      shrinkWrap: true,
                      mainAxisSpacing: 24,
                      crossAxisSpacing: 24,
                      childAspectRatio: 0.95,
                      physics: NeverScrollableScrollPhysics(),
                      children: [
                        gameTile("Maze", mazeImage, "maze"),
                        gameTile("Snake", snakeImage, "snake"),
                        gameTile("Wires", wiresImage, "wires"),
                        gameTile("Morse Code", morseImage, "morse"),
                      ],
                    ),
                    SizedBox(height: 40),
                    ElevatedButton.icon(
                      icon: Icon(Icons.play_arrow),
                      label: Text("Start Game", style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
                      style: ElevatedButton.styleFrom(
                        backgroundColor: Colors.purpleAccent,
                        foregroundColor: Colors.white,
                        elevation: 8,
                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                        padding: EdgeInsets.symmetric(horizontal: 32, vertical: 16),
                      ),
                      onPressed: _startGames,
                    ),
                    SizedBox(height: 20),
                    Text(
                      "Bluetooth Status: ${widget.status}",
                      style: TextStyle(
                        color: Colors.white70,
                        fontSize: 16,
                        fontWeight: FontWeight.w500,
                      ),
                    ),
                    SizedBox(height: 8),
                    TextButton(
                      child: Text("Back", style: TextStyle(color: Colors.white70, fontSize: 16)),
                      onPressed: () => Navigator.pop(context),
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}