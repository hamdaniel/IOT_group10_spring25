import 'package:flutter/material.dart';

class SettingsScreen extends StatefulWidget {
  final int initialTime;
  final int initialVision;

  const SettingsScreen({
    required this.initialTime,
    required this.initialVision,
  });

  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  late int _time;
  late int _vision;

  @override
  void initState() {
    super.initState();
    _time = widget.initialTime.clamp(60, 150);
    _vision = widget.initialVision.clamp(0, 3);
  }

  void _saveSettings() {
    Navigator.pop(context, {'time': _time, 'vision': _vision});
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Change Settings")),
      body: Padding(
        padding: const EdgeInsets.all(24.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text("Time To Complete Maze (seconds): $_time"),
            Slider(
              value: _time.toDouble(),
              min: 60,
              max: 150,
              divisions: 90,
              label: _time.toString(),
              onChanged: (val) {
                setState(() {
                  _time = val.round();
                });
              },
            ),
            SizedBox(height: 20),
            Text("Vision At Maze (level): $_vision"),
            Slider(
              value: _vision.toDouble(),
              min: 0,
              max: 3,
              divisions: 3,
              label: _vision.toString(),
              onChanged: (val) {
                setState(() {
                  _vision = val.round();
                });
              },
            ),
            SizedBox(height: 30),
            ElevatedButton(
              child: Text("Save"),
              onPressed: _saveSettings,
            ),
          ],
        ),
      ),
    );
  }
}