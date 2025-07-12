import 'dart:async';
import 'dart:typed_data';
import 'dart:math';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'SymbolChoosing.dart';

const List<List<String>> symbolPatterns = [
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100011001100010",
    "0100011001100010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010000000000100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100001100110010",
    "0100001100110010",
    "0100000000000010",
    "0100000000000010",
    "0100001100110010",
    "0100001111110010",
    "0010000000000100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100011001100010",
    "0100011001100010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010001111000100",
    "0001001111001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100011001100010",
    "0100011001100010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010001111111100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100000001100010",
    "0100011000000010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010000000000100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100000000000010",
    "0100011001100010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010000000000100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
  [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100001001000010",
    "0100011001100010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010000000000100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
 [
    "0000000000000000",
    "0000000000000000",
    "0000111111110000",
    "0001000000001000",
    "0010000000000100",
    "0100011000000010",
    "0100011001100010",
    "0100000000000010",
    "0100000000000010",
    "0100011001100010",
    "0100011001100010",
    "0010000000000100",
    "0001000000001000",
    "0000111111110000",
    "0000000000000000",
    "0000000000000000",
  ],
];

List<String> flattenSymbol(List<String> symbol) {
  return symbol.expand((row) => row.split('')).toList();
}

List<String> shuffleSymbol(List<String> symbol) {
  List<List<String>> quadrants = List.generate(4, (_) => []);
  for (int i = 0; i < 16; i++) {
    String row = symbol[i];
    quadrants[0].add(row.substring(0, 8));
    quadrants[1].add(row.substring(8, 16));
  }
  for (int i = 8; i < 16; i++) {
    String row = symbol[i];
    quadrants[2].add(row.substring(0, 8));
    quadrants[3].add(row.substring(8, 16));
  }
  // Shuffle quadrants
  List<int> order = [0, 1, 2, 3]..shuffle();
  List<String> newSymbol = [];
  for (int i = 0; i < 8; i++) {
    newSymbol.add(quadrants[order[0]][i] + quadrants[order[1]][i]);
  }
  for (int i = 0; i < 8; i++) {
    newSymbol.add(quadrants[order[2]][i] + quadrants[order[3]][i]);
  }
  return newSymbol;
}

Future<String?> startSymbolGame({
  required BuildContext menuContext,
  required BluetoothConnection? connection,
  required int time,
  required int levels,
}) async {
  final rand = Random();
  List<int> chosenIndexes = [];
  List<List<String>> chosenSymbols = [];

  for (int i = 0; i < levels; i++) {
    int idx = rand.nextInt(symbolPatterns.length);
    chosenIndexes.add(idx);
    chosenSymbols.add(symbolPatterns[idx]);
  }
  connection?.output.add(Uint8List.fromList("symbol\n".codeUnits));
  connection?.output.add(Uint8List.fromList("init\n".codeUnits));
  connection?.output.add(Uint8List.fromList("$time\n".codeUnits));
  connection?.output.add(Uint8List.fromList("$levels\n".codeUnits));
  for (int i = 0; i < levels; i++) {
    await Future.delayed(Duration(milliseconds: 50));
    String unshuffled_flattened = flattenSymbol(chosenSymbols[i]).join();
    connection?.output.add(Uint8List.fromList("$unshuffled_flattened\n".codeUnits));
    List<String> shuffled = shuffleSymbol(chosenSymbols[i]);
    String shuffled_flattened = flattenSymbol(shuffled).join();
    await Future.delayed(Duration(milliseconds: 50));
    connection?.output.add(Uint8List.fromList("$shuffled_flattened\n".codeUnits));
  }

  final result = await Navigator.of(menuContext).push<String>(
    MaterialPageRoute(
      builder: (_) => SymbolChoosingScreen(
        connection: connection,
        symbols: chosenSymbols,
      ),
    ),
  );

  return result ?? "lose";
}