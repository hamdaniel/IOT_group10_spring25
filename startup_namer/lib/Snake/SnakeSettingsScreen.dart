import 'package:flutter/material.dart';

class SnakeSettingsScreen extends StatefulWidget {
  const SnakeSettingsScreen({Key? key}) : super(key: key);

  @override
  State<SnakeSettingsScreen> createState() => _SnakeSettingsScreenState();
}

class _SnakeSettingsScreenState extends State<SnakeSettingsScreen> {
  double _speed = 1.0;
  double _scoreToBeat = 10;

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
                    "Snake Settings",
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
                  Text(
                    "Score to beat: ${_scoreToBeat.round()}",
                    style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white),
                  ),
                  Slider(
                    value: _scoreToBeat,
                    min: 5,
                    max: 15,
                    divisions: 9,
                    label: "${_scoreToBeat.round()}",
                    onChanged: (val) {
                      setState(() {
                        _scoreToBeat = val;
                      });
                    },
                  ),
                  SizedBox(height: 20),
                  Text(
                    "Speed: ${_speed.toStringAsFixed(1)}",
                    style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white),
                  ),
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
                            Navigator.of(context).pop({
                              'scoreToBeat': _scoreToBeat.round(),
                              'speed': _speed,
                            });
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