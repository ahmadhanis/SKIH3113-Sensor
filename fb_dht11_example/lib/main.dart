import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
import 'package:firebase_database/firebase_database.dart';

Future<void> main() async {
  runApp(const MyApp());
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.amber),
        useMaterial3: true,
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  String temp = "", hum = "";

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
          title: const Text("DHT App"),
          backgroundColor: Colors.blue,
          actions: [
            IconButton(onPressed: onPressed, icon: const Icon(Icons.thermostat))
          ]),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text(
              "Temperature: $temp °C",
              style: const TextStyle(fontSize: 24),
            ),
            Text("Humidity: $hum %", style: const TextStyle(fontSize: 24)),
            ElevatedButton(onPressed: loadData, child: const Text("Load Data")),
          ],
        ),
      ),
    );
  }

  void onPressed() {}

  Future<void> loadData() async {
    DatabaseReference ref = FirebaseDatabase.instance.ref('101');
    var event = await ref.once(DatabaseEventType.value);
    var data = event.snapshot.value;
    if (data is Map) {
      data.forEach((key, value) {
        print("$key : $value");
        if (key == "temp") {
          temp = value.toString();
        }
        if (key == "humidity") {
          hum = value.toString();
        }
        setState(() {});
      });
    }
  }
}
