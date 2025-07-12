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
import 'package:shared_preferences/shared_preferences.dart';
import 'main.dart';

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
  bool isGameStarting = false;
  Set<String> completedGames = {};
  StreamSubscription<Uint8List>? _bombSubscription;
  bool _bombDialogShown = false;
  DateTime? _startTime;
  bool _showedWinDialog = false;
  bool _helpDialogOpen = false;

  @override
  void initState() {
    super.initState();
    _startTime = DateTime.now();
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
        if (_helpDialogOpen) {
          Navigator.of(context, rootNavigator: true).pop();
          _helpDialogOpen = false;
        }
        _bombDialogShown = true;
        _showWinDialogAndSave();
      } else if (msg == "bomb_over_l_lives") {
        if (_helpDialogOpen) {
          Navigator.of(context, rootNavigator: true).pop();
          _helpDialogOpen = false;
        }
        _bombDialogShown = true;
        _showBombDialog(
          "Out of Lives",
          "You ran out of lives!",
        );
      } else if (msg == "bomb_over_l_time") {
        if (_helpDialogOpen) {
          Navigator.of(context, rootNavigator: true).pop();
          _helpDialogOpen = false;
        }
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
      builder: (_) => WillPopScope(
        onWillPop: () async => false,
        child: AlertDialog(
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(20)),
          titlePadding: EdgeInsets.only(top: 24, left: 24, right: 24, bottom: 8),
          contentPadding: EdgeInsets.symmetric(horizontal: 24, vertical: 12),
          title: Row(
            children: [
              Icon(
                title.contains("Time")
                    ? Icons.timer_off
                    : Icons.warning_amber_rounded,
                color: Colors.redAccent,
                size: 32,
              ),
              SizedBox(width: 12),
              Expanded(
                child: Text(
                  title,
                  style: TextStyle(
                    color: Colors.redAccent,
                    fontWeight: FontWeight.bold,
                    fontSize: 24,
                  ),
                ),
              ),
            ],
          ),
          content: Text(
            message,
            style: TextStyle(fontSize: 18, color: Colors.white),
          ),
          actions: [
            TextButton(
              style: TextButton.styleFrom(
                foregroundColor: Colors.white,
                backgroundColor: Colors.redAccent,
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(12),
                ),
                padding: EdgeInsets.symmetric(horizontal: 24, vertical: 12),
              ),
              onPressed: () {
                Navigator.of(context).popUntil((route) => route.isFirst);
              },
              child: Text("Return to Menu", style: TextStyle(fontSize: 16)),
            ),
          ],
        ),
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
    if (isGameStarting) return;
    setState(() {
      isGameStarting = true;
    });
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
    setState(() {
      isGameStarting = false;
    });
  }

  Future<void> _showWinDialogAndSave() async {
    if (_showedWinDialog) return;
    _showedWinDialog = true;
    final duration = DateTime.now().difference(_startTime!);

    String tempName = "";

    Widget dialogContent({required Widget child}) {
      return Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(Icons.emoji_events, color: Colors.amber, size: 60),
          SizedBox(height: 12),
          Text(
            "Congratulations!",
            style: TextStyle(
              fontSize: 26,
              fontWeight: FontWeight.bold,
              color: Colors.amber[700],
              letterSpacing: 1.2,
              shadows: [
                Shadow(
                  blurRadius: 8,
                  color: Colors.black26,
                  offset: Offset(2, 2),
                ),
              ],
            ),
          ),
          SizedBox(height: 8),
          Divider(thickness: 2, color: Colors.amber[200]),
          SizedBox(height: 8),
          child,
        ],
      );
    }

    if (gameDifficulty == "Custom") {
      await showDialog(
        context: context,
        barrierDismissible: false,
        builder: (context) => AlertDialog(
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(24)),
          content: dialogContent(
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                Text(
                  "You defused the custom bomb in ${_formatDuration(duration)}.",
                  style: TextStyle(fontSize: 18),
                  textAlign: TextAlign.center,
                ),
                SizedBox(height: 18),
                Text(
                  "Since the game mode is custom, you cannot add it to the leaderboard.",
                  style: TextStyle(fontSize: 15, color: Colors.grey[700]),
                  textAlign: TextAlign.center,
                ),
              ],
            ),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text("Return to Menu"),
            ),
          ],
        ),
      );
      Navigator.of(context).popUntil((route) => route.isFirst);
      return;
    }

    final name = await showDialog<String>(
      context: context,
      barrierDismissible: false,
      builder: (context) {
        return WillPopScope(
          onWillPop: () async => false,
          child: AlertDialog(
            shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(24)),
            contentPadding: EdgeInsets.symmetric(horizontal: 16, vertical: 8),
            content: SingleChildScrollView(
              child: dialogContent(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Text(
                      'Your time is ${_formatDuration(duration)}.',
                      style: TextStyle(fontSize: 18),
                      textAlign: TextAlign.center,
                    ),
                    SizedBox(height: 16),
                    TextField(
                      decoration: InputDecoration(
                        labelText: "Enter your name",
                        border: OutlineInputBorder(
                          borderRadius: BorderRadius.circular(12),
                        ),
                      ),
                      onChanged: (val) => tempName = val,
                      textAlign: TextAlign.center,
                      maxLength: 16,
                    ),
                    SizedBox(height: 8),
                    Text(
                      "Your name will appear on the leaderboard!",
                      style: TextStyle(fontSize: 13, color: Colors.grey[600]),
                      textAlign: TextAlign.center,
                    ),
                  ],
                ),
              ),
            ),
            actions: [
              TextButton(
                onPressed: () {
                  Navigator.of(context).pop(tempName);
                },
                child: Text("Submit"),
              ),
            ],
          ),
        );
      },
    );

    if (name != null && name.trim().isNotEmpty) {
      await _addToLeaderboardWithMode(name.trim(), _formatDuration(duration), gameDifficulty);
    }
    Navigator.of(context).popUntil((route) => route.isFirst);
  }

  Future<void> _addToLeaderboardWithMode(String name, String time, String difficulty) async {
    final prefs = await SharedPreferences.getInstance();
    final key = 'leaderboard_$difficulty';
    final List<String> leaderboard = prefs.getStringList(key) ?? [];
    leaderboard.add('$name|$time');
    print ("adding to the leaderboard with mode $difficulty: $name|$time");
    await prefs.setStringList(key, leaderboard);
  }

  String _formatDuration(Duration d) {
    String twoDigits(int n) => n.toString().padLeft(2, '0');
    return "${twoDigits(d.inMinutes)}:${twoDigits(d.inSeconds % 60)}";
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
      onWillPop: () async => false,
      child: Scaffold(
        appBar: AppBar(
          title: Text("Play Selected Games"),
          automaticallyImplyLeading: false,
        ),
        body: SafeArea(
          child: Container(
            decoration: BoxDecoration(
              gradient: LinearGradient(
                colors: [Colors.deepPurple.shade800, Colors.blue.shade600, Colors.cyan.shade400],
                begin: Alignment.topLeft,
                end: Alignment.bottomRight,
              ),
            ),
            child: ListView(
              padding: EdgeInsets.symmetric(vertical: 32, horizontal: 16),
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
                      onTap: (isGameStarting || isDone) ? null : () => _startGame(game),
                      child: Opacity(
                        opacity: isDone ? 0.4 : 1.0,
                        child: Container(
                          margin: EdgeInsets.symmetric(horizontal: 8, vertical: 8),
                          decoration: BoxDecoration(
                            border: Border.all(
                              color: isDone ? Colors.greenAccent.shade400 : Colors.transparent,
                              width: isDone ? 8 : 4,
                            ),
                            borderRadius: BorderRadius.circular(12),
                            color: isDone
                                ? Colors.greenAccent.withOpacity(0.18)
                                : Colors.white.withOpacity(0.13),
                          ),
                          padding: EdgeInsets.all(4),
                          child: Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: [
                              Expanded(
                                child: Stack(
                                  children: [
                                    Image.asset(
                                      gameImages[game] ?? '',
                                      width: 70,
                                      height: 70,
                                      fit: BoxFit.contain,
                                    ),
                                    if (!isDone)
                                      Positioned(
                                        top: 2,
                                        right: 2,
                                        child: GestureDetector(
                                          onTap: () {
                                            _helpDialogOpen = true;
                                            showDialog(
                                              context: context,
                                              builder: (_) => AlertDialog(
                                                title: Text("Game Info"),
                                                content: Text(getGameDescription(game)),
                                                actions: [
                                                  TextButton(
                                                    onPressed: () {
                                                      _helpDialogOpen = false;
                                                      Navigator.of(context).pop();
                                                    },
                                                    child: Text("OK"),
                                                  ),
                                                ],
                                              ),
                                            ).then((_) {
                                              _helpDialogOpen = false;
                                            });
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
                                "${game[0].toUpperCase()}${game.substring(1)}",
                                style: TextStyle(
                                  color: isDone ? Colors.greenAccent.shade400 : Colors.white,
                                  fontWeight: FontWeight.bold,
                                  fontSize: 13,
                                ),
                                textAlign: TextAlign.center,
                              ),
                              if (isDone)
                                Padding(
                                  padding: const EdgeInsets.only(top: 2.0),
                                  child: Icon(Icons.check, color: Colors.greenAccent.shade400, size: 20),
                                ),
                            ],
                          ),
                        ),
                      ),
                    );
                  }).toList(),
                ),
                SizedBox(height: 40),
                Padding(
                  padding: const EdgeInsets.only(bottom: 24.0, top: 8.0),
                  child: ElevatedButton(
                    style: ElevatedButton.styleFrom(
                      backgroundColor: Colors.red,
                      foregroundColor: Colors.white,
                      padding: EdgeInsets.symmetric(horizontal: 40, vertical: 18),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(16),
                      ),
                      textStyle: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                    ),
                    onPressed: completedGames.length == widget.selectedGames.length
                        ? null
                        : () {
                            widget.connection?.output.add(Uint8List.fromList("exit\n".codeUnits));
                            Navigator.of(context).popUntil((route) => route.isFirst);
                          },
                    child: Text("Exit game"),
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}