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
      appBar: AppBar(title: Text("Morse Code Settings")),
      body: Padding(
        padding: const EdgeInsets.all(24.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text("Word Length: $wordLength",
                style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
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
            ElevatedButton(
              child: Text("Save"),
              onPressed: () {
                Navigator.pop(context, {'wordLength': wordLength});
              },
            ),
          ],
        ),
      ),
    );
  }
}