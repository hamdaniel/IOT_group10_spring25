import 'dart:math';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

class CompassDialog extends StatefulWidget {
  final BluetoothConnection? connection;
  final Point<int> start;
  final Point<int> end;

  const CompassDialog({
    required this.connection,
    required this.start,
    required this.end,
  });

  @override
  State<CompassDialog> createState() => _CompassDialogState();
}

class _CompassDialogState extends State<CompassDialog> {
  late Point<int> player;
  double? angle;

  @override
  void initState() {
    super.initState();
    // Initialize player with start position before first ESP message
    player = widget.start;
    angle = atan2(widget.end.x - player.x, widget.end.y - player.y);
    _listenForPlayer();
  }

  void _listenForPlayer() async {
    widget.connection?.input?.listen((Uint8List data) {
      String msg = String.fromCharCodes(data).trim();
      // Expecting a string of length 3, e.g., "220"
      if (msg.length == 3 && int.tryParse(msg) != null) {
        int value = int.parse(msg);
        int x = value ~/ 16;
        int y = value % 16;
        setState(() {
          player = Point(x, y);
          angle = atan2(widget.end.x - x, widget.end.y - y);
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: Text('Compass to End'),
      content: SizedBox(
        width: 200,
        height: 200,
        child: CustomPaint(
          painter: CompassPainter(angle ?? 0),
          child: Container(),
        ),
      ),
    );
  }
}

class CompassPainter extends CustomPainter {
  final double angle;
  CompassPainter(this.angle);

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width / 2 - 20;

    // Draw gradient background
    final rect = Rect.fromCircle(center: center, radius: radius + 20);
    final gradient = RadialGradient(
      colors: [Colors.white, Colors.blue.shade100],
      stops: [0.7, 1.0],
    );
    canvas.drawRect(rect, Paint()..shader = gradient.createShader(rect));

    // Draw outer border
    canvas.drawCircle(center, radius + 10, Paint()
      ..color = Colors.blueGrey
      ..style = PaintingStyle.stroke
      ..strokeWidth = 4);

    // Draw main compass circle
    canvas.drawCircle(center, radius, Paint()
      ..color = Colors.grey.shade300
      ..style = PaintingStyle.fill);

    // Draw tick marks and direction labels
    final tickPaint = Paint()
      ..color = Colors.black
      ..strokeWidth = 2;
    final textStyle = TextStyle(
      color: Colors.black,
      fontWeight: FontWeight.bold,
      fontSize: 16,
    );
    const directions = ['N', 'E', 'S', 'W'];
    for (int i = 0; i < 4; i++) {
      final tickAngle = pi / 2 * i - pi / 2;
      final tickStart = Offset(
        center.dx + (radius - 8) * cos(tickAngle),
        center.dy + (radius - 8) * sin(tickAngle),
      );
      final tickEnd = Offset(
        center.dx + (radius + 8) * cos(tickAngle),
        center.dy + (radius + 8) * sin(tickAngle),
      );
      canvas.drawLine(tickStart, tickEnd, tickPaint);

      // Draw direction labels
      final textPainter = TextPainter(
        text: TextSpan(text: directions[i], style: textStyle),
        textDirection: TextDirection.ltr,
      )..layout();
      final labelOffset = Offset(
        center.dx + (radius + 18) * cos(tickAngle) - textPainter.width / 2,
        center.dy + (radius + 18) * sin(tickAngle) - textPainter.height / 2,
      );
      textPainter.paint(canvas, labelOffset);
    }

    // Draw minor tick marks
    for (int i = 0; i < 36; i++) {
      final tickAngle = pi * 2 * i / 36;
      final tickStart = Offset(
        center.dx + (radius - 4) * cos(tickAngle),
        center.dy + (radius - 4) * sin(tickAngle),
      );
      final tickEnd = Offset(
        center.dx + radius * cos(tickAngle),
        center.dy + radius * sin(tickAngle),
      );
      canvas.drawLine(tickStart, tickEnd, tickPaint..strokeWidth = 1);
    }

    // Draw arrow
    final arrowPaint = Paint()
      ..color = Colors.red
      ..strokeWidth = 6
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round;
    final arrowEnd = Offset(
      center.dx + (radius - 20) * cos(angle),
      center.dy + (radius - 20) * sin(angle),
    );
    canvas.drawLine(center, arrowEnd, arrowPaint);

    // Draw arrowhead
    final arrowHeadLength = 24.0;
    final arrowHeadAngle = pi / 7;
    final path = Path();
    path.moveTo(arrowEnd.dx, arrowEnd.dy);
    path.lineTo(
      arrowEnd.dx - arrowHeadLength * cos(angle - arrowHeadAngle),
      arrowEnd.dy - arrowHeadLength * sin(angle - arrowHeadAngle),
    );
    path.moveTo(arrowEnd.dx, arrowEnd.dy);
    path.lineTo(
      arrowEnd.dx - arrowHeadLength * cos(angle + arrowHeadAngle),
      arrowEnd.dy - arrowHeadLength * sin(angle + arrowHeadAngle),
    );
    canvas.drawPath(path, arrowPaint..strokeWidth = 4);

    // Draw center circle
    canvas.drawCircle(center, 10, Paint()..color = Colors.blueAccent);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => true;
}