import { test } from 'uvu'
import * as assert from 'uvu/assert'

import { createTodo, listTodo } from '../'

const MAX_RESPONSE_LEN = 65535

const TITLE_LEN = 1024
const TITLE = Array(TITLE_LEN).join("a")

const OVERFLOW_LIMIT = 1 + Math.floor(MAX_RESPONSE_LEN / TITLE_LEN)

test('limit', async () => {
    const todo = await createTodo(undefined, TITLE)
    let json: string
    let count = 0
    do {
        json = JSON.stringify(todo)
        count += json.length
    } while (count < MAX_RESPONSE_LEN)
    
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