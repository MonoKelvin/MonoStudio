import storageFactory from './StorageFactory.js';

/**
 * 通用 JSON 存储封装，不包含业务语义。
 */
export class JsonStore {
    constructor({ key, providerType = 'localStorage', provider = null, factory = storageFactory }) {
        if (!key) {
            throw new Error('JsonStore: key is required');
        }
        this.key = key;
        this.providerType = providerType;
        this.provider = provider || factory.create(providerType);
    }

    load(defaultValue = null) {
        try {
            const raw = this.provider.getItem(this.key);
            if (!raw) return defaultValue;
            return JSON.parse(raw);
        } catch (error) {
            console.error('JsonStore.load failed:', error);
            return defaultValue;
        }
    }

    save(value) {
        try {
            const raw = JSON.stringify(value);
            this.provider.setItem(this.key, raw);
            return true;
        } catch (error) {
            console.error('JsonStore.save failed:', error);
            return false;
        }
    }

    remove() {
        try {
            this.provider.removeItem(this.key);
            return true;
        } catch (error) {
            console.error('JsonStore.remove failed:', error);
            return false;
        }
    }
}

export default JsonStore;
