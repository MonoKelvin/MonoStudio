import LocalStorageProvider from './LocalStorageProvider.js';

/**
 * 存储提供者工厂：统一注册/创建介质实现。
 */
export class StorageFactory {
    constructor() {
        this.registry = new Map();
    }

    register(type, createProvider) {
        if (!type || typeof createProvider !== 'function') {
            throw new Error('StorageFactory.register(type, createProvider) 参数无效');
        }
        this.registry.set(type, createProvider);
    }

    create(type, options = {}) {
        const creator = this.registry.get(type);
        if (!creator) {
            throw new Error(`Unknown storage provider: ${type}`);
        }
        return creator(options);
    }
}

export const storageFactory = new StorageFactory();
storageFactory.register('localStorage', () => new LocalStorageProvider());

export default storageFactory;
