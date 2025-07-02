import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import '../globals.dart';

class SnakeMatrixDialog extends StatefulWidget {
  final BluetoothConnection? connection;

  const SnakeMatrixDialog({required this.connection});

  @override
  State<SnakeMatrixDialog> createState() => _SnakeMatrixDialogState();
}

class _SnakeMatrixDialogState extends State<SnakeMatrixDialog> {
  int? appleX;
  int? appleY;
  StreamSubscription<Uint8List>? _snakeSubscription;

  @override
  void initState() {
    super.initState();
    // Set up the global broadcast stream only once per connection
    if (widget.connection?.input != null && globalInputBroadcast == null) {
      globalInputBroadcast = widget.connection!.input!.asBroadcastStream();
    }
    _listenForPlayer();
  }

  void _listenForPlayer() {
    _snakeSubscription = globalInputBroadcast?.listen((Uint8List data) {
      String msg = String.fromCharCodes(data).trim();
      if (msg == "game_over_w") {
        Navigator.of(context).pop("win");
      } else if (msg == "game_over_l") {
        Navigator.of(context).pop("lose");
      } else if (msg.length == 3 && int.tryParse(msg) != null) {
        int value = int.parse(msg);
        setState(() {
          appleX = value ~/ 16;
          appleY = value % 16;
        });
      }
    });
  }

  @override
  void dispose() {
    _snakeSubscription?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      onWillPop: () async => false, // Prevents back navigation
      child: AlertDialog(
        title: Text('Snake Game'),
        content: SizedBox(
          width: 320,
          height: 320,
          child: GridView.builder(
            gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
              crossAxisCount: 16,
            ),
            itemCount: 256,
            itemBuilder: (context, index) {
              int x = index ~/ 16;
              int y = index % 16;
              bool isApple = (x == appleX && y == appleY);
              return Container(
                margin: EdgeInsets.all(1),
                decoration: BoxDecoration(
                  color: isApple ? Colors.red : Colors.grey[300],
                  border: Border.all(color: Colors.black12),
                ),
              );
            },
          ),
        ),
      ),
    );
  }
}