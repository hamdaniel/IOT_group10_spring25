import 'dart:math';
import 'package:flutter/material.dart';

class MazeWidget extends StatelessWidget {
  final List<List<int>> maze;
  final Point<int> start;
  final Point<int> end;
  final Point<int>? player;

  const MazeWidget({
    required this.maze,
    required this.start,
    required this.end,
    this.player,
  });

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 240,
      height: 240,
      child: CustomPaint(
        painter: _MazePainter(
          maze: maze,
          start: start,
          end: end,
          player: player,
        ),
        child: Container(),
      ),
    );
  }
}

class _MazePainter extends CustomPainter {
  final List<List<int>> maze;
  final Point<int> start;
  final Point<int> end;
  final Point<int>? player;

  _MazePainter({
    required this.maze,
    required this.start,
    required this.end,
    this.player,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final cellSize = size.width / maze.length;
    final wallPaint = Paint()..color = Colors.black;
    final pathPaint = Paint()..color = Colors.white;
    final startPaint = Paint()..color = Colors.green;
    final endPaint = Paint()..color = Colors.red;
    final playerPaint = Paint()..color = Colors.blue;

    // Draw maze grid
    for (int x = 0; x < maze.length; x++) {
      for (int y = 0; y < maze.length; y++) {
        Rect rect = Rect.fromLTWH(y * cellSize, x * cellSize, cellSize, cellSize);
        if (x == start.x && y == start.y) {
          canvas.drawRect(rect, startPaint);
        } else if (x == end.x && y == end.y) {
          canvas.drawRect(rect, endPaint);
        } else if (maze[x][y] == 0) {
          canvas.drawRect(rect, pathPaint);
        } else {
          canvas.drawRect(rect, wallPaint);
        }
      }
    }

    // Draw player
    if (player != null) {
      final playerCenter = Offset(
        player!.y * cellSize + cellSize / 2,
        player!.x * cellSize + cellSize / 2,
      );
      canvas.drawCircle(playerCenter, cellSize / 3, playerPaint);

      // Draw arrow from player to end
      final endCenter = Offset(
        end.y * cellSize + cellSize / 2,
        end.x * cellSize + cellSize / 2,
      );
      final arrowPaint = Paint()
        ..color = Colors.orange
        ..strokeWidth = 3;

      // Draw line
      canvas.drawLine(playerCenter, endCenter, arrowPaint);

      // Draw arrowhead
      final angle = atan2(endCenter.dy - playerCenter.dy, endCenter.dx - playerCenter.dx);
      const arrowHeadLength = 12.0;
      const arrowHeadAngle = pi / 6;
      final path = Path();
      path.moveTo(endCenter.dx, endCenter.dy);
      path.lineTo(
        endCenter.dx - arrowHeadLength * cos(angle - arrowHeadAngle),
        endCenter.dy - arrowHeadLength * sin(angle - arrowHeadAngle),
      );
      path.moveTo(endCenter.dx, endCenter.dy);
      path.lineTo(
        endCenter.dx - arrowHeadLength * cos(angle + arrowHeadAngle),
        endCenter.dy - arrowHeadLength * sin(angle + arrowHeadAngle),
      );
      canvas.drawPath(path, arrowPaint);
    }
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => true;
}