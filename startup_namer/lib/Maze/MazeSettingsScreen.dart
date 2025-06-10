import 'package:flutter/material.dart';

class MazeSettingsScreen extends StatefulWidget {


  @override
  State<MazeSettingsScreen> createState() => _MazeSettingsScreenState();
}

class _MazeSettingsScreenState extends State<MazeSettingsScreen> {
  late int _time = 9; // was 90, changed to 9 for testing
  late int _vision = 1;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Maze Settings")),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text("Time to complete maze: $_time seconds"),
            Slider(
              value: _time.toDouble(),
              min: 9,
              max: 180,
              divisions: 27,
              label: "$_time",
              onChanged: (val) {
                setState(() {
                  _time = val.round();
                });
              },
            ),
            SizedBox(height: 20),
            Text("Vision at maze: $_vision pixels"),
            Slider(
              value: _vision.toDouble(),
              min: 1,
              max: 3,
              divisions: 2,
              label: "$_vision",
              onChanged: (val) {
                setState(() {
                  _vision = val.round();
                });
              },
            ),
            SizedBox(height: 30),
            ElevatedButton(
              onPressed: () {
                Navigator.of(context).pop({
                  'time': _time,
                  'vision': _vision,
                });
              },
              child: Text("Select Game"),
            ),
          ],
        ),
      ),
    );
  }
}