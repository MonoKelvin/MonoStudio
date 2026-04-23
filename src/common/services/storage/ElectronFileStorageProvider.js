/**
 * Electron 文件存储实现
 * 用于在用户数据目录中存储 JSON 数据
 */
export class ElectronFileStorageProvider {
    constructor(options = {}) {
        this.storageKey = options.storageKey || 'default';
    }

    async getItem(key) {
        try {
            const data = await window.electronAPI.userData.getWorkNotes();
            if (!data) return null;
            const parsed = typeof data === 'string' ? JSON.parse(data) : data;
            return JSON.stringify(parsed[key] || null);
        } catch (error) {
            console.error('ElectronFileStorageProvider.getItem failed:', error);
            return null;
        }
    }

    async setItem(key, value) {
        try {
            const data = await window.electronAPI.userData.getWorkNotes();
            const parsed = typeof data === 'string' ? JSON.parse(data) : (data || {});
            parsed[key] = JSON.parse(value);
            await window.electronAPI.userData.saveWorkNotes(parsed);
            return true;
        } catch (error) {
            console.error('ElectronFileStorageProvider.setItem failed:', error);
            return false;
        }
    }

    async removeItem(key) {
        try {
            const data = await window.electronAPI.userData.getWorkNotes();
            const parsed = typeof data === 'string' ? JSON.parse(data) : (data || {});
            delete parsed[key];
            await window.electronAPI.userData.saveWorkNotes(parsed);
            return true;
        } catch (error) {
            console.error('ElectronFileStorageProvider.removeItem failed:', error);
            return false;
        }
    }

    async clear() {
        try {
            await window.electronAPI.userData.saveWorkNotes({});
            return true;
        } catch (error) {
            console.error('ElectronFileStorageProvider.clear failed:', error);
            return false;
        }
    }
}

export default ElectronFileStorageProvider;