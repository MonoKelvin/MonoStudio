/**
 * 笔记数据模型
 * 定义笔记的数据结构，与存储方式和UI分离
 */
export const NoteModel = {
    /**
     * 创建新的笔记对象
     * @param {Object} data - 笔记数据
     * @returns {Object} 规范化的笔记对象
     */
    create(data = {}) {
        const now = new Date().toISOString();
        const normalizedContent = typeof data.content === 'string' ? data.content : '';
        const normalizedTag = typeof data.tag === 'string' ? data.tag.trim() : '';
        return {
            id: data.id || (globalThis.crypto?.randomUUID?.() || `${Date.now().toString(36)}${Math.random().toString(36).slice(2, 10)}`),
            title: typeof data.title === 'string' ? data.title.trim() : '',
            content: normalizedContent,
            contentType: data.contentType || 'text', // text, markdown, html
            tag: normalizedTag,
            createdAt: data.createdAt || now,
            updatedAt: data.updatedAt || now,
            important: Boolean(data.important),
            pinned: Boolean(data.pinned),
            deleted: Boolean(data.deleted),
            deletedAt: data.deletedAt || null,
            metadata: data.metadata || {
                wordCount: 0,
                charCount: 0,
                lastEditPosition: 0
            }
        };
    },

    /**
     * 验证笔记对象是否有效
     * @param {Object} note - 笔记对象
     * @returns {Boolean} 是否有效
     */
    validate(note) {
        if (!note || typeof note !== 'object') return false;
        if (!note.id || typeof note.id !== 'string') return false;
        if (!note.createdAt || typeof note.createdAt !== 'string') return false;
        return true;
    },

    /**
     * 更新笔记的元数据
     * @param {Object} note - 笔记对象
     * @returns {Object} 更新后的笔记对象
     */
    updateMetadata(note) {
        const content = typeof note.content === 'string' ? note.content : '';
        const plainText = this.getPlainText(content);
        return {
            ...note,
            metadata: {
                ...note.metadata,
                wordCount: plainText.trim() ? plainText.trim().split(/\s+/).length : 0,
                charCount: plainText.length,
                lastEditPosition: plainText.length,
                lastEditedAt: new Date().toISOString()
            },
            updatedAt: new Date().toISOString()
        };
    },

    /**
     * 生成用于搜索和预览的纯文本
     * @param {String} content
     * @returns {String}
     */
    getPlainText(content = '') {
        return content
            .replace(/<style[^>]*>[\s\S]*?<\/style>/gi, ' ')
            .replace(/<script[^>]*>[\s\S]*?<\/script>/gi, ' ')
            .replace(/<[^>]+>/g, ' ')
            .replace(/&nbsp;/g, ' ')
            .replace(/&amp;/g, '&')
            .replace(/&lt;/g, '<')
            .replace(/&gt;/g, '>')
            .replace(/&#39;/g, "'")
            .replace(/&quot;/g, '"')
            .replace(/\s+/g, ' ')
            .trim();
    },

    /**
     * 将笔记标记为已删除
     * @param {Object} note - 笔记对象
     * @returns {Object} 更新后的笔记对象
     */
    markDeleted(note) {
        return {
            ...note,
            deleted: true,
            deletedAt: new Date().toISOString(),
            updatedAt: new Date().toISOString()
        };
    },

    /**
     * 恢复已删除的笔记
     * @param {Object} note - 笔记对象
     * @returns {Object} 更新后的笔记对象
     */
    restore(note) {
        return {
            ...note,
            deleted: false,
            deletedAt: null,
            updatedAt: new Date().toISOString()
        };
    },

    /**
     * 序列化笔记用于存储
     * @param {Object} note - 笔记对象
     * @returns {String} JSON字符串
     */
    serialize(note) {
        return JSON.stringify(note);
    },

    /**
     * 反序列化笔记
     * @param {String} json - JSON字符串
     * @returns {Object} 笔记对象
     */
    deserialize(json) {
        try {
            const note = JSON.parse(json);
            return this.create(note);
        } catch (e) {
            console.error('Failed to deserialize note:', e);
            return null;
        }
    }
};

export default NoteModel;