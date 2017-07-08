import { PojoStore } from 'coreds/lib/pstore/'

export interface Stores {
    todo: PojoStore<any>
}
export const stores: Stores = {} as Stores