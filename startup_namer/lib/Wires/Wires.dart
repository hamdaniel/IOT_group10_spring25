import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import '../globals.dart';

Future<String> startWiresGame({
  required BuildContext menuContext,
  required BluetoothConnection? connection,
  required int attempts,
  required int number,
}) async {
  connection?.output.add(Uint8List.fromList("wires\n".codeUnits));
  connection?.output.add(Uint8List.fromList("$number\n".codeUnits));
  connection?.output.add(Uint8List.fromList("$attempts\n".codeUnits));

  final result = await Navigator.of(menuContext).push<String>(
    MaterialPageRoute(
      builder: (_) => WiresGameScreen(
        connection: connection,
        attempts: attempts,
        number: number,
      ),
    ),
  );

  return result ?? "lose";
}

class WiresGameScreen extends StatefulWidget {
  final BluetoothConnection? connection;
  final int attempts;
  final int number;

  const WiresGameScreen({
    required this.connection,
    required this.attempts,
    required this.number,
    Key? key,
  }) : super(key: key);

  @override
  State<WiresGameScreen> createState() => _WiresGameScreenState();
}

class _WiresGameScreenState extends State<WiresGameScreen> {
  StreamSubscription<Uint8List>? _wiresSubscription;
  int currentAttempt = 1;
  int? perfect;
  int? leftCorrectRightNo;
  int? rightCorrectLeftNo;
  bool isGameOver = false;
  String? gameResult;

  @override
  void initState() {
    super.initState();
    if (widget.connection?.input != null && globalInputBroadcast == null) {
      globalInputBroadcast = widget.connection!.input!.asBroadcastStream();
    }
    _listenForGameResult();
  }

  void _listenForGameResult() {
    _wiresSubscription = globalInputBroadcast?.listen((Uint8List data) {
      final msg = String.fromCharCodes(data).trim();
      if (msg == "game_over_w") {
        setState(() {
          isGameOver = true;
          gameResult = "win";
        });
        Future.delayed(Duration(milliseconds: 500), () {
          if (mounted) Navigator.of(context).pop("win");
        });
      } else if (msg == "game_over_l") {
        setState(() {
          isGameOver = true;
          gameResult = "lose";
        });
        Future.delayed(Duration(milliseconds: 500), () {
          if (mounted) Navigator.of(context).pop("lose");
        });
      } else {
        final parts = msg.split(',');
        if (parts.length == 3) {
          setState(() {
            perfect = int.tryParse(parts[0]);
            leftCorrectRightNo = int.tryParse(parts[1]);
            rightCorrectLeftNo = int.tryParse(parts[2]);
            currentAttempt++;
          });
        }
      }
    });
  }

  @override
  void dispose() {
    _wiresSubscription?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      onWillPop: () async => false,
      child: Scaffold(
        body: Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              colors: [Colors.deepPurple.shade800, Colors.blue.shade600, Colors.cyan.shade400],
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
            ),
          ),
          child: Center(
            child: Card(
              color: Colors.white.withOpacity(0.13),
              shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(28)),
              elevation: 12,
              child: Container(
                width: 350,
                padding: const EdgeInsets.symmetric(horizontal: 32, vertical: 32),
                child: SingleChildScrollView(
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    mainAxisAlignment: MainAxisAlignment.center, 
                    children: [
                      Icon(Icons.cable, size: 60, color: Colors.amberAccent),
                      SizedBox(height: 16),
                      Text(
                        "Wires Game",
                        style: TextStyle(
                          fontSize: 32,
                          fontWeight: FontWeight.bold,
                          color: Colors.amberAccent,
                          letterSpacing: 2,
                          shadows: [
                            Shadow(
                              blurRadius: 8,
                              color: Colors.black54,
                              offset: Offset(2, 2),
                            ),
                          ],
                        ),
                      ),
                      SizedBox(height: 8),
                      // Show total wires to connect
                      Text(
                        "Total wires to connect: ${widget.number}",
                        style: TextStyle(
                          fontSize: 20,
                          color: Colors.white70,
                          fontWeight: FontWeight.w600,
                        ),
                      ),
                      SizedBox(height: 16),
                      Text(
                        "Attempts left: ${widget.attempts - currentAttempt + 1}",
                        style: TextStyle(fontSize: 20, color: Colors.white),
                      ),
                      SizedBox(height: 16),
                      Column(
                        children: [
                          Text(
                            "Perfect wires: ${perfect ?? '-'}",
                            style: TextStyle(
                              fontSize: 18,
                              color: Colors.lightGreenAccent.shade400,
                              fontWeight: FontWeight.bold,
                              shadows: [
                                Shadow(
                                  blurRadius: 4,
                                  color: Colors.black45,
                                  offset: Offset(1, 1),
                                ),
                              ],
                            ),
                          ),
                          Text(
                            "Correct pin on left only: ${leftCorrectRightNo ?? '-'}",
                            style: TextStyle(
                              fontSize: 18,
                              color: Colors.orangeAccent.shade200,
                              fontWeight: FontWeight.bold,
                              shadows: [
                                Shadow(
                                  blurRadius: 4,
                                  color: Colors.black45,
                                  offset: Offset(1, 1),
                                ),
                              ],
                            ),
                          ),
                          Text(
                            "Correct pin on right only: ${rightCorrectLeftNo ?? '-'}",
                            style: TextStyle(
                              fontSize: 18,
                              color: Colors.cyanAccent.shade400,
                              fontWeight: FontWeight.bold,
                              shadows: [
                                Shadow(
                                  blurRadius: 4,
                                  color: Colors.black45,
                                  offset: Offset(1, 1),
                                ),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}