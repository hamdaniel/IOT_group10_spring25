import 'package:flutter/material.dart';

class SymbolSettingsScreen extends StatefulWidget {
  @override
  State<SymbolSettingsScreen> createState() => _SymbolSettingsScreenState();
}

class _SymbolSettingsScreenState extends State<SymbolSettingsScreen> {
  int time = 45;
  int levels = 1;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      // Custom gradient background
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
                    "Symbol Game Settings",
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
                    "Time to complete: $time seconds",
                    style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white),
                  ),
                  Slider(
                    value: time.toDouble(),
                    min: 30,
                    max: 90,
                    divisions: 60,
                    label: time.toString(),
                    onChanged: (val) {
                      setState(() {
                        time = val.round();
                      });
                    },
                  ),
                  SizedBox(height: 30),
                  Text(
                    "Amount of levels: $levels",
                    style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white),
                  ),
                  Slider(
                    value: levels.toDouble(),
                    min: 1,
                    max: 3,
                    divisions: 2,
                    label: levels.toString(),
                    onChanged: (val) {
                      setState(() {
                        levels = val.round();
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
                            Navigator.pop(context, {'time': time, 'levels': levels});
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