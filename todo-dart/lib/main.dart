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

import 'dart:math' as math;
import 'package:flutter/material.dart';

import 'package:dobx/dobx.dart';
import 'package:dobx_ds/rpc.dart' as rpc;

import 'package:todo/g/todo/user.dart' as user;
import 'package:todo/app.dart';

import './ui.dart' as ui;
import './todo_item.dart';

void main() {
  rpc.setPrefix('https://todo.dyuproject.com');
  runApp(new AppWidget());
}

enum Root {
  $loading,
  $nav_sort,
  $nav_page,
  $nav_filter_text,
  $pnew_form,
  $list,
}

const String HEADER_TITLE = 'Todo';

class AppWidget extends StatelessWidget {
  final WF wf = WF.get(0);
  final App app = new App();
  final TextEditingController _titleCtrl = new TextEditingController();
  BuildContext ctx;

  @override
  Widget build(BuildContext context) {
    return new MaterialApp(
      title: HEADER_TITLE,
      theme: ui.THEME,
      home: new Scaffold(
        appBar: new AppBar(
          actions: [
            ui.padded_top(wf.$($nav_filter_text, Root.$nav_filter_text)),
            new Text('  '),
            ui.padded_top(wf.$($nav_page, Root.$nav_page)),
            ui.icon_defpad_dyn_btn(wf.$($nav_sort, Root.$nav_sort), () => app.store.toggleDesc(_propagateListUpdate, null), color: Colors.white),
            ui.icon_defpad_btn(Icons.refresh, app.store.fetchUpdate, color: Colors.white),
            ui.icon_defpad_btn(Icons.add, () => app.$pnew.ts ^= 1, color: Colors.white),
            new ui.AppBarPopup(_filterSelected, Todo_Filter$$str),
          ],
          bottom: new ui.AppBarWidget(buildAppBar, ui.LINEAR_PROGRESS_HEIGHT),
        ),
        body: new Column(
          mainAxisAlignment: MainAxisAlignment.start,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            ui.fluid(wf.$($pnew_form, Root.$pnew_form)),
            new Expanded(child: wf.$($list, Root.$list)),
          ],
        ),
      ),
    );
  }

  Widget _buildErrmsg(BuildContext context) {
    return ui.bottomsheet_error(app.store.errmsg);
  }

  void _propagateErrmsg(Duration d) {
    ui.showBottomSheet(ctx, _buildErrmsg);
  }

  void _propagateListUpdate(Duration d) {
    app.$pub(AppState.LIST_UPDATE);
    app.$pub(AppState.NAV_PAGE);
  }

  // ==================================================
  // top bar

  Widget $nav_sort(BuildContext context) {
    var store = app.store,
        desc = store.desc;
    if (app.init > 0) {
      desc = store.$desc;
      if (--app.init == 0)
        app.store.fetchNewer();
    }

    return new Icon(desc ? Icons.arrow_downward : Icons.arrow_upward);
  }

  Widget $nav_page(BuildContext context) {
    if (app.init > 0) {
      app.$sub(AppState.NAV_PAGE);
      if (--app.init == 0)
        app.store.fetchNewer();

      return new Text('');
    }

    return new Text(app.store.page_info(app.navPage));
  }

  Widget $nav_filter_text(BuildContext context) {
    if (app.init > 0) {
      app.$sub(AppState.NAV_FILTER_TEXT);
      if (--app.init == 0)
        app.store.fetchNewer();
    }

    return new Text(Todo_Filter$$str[app.filter.index]);
  }

  Widget $loading(BuildContext context) {
    if (app.init > 0 && --app.init == 0) {
      app.store.fetchNewer();
    }

    final store = app.store,
        loading = store.$state.loading;

    if (loading)
      return new LinearProgressIndicator();

    ui.schedule(store.errmsg.isEmpty ? _propagateListUpdate : _propagateErrmsg);
    return new Text('');
  }

  Widget buildAppBar(BuildContext context) {
    ctx = context;
    return wf.$($loading, Root.$loading);
  }

  // ==================================================
  // todo_new

  void _pnewSuccess(user.Todo_PList data) {
    final store = app.store;

    if (app.filter == Todo_Filter.ALL)
      store.list.insertAll(0, data.p.reversed.map(pairTodo));

    ui.showSnackbarSuccess(ctx, 'Added ${app.pnew.title}.');

    store.$state.loading = false;
  }

  void _pnewFailed(dynamic e) {
    // restore
    app.$pnew.title = app.pnew.title;

    final store = app.store;

    store.errmsg = rpc.getErrMsg(e);
    store.$state.loading = false;
  }

  void _titleChanged(String text) {
    final title = text.trim(),
        store = app.store;
    if (title.isEmpty || store.$state.loading)
      return;

    // send request
    final req = user.Todo.$create(title);
    // last seen key
    if (!store.list.isEmpty && app.filter == Todo_Filter.ALL)
      req.key = store.latest.key;

    user.Todo.ForUser$$create(req)
        .then(_pnewSuccess).catchError(_pnewFailed);

    // backup
    app.pnew.title = title;
    // clear
    _titleCtrl.clear();
    app.$pnew.title = null;

    store.errmsg = '';
    store.$state.loading = true;
  }

  Widget $pnew_form(BuildContext context) {
    final title = app.$pnew.title,
        // ts = timestamp, but could also be 'toggle state'
        // Both key and ts aren't used on new forms.
        state = app.$pnew.ts;

    if (state == 0)
      return new Text('');

    return new Column(
      mainAxisAlignment: MainAxisAlignment.start,
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        ui.fluid(ui.box(ui.input_label('What needs to be done?'))),
        ui.fluid(ui.box(ui.input(_titleCtrl, _titleChanged))),
      ],
    );
  }

  // ==================================================
  // todo_list

  void _filterSelected(int idx) {
    final store = app.store,
        prevIdx = app.filter.index;

    if (prevIdx == idx || store.$state.loading)
      return;

    final desc = store.desc,
        prev = app.filters[prevIdx],
        current = app.filters[idx];

    // save state
    prev.list = store.list;
    prev.desc = desc;

    // set filter
    app.filter = Todo_Filter.values[idx];

    if (current.list.isEmpty) {
      store.list = current.list;
      current.desc = true;
      if (desc) {
        _propagateListUpdate(null);
      } else {
        store.toggleDesc(_propagateListUpdate, null);
      }
      store.fetchNewer();
    } else {
      store.list = desc == current.desc ?
          current.list : current.list.reversed.toList(growable: true);
      _propagateListUpdate(null);
    }

    app.$pub(AppState.NAV_FILTER_TEXT);
  }

  void _updateNavPage(int navPage) {
    if (navPage == app.navPage) return;

    app.navPage = navPage;
    app.$pub(AppState.NAV_PAGE);
  }

  void _fetch(bool newer) {
    if (!app.store.desc)
      newer = !newer;

    if (newer)
      app.store.fetchNewer();
    else
      app.store.fetchOlder();
  }

  Widget newItems(BuildContext context, int idx) {
    final store = app.store,
        pageSize = store.pageSize;

    store.page = idx;

    int start = idx * pageSize;

    return ui.block_lazy(
      math.min(store.list.length - start, pageSize),
      new TodoItemBuilder(app, start),
    );
  }

  Widget $list(BuildContext context) {
    final store = app.store,
        list = store.list,
        page = list.isEmpty ? -1 : ((list.length - 1) / store.pageSize).floor();

    if (app.init > 0) {
      app.$sub(AppState.LIST_UPDATE);
      if (--app.init == 0)
        app.store.fetchNewer();
    }

    return ui.pager_dynamic(page + 1, newItems, _updateNavPage, _fetch);
  }
}

