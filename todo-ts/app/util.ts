import ToProgress from 'toprogress'

export const CONFIG = window['app_config'] || {},
    TARGET = CONFIG['target'] || 'app'

export function wrapPojo(orig) {
    return { orig }
}

export function mergePojo(src, pair) {
    mergeFrom(src, pair.orig/*, pair.copy*/)
}

export function mergeFrom(src, target/*, other?*/) {
    for (var i of Object.keys(src)) {
        target[i] = src[i]
        //if (other) other[i] = src[i]
    }
    return target
}

function plural(num, unit, suffix) {
    num = ~~num
    var buf = ''
    buf += num
    buf += ' '
    buf += unit
    if (num !== 1)
        buf += 's'
    if (suffix)
        buf += suffix
    
    return buf
}

const MINUTE = 60
const HOUR = 60 * MINUTE
const DAY = 24 * HOUR

export function timebetween(a, b, suffix) {
    const elapsed = b - a;

    if (elapsed === 0) {
        return 'just moments' + suffix
    } else if (elapsed < MINUTE) {
        return plural(elapsed, 'second', suffix);
    } else if (elapsed < HOUR) {
        return plural(elapsed / MINUTE, 'minute', suffix);
    } else if (elapsed < DAY) {
        return plural(elapsed / HOUR, 'hour', suffix);
    } else {
        return plural(elapsed / DAY, 'day', suffix);
    }
}

export function timeago(ts) {
    return timebetween(Math.floor(ts/1000), Math.floor(Date.now()/1000), ' ago')
}

// ================================================== 
// override

const tp = new ToProgress({
    color: '#0080FF'
})
function finish() {
    tp.finish()
}
let ts = 0
// only show progress bar after first successful request
function passThrough(data) {
    if (ts === 0)
        ts = Date.now()
    else
        window.setTimeout(finish, 150)
    return data
}

let rpc_config = window['rpc_config'],
    delegate
function beforeSend() {
    if (!delegate)
        delegate = rpc_config || window['rpc_config_d']

    if (ts !== 0) {
        ts = Date.now()
        tp.show()
    }
}
let override = {
    get$$(location, opts) {
        beforeSend()
        return delegate.get$$(location, opts).then(passThrough)
    },
    post$$(location, opts) {
        beforeSend()
        return delegate.post$$(location, opts).then(passThrough)
    }
}
window['rpc_config'] = override

// resolve rpc host
function isLocal(host: string) {
    return host === '127.0.0.1' || host === 'localhost'
}
let rpc_host = window['rpc_host']
if (!rpc_host && isLocal(window.location.hostname))
    window['rpc_host'] = 'http://127.0.0.1:5000'