declare function require(path: string): any;

// ==================================================

import * as Vue from 'vue'
import { TARGET, filters } from './util'

// filters
for (var i of Object.keys(filters)) Vue.filter(i, filters[i])

// directives
Vue.directive('append', require('coreds-ui/lib/d2/append'))
Vue.directive('clear', require('coreds-ui/lib/d2/clear'))
Vue.directive('defp', require('coreds-ui/lib/d2/defp'))
Vue.directive('dpicker', require('coreds-ui/lib/d2/dpicker'))
Vue.directive('pager', require('coreds-ui/lib/d2/pager'))
Vue.directive('sval', require('coreds-ui/lib/d2/sval'))
Vue.directive('toggle', require('coreds-ui/lib/d2/toggle'))

let app = require('./App.vue')
new Vue(app).$mount(document.getElementById(TARGET) as any)
