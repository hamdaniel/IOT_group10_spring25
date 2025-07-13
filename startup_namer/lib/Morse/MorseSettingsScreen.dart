import 'package:flutter/material.dart';

class MorseSettingsScreen extends StatefulWidget {
  @override
  State<MorseSettingsScreen> createState() => _MorseSettingsScreenState();
}

class _MorseSettingsScreenState extends State<MorseSettingsScreen> {
  int wordLength = 4;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
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
              child: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                    "Morse Code Settings",
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
                  SizedBox(height: 32),
                  Text("Word Length: $wordLength",
                      style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white)),
                  Slider(
                    value: wordLength.toDouble(),
                    min: 3,
                    max: 6,
                    divisions: 3,
                    label: wordLength.toString(),
                    onChanged: (val) {
                      setState(() {
                        wordLength = val.round();
                      });
                    },
                  ),
                  SizedBox(height: 30),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Expanded(
                        child: ElevatedButton(
                          child: Text("Save", style: TextStyle(fontSize: 18, color: Colors.white)),
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.green,
                            elevation: 6,
                            shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                            padding: EdgeInsets.symmetric(horizontal: 0, vertical: 16),
                          ),
                          onPressed: () {
                            Navigator.pop(context, {'wordLength': wordLength});
                          },
                        ),
                      ),
                      SizedBox(width: 20),
                      Expanded(
                        child: ElevatedButton.icon(
                          icon: Icon(Icons.arrow_back, color: Colors.white),
                          label: Text("Back", style: TextStyle(fontSize: 18, color: Colors.white)),
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.redAccent,
                            elevation: 6,
                            shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                            padding: EdgeInsets.symmetric(horizontal: 0, vertical: 16),
                          ),
                          onPressed: () => Navigator.pop(context),
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
    );
  }
}