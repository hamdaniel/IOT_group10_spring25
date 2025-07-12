import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import '../globals.dart';

Future<String> startMorseGame({
  required BuildContext menuContext,
  required BluetoothConnection? connection,
  required int wordLength,
}) async {
  final List<String> morseWords = [
    "jam", "vet", "mud", "jump","wolf","bulb","jerk","quiz", "blitz", "crisp", "whisk", "plumb", "drink", "brick",
    "lunch", "grind", "proud", "frown", "glove", "march", "squid", "planet", "injury", "blaze"
  ];
  final filtered = morseWords.where((w) => w.length == wordLength).toList();
  final wordList = filtered.isNotEmpty ? filtered : morseWords;
  final word = (wordList..shuffle()).first;

  connection?.output.add(Uint8List.fromList("morse\n".codeUnits));
  connection?.output.add(Uint8List.fromList('$word\n'.codeUnits));

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
  StreamSubscription<Uint8List>? _morseSubscription;
  bool _helpDialogOpen = false;

  static const Map<String, String> morseMap = {
    'A': '.-',    'B': '-...',  'C': '-.-.',  'D': '-..',   'E': '.',
    'F': '..-.',  'G': '--.',   'H': '....',  'I': '..',    'J': '.---',
    'K': '-.-',   'L': '.-..',  'M': '--',    'N': '-.',    'O': '---',
    'P': '.--.',  'Q': '--.-',  'R': '.-.',   'S': '...',   'T': '-',
    'U': '..-',   'V': '...-',  'W': '.--',   'X': '-..-',  'Y': '-.--',
    'Z': '--..'
  };

  String getGameDescription() {
    return "Decode the Morse code shown on the LED and unscramble the letters to form a real word. Then, for a word of length n with letters l₁, l₂, ..., lₙ (where lᵢ is the alphabetical position of the i-th letter), calculate the sum:\n\nS = 1×l₁ + 2×l₂ + ... + n×lₙ = ∑ᵢ₌₁ⁿ (i × lᵢ)\n\nThen, compute S mod 3:\n0: 1 long press on the button\n1: 1 short press on the button\n2: 2 short presses on the button";
  }

  @override
  void initState() {
    super.initState();
    // Set up the global broadcast stream only once per connection
    if (widget.connection?.input != null && globalInputBroadcast == null) {
      globalInputBroadcast = widget.connection!.input!.asBroadcastStream();
    }
    _listenForGameResult();
  }

  void _listenForGameResult() {
    _morseSubscription = globalInputBroadcast?.listen((Uint8List data) async {
      final msg = String.fromCharCodes(data).trim();
      if (msg == "game_over_w" || msg == "game_over_l") {
        if (_helpDialogOpen) {
          Navigator.of(context, rootNavigator: true).pop();
          _helpDialogOpen = false;
          // Wait a short moment to ensure the dialog is closed
          await Future.delayed(Duration(milliseconds: 100));
        }
        Navigator.of(context).pop(msg == "game_over_w" ? "win" : "lose");
      }
    });
  }

  @override
  void dispose() {
    _morseSubscription?.cancel();
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
              child: Stack(
                children: [
                  Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 32, vertical: 32),
                    child: SingleChildScrollView(
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
                                  padding: EdgeInsets.symmetric(vertical: 8, horizontal: 8), // reduce horizontal padding
                                  child: Row(
                                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                    children: [
                                      Text(
                                        entry.key,
                                        style: TextStyle(
                                          color: Colors.amberAccent,
                                          fontWeight: FontWeight.bold,
                                          fontSize: 22,
                                        ),
                                      ),
                                      Flexible(
                                        child: Wrap(
                                          spacing: 2,
                                          children: entry.value.split('').map((symbol) {
                                            String displaySymbol = symbol == '.' ? '●' : '—';
                                            return Text(
                                              displaySymbol,
                                              style: TextStyle(
                                                color: Colors.white,
                                                fontSize: 10,
                                                fontWeight: FontWeight.bold,
                                              ),
                                            );
                                          }).toList(),
                                        ),
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
                  // Question mark icon at the top right of the card
                  Positioned(
                    top: 8,
                    right: 8,
                    child: IconButton(
                      icon: Icon(Icons.help_outline, color: Colors.amberAccent, size: 32),
                      splashColor: Colors.transparent,
                      highlightColor: Colors.transparent,
                      hoverColor: Colors.transparent,
                      onPressed: () {
                        _helpDialogOpen = true;
                        showDialog(
                          context: context,
                          builder: (_) => AlertDialog(
                            title: Text("Game Info"),
                            content: Text(getGameDescription(), style: TextStyle(fontSize: 15)),
                            actions: [
                              TextButton(
                                onPressed: () {
                                  _helpDialogOpen = false;
                                  Navigator.of(context).pop();
                                },
                                child: Text("OK"),
                              ),
                            ],
                          ),
                        ).then((_) {
                          _helpDialogOpen = false;
                        });
                      },
                    ),
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}