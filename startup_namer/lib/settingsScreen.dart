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
  late TextEditingController _timeController;
  late TextEditingController _visionController;

  @override
  void initState() {
    super.initState();
    _timeController = TextEditingController(text: widget.initialTime.toString());
    _visionController = TextEditingController(text: widget.initialVision.toString());
  }

  @override
  void dispose() {
    _timeController.dispose();
    _visionController.dispose();
    super.dispose();
  }

  void _saveSettings() {
    final int? time = int.tryParse(_timeController.text);
    final int? vision = int.tryParse(_visionController.text);

    if (time == null || vision == null) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Please enter valid numbers.")),
      );
      return;
    }

    Navigator.pop(context, {'time': time, 'vision': vision});
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
            TextField(
              controller: _timeController,
              keyboardType: TextInputType.number,
              decoration: InputDecoration(
                labelText: "Time To Complete Maze (seconds)",
                border: OutlineInputBorder(),
              ),
            ),
            SizedBox(height: 20),
            TextField(
              controller: _visionController,
              keyboardType: TextInputType.number,
              decoration: InputDecoration(
                labelText: "Vision At Maze (pixels)",
                border: OutlineInputBorder(),
              ),
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