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

import 'package:flutter/scheduler.dart' as scheduler;
import 'package:flutter/material.dart';

final ThemeData THEME = new ThemeData(primarySwatch: Colors.blueGrey);

// from Colors.blueGrey
const Color SWATCH_COLOR = const Color(0xFF90A4AE),
    SWATCH_LIGHT_COLOR = const Color(0xFFB0BEC5),
    SWATCH_DARK_COLOR = const Color(0xFF78909C),
    SWATCH_DARKER_COLOR = const Color(0xFF607D8B),
    SWATCH_DARKEST_COLOR = const Color(0xFF546E7A),

    INPUT_LABEL_COLOR = const Color.fromRGBO(85, 85, 85, 100.0),
    INPUT_COLOR = const Color.fromRGBO(85, 85, 85, 40.0),
    INPUT_BORDER_COLOR = const Color.fromRGBO(85, 85, 85, 200.0),

    ALT_COLOR = const Color(0xff737373),

    SUCCESS_COLOR = const Color(0xff52A954),
    SUCCESS_BG_COLOR = const Color(0xffDEFCD5),

    ERROR_COLOR = const Color(0xffA95252),
    ERROR_BG_COLOR = const Color(0xffF1D7D7),

    WARNING_COLOR = const Color(0xff96904D),
    WARNING_BG_COLOR = const Color(0xffF6F3D5),

    TEXT_LIGHT_COLOR = const Color.fromRGBO(85, 85, 85, 80.0),
    TEXT_LIGHTER_COLOR = const Color.fromRGBO(85, 85, 85, 120.0),
    TEXT_LIGHTEST_COLOR = const Color.fromRGBO(85, 85, 85, 160.0),

    TEXT_COLOR = const Color.fromRGBO(85, 85, 85, 40.0),
    TEXT_DARK_COLOR = const Color.fromRGBO(85, 85, 85, 30.0),
    TEXT_DARKER_COLOR = const Color.fromRGBO(85, 85, 85, 20.0),
    TEXT_DARKEST_COLOR = const Color.fromRGBO(85, 85, 85, 10.0);

const double FONT_SIZE = 18.0,
    PADDING_SIZE = 5.0,
    LIST_ITEM_FONT_SIZE = 17.0,
    LIST_PADDING_SIZE = 16.5,
    INPUT_EXTRA_SIZE = 4.0,
    BOXED_HEIGHT = FONT_SIZE * 1.723,
    LINEAR_PROGRESS_HEIGHT = 6.0;

const int LIST_ITEM_MAX_CHARS = 78;

const Duration SNACKBAR_DURATION = const Duration(milliseconds: 2000);

// ==================================================
// widgets

class AppBarWidget extends StatelessWidget implements PreferredSizeWidget {
  final WidgetBuilder wb;
  final double bottomHeight;
  final Size _preferredSize;

  AppBarWidget(this.wb, this.bottomHeight)
      : _preferredSize = new Size.fromHeight(bottomHeight);

  @override
  Widget build(BuildContext context) {
    return wb(context);
  }

  @override
  Size get preferredSize => _preferredSize;
}

class AppBarPopup extends StatelessWidget {
  static PopupMenuItem<int> new_item(String text, int idx) {
    return new PopupMenuItem<int>(
      value: idx,
      child: new Text(text),
    );
  }

  final ValueChanged<int> cb;
  final List<String> items;
  final List<PopupMenuItem<int>> popupItems;

  AppBarPopup(this.cb, this.items) : popupItems = new List<PopupMenuItem<int>>(items.length) {
    for (int i = 0, len = items.length; i < len; i++) {
      popupItems[i] = new_item(items[i], i);
    }
  }

  List<PopupMenuItem<int>> buildItems(BuildContext context) {
    return popupItems;
  }

  @override
  Widget build(BuildContext context) {
    return new PopupMenuButton<int>(
      onSelected: cb,
      itemBuilder: buildItems,
    );
  }
}

// ==================================================
// utils

String cut(String str) {
  return str.length <= LIST_ITEM_MAX_CHARS ? str : (str.substring(
      0, LIST_ITEM_MAX_CHARS - 4) + ' ...');
}

void showSnackbar(BuildContext context, String value, {
  Color color = ALT_COLOR,
  Color bgColor = Colors.white,
  Duration duration = SNACKBAR_DURATION,
}) {
  Scaffold.of(context).showSnackBar(new SnackBar(
    content: new Text(value, style: new TextStyle(
      color: color,
    )),
    backgroundColor: bgColor,
    duration: SNACKBAR_DURATION,
  ));
}

void showSnackbarSuccess(BuildContext context, String value, {
  Duration duration = SNACKBAR_DURATION,
}) {
  Scaffold.of(context).showSnackBar(new SnackBar(
    content: new Text(value, style: new TextStyle(
      color: SUCCESS_COLOR,
    )),
    backgroundColor: SUCCESS_BG_COLOR,
    duration: SNACKBAR_DURATION,
  ));
}

/*void showSnackbarSuccess(BuildContext context, String value, {
  Duration duration = SNACKBAR_DURATION,
}) {
  Scaffold.of(context).showSnackBar(new SnackBar(
    content: new Container(
      padding: const EdgeInsets.only(
        top: PADDING_SIZE * 3,
        right: PADDING_SIZE,
        left: PADDING_SIZE,
        bottom: PADDING_SIZE,
      ),
      decoration: new BoxDecoration(
        backgroundColor: SUCCESS_BG_COLOR,
        border: const Border(top: const BorderSide(color: SUCCESS_COLOR))
      ),
      child: new Text(value, style: new TextStyle(
          color: SUCCESS_COLOR,
      )),
    ),
    //backgroundColor: SUCCESS_BG_COLOR,
    duration: SNACKBAR_DURATION,
  ));
}*/

void showBottomSheet(BuildContext context, WidgetBuilder wb) {
  Scaffold.of(context).showBottomSheet(wb);
}

Widget bottomsheet_error(String msg) {
  return fluid(box(text(msg, color: ERROR_COLOR),
    bg_color: ERROR_BG_COLOR,
    pad: const EdgeInsets.all(PADDING_SIZE * 2.5),
  ));
}

// ==================================================
// async

typedef ScheduleCallback(Duration d);

void schedule(ScheduleCallback cb) {
  scheduler.SchedulerBinding.instance.scheduleFrameCallback(cb);
}

// ==================================================
// border

Border bottom({
  width: 1.5,
  Color color: const Color(0xCCCCCC),
  BorderStyle style: BorderStyle.solid
}) {
  return new Border(
    bottom: new BorderSide(color: color, width: width, style: style),
  );
}

Widget padded_top(Widget w, {
  EdgeInsets topPad: const EdgeInsets.only(top: PADDING_SIZE),
}) {
  return new Padding(
    padding: topPad,
    child: w,
  );
}

Widget padded_bottom(Widget w, {
  EdgeInsets padB: const EdgeInsets.only(bottom: PADDING_SIZE),
}) {
  return new Padding(
    padding: padB,
    child: w,
  );
}

Widget padded_right(Widget w, {
  EdgeInsets padR: const EdgeInsets.only(right: PADDING_SIZE),
}) {
  return new Padding(
    padding: padR,
    child: w,
  );
}

// ==================================================
// padding

Widget rel_box(Widget w, {
  Color bg_color = Colors.white,
  num padLR = PADDING_SIZE,
  num padTop = PADDING_SIZE,
}) {
  return new Container(
    padding: new EdgeInsets.only(left: padLR, right: padLR, top: padTop),
    decoration: new BoxDecoration(
      color: bg_color,
    ),
    child: w,
  );
}

Widget box(Widget w, {
  Color bg_color = Colors.white,
  EdgeInsets pad = const EdgeInsets.all(PADDING_SIZE),
}) {
  return new Container(
    padding: pad,
    decoration: new BoxDecoration(
      color: bg_color,
    ),
    child: w,
  );
}

// ==================================================
// rows

Widget row2col(Widget w1, Widget w2) {
  return new Row(
    mainAxisAlignment: MainAxisAlignment.spaceBetween,
    children: [
      w1,
      w2,
    ],
  );
}

Widget fluid(Widget w) {
  return new Row(
    children: [
      new Expanded(
        child: w,
      ),
    ],
  );
}

// ==================================================
// input

Widget text_input(TextEditingController ctrl, ValueChanged<String> cb, {
  num fontSize = FONT_SIZE + INPUT_EXTRA_SIZE,
  color: INPUT_COLOR,
  borderColor: INPUT_BORDER_COLOR,
}) {
  return new Container(
    decoration: new BoxDecoration(
      border: bottom(color: borderColor),
    ),
    child: new TextField(
      controller: ctrl,
      decoration: null,
      autofocus: true,
      onSubmitted: cb,
      style: new TextStyle(
        color: color,
        fontSize: fontSize,
      ),
    ),
  );
}

Widget input(TextEditingController ctrl, ValueChanged<String> cb, {
  num fontSize = FONT_SIZE + INPUT_EXTRA_SIZE,
  color: INPUT_COLOR,
}) {
  return new TextField(
    controller: ctrl,
    decoration: null,
    autofocus: true,
    onSubmitted: cb,
    style: new TextStyle(
      color: color,
      fontSize: fontSize,
    ),
  );
}

Widget input_label(String value, {
  num fontSize = FONT_SIZE,
  Color color = INPUT_LABEL_COLOR,
}) {
  return new Padding(
    padding: new EdgeInsets.only(bottom: 1.0),
    child: new Text(value,
      style: new TextStyle(
        color: color,
        fontSize: fontSize,
      ),
    ),
  );
}

// ==================================================
// list

class ListPageController extends PageController {

  ListPageController() : super(initialPage: 1);
}

final _pageController = new ListPageController();
const Duration _bouncePageDuration = const Duration(milliseconds: 300);

class ListChildDelegate extends SliverChildDelegate {
  final IndexedWidgetBuilder builder;
  final int itemCount;
  final ValueChanged<int> cbPage;
  final ValueChanged<bool> cbFetch;

  const ListChildDelegate(this.builder, this.itemCount, this.cbPage, this.cbFetch);

  void onPage(int index) {
    if (index == 0) {
      cbFetch(true);
      _pageController.nextPage(duration: _bouncePageDuration, curve: Curves.ease);
    } else if (index > itemCount) {
      cbFetch(false);
      _pageController.previousPage(duration: _bouncePageDuration, curve: Curves.ease);
    } else {
      cbPage(index - 1);
    }
  }

  @override
  Widget build(BuildContext context, int index) {
    if (itemCount == 0)
      return null;

    if (index == 0 || index > itemCount)
      return new RepaintBoundary.wrap(new Text(''), index);

    return new RepaintBoundary.wrap(builder(context, index - 1), index);
  }

  @override
  int get estimatedChildCount => itemCount == 0 ? null : 2 + itemCount;

  @override
  bool shouldRebuild(covariant ListChildDelegate oldDelegate) => true;
}

Widget pager_static(int itemCount, IndexedWidgetBuilder builder) {
  return new PageView.builder(
    itemCount: itemCount,
    itemBuilder: builder,
  );
}

Widget pager_dynamic(int itemCount, IndexedWidgetBuilder builder,
    ValueChanged<int> pageCb,
    ValueChanged<bool> fetchCb) {

  final delegate = new ListChildDelegate(builder, itemCount, pageCb, fetchCb);
  return new PageView.custom(
    controller: _pageController,
    onPageChanged: delegate.onPage,
    childrenDelegate: delegate,
  );
}

Widget list_child(Widget child) {
  return new Padding(
    padding: const EdgeInsets.only(left: LIST_PADDING_SIZE, right: LIST_PADDING_SIZE, bottom: PADDING_SIZE),
    child: child,
  );
}

Widget list_text(String value, {
  num fontSize = FONT_SIZE,
  Color color = TEXT_COLOR,
  TextDecoration decoration
}) {
  return new Padding(
    padding: const EdgeInsets.only(left: LIST_PADDING_SIZE, right: LIST_PADDING_SIZE, bottom: PADDING_SIZE),
    child: new Text(value,
      style: new TextStyle(
        fontSize: fontSize,
        color: color,
        decoration: decoration,
      ),
    ),
  );
}

Widget list_row(List<Widget> children) {
  return new Padding(
    padding: const EdgeInsets.only(left: LIST_PADDING_SIZE, bottom: PADDING_SIZE),
    child: new Row(
      children: children,
    ),
  );
}
Widget list_flex(List<Widget> children) {
  return new Padding(
    padding: const EdgeInsets.only(left: LIST_PADDING_SIZE, bottom: PADDING_SIZE),
    child: new Flex(
      mainAxisAlignment: MainAxisAlignment.start,
      crossAxisAlignment: CrossAxisAlignment.start,
      direction: children.length <= 3 ? Axis.horizontal : Axis.vertical,
      children: children.map(children.length <= 3 ? padded_right : padded_bottom).toList(growable: false),
    ),
  );
}


// ==================================================
// text

Widget text(String value, {
  num fontSize = FONT_SIZE,
  Color color = TEXT_COLOR,
  TextDecoration decoration
}) {
  return new Text(value,
    style: new TextStyle(
      fontSize: fontSize,
      color: color,
      decoration: decoration,
    ),
  );
}

Widget label(String value, {
  num fontSize = FONT_SIZE - 2,
  Color color = SWATCH_COLOR,
  TextDecoration decoration
}) {
  return new Chip(
    label: new Text(value,
      style: new TextStyle(
        fontSize: fontSize,
        color: color,
        decoration: decoration,
      ),
    ),
  );
}

// ==================================================
// button

Widget icon_defpad_dyn_btn(Widget icon, VoidCallback onPressed, {
  Color color = SWATCH_COLOR,
}) {
  return new IconButton(
    icon: icon,
    onPressed: onPressed,
  );
}

Widget icon_defpad_btn(IconData iconData, VoidCallback onPressed, {
  Color color = SWATCH_COLOR,
}) {
  return new IconButton(
    icon: new Icon(iconData, color: color),
    onPressed: onPressed,
  );
}

Widget icon_btn(IconData icon, VoidCallback onPressed, {
  EdgeInsets pad = EdgeInsets.zero,
  Color color = SWATCH_COLOR,
}) {
  return new IconButton(
    icon: new Icon(icon, color: color),
    padding: pad,
    onPressed: onPressed,
  );
}

// ==================================================
// block

abstract class ItemBuilder {

  Widget buildItem(BuildContext context, int index);
}

Widget block(List<Widget> children) {
  return new ListView(
    children: children,
  );
}

Widget block_lazy(int itemCount, ItemBuilder builder) {
  return new ListView.builder(
    itemCount: itemCount,
    itemBuilder: builder.buildItem,
  );
}