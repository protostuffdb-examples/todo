import { ds } from 'coreds/lib/ds/';
import { user } from '../g/user/';

import * as assert from 'uvu/assert';

const $ = user.Todo;

export async function createTodo() {
    const title = 'hello';
    const req = user.Todo.$new(title);
    console.log(JSON.stringify(req));
    const res = await user.Todo.ForUser.create(req);
    const list = res['1']!;
    assert.ok(list.length);
    const latestTodo = list[0];
    assert.equal(latestTodo[$.$.title], title);
    console.log('###========== CREATE ' + JSON.stringify(req) + '\n' + JSON.stringify(res));
}

export async function getFirstTodo() {
    const req = ds.ParamRangeKey.$new(true, 0, 'CgAAAAAAAAAF');
    const res = await user.Todo.ForUser.list(req);
    const list = res['1']!;
    assert.ok(list.length);
    console.log('###========== GET ' + JSON.stringify(req) + '\n' + JSON.stringify(res));
}

export async function listTodos(){
    const limit = 50;
    const req = ds.ParamRangeKey.$new(true, limit);
    const res = await user.Todo.ForUser.list(req);
    const list = res['1']!;
    assert.ok(list.length);
    console.log('###========== LIST(' + limit +', ' + list.length + ') ' + JSON.stringify(req) + '\n' + JSON.stringify(res));
}
