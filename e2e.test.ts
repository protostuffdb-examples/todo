import { expect, test } from "bun:test";
import { listTodo } from './todo-ts/test/'

/*
test("2 + 2", () => {
    expect(2 + 2).toBe(4);
});
*/

test("get latest todo", async () => {
    let todos = await listTodo(undefined, 1)
    expect(todos.length).toBe(1)
    console.log(todos)
});
