import 'dart:async';

import 'package:build_runner/build_runner.dart';
import 'package:dobx_gen/core_gen.dart';
import 'package:source_gen/source_gen.dart';

Future main(List<String> args) async {
  await watch(
      new PhaseGroup.singleAction(
          new GeneratorBuilder([new DobxGenerator()]),
          new InputSet('todo', const ['lib/observables.dart'])),
      deleteFilesByDefault: true);
}
