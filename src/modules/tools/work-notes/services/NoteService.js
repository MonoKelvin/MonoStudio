/**
 * 笔记业务服务
 * 使用 Electron API 直接进行文件存储
 */
import { NoteModel } from './NoteModel.js';
import workNotesStorageConfig from './storage.config.js';

/**
 * 存储服务类
 * 提供笔记的CRUD操作，使用 Electron 文件存储
 */
class NoteService {
    constructor() {
        this._cache = [];
        this._initialized = false;
        this._config = { ...workNotesStorageConfig };
    }

    /**
     * 初始化存储服务
     * @param {Object} config - 存储配置
     * @returns {Promise<Boolean>}
     */
    async init(config = {}) {
        try {
            this._config = {
                ...this._config,
                ...config
            };
            this._cache = await this._loadFromStore();
            this._initialized = true;
            return true;
        } catch (e) {
            console.error('NoteService init failed:', e);
            return false;
        }
    }

    /**
     * 检查服务是否已初始化
     * @returns {Boolean}
     */
    isInitialized() {
        return this._initialized;
    }

    /**
     * 获取当前存储适配器类型
     * @returns {String}
     */
    getAdapterType() {
        return 'electronFile';
    }

    /**
     * 获取所有笔记
     * @returns {Promise<Array>}
     */
    async getAllNotes() {
        this._checkInit();
        return this._activeNotes();
    }

    /**
     * 根据ID获取笔记
     * @param {String} id
     * @returns {Promise<Object|null>}
     */
    async getNoteById(id) {
        this._checkInit();
        return this._activeNotes().find((note) => note.id === id) || null;
    }

    /**
     * 创建新笔记
     * @param {Object} noteData
     * @returns {Promise<Object>}
     */
    async createNote(noteData) {
        this._checkInit();
        const note = NoteModel.updateMetadata(NoteModel.create(noteData));
        this._cache.push(note);
        const success = await this._persist();
        return success ? note : null;
    }

    /**
     * 更新笔记
     * @param {Object} noteData
     * @returns {Promise<Boolean>}
     */
    async updateNote(noteData) {
        this._checkInit();
        const existing = await this.getNoteById(noteData.id);
        if (!existing) return false;
        const note = NoteModel.updateMetadata(NoteModel.create({
            ...existing,
            ...noteData,
            createdAt: noteData.createdAt || existing.createdAt
        }));
        return this._upsert(note);
    }

    /**
     * 保存笔记（新增或更新）
     * @param {Object} noteData
     * @returns {Promise<Boolean>}
     */
    async saveNote(noteData) {
        this._checkInit();
        const existing = noteData.id ? await this.getNoteById(noteData.id) : null;
        const note = NoteModel.updateMetadata(NoteModel.create({
            ...existing,
            ...noteData,
            createdAt: existing?.createdAt || noteData.createdAt
        }));
        return this._upsert(note);
    }

    /**
     * 将笔记移至回收站（软删除）
     * @param {String} id
     * @returns {Promise<Boolean>}
     */
    async deleteNote(id) {
        this._checkInit();
        const index = this._cache.findIndex((note) => note.id === id);
        if (index === -1) return false;
        this._cache[index] = NoteModel.markDeleted(this._cache[index]);
        return await this._persist();
    }

    /**
     * 永久删除笔记
     * @param {String} id
     * @returns {Promise<Boolean>}
     */
    async permanentDeleteNote(id) {
        this._checkInit();
        const before = this._cache.length;
        this._cache = this._cache.filter((note) => note.id !== id);
        if (this._cache.length === before) return false;
        return await this._persist();
    }

    /**
     * 批量永久删除笔记
     * @param {Array<String>} ids
     * @returns {Promise<Boolean>}
     */
    async permanentDeleteNotes(ids) {
        this._checkInit();
        const idSet = new Set(ids);
        this._cache = this._cache.filter((note) => !idSet.has(note.id));
        return await this._persist();
    }

    /**
     * 恢复已删除的笔记
     * @param {Object} note
     * @returns {Promise<Boolean>}
     */
    async restoreNote(note) {
        this._checkInit();
        const restored = NoteModel.restore(note);
        return await this._upsert(restored);
    }

    /**
     * 搜索笔记
     * @param {String} query
     * @returns {Promise<Array>}
     */
    async searchNotes(query) {
        this._checkInit();
        const lowerQuery = query.toLowerCase();
        return this._activeNotes().filter((note) => {
            const content = NoteModel.getPlainText(note.content).toLowerCase();
            return note.title.toLowerCase().includes(lowerQuery)
                || content.includes(lowerQuery)
                || (note.tag && note.tag.toLowerCase().includes(lowerQuery));
        });
    }

    /**
     * 获取所有标签
     * @returns {Promise<Array<String>>}
     */
    async getAllTags() {
        this._checkInit();
        const tags = new Set();
        this._activeNotes()
            .filter((note) => note.tag)
            .forEach((note) => tags.add(note.tag));
        return Array.from(tags).sort();
    }

    /**
     * 获取统计数据
     * @returns {Promise<Object>}
     */
    async getStats() {
        this._checkInit();
        const notes = this._activeNotes();
        const today = new Date().toISOString().split('T')[0];
        return {
            total: notes.length,
            important: notes.filter((n) => n.important).length,
            today: notes.filter((n) => n.createdAt.split('T')[0] === today).length,
            tags: (await this.getAllTags()).length
        };
    }

    /**
     * 获取回收站中的笔记
     * @returns {Promise<Array>}
     */
    async getTrash() {
        this._checkInit();
        return this._cache
            .filter((note) => note.deleted)
            .sort((a, b) => new Date(b.deletedAt) - new Date(a.deletedAt));
    }

    /**
     * 清空回收站
     * @returns {Promise<Boolean>}
     */
    async clearTrash() {
        this._checkInit();
        this._cache = this._cache.filter((note) => !note.deleted);
        return this._persist();
    }

    /**
     * 导出所有笔记
     * @returns {Promise<String>}
     */
    async exportNotes() {
        this._checkInit();
        const exportData = {
            version: this._config.version || '2.0',
            exportDate: new Date().toISOString(),
            notes: this._activeNotes()
        };
        return JSON.stringify(exportData, null, 2);
    }

    /**
     * 导入笔记
     * @param {String} data
     * @returns {Promise<Boolean>}
     */
    async importNotes(data) {
        this._checkInit();
        try {
            const importData = JSON.parse(data);
            if (!Array.isArray(importData.notes)) return false;
            for (const item of importData.notes) {
                const note = NoteModel.updateMetadata(NoteModel.create(item));
                this._upsertCacheOnly(note);
            }
            return this._persist();
        } catch (error) {
            console.error('NoteService import failed:', error);
            return false;
        }
    }

    /**
     * 切换存储后端
     * @param {String} adapterType
     * @returns {Promise<Boolean>}
     */
    async switchAdapter(providerType) {
        this._initialized = false;
        return await this.init({ providerType });
    }

    async _loadFromStore() {
        try {
            const payload = await window.electronAPI.userData.getWorkNotes();
            const rawNotes = Array.isArray(payload) ? payload : (payload?.notes || []);
            return rawNotes
                .map((note) => NoteModel.updateMetadata(NoteModel.create(note)))
                .sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
        } catch (error) {
            console.error('NoteService _loadFromStore failed:', error);
            return [];
        }
    }

    _activeNotes() {
        return this._cache
            .filter((note) => !note.deleted)
            .sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
    }

    async _persist() {
        try {
            const data = {
                version: this._config.version || '2.0',
                lastModified: new Date().toISOString(),
                notes: this._cache
            };
            await window.electronAPI.userData.saveWorkNotes(data);
            return true;
        } catch (error) {
            console.error('NoteService _persist failed:', error);
            return false;
        }
    }

    _upsertCacheOnly(note) {
        const index = this._cache.findIndex((n) => n.id === note.id);
        if (index === -1) {
            this._cache.push(note);
        } else {
            this._cache[index] = {
                ...this._cache[index],
                ...note
            };
        }
    }

    async _upsert(note) {
        this._upsertCacheOnly(note);
        return await this._persist();
    }

    /**
     * 检查服务是否已初始化
     * @throws {Error} 如果未初始化
     * @private
     */
    _checkInit() {
        if (!this._initialized) {
            throw new Error('NoteService is not initialized. Call init() first.');
        }
    }
}

// 导出单例
export const noteService = new NoteService();
export default noteService;
