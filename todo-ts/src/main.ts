declare function require(path: string): any;

// ==================================================

import * as Vue from 'vue'
import { TARGET } from './util'

import { registerDefaults } from 'coreds-ui/lib/screen_util'
//import * as filters from 'coreds-ui/lib/filters'

registerDefaults()

//for (var i in filters) {
//    Vue.filter(i, filters[i])
//}

// directives
Vue.directive('append', require('coreds-ui/lib/d2/append'))
//Vue.directive('appendto', require('coreds-ui/lib/d2/appendto'))
Vue.directive('defp', require('coreds-ui/lib/d2/defp'))
//Vue.directive('disable', require('coreds-ui/lib/d2/disable'))
Vue.directive('pager', require('coreds-ui/lib/d2/pager'))
//Vue.directive('pclass', require('coreds-ui/lib/d2/pclass'))
//Vue.directive('sclass', require('coreds-ui/lib/d2/sclass'))
Vue.directive('sval', require('coreds-ui/lib/d2/sval'))
//Vue.directive('itoggle', require('coreds-ui/lib/d2/itoggle'))
Vue.directive('toggle', require('coreds-ui/lib/d2/toggle'))
//Vue.directive('close', require('coreds-ui/lib/d2/close'))
Vue.directive('clear', require('coreds-ui/lib/d2/clear'))
Vue.directive('suggest', require('coreds-ui/lib/d2/suggest'))
Vue.directive('dpicker', require('coreds-ui/lib/d2/dpicker'))
//Vue.directive('rappendto', require('coreds-ui/lib/d2/rappendto'))
//Vue.directive('rclass', require('coreds-ui/lib/d2/rclass'))
//Vue.directive('lsearch', require('coreds-ui/lib/d2/lsearch'))

// components

let app = require('./App.vue')
new Vue(app).$mount(document.getElementById(TARGET) as any)
