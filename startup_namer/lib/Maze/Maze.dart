import 'dart:math';
import 'dart:collection';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'MazeCompass.dart';

Future<String?> startMazeGame({
  required BuildContext menuContext,
  required BluetoothConnection? connection,
  required int timeToCompleteMaze,
  required int visionAtMaze,
}) async {
  // 1. Generate the maze
  final int size = 16;
  final rand = Random();
  List<List<int>> maze = List.generate(size, (_) => List.generate(size, (_) => 1));
  List<Point<int>> edgeCells = [];
  for (int i = 1; i < size; i += 2) {
    edgeCells.add(Point(i, 0));
    edgeCells.add(Point(i, size - 1));
    edgeCells.add(Point(0, i));
    edgeCells.add(Point(size - 1, i));
  }
  connection?.output.add(Uint8List.fromList("maze\n".codeUnits));
  Point<int> start = edgeCells[rand.nextInt(edgeCells.length)];
  void carve(int x, int y) {
    maze[x][y] = 0;
    var dirs = [
      [0, 2],
      [0, -2],
      [2, 0],
      [-2, 0],
    ]..shuffle(rand);
    for (var d in dirs) {
      int nx = x + d[0], ny = y + d[1];
      if (nx > 0 && nx < size && ny > 0 && ny < size && maze[nx][ny] == 1) {
        maze[x + d[0] ~/ 2][y + d[1] ~/ 2] = 0;
        carve(nx, ny);
      }
    }
  }
  carve(start.x, start.y);

  // 2. Find the farthest edge cell as end
  Point<int> end = start;
  int maxDist = -1;
  List<List<int>> dist = List.generate(size, (_) => List.filled(size, -1));
  Queue<Point<int>> queue = Queue();
  queue.add(start);
  dist[start.x][start.y] = 0;
  while (queue.isNotEmpty) {
    Point<int> p = queue.removeFirst();
    for (var d in [
      Point(0, 1),
      Point(1, 0),
      Point(0, -1),
      Point(-1, 0),
    ]) {
      int nx = p.x + d.x, ny = p.y + d.y;
      if (nx >= 0 && nx < size && ny >= 0 && ny < size &&
          maze[nx][ny] == 0 && dist[nx][ny] == -1) {
        dist[nx][ny] = dist[p.x][p.y] + 1;
        queue.add(Point(nx, ny));
        if ((nx == 0 || nx == size - 1 || ny == 0 || ny == size - 1) &&
            dist[nx][ny] > maxDist) {
          maxDist = dist[nx][ny];
          end = Point(nx, ny);
        }
      }
    }
  }
  maze[start.x][start.y] = 2;
  maze[end.x][end.y] = 3;


  String mazeString = maze.expand((row) => row).join();
  String time_send = "${timeToCompleteMaze.toString().padLeft(3, '0')}";
  String vision_send = "${visionAtMaze.toString()}";
  if (connection != null && connection.isConnected) {
    connection.output.add(Uint8List.fromList("${mazeString}\n${time_send}\n${vision_send}\n".codeUnits));
  }

  final result = await showDialog<String>(
    context: menuContext,
    barrierDismissible: false,
    builder: (context) => CompassDialog(
      connection: connection,
      start: start,
      end: end,
    ),
  );
  return result; 
}