import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:async';

void main() {
  runApp(SmartBinApp());
}

class SmartBinApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Smart Waste Bin',
      home: BinStatusScreen(),
    );
  }
}

class BinStatusScreen extends StatefulWidget {
  @override
  _BinStatusScreenState createState() => _BinStatusScreenState();
}

class _BinStatusScreenState extends State<BinStatusScreen> {
  String bioStatus = "Loading...";
  String nonBioStatus = "Loading...";

  final String apiUrl = "http://<RPI_IP>:5000/status"; // Replace <RPI_IP> with your server IP

  Timer? timer;

  @override
  void initState() {
    super.initState();
    fetchBinStatus(); // Initial fetch
    timer = Timer.periodic(Duration(seconds: 5), (Timer t) => fetchBinStatus());
  }

  @override
  void dispose() {
    timer?.cancel();
    super.dispose();
  }

  Future<void> fetchBinStatus() async {
    try {
      final response = await http.get(Uri.parse(apiUrl));
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        setState(() {
          bioStatus = data['bio'];
          nonBioStatus = data['nonbio'];
        });
      } else {
        print("Failed to fetch data");
      }
    } catch (e) {
      print("Error: $e");
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Smart Waste Bin")),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            BinCard(title: "Bio Waste Bin", status: bioStatus),
            SizedBox(height: 20),
            BinCard(title: "Non-Bio Waste Bin", status: nonBioStatus),
          ],
        ),
      ),
    );
  }
}

class BinCard extends StatelessWidget {
  final String title;
  final String status;

  BinCard({required this.title, required this.status});

  @override
  Widget build(BuildContext context) {
    return Card(
      child: ListTile(
        title: Text(title),
        subtitle: Text("Status: $status"),
        trailing: Icon(
          status == "Full" ? Icons.warning : Icons.check_circle,
          color: status == "Full" ? Colors.red : Colors.green,
        ),
      ),
    );
  }
}
