import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'dart:typed_data';
import 'GameInProgress.dart';
import 'Snake/SnakeSettingsScreen.dart';
import 'Maze/MazeSettingsScreen.dart';
import 'Morse/MorseSettingsScreen.dart';
import 'Symbol/SymbolSettingsScreen.dart';
import 'Wires/WiresSettingsScreen.dart';

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
  double _gameTimeMinutes = 5.0;
  int? mazeTime = 90;
  int? mazeVision = 1;
  int? snakeScoreToBeat = 20;
  double? snakeSpeed = 1.0;
  int? morseWordLength = 4;
  int? symbolTime = 45;
  int? symbolLevels = 1;
  int? wiresAttempts = 10;
  int? wiresNumber = 5;

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
      final result = await Navigator.push<Map<String, int>>(
        context,
        MaterialPageRoute(
          builder: (context) => WiresSettingsScreen(),
        ),
      );
      if (result != null) {
        setState(() {
          if (!selectedGames.contains("wires")) {
            selectedGames.add("wires");
          }
          wiresAttempts = result['attempts'];
          wiresNumber = result['number'];
        });
      }
    } else if (game == "morse") {
      final result = await Navigator.push<Map<String, int>>(
        context,
        MaterialPageRoute(
          builder: (context) => MorseSettingsScreen(),
        ),
      );
      if (result != null) {
        setState(() {
          if (!selectedGames.contains("morse")) {
            selectedGames.add("morse");
          }
          morseWordLength = result['wordLength'];
        });
      }
    } else if (game == "symbol") {
      final result = await Navigator.push<Map<String, int>>(
        context,
        MaterialPageRoute(
          builder: (context) => SymbolSettingsScreen(),
        ),
      );
      if (result != null) {
        setState(() {
          if (!selectedGames.contains("symbol")) {
            selectedGames.add("symbol");
          }
          symbolTime = result['time'];
          symbolLevels = result['levels'];
        });
      }
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
          snakeScoreToBeat: snakeScoreToBeat ?? 10,
          snakeSpeed: snakeSpeed ?? 1.0,
          morseWordLength: morseWordLength ?? 4,
          symbolTime: symbolTime ?? 45,
          symbolLevels: symbolLevels ?? 1,
          wiresAttempts: wiresAttempts ?? 10,
          wiresNumber: wiresNumber ?? 5,
        ),
      ),
    );
  }

  String getGameDescription(String key) {
    switch (key) {
      case "maze":
        return "This is a maze game. You cannot see the full maze, only a small area around your position is visible. Use the compass on your phone to navigate to the exit.";
      case "snake":
        return "This is a snake game. Use the buttons to turn and eat apples. The apples are not visible on the matrix, you can only see their position on your phone.";
      case "morse":
        return "Decode the Morse code shown on the LED and unscramble the letters to form a real word. Then, for a word of length n with letters l₁, l₂, ..., lₙ (where lᵢ is the alphabetical position of the i-th letter), calculate the sum:\n\nS = 1×l₁ + 2×l₂ + ... + n×lₙ = ∑ᵢ₌₁ⁿ (i × lᵢ)\n\nThen, compute S mod 3:\n0: 1 long press on the button\n1: 1 short press on the button\n2: 2 short presses on the button";
      case "wires":
        return "In this game you have to figure out how to connect the wires the right way. After every attempt you will be told:\n"
               "- How many wires you placed perfectly\n"
               "- How many wires were connected to a correct pin on the right (but not on the left)\n"
               "- How many were connected to the correct pin on the left (but not on the right)";
      case "symbol":
        return "At the start of the game, a picture is shown on the ESP32, but it is split into 4 pieces and the pieces are shuffled in random order. On the application, you must choose the correct symbol that matches the original picture.";
      default:
        return "No description available for this game.";
    }
  }

  @override
  Widget build(BuildContext context) {
    final mazeImage = 'assets/maze.png';
    final snakeImage = 'assets/snake.png';
    final wiresImage = 'assets/wires.png';
    final morseImage = 'assets/morse.png';
    final symbolImage = 'assets/symbol.png';

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
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Expanded( // <-- This makes the image flexible!
            child: Stack(
              alignment: Alignment.center,
              children: [
                Image.asset(asset, width: 70, height: 70, fit: BoxFit.contain), // reduce size
                Positioned(
                  top: 2,
                  right: 2,
                  child: GestureDetector(
                    onTap: () {
                      showDialog(
                        context: context,
                        builder: (_) => AlertDialog(
                          title: Text("Game Info"),
                          content: Text(
                            getGameDescription(key),
                            style: TextStyle(fontSize: 15),
                          ),
                          actions: [
                            TextButton(
                              onPressed: () => Navigator.of(context).pop(),
                              child: Text("OK"),
                            ),
                          ],
                        ),
                      );
                    },
                    child: Container(
                      decoration: BoxDecoration(
                        color: Colors.black54,
                        shape: BoxShape.circle,
                      ),
                      padding: EdgeInsets.all(6),
                      child: Icon(Icons.help_outline, color: Colors.white, size: 20),
                    ),
                  ),
                ),
              ],
            ),
          ),
          SizedBox(height: 4),
          Text(
            label,
            textAlign: TextAlign.center,
            style: TextStyle(
              color: isSelected ? Colors.amberAccent : Colors.white,
              fontWeight: FontWeight.bold,
              fontSize: 15,
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
                        gameTile("Symbol", symbolImage, "symbol"),
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
                    ElevatedButton.icon(
                      icon: Icon(Icons.arrow_back, color: Colors.white),
                      label: Text("Back", style: TextStyle(fontSize: 18, color: Colors.white)),
                      style: ElevatedButton.styleFrom(
                        backgroundColor: Colors.redAccent,
                        elevation: 6,
                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                        padding: EdgeInsets.symmetric(horizontal: 32, vertical: 12),
                      ),
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