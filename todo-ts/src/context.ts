import { PojoStore } from 'coreds/lib/pstore/'
import { user } from '../g/user/'

export interface Stores {
    todo: PojoStore<user.Todo>
}
export const stores: Stores = {} as Stores