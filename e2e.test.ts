// @ts-ignore
import { afterEach, beforeEach, expect, test } from "bun:test"
import { user } from "./todo-ts/g/user"
import { createTodo, listTodo } from './todo-ts/test/'

const { $ } = user.Todo

const ESTIMATED_TARGET_RESPONSE_LEN = 65535

const TITLE_LEN = 1024
const TITLE = Array(TITLE_LEN).join('a')
const WRAPPER_LEN = '{"1":"CgAAAAAAAAAF","2":1000000000001,"3":"","4":false}]}'.length
const JSON_LEN = TITLE_LEN + WRAPPER_LEN

const OVERFLOW_LIMIT = 1 + Math.floor(ESTIMATED_TARGET_RESPONSE_LEN / JSON_LEN)

let runCount = 0
beforeEach(() => {
    console.log(`\n# ${++runCount}\n# --------------------------------------------------\n`);
})
afterEach(() => console.log(''))

const log = (arg: any[]) => {
    console.log(arg, '\n')
}

test('limit listTodo', async () => {
    let todo: user.Todo
    let json: string
    let count = 0
    do {
        todo = await createTodo(undefined, TITLE)
        json = JSON.stringify(todo)
        count += json.length
    } while (count < ESTIMATED_TARGET_RESPONSE_LEN)
    
    const todos = await listTodo(undefined, OVERFLOW_LIMIT)
    expect(todos.length).toBe(OVERFLOW_LIMIT)
    log([
        'count', count,
        //json,
        'limit', OVERFLOW_LIMIT,
        'expect', todos.length,
    ])
})

const newTitle = 'hello'
test("new todo", async () => {
    const todo = await createTodo(undefined, newTitle)
    expect(todo[$.title]).toBe(newTitle)
    expect(todo[$.key]!.length, 12)
    expect(todo[$.completed]).toBe(false)
    console.log(todo)
});

test("get latest todo", async () => {
    let todos = await listTodo(undefined, 1)
    expect(todos.length).toBe(1)
    expect(todos[0][$.title]).toBe(newTitle)
    console.log(todos)
});


