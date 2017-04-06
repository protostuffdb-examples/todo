// ========================================================================
// Copyright 2017 David Yu
// ------------------------------------------------------------------------
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ========================================================================
// @author dyu
// @created 2017/03/22

import 'package:dobx/dobx.dart';
import 'package:dobx_ds/ds.dart' as ds;
import 'package:dobx_ds/store.dart';

import './g/todo/user.dart' as user;

//import './observables.dart';

class ItemPair<T> implements Pair<T> {
  final T orig;
  final T copy;

  bool active = false;
  bool loading = false;
  String msg = '';

  ItemPair(this.orig, this.copy);
}

class FilterState<T> {
  List<ItemPair<T>> list = [];
  bool desc = true;
}

// ==================================================

enum Todo_Filter {
  ALL,
  ACTIVE,
  COMPLETED,
}

const List<String> Todo_Filter$$str = const <String>[
  'All',
  'Active',
  'Completed',
];

// ==================================================

const PAGE_SIZE = 10;

enum AppState {
  LIST_UPDATE,
  NAV_SORT,
  NAV_PAGE,
  NAV_FILTER_TEXT,
  LOADING
}

class App {
  final _pubsub = new PubSub(),
      $pnew = user.Todo.$createObservable(),
      // backup
      pnew = user.Todo.$create(''),
      fsAll = new FilterState<user.Todo>(),
      fsActive = new FilterState<user.Todo>(),
      fsCompleted = new FilterState<user.Todo>();

  List<FilterState<user.Todo>> filters;
  Store<user.Todo> _store;
  Todo_Filter filter = Todo_Filter.ALL;

  // zero based, -1 means none
  int navPage = 0;

  // countdown
  int init = AppState.values.length;

  App() {
    filters = [ fsAll, fsActive, fsCompleted ];
    _store = new Store(PAGE_SIZE, pairTodo, _mergeTodo, _fetchTodo,
        list: fsAll.list, multiplier: 3);
  }

  Store<user.Todo> get store => _store;

  void $sub(AppState state) {
    _pubsub.$sub(state.index + 1);
  }

  void $pub(AppState state) {
    _pubsub.$pub(state.index + 1);
  }

  void _fetchTodo(ds.ParamRangeKey prk) {
    if (filter == Todo_Filter.ALL) {
      user.Todo.ForUser$$list(prk)
          .then(user.Todo.$extractList)
          .then(_store.cbFetchSuccess).catchError(_store.cbFetchFailed);
    } else {
      user.Todo.q$$COMPLETED(ds.P1.$create(filter == Todo_Filter.COMPLETED ? 1 : 0, prk))
          .then(user.Todo.$extractList)
          .then(_store.cbFetchSuccess).catchError(_store.cbFetchFailed);
    }
  }
}

// ==================================================
// todo_misc

ItemPair<user.Todo> pairTodo(user.Todo orig) {
  return new ItemPair(orig, user.Todo.$copy(orig));
}

void _mergeTodo(user.Todo src, Pair<user.Todo> pair) {
  pair.copy.title = pair.orig.title = src.title;
  pair.copy.completed = pair.orig.completed = src.completed;
}
