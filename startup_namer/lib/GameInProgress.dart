import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'Maze/Maze.dart';

class GameSelectionScreen extends StatefulWidget {
  final List<String> selectedGames;
  final BluetoothConnection? connection;

  // You can pass maze settings or other game settings as needed
  final int mazeTime;
  final int mazeVision;
  final List<List<int>>? maze; // Pass the maze if you want to use a specific one

  const GameSelectionScreen({
    required this.selectedGames,
    required this.connection,
    this.mazeTime = 90,
    this.mazeVision = 1,
    this.maze,
  });

  @override
  State<GameSelectionScreen> createState() => _GameSelectionScreenState();
}

class _GameSelectionScreenState extends State<GameSelectionScreen> {
  late List<String> remainingGames;
  Set<String> completedGames = {};

  @override
  void initState() {
    super.initState();
    remainingGames = List<String>.from(widget.selectedGames);
  }

  Future<void> _startGame(String game) async {
    if (game == "maze") {
      await startMazeGame(
        menuContext: context,
        connection: widget.connection,
        timeToCompleteMaze: widget.mazeTime,
        visionAtMaze: widget.mazeVision,
      );
    } 
     else {
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
      completedGames.add(game);
    });
  }

  @override
  Widget build(BuildContext context) {
    final gameImages = {
      "maze": 'assets/maze.png',
      "snake": 'assets/snake.png',
      "minesweeper": 'assets/minesweeper.png',
    };

    return Scaffold(
      appBar: AppBar(title: Text("Play Selected Games")),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text("Tap a game to play it:", style: TextStyle(fontSize: 18)),
            SizedBox(height: 20),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: widget.selectedGames.map((game) {
                final isDone = completedGames.contains(game);
                return GestureDetector(
                  onTap: isDone ? null : () => _startGame(game),
                  child: Opacity(
                    opacity: isDone ? 0.4 : 1.0,
                    child: Container(
                      margin: EdgeInsets.symmetric(horizontal: 16),
                      decoration: BoxDecoration(
                        border: Border.all(
                          color: isDone ? Colors.green : Colors.blue,
                          width: 4,
                        ),
                        borderRadius: BorderRadius.circular(12),
                      ),
                      padding: EdgeInsets.all(4),
                      child: Column(
                        children: [
                          Image.asset(
                            gameImages[game] ?? '',
                            width: 100,
                            height: 100,
                          ),
                          SizedBox(height: 8),
                          Text(
                            "${game[0].toUpperCase()}${game.substring(1)}",
                            style: TextStyle(
                              color: isDone ? Colors.green : Colors.black,
                              fontWeight: FontWeight.bold,
                            ),
                          ),
                          if (isDone)
                            Padding(
                              padding: const EdgeInsets.only(top: 4.0),
                              child: Icon(Icons.check, color: Colors.green),
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
                style: TextStyle(fontSize: 20, color: Colors.green),
              ),
          ],
        ),
      ),
    );
  }
}