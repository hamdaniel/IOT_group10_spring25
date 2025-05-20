import 'dart:math';
import 'package:flutter/material.dart';

class MazeWidget extends StatelessWidget {
  final List<List<int>> maze;
  final Point<int> start;
  final Point<int> end;

  const MazeWidget({required this.maze, required this.start, required this.end});

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 240,
      height: 240,
      child: GridView.builder(
        gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
          crossAxisCount: maze.length,
        ),
        itemCount: maze.length * maze.length,
        itemBuilder: (context, index) {
          int x = index ~/ maze.length;
          int y = index % maze.length;
          Color color;
          if (x == start.x && y == start.y) {
            color = Colors.green;
          } else if (x == end.x && y == end.y) {
            color = Colors.red;
          } else {
            color = maze[x][y] == 0 ? Colors.white : Colors.black;
          }
          return Container(
            margin: EdgeInsets.all(1),
            color: color,
          );
        },
      ),
    );
  }
}