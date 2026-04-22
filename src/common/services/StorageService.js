import LocalStorageProvider from './storage/LocalStorageProvider.js';

// 通用存储服务（兼容旧 API）
class StorageService {
    constructor(storageKey, provider = new LocalStorageProvider()) {
        this.storageKey = storageKey;
        this.provider = provider;
    }

    /**
     * 保存数据到存储
     * @param {any} data - 要保存的数据
     */
    save(data) {
        try {
            this.provider.setItem(this.storageKey, JSON.stringify(data));
            return true;
        } catch (error) {
            console.error('保存数据失败:', error);
            return false;
        }
    }

    /**
     * 从存储中加载数据
     * @returns {any} 加载的数据，如果不存在则返回null
     */
    load() {
        try {
            const data = this.provider.getItem(this.storageKey);
            return data ? JSON.parse(data) : null;
        } catch (error) {
            console.error('加载数据失败:', error);
            return null;
        }
    }

    /**
     * 从存储中删除数据
     */
    remove() {
        try {
            this.provider.removeItem(this.storageKey);
            return true;
        } catch (error) {
            console.error('删除数据失败:', error);
            return false;
        }
    }

    /**
     * 清除所有存储数据
     */
    clear() {
        try {
            this.provider.clear();
            return true;
        } catch (error) {
            console.error('清除数据失败:', error);
            return false;
        }
    }
}

export default StorageService;