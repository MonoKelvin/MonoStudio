<template>
    <div class="inline-editor" :class="{ 'editor-enter': isOpen, 'editor-exit': !isOpen }">
        <div class="editor-header">
            <h3>{{ editingNote ? '编辑笔记' : '新建笔记' }}</h3>
        </div>
        <div class="editor-body">
            <div class="editor-meta">
                <input v-model="localDate" type="date" class="date-input" />
                <div class="tag-input-container">
                    <input v-model="localTag" type="text" class="tag-input" placeholder="添加标签" 
                           @keydown.enter.prevent="addTag" list="tag-suggestions" />
                    <datalist id="tag-suggestions">
                        <option v-for="tag in allTags" :key="tag" :value="tag"></option>
                    </datalist>
                    <div class="tag-suggestions" v-if="showTagSuggestions && filteredTags.length > 0">
                        <div v-for="tag in filteredTags" :key="tag" 
                             class="tag-suggestion" @click="selectTag(tag)">
                            {{ tag }}
                        </div>
                    </div>
                </div>
            </div>
            <input v-model="localTitle" type="text" class="title-input" placeholder="标题（可选）" />
            <textarea v-model="localContent" class="content-textarea" placeholder="开始记录..." rows="4"
                      @keydown.ctrl.enter="saveNote"></textarea>
        </div>
        <div class="editor-footer">
            <div class="editor-options">
                <label class="important-toggle" :class="{ active: localImportant }">
                    <input type="checkbox" v-model="localImportant" />
                    <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor" stroke-width="2">
                        <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
                    </svg>
                    标记重要
                </label>
                <div class="editor-stats">
                    <span class="char-count">{{ localContent.length }} 字符</span>
                </div>
            </div>
            <div class="editor-actions">
                <BaseButton @click="$emit('cancel')">取消</BaseButton>
                <BaseButton variant="primary" @click="saveNote" :disabled="!localContent.trim()">
                    保存
                    <span class="save-hint">(Ctrl+Enter)</span>
                </BaseButton>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, watch } from 'vue';
import BaseButton from '../../../../components/base/BaseButton.vue';

// Props
const props = defineProps({
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
});

// Emits
const emit = defineEmits(['cancel', 'save']);

// Reactive state
const localTitle = ref('');
const localContent = ref('');
const localTag = ref('');
const localDate = ref('');
const localImportant = ref(false);
const showTagSuggestions = ref(false);
const autoSaveTimeout = ref(null);
const lastSavedContent = ref('');

// Computed properties
const filteredTags = computed(() => {
    if (!localTag.value) return [];
    return props.allTags.filter(tag => 
        tag.toLowerCase().includes(localTag.value.toLowerCase())
    );
});

// Watchers
watch(() => props.editingNote, (note) => {
    if (note) {
        localTitle.value = note.title || '';
        localContent.value = note.content;
        localTag.value = note.tag || '';
        localDate.value = note.createdAt.split('T')[0];
        localImportant.value = note.important || false;
        lastSavedContent.value = note.content;
    } else {
        localTitle.value = '';
        localContent.value = '';
        localTag.value = '';
        localImportant.value = false;
        localDate.value = props.selectedDate || new Date().toISOString().split('T')[0];
        lastSavedContent.value = '';
    }
}, { immediate: true });

watch(() => props.selectedDate, (newDate, oldDate) => {
    if (!props.editingNote && newDate && newDate !== oldDate && localDate.value !== newDate) {
        localDate.value = newDate;
    }
});

// 监听内容变化，实现自动保存
watch([localTitle, localContent, localTag, localDate, localImportant], () => {
    if (!localContent.value.trim()) return;
    
    // 清除之前的超时
    if (autoSaveTimeout.value) {
        clearTimeout(autoSaveTimeout.value);
    }
    
    // 设置新的超时，3秒后自动保存
    autoSaveTimeout.value = setTimeout(() => {
        if (localContent.value !== lastSavedContent.value) {
            saveNote();
            lastSavedContent.value = localContent.value;
        }
    }, 3000);
}, { deep: true });

// Methods
const saveNote = () => {
    if (!localContent.value.trim()) return;

    emit('save', {
        title: localTitle.value,
        content: localContent.value,
        tag: localTag.value,
        date: localDate.value,
        important: localImportant.value
    });
};

const addTag = () => {
    if (localTag.value.trim()) {
        // Tag is already handled by v-model
        showTagSuggestions.value = false;
    }
};

const selectTag = (tag) => {
    localTag.value = tag;
    showTagSuggestions.value = false;
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

.tag-input-container {
    position: relative;
    flex: 1;
}

.tag-suggestions {
    position: absolute;
    top: 100%;
    left: 0;
    right: 0;
    background: var(--bg-primary);
    border: 1px solid var(--border-color);
    border-radius: 6px;
    max-height: 120px;
    overflow-y: auto;
    z-index: 100;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
    margin-top: 2px;
    animation: slideDown 0.2s ease;
}

.tag-suggestion {
    padding: 6px 10px;
    cursor: pointer;
    font-size: 12px;
    color: var(--text-primary);
    transition: all 0.2s ease;
}

.tag-suggestion:hover {
    background: var(--bg-secondary);
    color: var(--accent-color);
}

.date-input,
.category-select,
.tag-input {
    padding: 6px 8px;
    border: 1px solid var(--border-color);
    border-radius: 6px;
    font-size: 12px;
    background: var(--bg-primary);
    color: var(--text-primary);
    flex: 1;
}

.category-select {
    cursor: pointer;
}

.category-select:focus {
    outline: none;
    border-color: var(--accent-color);
    box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.1);
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
    gap: 12px;
}

.editor-stats {
    font-size: 11px;
    color: var(--text-tertiary);
}

.char-count {
    font-family: monospace;
}

.save-hint {
    font-size: 11px;
    opacity: 0.7;
    margin-left: 4px;
    font-style: italic;
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
