import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'SnakeMatrix.dart';

Future<String?> startSnakeGame({
  required BuildContext menuContext,
  required BluetoothConnection? connection,
  required int scoreToBeat,
  required double speed,
}) async {
  connection?.output.add(Uint8List.fromList("snake\n".codeUnits));
  connection?.output.add(Uint8List.fromList("$scoreToBeat\n".codeUnits));
  connection?.output.add(Uint8List.fromList("${speed.toStringAsFixed(1)}\n".codeUnits));

  final result = await showDialog<String>(
    context: menuContext,
    barrierDismissible: false,
    builder: (context) => SnakeMatrixDialog(connection: connection),
  );

  return result; 
}