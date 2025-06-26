import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

Future<String> startMorseGame({
  required BuildContext menuContext,
  required BluetoothConnection? connection,
  required int wordLength,
}) async {
  final List<String> morseWords = [
    "jam", "vet", "mud", "jump", "blitz", "crisp", "whisk", "plumb", "drink", "brick",
    "lunch", "grind", "proud", "frown", "glove", "march", "squid", "planet", "injury", "blaze"
  ];
  final filtered = morseWords.where((w) => w.length == wordLength).toList();
  final wordList = filtered.isNotEmpty ? filtered : morseWords;
  final word = (wordList..shuffle()).first;
  final mutated = String.fromCharCodes(word.runes.toList()..shuffle());

  // Send mutated word to ESP32
  connection?.output.add(Uint8List.fromList('$mutated\n'.codeUnits));

  // Show the Morse wait screen and return the result ("win" or "lose")
  final result = await Navigator.of(menuContext).push<String>(
    MaterialPageRoute(
      builder: (_) => MorseWaitScreen(connection: connection),
    ),
  );

  return result ?? "lose";
}

class MorseWaitScreen extends StatefulWidget {
  final BluetoothConnection? connection;
  const MorseWaitScreen({required this.connection, super.key});

  @override
  State<MorseWaitScreen> createState() => _MorseWaitScreenState();
}

class _MorseWaitScreenState extends State<MorseWaitScreen> {
  StreamSubscription? _sub;

  static const Map<String, String> morseMap = {
    'A': '.-',    'B': '-...',  'C': '-.-.',  'D': '-..',   'E': '.',
    'F': '..-.',  'G': '--.',   'H': '....',  'I': '..',    'J': '.---',
    'K': '-.-',   'L': '.-..',  'M': '--',    'N': '-.',    'O': '---',
    'P': '.--.',  'Q': '--.-',  'R': '.-.',   'S': '...',   'T': '-',
    'U': '..-',   'V': '...-',  'W': '.--',   'X': '-..-',  'Y': '-.--',
    'Z': '--..'
  };

  @override
  void initState() {
    super.initState();
    _sub = widget.connection?.input?.listen((Uint8List data) {
      final msg = String.fromCharCodes(data).trim();
      if (msg == "game_over_w") {
        _sub?.cancel();
        Navigator.of(context).pop("win");
      } else if (msg == "game_over_l") {
        _sub?.cancel();
        Navigator.of(context).pop("lose");
      }
    });
  }

  @override
  void dispose() {
    _sub?.cancel();
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
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 32, vertical: 32),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Icon(Icons.code, size: 60, color: Colors.amberAccent),
                    SizedBox(height: 16),
                    Text(
                      "Morse Code Chart",
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
                    SizedBox(height: 24),
                    SizedBox(
                      width: 420,
                      child: GridView.count(
                        crossAxisCount: 3,
                        shrinkWrap: true,
                        mainAxisSpacing: 16,
                        crossAxisSpacing: 16,
                        childAspectRatio: 2.2,
                        physics: NeverScrollableScrollPhysics(),
                        children: morseMap.entries.map((entry) {
                          return Container(
                            decoration: BoxDecoration(
                              color: Colors.white.withOpacity(0.10),
                              borderRadius: BorderRadius.circular(14),
                            ),
                            padding: EdgeInsets.symmetric(vertical: 8, horizontal: 16),
                            child: Row(
                              mainAxisAlignment: MainAxisAlignment.spaceBetween,
                              children: [
                                Text(
                                  entry.key,
                                  style: TextStyle(
                                    color: Colors.amberAccent,
                                    fontWeight: FontWeight.bold,
                                    fontSize: 25,
                                  ),
                                ),
                                Row(
                                  children: entry.value.split('').map((symbol) {
                                    String displaySymbol = symbol == '.' ? '●' : '—';
                                    return Padding(
                                      padding: const EdgeInsets.symmetric(horizontal: 2.0),
                                      child: Text(
                                        displaySymbol,
                                        style: TextStyle(
                                          color: Colors.white,
                                          fontSize: 10,
                                          fontWeight: FontWeight.bold,
                                        ),
                                      ),
                                    );
                                  }).toList(),
                                ),
                              ],
                            ),
                          );
                        }).toList(),
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}