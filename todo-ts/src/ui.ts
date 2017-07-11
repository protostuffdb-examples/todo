import { PagerState, PojoState, FieldType } from 'coreds/lib/types'

export const enum ContentSlot {
    TOP = 0,
    BOTTOM = 1
}

export const suggest_controls = /**/`
<ul class="ui horizontal list">
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.LOADING}) || 0 === pager.page"
        @click.prevent="pager.store.repaint((pager.page = 0))">
      <i class="icon angle-double-left"></i>
    </button>
  </li>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.MASK_RPC_DISABLE})"
        @click.prevent="pager.store.pagePrevOrLoad(0)">
      <b><i class="icon angle-left"></i></b>
    </button>
  </li>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.MASK_RPC_DISABLE}) || 0 === pager.size"
        @click.prevent="pager.store.pageNextOrLoad(0)">
      <b><i class="icon angle-right"></i></b>
    </button>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.LOADING}) || pager.page_count === pager.page"
        @click.prevent="pager.store.repaint((pager.page = pager.page_count))">
      <i class="icon angle-double-right"></i>
    </button>
  <li class="item" v-show="pager.size">
    {{ pager.page_from }}{{ pager.page_from === pager.page_to ? ' of ' : (' - ' + pager.page_to + ' of ') }}{{ pager.size }}
  </li>
</ul>
`/**/

export const pager_controls = /**/`
<ul class="ui right floated horizontal list">
  <li class="item">
      <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.LOADING}) || 2 > pager.size"
          @click.prevent="pager.store.repaint((pager.state ^= ${PagerState.DESC}))">
        <i :class="(pager.state & ${PagerState.DESC}) ? 'icon desc-yes' : 'icon desc-no'"></i>
      </button>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.MASK_RPC_DISABLE}) || 0 === pager.size"
        @click.prevent="pager.store.reload()">
      <i class="icon cw"></i>
    </button>
  </li>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.LOADING}) || 0 === pager.page"
        @click.prevent="pager.store.repaint((pager.page = 0))">
      <i class="icon angle-double-left"></i>
    </button>
  </li>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.MASK_RPC_DISABLE})"
        @click.prevent="pager.store.pagePrevOrLoad(0)">
      <b><i class="icon angle-left"></i></b>
    </button>
  </li>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.MASK_RPC_DISABLE}) || 0 === pager.size"
        @click.prevent="pager.store.pageNextOrLoad(0)">
      <b><i class="icon angle-right"></i></b>
    </button>
  </li>
  <li class="item">
    <button class="stripped" :disabled="0 !== (pager.state & ${PagerState.LOADING}) || pager.page_count === pager.page"
        @click.prevent="pager.store.repaint((pager.page = pager.page_count))">
      <i class="icon angle-double-right"></i>
    </button>
  </li>
</ul>
<ul class="ui horizontal list">
  <li class="item" v-show="pager.size">
    <span class="page-info">
      {{ pager.page_from }}{{ pager.page_from === pager.page_to ? ' of ' : (' - ' + pager.page_to + ' of ') }}{{ pager.size }}
    </span>
  </li>
</ul>
`/**/

export const pager_msg = /**/`
<div v-show="pager.msg && (pager.state & ${PagerState.MASK_STATUS})">
  <div :class="'ui message status-' + (pager.state & ${PagerState.MASK_STATUS})">
    <i class="close icon" @click.prevent="pager.msg = null"></i>
    <span v-text="pager.msg"></span>
  </div>
</div>
`/**/

// ================================================== 
// icons

export const icon_timeago = /**/`
<i class="icon clock"></i>{{ pojo['2'] | prettydate }}
`/**/

// ================================================== 
// form

function form_msg_show(pojo: string): string {
    return ` && (${pojo}._.state & ${PojoState.MASK_STATUS})`
}

function form_msg(pojo: string, update: boolean): string {
    return /**/`
<div :class="'ui message status-' + (${pojo}._.state & ${PojoState.MASK_STATUS})"
    v-show="${pojo}._.msg${update && form_msg_show(pojo) || ''}">
  <i class="icon close" @click.prevent="${pojo}._.msg = null"></i>
  <span v-text="${pojo}._.msg"></span>
</div>
`/**/
}

export function form(pojo: string, $d: any, ffid: string|null, 
        content?: string, content_slot?: ContentSlot): string {
    let update = !ffid
    if (content && content_slot === undefined)
        content_slot = ContentSlot.TOP
    
    return /**/`
<form :class="'ui form status-' + (${pojo}._.state & ${PojoState.MASK_STATUS})">
  ${content_slot === ContentSlot.TOP && content || ''}
  ${form_body(pojo, $d, update, { pojo, ffid })}
  ${content_slot === ContentSlot.BOTTOM && content || ''}
  ${form_msg(pojo, update)}
  <button type="submit" class="outlined" @click.prevent="${pojo}$$">
    ${update ? 'Update' : 'Submit'}
  </button>
</form>
`/**/
}

interface FormRoot {
    pojo: string
    ffid: string|null
}

function form_body(pojo: string, $d: any, update: boolean, root: FormRoot): string {
    let out = '',
        array = $d.$fdf
    
    if ($d.$fmf) {
        for (let fk of $d.$fmf) {
            out += form_body(`${pojo}['${fk}']`, $d[fk].d_fn(), update, root)
        }
    }
    
    if (!array)
        return out

    let mask = update ? 13 : 3, 
        ffid = root.ffid
    
    if (ffid && array.length)
        root.ffid = null

    for (var i = 0, len = array.length; i < len; i++) {
        let fk = array[i],
            fd = $d[fk]
        if (!fd.t || (fd.a & mask)) continue

        out += `<div ${form_field_class(pojo, fd)}>${form_field_switch(pojo, fd, update, root, i, ffid)}</div>`
        ffid = null
    }

    return out
}

function form_field_class(pojo: string, fd: any): string {
    let base = fd.m === 2 ? 'field required' : 'field'
    if (fd.t === FieldType.BOOL || fd.t === FieldType.ENUM)
        return `class="${base}"`
    else
        return `:class="'${base}' + ((${pojo}._.vfbs & ${1 << (fd._ - 1)}) && ${pojo}._['${fd._}'] && ' error' || '')"`
}

function form_field_switch(pojo: string, fd: any, update: boolean, root: FormRoot, idx: number, ffid: any): string {
    let buf = '',
        t = fd.t
    
    if (t !== FieldType.BOOL)
        buf += `<label>${fd.$n}${fd.m === 2 && ' *' || ''}</label>`
    
    if (t === FieldType.BOOL)
        buf += field_bool(pojo, fd, update, root, ffid)
    else if (t === FieldType.ENUM)
        buf += field_enum(pojo, fd, update, root, ffid)
    else if (t !== FieldType.STRING)
        buf += field_num(pojo, fd, update, root, ffid)
    else if (fd.ta)
        buf += field_textarea(pojo, fd, update, root, ffid)
    else
        buf += field_default(pojo, fd, update, root, ffid)
    
    return buf
}

function ffid_attr(ffid): string {
    return ` id="${ffid}"`
}

function help_text(str): string {
    return /**/`<div class="help-text">${str}</div>`/**/
}

function field_bool(pojo: string, fd: any, update: boolean, root: FormRoot, ffid: any): string {
    return /**/`
<label class="switch">
  <input${ffid && ffid_attr(ffid) || ''} type="checkbox" v-sval:${fd.t}="${pojo}['${fd._}']"
      @change="change($event, '${fd._}', ${pojo}, ${update}, ${root.pojo})" />
  <i></i> ${fd.$n}
</label>
`/**/
}

const option_empty = '<option value=""></option>'
function enum_options(arrayValue: any[], arrayDisplay: any[]): string {
    let out = ''
    for (var i = 0, len = arrayValue.length; i < len; i++) {
        out += `<option value="${arrayValue[i]}">${arrayDisplay[i]}</option>`
    }
    return out
}
function field_enum(pojo: string, fd: any, update: boolean, root: FormRoot, ffid: any): string {
    return /**/`
<div class="fluid picker">
  <select${ffid && ffid_attr(ffid) || ''} v-sval:${fd.t}="${pojo}['${fd._}']"
      @change="change($event, '${fd._}', ${pojo}, ${update}, ${root.pojo})">
    ${!update && option_empty || ''}${enum_options(fd.v_fn(), fd.$v_fn())}
  </select>
</div>
`/**/
}

// datepicker flags copied here
export const enum DPFlags {
    UPDATE = 16,
    TRIGGER_CHANGE_ON_SELECT = 32
}
function dpicker(pojo: string, field: number, update: boolean): string {
    return ` v-dpicker:${DPFlags.TRIGGER_CHANGE_ON_SELECT | (update ? DPFlags.UPDATE : 0)}="{ pojo: ${pojo}, field: '${field}' }"`
}
function field_num(pojo: string, fd: any, update: boolean, root: FormRoot, ffid: any): string {
    return /**/`
<div class="ui input">
  <input${ffid && ffid_attr(ffid) || ''} type="text"${fd.o === 2 && dpicker(pojo, fd._, update) || ''}
      v-sval:${!fd.o ? fd.t : (fd.t + ',' + fd.o)}="${pojo}['${fd._}']"
      @change="change($event, '${fd._}', ${pojo}, ${update}, ${root.pojo})" />
  ${fd.$h && help_text(fd.$h) || ''}
  <div v-text="!(${pojo}._.vfbs & ${1 << (fd._ - 1)}) ? '' : ${pojo}._['${fd._}']"></div>
</div>
`/**/
}

function field_textarea(pojo: string, fd: any, update: boolean, root: FormRoot, ffid: any): string {
    return /**/`
<div class="ui input">
  <textarea${ffid && ffid_attr(ffid) || ''} v-sval:${fd.t}="${pojo}['${fd._}']"
      @change="change($event, '${fd._}', ${pojo}, ${update}, ${root.pojo})"></textarea>
  ${fd.$h && help_text(fd.$h) || ''}
  <div v-text="!(${pojo}._.vfbs & ${1 << (fd._ - 1)}) ? '' : ${pojo}._['${fd._}']"></div>
</div>
`/**/
}

function field_default(pojo: string, fd: any, update: boolean, root: FormRoot, ffid: any): string {
    return /**/`
<div class="ui input">
  <input${ffid && ffid_attr(ffid) || ''} type="${fd.pw ? 'password' : 'text'}"
      v-sval:${fd.t}="${pojo}['${fd._}']"
      @change="change($event, '${fd._}', ${pojo}, ${update}, ${root.pojo})" />
  ${fd.$h && help_text(fd.$h) || ''}
  <div v-text="!(${pojo}._.vfbs & ${1 << (fd._ - 1)}) ? '' : ${pojo}._['${fd._}']"></div>
</div>
`/**/
}