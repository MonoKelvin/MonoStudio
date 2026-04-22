import StorageProvider from './StorageProvider.js';

/**
 * localStorage 实现（基础设施层）。
 */
export class LocalStorageProvider extends StorageProvider {
    constructor() {
        super('localStorage');
    }

    getItem(key) {
        return localStorage.getItem(key);
    }

    setItem(key, value) {
        localStorage.setItem(key, value);
        return true;
    }

    removeItem(key) {
        localStorage.removeItem(key);
        return true;
    }

    clear() {
        localStorage.clear();
        return true;
    }
}

export default LocalStorageProvider;
