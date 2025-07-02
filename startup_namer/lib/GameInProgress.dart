import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'Maze/Maze.dart';
import 'Snake/Snake.dart';
import 'Morse/Morse.dart';
import 'Symbol/Symbol.dart';
import 'Wires/Wires.dart'; 
import '../globals.dart';

class GameSelectionScreen extends StatefulWidget {
  final List<String> selectedGames;
  final BluetoothConnection? connection;
  final int mazeTime;
  final int mazeVision;
  final int snakeScoreToBeat;
  final double snakeSpeed;
  final int morseWordLength;
  final int? symbolTime;
  final int? symbolLevels;
  final int? wiresAttempts;
  final int? wiresNumber;

  const GameSelectionScreen({
    required this.selectedGames,
    required this.connection,
    required this.mazeTime,
    required this.mazeVision,
    required this.snakeScoreToBeat,
    required this.snakeSpeed,
    required this.morseWordLength,
    this.symbolTime,
    this.symbolLevels,
    this.wiresAttempts,
    this.wiresNumber,
  });

  @override
  State<GameSelectionScreen> createState() => _GameSelectionScreenState();
}

class _GameSelectionScreenState extends State<GameSelectionScreen> {
  Set<String> completedGames = {};
  StreamSubscription<Uint8List>? _bombSubscription;
  bool _bombDialogShown = false;

  @override
  void initState() {
    super.initState();
    // Set up the global broadcast stream only once per connection
    if (widget.connection?.input != null && globalInputBroadcast == null) {
      globalInputBroadcast = widget.connection!.input!.asBroadcastStream();
    }
    _listenForBombOver();
  }

  void _listenForBombOver() {
    _bombSubscription = globalInputBroadcast?.listen((Uint8List data) {
      String msg = String.fromCharCodes(data).trim();
      if (_bombDialogShown) return;
      if (msg == "bomb_over_w") {
        _bombDialogShown = true;
        _showBombDialog(
          "Congratulations!",
          "You finished all the puzzles and won!",
        );
      } else if (msg == "bomb_over_l_lives") {
        _bombDialogShown = true;
        _showBombDialog(
          "Out of Lives",
          "You ran out of lives!",
        );
      } else if (msg == "bomb_over_l_time") {
        _bombDialogShown = true;
        _showBombDialog(
          "Out of Time",
          "You ran out of time!",
        );
      }
    });
  }

  void _showBombDialog(String title, String message) {
    showDialog(
      context: context,
      barrierDismissible: false,
      builder: (_) => AlertDialog(
        title: Text(title),
        content: Text(message),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(context).popUntil((route) => route.isFirst);
            },
            child: Text("Return to Menu"),
          ),
        ],
      ),
    );
  }

  @override
  void dispose() {
    _bombSubscription?.cancel();
    super.dispose();
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
        return "The game is $key";
    }
  }

  Future<void> _startGame(String game) async {
    if (game == "maze") {
      final result = await startMazeGame(
        menuContext: context,
        connection: widget.connection,
        timeToCompleteMaze: widget.mazeTime,
        visionAtMaze: widget.mazeVision,
      );
      setState(() {
        if (result == "win") completedGames.add(game);
      });
    } else if (game == "snake") {
      final result = await startSnakeGame(
        menuContext: context,
        connection: widget.connection,
        scoreToBeat: widget.snakeScoreToBeat,
        speed: widget.snakeSpeed,
      );
      setState(() {
        if (result == "win") completedGames.add(game);
      });
    } else if (game == "morse") {
      final result = await startMorseGame(
        menuContext: context,
        connection: widget.connection,
        wordLength: widget.morseWordLength,
      );
      setState(() {
        if (result == "win") completedGames.add(game);
      });
    } else if (game == "symbol") {
      final result = await startSymbolGame(
        menuContext: context,
        connection: widget.connection,
        time: widget.symbolTime ?? 45,
        levels: widget.symbolLevels ?? 1,
      );
      setState(() {
        if (result == "win") completedGames.add(game);
      });
    } else if (game == "wires") {
      final result = await startWiresGame(
        menuContext: context,
        connection: widget.connection,
        attempts: widget.wiresAttempts ?? 10,
        number: widget.wiresNumber ?? 5,
      );
      setState(() {
        if (result == "win") completedGames.add(game);
      });
    } else {
      await showDialog(
        context: context,
        builder: (context) => AlertDialog(
          title: Text("${game[0].toUpperCase()}${game.substring(1)}"),
          content: Text("$game game coming soon!"),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text("OK"),
            ),
          ],
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    final gameImages = {
      "maze": 'assets/maze.png',
      "snake": 'assets/snake.png',
      "wires": 'assets/wires.png',
      "morse": 'assets/morse.png',
      "symbol": 'assets/symbol.png',
    };

    return WillPopScope(
      onWillPop: () async => false, // Prevents back navigation
      child: Scaffold(
        appBar: AppBar(
          title: Text("Play Selected Games"),
          automaticallyImplyLeading: false, // No back arrow
        ),
        body: Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              colors: [Colors.deepPurple.shade800, Colors.blue.shade600, Colors.cyan.shade400],
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
            ),
          ),
          child: Center(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Text(
                  "Tap a game to play it:",
                  style: TextStyle(fontSize: 18, color: Colors.white),
                ),
                SizedBox(height: 20),
                GridView.count(
                  crossAxisCount: 2,
                  shrinkWrap: true,
                  mainAxisSpacing: 24,
                  crossAxisSpacing: 24,
                  childAspectRatio: 0.95,
                  physics: NeverScrollableScrollPhysics(),
                  children: widget.selectedGames.map((game) {
                    final isDone = completedGames.contains(game);
                    return GestureDetector(
                      onTap: isDone ? null : () => _startGame(game),
                      child: Opacity(
                        opacity: isDone ? 0.4 : 1.0,
                        child: Container(
                          margin: EdgeInsets.symmetric(horizontal: 8, vertical: 8),
                          decoration: BoxDecoration(
                            border: Border.all(
                              color: isDone ? Colors.greenAccent.shade400 : Colors.transparent,
                              width: isDone ? 8 : 4, // Thicker border for completed
                            ),
                            borderRadius: BorderRadius.circular(12),
                            color: isDone
                                ? Colors.greenAccent.withOpacity(0.18) // Subtle green background
                                : Colors.white.withOpacity(0.13),
                          ),
                          padding: EdgeInsets.all(4),
                          child: Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: [
                              Stack(
                                children: [
                                  Image.asset(
                                    gameImages[game] ?? '',
                                    width: 90,
                                    height: 90,
                                  ),
                                  if (!isDone)
                                    Positioned(
                                      top: 2,
                                      right: 2,
                                      child: GestureDetector(
                                        onTap: () {
                                          showDialog(
                                            context: context,
                                            builder: (_) => AlertDialog(
                                              title: Text("Game Info"),
                                              content: Text(getGameDescription(game)),
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
                                          child: Icon(Icons.help_outline, color: Colors.white, size: 26),
                                        ),
                                      ),
                                    ),
                                ],
                              ),
                              SizedBox(height: 8),
                              Text(
                                "${game[0].toUpperCase()}${game.substring(1)}",
                                style: TextStyle(
                                  color: isDone ? Colors.greenAccent.shade400 : Colors.white,
                                  fontWeight: FontWeight.bold,
                                  fontSize: 14, // Smaller label text
                                ),
                              ),
                              if (isDone)
                                Padding(
                                  padding: const EdgeInsets.only(top: 4.0),
                                  child: Icon(Icons.check, color: Colors.greenAccent.shade400, size: 28),
                                ),
                            ],
                          ),
                        ),
                      ),
                    );
                  }).toList(),
                ),
                SizedBox(height: 40),
                if (completedGames.length == widget.selectedGames.length)
                  Text(
                    "All games completed!",
                    style: TextStyle(
                      fontSize: 26,
                      color: Colors.greenAccent.shade400,
                      fontWeight: FontWeight.bold,
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
                ElevatedButton.icon(
                  icon: Icon(Icons.exit_to_app, color: Colors.white),
                  label: Text("Exit to Main Menu", style: TextStyle(fontSize: 18, color: Colors.white)),
                  style: ElevatedButton.styleFrom(
                    backgroundColor: const Color.fromARGB(255, 219, 95, 95),
                    elevation: 6,
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                    padding: EdgeInsets.symmetric(horizontal: 32, vertical: 12),
                  ),
                  onPressed: () {
                    widget.connection?.output.add(Uint8List.fromList("exit\n".codeUnits));
                    Navigator.of(context).popUntil((route) => route.isFirst);
                  },
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}