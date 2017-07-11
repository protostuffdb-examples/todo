import { component } from 'vuets'
import { defp, nullp  } from 'coreds/lib/util'
import { Pager, ItemSO, SelectionFlags, PojoListState, PojoSO, PojoState } from 'coreds/lib/types'
import { PojoStore } from 'coreds/lib/pstore/'
import { mergeFrom } from 'coreds/lib/diff'
import { ParamRangeKey } from 'coreds/lib/prk'
import * as form from 'coreds/lib/form'
import * as ui from '../ui'
import { stores } from '../context'
import { user } from '../../g/user/'
const $ = user.Todo

const PAGE_SIZE = 10,
    MULTIPLIER = 3

export class TodoPage {
    pager: Pager
    pstore: PojoStore<user.Todo>

    pnew = form.initObservable($.$new0(), $.$d)
    pupdate = form.initObservable($.$new0(), $.$d)

    constructor() {
        nullp(this, 'pager')
    }

    static created(self: TodoPage) {
        let pstore = defp(self, 'pstore', new PojoStore([], {
            desc: true,
            pageSize: PAGE_SIZE,
            multiplier: MULTIPLIER,
            descriptor: $.$d,
            createObservable(so: ItemSO, idx: number) {
                return $.$new('')
            },
            onSelect(selected: user.Todo, flags: SelectionFlags): number {
                if (!(flags & SelectionFlags.CLICKED_UPDATE))
                    return 0

                let selected_ = selected['_'] as PojoSO,
                    state = selected_.state,
                    pupdate = self.pupdate,
                    pupdate_: PojoSO,
                    original

                if ((flags & SelectionFlags.REFRESH)) {
                    if (!(state & PojoState.UPDATE))
                        return 0
                } else if (!(state & PojoState.UPDATE)) {
                    selected_.state = state | PojoState.UPDATE
                    if (selected['1'] === pupdate['1'])
                        return 0
                } else if (selected['1'] === pupdate['1']) {
                    selected_.state = state ^ PojoState.UPDATE
                    return 0
                }

                pupdate_ = pupdate['_'] as PojoSO
                original = self.pstore.getOriginal(selected)

                mergeFrom(original, selected['$d'], pupdate)
                if (pupdate_.msg)
                    pupdate_.msg = ''

                return 0
            },
            fetch(prk: ParamRangeKey, pager: Pager) {
                return $.ForUser.list(prk).then(self.fetch$$S).then(undefined, self.fetch$$F)
            }
        }))
        stores.todo = pstore
        self.pager = pstore.pager
    }

    static mounted(self: TodoPage) {
        self.pstore.requestNewer()
    }

    fetch$$S(data) {
        this.pstore.cbFetchSuccess(data['1'])
    }
    fetch$$F(err) {
        this.pstore.cbFetchFailed(err)
    }

    pnew$$S(data) {
        let pnew = this.pnew,
            el = document.getElementById('todo-ff') as any
        
        this.pstore.addAll(data['1'], true, true)
        form.$success(pnew)
        el.focus()
        return true
    }
    pnew$$F(err) {
        form.$failed(this.pnew, err)
    }
    pnew$$() {
        let pnew = this.pnew,
            lastSeen
        if (!form.$prepare(pnew))
            return
        
        pnew['1'] = (lastSeen = this.pstore.getLastSeenObj()) && lastSeen['1']
        
        $.ForUser.create(pnew)
            .then(this.pnew$$S).then(undefined, this.pnew$$F)
    }
    pupdate$$S(data) {
        let pager = this.pager,
            selected = pager.pojo as user.Todo,
            original = this.pstore.getOriginal(selected)
        form.$update_success(this.pupdate, this.pager, original, selected)
    }
    pupdate$$F(err) {
        form.$update_failed(this.pupdate, this.pager, err)
    }
    pupdate$$() {
        let pager = this.pager,
            selected = pager.pojo as user.Todo,
            original = this.pstore.getOriginal(selected),
            mc = form.$update(this.pupdate, pager, original)

        if (!mc)
            return

        $.ForUser.update(form.$update_req(original['1'] as string, mc))
            .then(this.pupdate$$S).then(undefined, this.pupdate$$F)
    }
    change(e, field, pojo, update, root) {
        return form.$change(e, field, pojo, update, root)
    }

    toggle$$S(data) {
        form.$toggle_success(this.pager, this.pupdate)
    }
    toggle$$F(err) {
        form.$toggle_failed(this.pager, err)
    }
    toggle(field, pojo) {
        let mc = form.$toggle(this.pager, field)
        mc && $.ForUser.update(form.$update_req(pojo['1'] as string, mc))
            .then(this.toggle$$S).then(undefined, this.toggle$$F)
    }
}
export default component({
    created(this: TodoPage) { TodoPage.created(this) },
    mounted(this: TodoPage) { TodoPage.mounted(this) },
    template: /**/`
<div class="col-pp-100 col-pl-50 col-tl-33" v-pager="pager">
<ul class="ui right floated horizontal list">
  <li class="item" title="add" v-toggle="'1__.3'">
    &nbsp;&nbsp;<button class="stripped"><i class="icon plus"></i></button>
  </li>
  <li class="item" title="filter" v-toggle="'1__.4'">
    <button class="stripped"><i class="icon filter"></i></button>
  </li>
</ul>
${ui.pager_controls}
<div class="ui tab box">
  ${ui.form('pnew', $.$d, 'todo-ff')}
</div>
<div class="ui tab">
  TODO filter
</div>
${ui.pager_msg}
<ul class="ui small divided selection list">
<pi v-for="pojo of pager.array" :pojo="pojo">
  <div class="content right floated">
    ${ui.icon_toggle($.$.completed, 32, 'circle', $.$d[$.$.completed].$n)}
  </div>
  <div class="content">
    <small class="description">${ui.icon_timeago}</small>
  </div>
  <div :class="!pojo['${$.$.completed}'] ? 'content dd' : 'content dd completed'">
    {{ pojo['${$.$.title}'] }}
  </div>
  <div v-show="pojo._.state & ${PojoState.UPDATE}" v-append:todo-detail="pojo._.state & ${PojoState.UPDATE}"></div>
  ${ui.pi_msg}
</pi>
</ul>
<div style="display:none">
  <div id="todo-detail" class="detail">
    <hr />
    ${ui.form('pupdate', $.$d, null)}
  </div>
</div>
</div>`/**/
}, TodoPage)