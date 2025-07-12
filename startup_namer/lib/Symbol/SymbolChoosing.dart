import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'Symbol.dart';
import '../globals.dart';

class SymbolChoosingScreen extends StatefulWidget {
  final BluetoothConnection? connection;
  final List<List<String>> symbols; // The correct symbols for each level

  const SymbolChoosingScreen({
    required this.connection,
    required this.symbols,
    Key? key,
  }) : super(key: key);

  @override
  State<SymbolChoosingScreen> createState() => _SymbolChoosingScreenState();
}

class _SymbolChoosingScreenState extends State<SymbolChoosingScreen> {
  int currentLevel = 0;
  bool isProcessing = false;
  StreamSubscription<Uint8List>? _symbolSubscription;

  @override
  void initState() {
    super.initState();
    // Listen for game_over_w and game_over_l messages
    if (widget.connection?.input != null && globalInputBroadcast == null) {
      globalInputBroadcast = widget.connection!.input!.asBroadcastStream();
    }
    _listenForGameOver();
  }

  void _listenForGameOver() {
    _symbolSubscription = globalInputBroadcast?.listen((Uint8List data) {
      final msg = String.fromCharCodes(data).trim();
      if (msg == "game_over_w") {
        if (mounted) {
          setState(() {
            isProcessing = false;
          });
          Navigator.of(context).pop("win");
        }
      } else if (msg == "game_over_l") {
        print ("Game over: lose");
        if (mounted) {
          setState(() {
            isProcessing = false;
          });
          Navigator.of(context).pop("lose");
        }
      }
    });
  }

  @override
  void dispose() {
    _symbolSubscription?.cancel();
    super.dispose();
  }

void _onSymbolSelected(int selectedIndex) async {
  if (isProcessing) return;
  setState(() {
    isProcessing = true;
  });

  int correctIndex = SymbolPatternsHelper.getSymbolIndex(widget.symbols[currentLevel]);

  if (selectedIndex == correctIndex) {
    widget.connection?.output.add(Uint8List.fromList("symbol\n".codeUnits));
    widget.connection?.output.add(Uint8List.fromList("pass\n".codeUnits));
    print ("Correct symbol selected for level ${currentLevel + 1}");

    // If there are more levels, go to next level after a short delay
    if (currentLevel < widget.symbols.length - 1) {
      await Future.delayed(Duration(milliseconds: 600));
      setState(() {
        currentLevel++;
        isProcessing = false;
      });
    }
  } else {
    print ("Incorrect symbol selected for level ${currentLevel + 1}");
    widget.connection?.output.add(Uint8List.fromList("symbol\n".codeUnits));
    widget.connection?.output.add(Uint8List.fromList("fail\n".codeUnits));
    // Wait for game_over_l from ESP
  }
}

  @override
  Widget build(BuildContext context) {
    // Show all 8 symbols as options
    return WillPopScope(
      onWillPop: () async => false, // Prevent back navigation
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
              child: Padding(
                padding: const EdgeInsets.all(24.0),
                child: SingleChildScrollView(
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      Text(
                        "Choose the correct symbol",
                        style: TextStyle(
                          fontSize: 28,
                          fontWeight: FontWeight.bold,
                          color: Colors.amberAccent,
                          letterSpacing: 1.2,
                          shadows: [
                            Shadow(
                              blurRadius: 8,
                              color: Colors.black54,
                              offset: Offset(2, 2),
                            ),
                          ],
                        ),
                      ),
                      SizedBox(height: 16),
                      Text(
                        "Level ${currentLevel + 1} of ${widget.symbols.length}",
                        style: TextStyle(fontSize: 18, color: Colors.white),
                      ),
                      SizedBox(height: 24),
                      GridView.builder(
                        shrinkWrap: true,
                        itemCount: symbolPatterns.length,
                        gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
                          crossAxisCount: 4,
                          mainAxisSpacing: 16,
                          crossAxisSpacing: 16,
                          childAspectRatio: 1,
                        ),
                        itemBuilder: (context, idx) {
                          return GestureDetector(
                            onTap: isProcessing ? null : () => _onSymbolSelected(idx),
                            child: Container(
                              decoration: BoxDecoration(
                                color: Colors.white.withOpacity(0.10),
                                borderRadius: BorderRadius.circular(14),
                                border: Border.all(
                                  color: Colors.amberAccent,
                                  width: 2,
                                ),
                              ),
                              padding: EdgeInsets.all(4),
                              child: CustomPaint(
                                size: Size(48, 48),
                                painter: SymbolPainter(symbolPatterns[idx]),
                              ),
                            ),
                          );
                        },
                      ),
                      if (isProcessing)
                        Padding(
                          padding: const EdgeInsets.only(top: 24.0),
                          child: CircularProgressIndicator(color: Colors.amberAccent),
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

// Helper to get the index of a symbol in symbolPatterns
class SymbolPatternsHelper {
  static int getSymbolIndex(List<String> symbol) {
    for (int i = 0; i < symbolPatterns.length; i++) {
      bool match = true;
      for (int j = 0; j < 16; j++) {
        if (symbolPatterns[i][j] != symbol[j]) {
          match = false;
          break;
        }
      }
      if (match) return i;
    }
    return -1;
  }
}

// Paints a 16x16 symbol as a grid of squares
class SymbolPainter extends CustomPainter {
  final List<String> symbol;
  SymbolPainter(this.symbol);

  @override
  void paint(Canvas canvas, Size size) {
    final paintOn = Paint()..color = Colors.amberAccent;
    final paintOff = Paint()..color = Colors.transparent;
    double cellSize = size.width / 16;
    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        bool on = symbol[y][x] == '1';
        canvas.drawRect(
          Rect.fromLTWH(x * cellSize, y * cellSize, cellSize, cellSize),
          on ? paintOn : paintOff,
        );
      }
    }
  }

  @override
  bool shouldRepaint(covariant SymbolPainter oldDelegate) => false;
}