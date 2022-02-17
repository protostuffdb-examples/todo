import { test } from 'uvu'

import * as tests from '../'

for (const key of Object.keys(tests)) {
    const val = tests[key]
    typeof val === 'function' && test(key, val)
}

test.run();