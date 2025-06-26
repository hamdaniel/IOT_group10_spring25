import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'Maze/Maze.dart';
import 'Snake/Snake.dart';
import 'Morse/Morse.dart';

class GameSelectionScreen extends StatefulWidget {
  final List<String> selectedGames;
  final BluetoothConnection? connection;
  final int mazeTime;
  final int mazeVision;
  final List<List<int>>? maze;
  final int snakeScoreToBeat;
  final double snakeSpeed;
  final int morseWordLength;

  const GameSelectionScreen({
    required this.selectedGames,
    required this.connection,
    this.mazeTime = 90,
    this.mazeVision = 1,
    this.maze,
    this.snakeScoreToBeat = 20,
    this.snakeSpeed = 1,
    this.morseWordLength = 4,
  });

  @override
  State<GameSelectionScreen> createState() => _GameSelectionScreenState();
}

class _GameSelectionScreenState extends State<GameSelectionScreen> {
  Set<String> completedGames = {};

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
                              Image.asset(
                                gameImages[game] ?? '',
                                width: 90,
                                height: 90,
                              ),
                              SizedBox(height: 8),
                              Text(
                                "${game[0].toUpperCase()}${game.substring(1)}",
                                style: TextStyle(
                                  color: isDone ? Colors.greenAccent.shade400 : Colors.white,
                                  fontWeight: FontWeight.bold,
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
              ],
            ),
          ),
        ),
      ),
    );
  }
}