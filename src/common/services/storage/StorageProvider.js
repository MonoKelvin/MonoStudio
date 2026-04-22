/**
 * 通用存储提供者抽象类（基础设施层）。
 * 提供 key-value 能力，不包含任何业务语义。
 */
export class StorageProvider {
    constructor(type = 'abstract') {
        this.type = type;
    }

    getItem() {
        throw new Error('StorageProvider.getItem() must be implemented');
    }

    setItem() {
        throw new Error('StorageProvider.setItem() must be implemented');
    }

    removeItem() {
        throw new Error('StorageProvider.removeItem() must be implemented');
    }

    clear() {
        throw new Error('StorageProvider.clear() must be implemented');
    }
}

export default StorageProvider;
