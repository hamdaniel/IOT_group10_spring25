import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';

class LeaderboardScreen extends StatefulWidget {
  @override
  State<LeaderboardScreen> createState() => _LeaderboardScreenState();
}

class _LeaderboardScreenState extends State<LeaderboardScreen> with SingleTickerProviderStateMixin {
  List<List<String>> easy = [];
  List<List<String>> medium = [];
  List<List<String>> hard = [];
  late TabController _tabController;
  
  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    _loadLeaderboards();
  }
  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: 3, vsync: this);
    _loadLeaderboards();
  }

  Future<void> _loadLeaderboards() async {
    final prefs = await SharedPreferences.getInstance();
    setState(() {
      easy = (prefs.getStringList('leaderboard_easy') ?? [])
          .map((e) => e.split('|'))
          .toList();
      medium = (prefs.getStringList('leaderboard_medium') ?? [])
          .map((e) => e.split('|'))
          .toList();
      hard = (prefs.getStringList('leaderboard_hard') ?? [])
          .map((e) => e.split('|'))
          .toList();
    });
  }

  Widget _buildSection(List<List<String>> data) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Row(
          children: [
            Expanded(
              child: Text(
                "Name",
                style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
              ),
            ),
            Expanded(
              child: Text(
                "Time",
                style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
              ),
            ),
          ],
        ),
        SizedBox(height: 4),
        data.isEmpty
            ? Padding(
                padding: const EdgeInsets.symmetric(vertical: 8.0),
                child: Text("No entries yet."),
              )
            : ListView.builder(
                shrinkWrap: true,
                physics: NeverScrollableScrollPhysics(),
                itemCount: data.length,
                itemBuilder: (context, index) {
                  final entry = data[index];
                  return Padding(
                    padding: const EdgeInsets.symmetric(vertical: 8.0),
                    child: Row(
                      children: [
                        Expanded(child: Text(entry[0])),
                        Expanded(child: Text(entry.length > 1 ? entry[1] : "")),
                      ],
                    ),
                  );
                },
              ),
      ],
    );
  }

  @override
  void dispose() {
    _tabController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Fastest Time"),
        centerTitle: true,
        backgroundColor: Colors.transparent,
        elevation: 0,
        bottom: TabBar(
          controller: _tabController,
          indicatorColor: Colors.amberAccent,
          labelColor: Colors.amberAccent,
          unselectedLabelColor: Colors.white70,
          tabs: [
            Tab(text: "Easy"),
            Tab(text: "Medium"),
            Tab(text: "Hard"),
          ],
        ),
      ),
      extendBodyBehindAppBar: true,
      body: Container(
        decoration: BoxDecoration(
          gradient: LinearGradient(
            colors: [Colors.deepPurple.shade800, Colors.blue.shade600, Colors.cyan.shade400],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
        ),
        child: SafeArea(
          child: Padding(
            padding: const EdgeInsets.all(24.0),
            child: TabBarView(
              controller: _tabController,
              children: [
                _buildSection(easy),
                _buildSection(medium),
                _buildSection(hard),
              ],
            ),
          ),
        ),
      ),
    );
  }
}