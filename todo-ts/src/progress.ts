// converted from https://github.com/djyde/ToProgress
// This files exists because the original impl used arguments.callee, 
// which is not allwed strict mode

function mergeFrom<T>(src, target: T): T {
    for (var i of Object.keys(src)) {
        if (undefined === target[i]) target[i] = src[i]
    }
    return target
}

function applyStyle(el: any/*Element*/, style_map: any) {
    let style = el.style
    for (var i of Object.keys(style_map)) style[i] = style_map[i]
    return el
}

function resolveTransitionEvent() {
    var el = document.createElement("fakeelement")
    var transitions = {
        "transition": "transitionend",
        "OTransition": "oTransitionEnd",
        "MozTransition": "transitionend",
        "WebkitTransition": "webkitTransitionEnd"
    }
    for (var i of Object.keys(transitions)) {
        if (undefined !== el.style[i]) return transitions[i]
    }
    return undefined
}

const TRANSITION_EVENT = resolveTransitionEvent()

export interface Opts {
    id?: string
    color?: string
    height?: string
    duration?: number
}

export class ToProgress {
    progress = 0
    options = {
        color: '#0080FF',
        height: '2px',
        duration: 0.2
    }
    opapacity_duration: number
    el: any
    listen$$key: any
    listen$$: any
    constructor(opts: Opts, parentEl?: any, id?: string) {
        let options = mergeFrom(opts, this.options),
            parent = parentEl || document.body,
            d = options.duration,
            od = this.opapacity_duration = d * 3
        this.el = applyStyle(document.createElement('div'), {
            'position': parentEl ? 'relative' : 'fixed',
            'top': '0',
            'left': '0',
            'right': '0',
            'background-color': options.color,
            'height': options.height,
            'width': '0%',
            'transition': 'width ' + d + 's' + ', opacity ' + od + 's',
            '-moz-transition': 'width ' + d + 's' + ', opacity ' + od + 's',
            '-webkit-transition': 'width ' + d + 's' + ', opacity ' + od + 's'
        })
        if (id) this.el.id = id
        parent.appendChild(this.el)
        if (TRANSITION_EVENT) this.listen$$ = this.listen.bind(this)
    }

    show() {
        this.el.style.opacity = '1'
    }

    hide () {
        this.el.style.opacity = '0'
    }

    transit() {
        this.el.style.width = this.progress + '%'
    }

    setProgress(progress, callback?) {
        this.show()
        if (progress > 100) {
            this.progress = 100
        } else if (progress < 0) {
            this.progress = 0
        } else {
            this.progress = progress
        }
        this.transit()
        callback && callback()
    }
    increase(val, callback?) {
        this.show()
        this.setProgress(this.progress + val, callback)
    }
    decrease(val, callback?) {
        this.show()
        this.setProgress(this.progress - val, callback)
    }
    reset(callback?) {
        this.progress = 0
        this.transit()
        callback && callback()
    }
    listen(e) {
        this.reset()
        this.el.removeEventListener(e.type, this.listen$$key)
    }
    finish(callback?) {
        this.setProgress(100, callback)
        this.hide()
        if (TRANSITION_EVENT)
            this.listen$$key = this.el.addEventListener(TRANSITION_EVENT, this.listen$$)
    }
}
