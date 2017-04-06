// GENERATED CODE - DO NOT MODIFY BY HAND

part of todo.observables;

// **************************************************************************
// Generator: DobxGenerator
// Target: abstract class Root
// **************************************************************************

class _Root extends PubSub implements Root {
  bool _loading;
  bool get loading {
    $sub(1);
    return _loading;
  }

  void set loading(bool loading) {
    if (loading != null && loading == _loading) return;
    _loading = loading ?? false;
    $pub(1);
  }
}

Root _$Root({
  bool loading: false,
}) {
  return new _Root().._loading = loading;
}
