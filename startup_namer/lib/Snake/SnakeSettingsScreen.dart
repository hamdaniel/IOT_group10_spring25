import 'package:flutter/material.dart';

class SnakeSettingsScreen extends StatefulWidget {
  const SnakeSettingsScreen({Key? key}) : super(key: key);

  @override
  State<SnakeSettingsScreen> createState() => _SnakeSettingsScreenState();
}

class _SnakeSettingsScreenState extends State<SnakeSettingsScreen> {
  double _speed = 1.0;
  double _scoreToBeat = 20;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Snake Settings")),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text("Score to beat: ${_scoreToBeat.round()}"),
            Slider(
              value: _scoreToBeat,
              min: 20,
              max: 40,
              divisions: 20,
              label: "${_scoreToBeat.round()}",
              onChanged: (val) {
                setState(() {
                  _scoreToBeat = val;
                });
              },
            ),
            SizedBox(height: 20),
            Text("Speed: ${_speed.toStringAsFixed(1)}"),
            Slider(
              value: _speed,
              min: 1.0,
              max: 3.0,
              divisions: 20,
              label: "${_speed.toStringAsFixed(1)}",
              onChanged: (val) {
                setState(() {
                  _speed = val;
                });
              },
            ),
            SizedBox(height: 30),
            ElevatedButton(
              onPressed: () {
                Navigator.of(context).pop({
                  'scoreToBeat': _scoreToBeat.round(),
                  'speed': _speed,
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