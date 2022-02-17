declare const process: any
import { ds } from 'coreds/lib/ds/'
import { user } from '../g/user/'

import * as assert from 'uvu/assert'

const $ = user.Todo

export interface TestArg {
    __test__: string
    __suite__: string
}

export async function createTodo(arg?: TestArg, title_?: string) {
    const title = title_ || process.env.TODO_NAME || 'hello'
    const req = user.Todo.$new(title)
    const res = await user.Todo.ForUser.create(req)
    const list = res['1']!
    assert.ok(list.length)
    const latestTodo = list[0]
    assert.equal(latestTodo[$.$.title], title)
    arg && console.log(
        '\n### ==================== CREATE',
        `${JSON.stringify(req)}\n${JSON.stringify(res)}`,
    )
    return latestTodo
}

export async function getFirstTodo(arg?: TestArg) {
    const req = ds.ParamRangeKey.$new(true, 0, 'CgAAAAAAAAAF')
    const res = await user.Todo.ForUser.list(req)
    const list = res['1']!
    assert.ok(list.length)
    arg && console.log(
        '\n### ==================== GET',
        `${JSON.stringify(req)}\n${JSON.stringify(res)}`,
    )
    return list[0]
}

export async function listTodo(arg?: TestArg, limit_?: number) {
    const limit = limit_ || 50
    const req = ds.ParamRangeKey.$new(true, limit)
    const res = await user.Todo.ForUser.list(req)
    const list = res['1']!
    assert.ok(list.length)
    arg && console.log(
        '\n### ==================== LIST',
        `${JSON.stringify(req)}\n${JSON.stringify(res)}`,
        `\n### LIMIT: ${limit}, COUNT: ${list.length}`,
    )
    return list
}
