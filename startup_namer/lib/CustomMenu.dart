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
      // If already selected, just deselect
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
      }
      else if (game == "snake") {
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
      }else if (game == "minesweeper") {
        setState(() {
          selectedGames.add(game);
        });
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Minesweeper game coming soon!')),
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
    // Send the number of games to ESP32
    widget.connection!.output.add(
      Uint8List.fromList('${selectedGames.length}\n'.codeUnits),
    );
    // Send the game time in seconds
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
        mazeTime: mazeTime ?? 90, // fallback to slider value if not set
        mazeVision: mazeVision ?? 1,           // fallback to 1 if not set
        snakeScoreToBeat: snakeScoreToBeat ?? 100, // fallback value
        snakeSpeed: snakeSpeed ?? 1.0,     
          ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final mazeImage = 'assets/maze.png';
    final snakeImage = 'assets/snake.png';
    final minesweeperImage = 'assets/minesweeper.png';

    Widget gameTile(String label, String asset, String key) {
      final isSelected = selectedGames.contains(key);
      return GestureDetector(
        onTap: () => _onGameTap(key),
        child: Container(
          decoration: BoxDecoration(
            border: Border.all(
              color: isSelected ? Colors.blue : Colors.transparent,
              width: 4,
            ),
            borderRadius: BorderRadius.circular(12),
          ),
          padding: EdgeInsets.all(4),
          child: Column(
            children: [
              Image.asset(asset, width: 100, height: 100),
              SizedBox(height: 8),
              Text(label),
            ],
          ),
        ),
      );
    }

    return Scaffold(
      appBar: AppBar(title: Text("Custom Game Selection")),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            // Game time slider
            Text("Game time: ${_gameTimeMinutes.toStringAsFixed(1)} min"),
            Slider(
              value: _gameTimeMinutes,
              min: 0.5,
              max: 10,
              divisions: 10,
              label: "${_gameTimeMinutes.toStringAsFixed(1)} min",
              onChanged: (val) {
                setState(() {
                  _gameTimeMinutes = val;
                });
              },
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                gameTile("Maze", mazeImage, "maze"),
                SizedBox(width: 30),
                gameTile("Snake", snakeImage, "snake"),
                SizedBox(width: 30),
                gameTile("Minesweeper", minesweeperImage, "minesweeper"),
              ],
            ),
            SizedBox(height: 40),
            ElevatedButton(
              onPressed: _startGames,
              child: Text("Start Game"),
            ),
            SizedBox(height: 20),
            Text("Bluetooth Status: ${widget.status}"),
          ],
        ),
      ),
    );
  }
}