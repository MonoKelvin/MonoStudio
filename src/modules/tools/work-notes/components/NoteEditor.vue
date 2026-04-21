<template>
    <div class="inline-editor" :class="{ 'editor-enter': isOpen, 'editor-exit': !isOpen }">
        <div class="editor-header">
            <h3>{{ editingNote ? '编辑笔记' : '新建笔记' }}</h3>
        </div>
        <div class="editor-body">
            <div class="editor-meta">
                <input v-model="localDate" type="date" class="date-input" />
                <input v-model="localTag" type="text" class="tag-input" placeholder="添加标签" list="tag-suggestions" />
                <datalist id="tag-suggestions">
                    <option v-for="tag in allTags" :key="tag" :value="tag"></option>
                </datalist>
            </div>
            <input v-model="localTitle" type="text" class="title-input" placeholder="标题（可选）" />
            <textarea v-model="localContent" class="content-textarea" placeholder="开始记录..." rows="4"></textarea>
        </div>
        <div class="editor-footer">
            <div class="editor-options">
                <label class="important-toggle" :class="{ active: localImportant }">
                    <input type="checkbox" v-model="localImportant" />
                    <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor"
                        stroke-width="2">
                        <polygon
                            points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2">
                        </polygon>
                    </svg>
                    标记重要
                </label>
            </div>
            <div class="editor-actions">
                <BaseButton @click="$emit('cancel')">取消</BaseButton>
                <BaseButton variant="primary" @click="saveNote" :disabled="!localContent.trim()">保存</BaseButton>
            </div>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../components/base/BaseButton.vue';

export default {
    name: 'NoteEditor',
    components: {
        BaseButton
    },
    props: {
        isOpen: {
            type: Boolean,
            default: false
        },
        editingNote: {
            type: Object,
            default: null
        },
        selectedDate: {
            type: String,
            default: null
        },
        allTags: {
            type: Array,
            default: () => []
        }
    },
    emits: ['cancel', 'save'],
    data() {
        return {
            localTitle: '',
            localContent: '',
            localTag: '',
            localDate: '',
            localImportant: false
        };
    },
    watch: {
        editingNote: {
            handler(note) {
                if (note) {
                    this.localTitle = note.title || '';
                    this.localContent = note.content;
                    this.localTag = note.tag || '';
                    this.localDate = note.createdAt.split('T')[0];
                    this.localImportant = note.important || false;
                } else {
                    this.localTitle = '';
                    this.localContent = '';
                    this.localTag = '';
                    this.localImportant = false;
                    this.localDate = this.selectedDate || new Date().toISOString().split('T')[0];
                }
            },
            immediate: true
        },
        selectedDate: {
            handler(newDate, oldDate) {
                if (!this.editingNote && newDate && newDate !== oldDate && this.localDate !== newDate) {
                    this.localDate = newDate;
                }
            }
        }
    },
    methods: {
        saveNote() {
            if (!this.localContent.trim()) return;

            this.$emit('save', {
                title: this.localTitle,
                content: this.localContent,
                tag: this.localTag,
                date: this.localDate,
                important: this.localImportant
            });
        }
    }
};
</script>

<style scoped>
.inline-editor {
    background: var(--bg-secondary);
    border-radius: 8px;
    padding: 16px;
    margin-bottom: 16px;
    border: 1px solid var(--border-color);
    transition: all 0.3s ease;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}

.editor-enter {
    animation: slideDown 0.3s ease forwards;
}

.editor-exit {
    animation: slideUp 0.3s ease forwards;
}

.editor-header {
    margin-bottom: 12px;
}

.editor-header h3 {
    margin: 0;
    font-size: 14px;
    font-weight: 600;
    color: var(--text-primary);
}

.editor-body {
    margin-bottom: 12px;
    display: flex;
    flex-direction: column;
    gap: 8px;
}

.editor-meta {
    display: flex;
    gap: 8px;
}

.date-input,
.tag-input {
    padding: 6px 8px;
    border: 1px solid var(--border-color);
    border-radius: 6px;
    font-size: 12px;
    background: var(--bg-primary);
    color: var(--text-primary);
    flex: 1;
}

.date-input:focus,
.tag-input:focus,
.title-input:focus,
.content-textarea:focus {
    outline: none;
    border-color: var(--accent-color);
    box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.1);
}

.title-input {
    padding: 8px 10px;
    border: 1px solid var(--border-color);
    border-radius: 6px;
    font-size: 14px;
    font-weight: 500;
    background: var(--bg-primary);
    color: var(--text-primary);
}

.content-textarea {
    padding: 10px;
    border: 1px solid var(--border-color);
    border-radius: 6px;
    font-size: 13px;
    line-height: 1.5;
    background: var(--bg-primary);
    color: var(--text-primary);
    resize: vertical;
    font-family: inherit;
    min-height: 100px;
}

.editor-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 12px;
}

.editor-options {
    display: flex;
    align-items: center;
}

.important-toggle {
    display: flex;
    align-items: center;
    gap: 4px;
    font-size: 12px;
    color: var(--text-secondary);
    cursor: pointer;
    padding: 4px 8px;
    border-radius: 6px;
    transition: all 0.2s ease;
}

.important-toggle input {
    display: none;
}

.important-toggle:hover {
    background: var(--bg-tertiary);
}

.important-toggle.active {
    color: var(--warning-color, #f59e0b);
    background: rgba(245, 158, 11, 0.1);
}

.editor-actions {
    display: flex;
    gap: 8px;
}

@keyframes slideDown {
    from {
        opacity: 0;
        transform: translateY(-10px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

@keyframes slideUp {
    from {
        opacity: 1;
        transform: translateY(0);
    }

    to {
        opacity: 0;
        transform: translateY(-10px);
    }
}
</style>
