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
// @created 2017/03/29

import 'package:flutter/material.dart';

import 'package:dobx/dobx.dart';
import 'package:dobx_ds/ds.dart' as ds;
import 'package:dobx_ds/rpc.dart' as rpc;
import 'package:dobx_ds/store.dart' show SelectableItem;

import 'package:todo/g/todo/user.dart' as user;
import 'package:todo/app.dart';

import './ui.dart' as ui;

class TodoItemBuilder implements ui.ItemBuilder {
  final App app;
  final int start;

  TodoItemBuilder(this.app, this.start);

  Widget buildItem(BuildContext context, int index) {
    int offset = start + index;
    return new TodoItemWidget(app, app.store.list[offset], offset);
  }
}

class TodoItemWidget extends StatefulWidget {
  final App app;
  final ItemPair<user.Todo> pair;
  final int idx;
  final TextEditingController _titleCtrl = new TextEditingController();

  TodoItemWidget(this.app, this.pair, this.idx) {
    reset();
    _titleCtrl.text = this.pair.orig.title;
  }

  void reset() {
    pair.active = false;
    pair.msg = '';
  }

  @override
  State createState() {
    return new TodoItemState();
  }
}

class TodoItemState extends State<TodoItemWidget> implements SelectableItem {

  int _fieldUpdate = 0;

  int get index => config.idx;

  void reset() {
    config.reset();

    if (mounted)
      setState(noop);
  }

  void _longPressed() {
    final pair = config.pair;
    pair.msg = '';
    pair.active = !pair.active;

    if (pair.active)
      config.app.store.select(this);

    setState(noop);
  }

  void _updateSuccess(dynamic data) {
    final pair = config.pair;

    pair.loading = false;
    pair.msg = '';

    switch (_fieldUpdate) {
      case user.Todo.FN_TITLE:
        pair.orig.title = pair.copy.title;
        pair.active = false;
        break;
      case user.Todo.FN_COMPLETED:
        pair.orig.completed = pair.copy.completed;
        break;
    }

    if (mounted)
      setState(noop);
  }

  void _updateFailed(dynamic e) {
    var pair = config.pair,
        msg = rpc.getErrMsg(e);

    if (msg.startsWith('CAS'))
      msg += '. Hit refresh to see the updated value.';

    pair.loading = false;
    pair.msg = msg;

    if (mounted)
      setState(noop);
  }

  void _updateWith(ds.MultiCAS mc) {
    final pair = config.pair;

    user.Todo.ForUser$$update(ds.ParamUpdate.$create(pair.orig.key, mc))
        .then(_updateSuccess).catchError(_updateFailed);

    pair.loading = true;
  }

  void _toggleCompleted() {
    final pair = config.pair;
    if (pair.loading)
      return;

    final orig = pair.orig,
        copy = pair.copy;

    copy.completed = !copy.completed;

    if (copy.completed != orig.completed) {
      ds.MultiCAS mc = ds.MultiCAS.$create()
        ..addBool(_fieldUpdate = user.Todo.FN_COMPLETED, orig.completed);
      _updateWith(mc);
    }

    pair.msg = '';
    setState(noop);
  }

  void _titleChanged(String text) {
    final pair = config.pair;
    if (pair.loading)
      return;

    // validate
    final title = text.trim();
    if (title.isEmpty) {
      pair.msg = 'Title cannot be empty.';
      setState(noop);
      return;
    }

    final orig = pair.orig;

    pair.copy.title = title;

    if (title != orig.title) {
      ds.MultiCAS mc = ds.MultiCAS.$create()
        ..addString(_fieldUpdate = user.Todo.FN_TITLE, orig.title, title);
      _updateWith(mc);
    }

    // hide
    pair.active = false;

    pair.msg = '';
    setState(noop);
  }

  @override
  Widget build(BuildContext context) {
    final c = config,
        pair = c.pair,
        copy = pair.copy,
        children = <Widget>[];

    if (0 == (c.idx % c.app.store.pageSize)) {
      children.add(new Divider());
    }

    if (pair.loading) {
      children.add(ui.list_child(new LinearProgressIndicator()));
    }

    children.add(
      new ListTile(
        dense: true,
        title: new Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            ui.text(
              ui.cut(copy.title),
              color: ui.SWATCH_DARKER_COLOR, fontSize: ui.LIST_ITEM_FONT_SIZE,
              decoration: copy.completed ? TextDecoration.lineThrough : null,
            ),
            ui.icon_btn(copy.completed ? Icons.check_box : Icons.check_box_outline_blank,
                _toggleCompleted),
          ],
        ),
        onLongPress: _longPressed,
      ),
    );

    if (pair.active) {
      children.add(ui.list_child(ui.text_input(c._titleCtrl, _titleChanged)));
    }

    if (!pair.msg.isEmpty) {
      children.add(ui.list_text(pair.msg, color: ui.ERROR_COLOR));
    }

    children.add(new Divider());

    return new Column(
        mainAxisAlignment: MainAxisAlignment.start,
        crossAxisAlignment: CrossAxisAlignment.start,
        children: children,
    );
  }
}