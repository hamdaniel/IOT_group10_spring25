import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'Maze/Maze.dart';
import 'dart:typed_data';
import 'GameInProgress.dart';

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
  // Track selected games in order
  final List<String> selectedGames = [];

  void _onGameTap(String game) async {
    if (selectedGames.contains(game)) {
      // If already selected, just deselect
      setState(() {
        selectedGames.remove(game);
      });
      return;
    }

    if (game == "maze") {
      // Only open Maze menu if not selected
      final result = await _openMazeMenu(context);
      if (result == true) {
        setState(() {
          if (!selectedGames.contains("maze")) {
            selectedGames.add("maze");
          }
        });
      }
      // If result is not true (user pressed back), do not select
    } else if (game == "snake" || game == "minesweeper") {
      setState(() {
        selectedGames.add(game);
      });
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('${game[0].toUpperCase()}${game.substring(1)} game coming soon!')),
      );
    }
  }

  Future<bool?> _openMazeMenu(BuildContext context) async {
    // Returns true if "Select Game" was pressed, null/false otherwise
    return await Navigator.push<bool>(
      context,
      MaterialPageRoute(
        builder: (context) => MazeMainMenu(
          status: widget.status,
          onConnect: () {},
          connection: widget.connection,
          menuContext: context,
        ),
      ),
    );
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
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => GameSelectionScreen(
          selectedGames: List<String>.from(selectedGames),
          connection: widget.connection,
          // Optionally pass maze/time/vision here
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