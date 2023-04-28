import { test } from 'uvu'
import * as assert from 'uvu/assert'

import { createTodo, listTodo } from '../'

const ESTIMATED_TARGET_RESPONSE_LEN = 65535

const TITLE_LEN = 1024
const TITLE = Array(TITLE_LEN).join('a')
const WRAPPER_LEN = '{"1":"CgAAAAAAAAAF","2":1000000000001,"3":"","4":false}]}'.length
const JSON_LEN = TITLE_LEN + WRAPPER_LEN

const OVERFLOW_LIMIT = 1 + Math.floor(ESTIMATED_TARGET_RESPONSE_LEN / JSON_LEN)

test('limit', async () => {
    let todo
    let json: string
    let count = 0
    do {
        todo = await createTodo(undefined, TITLE)
        json = JSON.stringify(todo)
        count += json.length
    } while (count < ESTIMATED_TARGET_RESPONSE_LEN)
    
    const todos = await listTodo(undefined, OVERFLOW_LIMIT)
    console.log(
        `${count}\n`,
        //json,
        `\nLIMIT: ${OVERFLOW_LIMIT}`,
        `\nCOUNT: ${todos.length}`,
    )
    assert.equal(todos.length, OVERFLOW_LIMIT)
})

test.run()