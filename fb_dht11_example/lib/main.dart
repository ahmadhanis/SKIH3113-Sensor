import 'package:fb_dht11_example/firebase_options.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:intl/intl.dart';
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
  String datetime = "";
  String status = "On";

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
            Text(datetime),
            Text(
              "Temperature: $temp °C",
              style: const TextStyle(fontSize: 24),
            ),
            Text("Humidity: $hum %", style: const TextStyle(fontSize: 24)),
            ElevatedButton(onPressed: loadData, child: const Text("Load Data")),
            const SizedBox(height: 40),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                ElevatedButton(
                    onPressed: onSwitch, child: const Text("Switch")),
                const SizedBox(
                  width: 20,
                ),
                Text("Relay: $status"),
              ],
            )
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
        if (key == "relay") {
          status = value.toString();
        }
        if (key == "time") {
          String localDateTimeString = value.toString();
          DateTime localDateTime = DateTime.fromMillisecondsSinceEpoch(
              int.parse(localDateTimeString) * 1000);
          Duration offset = Duration(hours: 8); // GMT+8
          DateTime gmt8DateTime = localDateTime.add(offset);
          datetime = DateFormat('dd-MM-yyyy HH:mm a').format(gmt8DateTime);
        }
        setState(() {});
      });
    }
  }

  Future<void> onSwitch() async {
    print(status);
    if (status == "On") {
      status = "Off";
    }else{
        status = "On";
    }
    
    DatabaseReference ref = FirebaseDatabase.instance.ref("101");
    await ref.update({
      "relay": status,
    });
    setState(() {});
  }
}
