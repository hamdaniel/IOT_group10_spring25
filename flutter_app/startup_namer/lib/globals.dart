import 'dart:typed_data';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

BluetoothConnection? globalConnection;
Stream<Uint8List>? globalInputBroadcast;
void Function()? onBluetoothDisconnected;