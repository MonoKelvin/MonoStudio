<template>
    <div class="inline-editor" :class="{ 'editor-enter': isOpen, 'editor-exit': !isOpen }">
        <div class="editor-body">
            <div class="editor-meta">
                <input v-model="localDate" type="date" class="date-input" />
                <div class="tag-input-container">
                    <input v-model="localTag" type="text" class="tag-input" placeholder="添加标签"
                        @keydown.enter.prevent="addTag" list="tag-suggestions" />
                    <datalist id="tag-suggestions">
                        <option v-for="tag in allTags" :key="tag" :value="tag"></option>
                    </datalist>
                </div>
            </div>
            <input v-model="localTitle" type="text" class="title-input" placeholder="标题（可选）" />
            <div class="editor-toolbar" v-if="editor">
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('bold') }"
                    @click="editor.chain().focus().toggleBold().run()" title="粗体">
                    <img src="@/assets/icons/bold.svg" alt="粗体" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('italic') }"
                    @click="editor.chain().focus().toggleItalic().run()" title="斜体">
                    <img src="@/assets/icons/italic.svg" alt="斜体" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('underline') }"
                    @click="editor.chain().focus().toggleUnderline().run()" title="下划线">
                    <img src="@/assets/icons/underline.svg" alt="下划线" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('strike') }"
                    @click="editor.chain().focus().toggleStrike().run()" title="删除线">
                    <img src="@/assets/icons/strike.svg" alt="删除线" class="icon" />
                </button>
                <span class="toolbar-divider"></span>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('highlight') }"
                    @click="editor.chain().focus().toggleHighlight().run()" title="高亮">
                    <img src="@/assets/icons/highlight.svg" alt="高亮" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('link') }"
                    @click="showLinkModal = true" title="链接">
                    <img src="@/assets/icons/link.svg" alt="链接" class="icon" />
                </button>
                <span class="toolbar-divider"></span>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('bulletList') }"
                    @click="editor.chain().focus().toggleBulletList().run()" title="无序列表">
                    <img src="@/assets/icons/bullet-list.svg" alt="无序列表" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('orderedList') }"
                    @click="editor.chain().focus().toggleOrderedList().run()" title="有序列表">
                    <img src="@/assets/icons/ordered-list.svg" alt="有序列表" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('taskList') }"
                    @click="editor.chain().focus().toggleTaskList().run()" title="任务列表">
                    <img src="@/assets/icons/task-list.svg" alt="任务列表" class="icon" />
                </button>
                <span class="toolbar-divider"></span>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('blockquote') }"
                    @click="editor.chain().focus().toggleBlockquote().run()" title="引用">
                    <img src="@/assets/icons/blockquote.svg" alt="引用" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" :class="{ active: editor.isActive('codeBlock') }"
                    @click="editor.chain().focus().toggleCodeBlock().run()" title="代码块">
                    <img src="@/assets/icons/code-block.svg" alt="代码块" class="icon" />
                </button>
                <span class="toolbar-divider"></span>
                <button type="button" class="toolbar-btn" @click="editor.chain().focus().undo().run()"
                    :disabled="!editor.can().undo()" title="撤销">
                    <img src="@/assets/icons/undo.svg" alt="撤销" class="icon" />
                </button>
                <button type="button" class="toolbar-btn" @click="editor.chain().focus().redo().run()"
                    :disabled="!editor.can().redo()" title="重做">
                    <img src="@/assets/icons/redo.svg" alt="重做" class="icon" />
                </button>
            </div>
            <div class="editor-content-wrapper">
                <editor-content :editor="editor" class="editor-content" />
            </div>
        </div>
        <div class="editor-footer">
            <div class="editor-options">
                <label class="important-toggle" :class="{ active: localImportant }">
                    <input type="checkbox" v-model="localImportant" />
                    <img src="@/assets/icons/star.svg" alt="重要" class="icon" />
                    标记重要
                </label>
                <div class="editor-stats">
                    <span class="char-count">{{ localContent.length }} 字符</span>
                </div>
            </div>
        </div>

        <!-- 链接输入模态框 -->
        <div class="modal-overlay" v-if="showLinkModal" @click="showLinkModal = false">
            <div class="link-modal" @click.stop>
                <h4>添加链接</h4>
                <div class="link-input-group">
                    <label>链接地址</label>
                    <input v-model="linkUrl" type="text" class="link-input" placeholder="https://" />
                </div>
                <div class="modal-actions">
                    <BaseButton @click="showLinkModal = false">取消</BaseButton>
                    <BaseButton variant="primary" @click="confirmLink">确认</BaseButton>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, watch, onBeforeUnmount, computed } from 'vue';
import BaseButton from '../../../../components/base/BaseButton.vue';
import { useEditor, EditorContent } from '@tiptap/vue-3';
import StarterKit from '@tiptap/starter-kit';
import Underline from '@tiptap/extension-underline';
import Highlight from '@tiptap/extension-highlight';
import Link from '@tiptap/extension-link';
import TaskList from '@tiptap/extension-task-list';
import TaskItem from '@tiptap/extension-task-item';
import Placeholder from '@tiptap/extension-placeholder';
import { NoteModel } from '../services/NoteModel.js';

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

const emit = defineEmits(['cancel', 'save']);

const localTitle = ref('');
const localContent = ref('');
const localTag = ref('');
const localDate = ref('');
const localImportant = ref(false);
const showLinkModal = ref(false);
const linkUrl = ref('');

const editor = useEditor({
    extensions: [
        StarterKit.configure({
            heading: false,
        }),
        Underline,
        Highlight.configure({
            multicolor: false,
        }),
        Link.configure({
            openOnClick: false,
            HTMLAttributes: {
                class: 'editor-link',
            },
        }),
        TaskList,
        TaskItem.configure({
            nested: true,
        }),
        Placeholder.configure({
            placeholder: '开始记录...',
        }),
    ],
    content: '',
    editorProps: {
        attributes: {
            class: 'prose-editor',
        },
    },
    onUpdate: ({ editor }) => {
        localContent.value = editor.getHTML();
    },
});

const confirmLink = () => {
    if (!linkUrl.value) {
        editor.value.chain().focus().extendMarkRange('link').unsetLink().run();
        showLinkModal.value = false;
        linkUrl.value = '';
        return;
    }

    editor.value.chain().focus().extendMarkRange('link').setLink({ href: linkUrl.value }).run();
    showLinkModal.value = false;
    linkUrl.value = '';
};

watch(() => props.editingNote, (note) => {
    if (note) {
        localTitle.value = note.title || '';
        localContent.value = note.content;
        localTag.value = note.tag || '';
        localDate.value = note.createdAt.split('T')[0];
        localImportant.value = note.important || false;

        if (editor.value) {
            editor.value.commands.setContent(note.content || '<p></p>');
        }
    } else {
        localTitle.value = '';
        localContent.value = '';
        localTag.value = '';
        localImportant.value = false;
        localDate.value = props.selectedDate || new Date().toISOString().split('T')[0];

        if (editor.value) {
            editor.value.commands.setContent('<p></p>');
        }
    }
}, { immediate: true });

watch(() => props.selectedDate, (newDate, oldDate) => {
    if (!props.editingNote && newDate && newDate !== oldDate && localDate.value !== newDate) {
        localDate.value = newDate;
    }
});

// 移除自动保存，只保留手动保存

const saveNote = () => {
    const content = localContent.value === '<p></p>' ? '' : localContent.value;
    if (!NoteModel.getPlainText(content).trim()) return;

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
    }
};

onBeforeUnmount(() => {
    if (editor.value) {
        editor.value.destroy();
    }
});

const canSave = computed(() => localContent.value.trim() !== '' && localContent.value !== '<p></p>');

const getNoteData = () => {
    return {
        title: localTitle.value,
        content: localContent.value,
        tag: localTag.value,
        date: localDate.value,
        important: localImportant.value
    };
};

defineExpose({
    getNoteData,
    canSave
});
</script>

<style scoped>
.inline-editor {
    background: var(--background-secondary);
    border-radius: var(--radius-lg);
    padding: var(--spacing-lg);
    margin-bottom: var(--spacing-md);
    border: 1px solid var(--border-color);
    transition: all 0.3s ease;
}

.editor-enter {
    animation: slideDown 0.3s ease forwards;
}

.editor-exit {
    animation: slideUp 0.3s ease forwards;
}

.editor-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: var(--spacing-md);
}

.editor-header h3 {
    margin: 0;
    font-size: var(--font-size-md);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
}

.editor-header-actions {
    display: flex;
    gap: var(--spacing-xs);
}

.editor-body {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.editor-meta {
    display: flex;
    gap: var(--spacing-sm);
}

.tag-input-container {
    position: relative;
    flex: 1;
}

.date-input,
.tag-input {
    padding: var(--spacing-xs) var(--spacing-sm);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    font-size: var(--font-size-sm);
    background: var(--background-tertiary);
    color: var(--text-primary);
    flex: 1;
    transition: all 0.2s ease;
}

.date-input:focus,
.tag-input:focus {
    outline: none;
    border-color: var(--accent-color);
    background: var(--background-primary);
    box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
}

.title-input {
    padding: var(--spacing-sm) var(--spacing-md);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    font-size: var(--font-size-md);
    font-weight: var(--font-weight-medium);
    background: var(--background-tertiary);
    color: var(--text-primary);
    transition: all 0.2s ease;
}

.title-input:focus {
    outline: none;
    border-color: var(--accent-color);
    background: var(--background-primary);
    box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
}

.editor-toolbar {
    display: flex;
    flex-wrap: wrap;
    gap: 2px;
    padding: var(--spacing-xs);
    background: var(--background-tertiary);
    border-radius: var(--radius-md);
    border: 1px solid var(--border-color);
}

.toolbar-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 28px;
    height: 28px;
    padding: 0;
    border: none;
    background: transparent;
    color: var(--text-secondary);
    cursor: pointer;
    border-radius: var(--radius-sm);
    transition: all 0.15s ease;
}

.toolbar-btn:hover:not(:disabled) {
    background: var(--background-hover);
    color: var(--text-primary);
    transform: scale(1.1);
}

.toolbar-btn:active:not(:disabled) {
    transform: scale(0.95);
}

.toolbar-btn.active {
    background: var(--accent-color);
    color: white;
}

.toolbar-btn:disabled {
    opacity: 0.4;
    cursor: not-allowed;
}

.toolbar-divider {
    width: 1px;
    height: 20px;
    background: var(--border-color);
    margin: 4px var(--spacing-xs);
}

.editor-content-wrapper {
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    background: var(--background-tertiary);
    transition: all 0.2s ease;
    overflow: hidden;
}

.editor-content-wrapper:focus-within {
    border-color: var(--accent-color);
    box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
}

.editor-content {
    min-height: 200px;
    max-height: 400px;
    overflow-y: auto;
}

.editor-content :deep(.ProseMirror) {
    padding: var(--spacing-md);
    min-height: 200px;
    outline: none;
    font-size: var(--font-size-sm);
    line-height: 1.6;
    color: var(--text-primary);
}

.editor-content :deep(.ProseMirror p.is-editor-empty:first-child::before) {
    content: attr(data-placeholder);
    float: left;
    color: var(--text-tertiary);
    pointer-events: none;
    height: 0;
}

.editor-content :deep(.ProseMirror p) {
    margin: 0 0 var(--spacing-sm) 0;
}

.editor-content :deep(.ProseMirror p:last-child) {
    margin-bottom: 0;
}

.editor-content :deep(.ProseMirror strong) {
    font-weight: 600;
}

.editor-content :deep(.ProseMirror ul),
.editor-content :deep(.ProseMirror ol) {
    padding-left: var(--spacing-lg);
    margin: var(--spacing-sm) 0;
}

.editor-content :deep(.ProseMirror li) {
    margin: var(--spacing-xs) 0;
}

.editor-content :deep(.ProseMirror blockquote) {
    border-left: 3px solid var(--accent-color);
    padding-left: var(--spacing-md);
    margin: var(--spacing-sm) 0;
    color: var(--text-secondary);
}

.editor-content :deep(.ProseMirror code) {
    background: var(--background-hover);
    padding: 2px 4px;
    border-radius: 3px;
    font-family: 'Consolas', 'Monaco', monospace;
    font-size: 0.9em;
}

.editor-content :deep(.ProseMirror pre) {
    background: var(--background-primary);
    padding: var(--spacing-md);
    border-radius: var(--radius-md);
    overflow-x: auto;
    margin: var(--spacing-sm) 0;
}

.editor-content :deep(.ProseMirror pre code) {
    background: none;
    padding: 0;
}

.editor-content :deep(.ProseMirror a),
.editor-content :deep(.editor-link) {
    color: var(--accent-color);
    text-decoration: underline;
    cursor: pointer;
}

.editor-content :deep(.ProseMirror mark) {
    background: rgba(245, 158, 11, 0.3);
    padding: 1px 2px;
    border-radius: 2px;
}

.editor-content :deep(.ProseMirror hr) {
    border: none;
    border-top: 1px solid var(--border-color);
    margin: var(--spacing-md) 0;
}

.editor-content :deep(.ProseMirror ul[data-type="taskList"]) {
    list-style: none;
    padding-left: 0;
}

.editor-content :deep(.ProseMirror ul[data-type="taskList"] li) {
    display: flex;
    align-items: flex-start;
    gap: var(--spacing-sm);
}

.editor-content :deep(.ProseMirror ul[data-type="taskList"] li input) {
    margin-top: 4px;
    cursor: pointer;
}

.editor-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-top: var(--spacing-md);
}

.editor-options {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
}

.editor-stats {
    font-size: var(--font-size-xs);
    color: var(--text-tertiary);
}

.important-toggle {
    display: flex;
    align-items: center;
    gap: var(--spacing-xs);
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
    cursor: pointer;
    padding: var(--spacing-xs) var(--spacing-sm);
    border-radius: var(--radius-md);
    transition: all 0.2s ease;
}

.important-toggle input {
    display: none;
}

.important-toggle:hover {
    background: var(--background-hover);
    color: var(--text-primary);
}

.important-toggle.active {
    color: var(--warning-color);
    background: rgba(245, 158, 11, 0.1);
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

/* 链接模态框样式 */
.modal-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.link-modal {
    background: var(--background-secondary);
    border-radius: var(--radius-lg);
    padding: var(--spacing-lg);
    min-width: 300px;
    border: 1px solid var(--border-color);
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
}

.link-modal h4 {
    margin: 0 0 var(--spacing-md) 0;
    font-size: var(--font-size-md);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
}

.link-input-group {
    margin-bottom: var(--spacing-md);
}

.link-input-group label {
    display: block;
    margin-bottom: var(--spacing-xs);
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
}

.link-input {
    width: 100%;
    padding: var(--spacing-sm) var(--spacing-md);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    font-size: var(--font-size-sm);
    background: var(--background-tertiary);
    color: var(--text-primary);
    transition: all 0.2s ease;
}

.link-input:focus {
    outline: none;
    border-color: var(--accent-color);
    background: var(--background-primary);
    box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
}

.modal-actions {
    display: flex;
    gap: var(--spacing-sm);
    justify-content: flex-end;
}

@media (max-width: 768px) {
    .inline-editor {
        padding: var(--spacing-md);
    }

    .editor-toolbar {
        gap: 1px;
    }

    .toolbar-btn {
        width: 26px;
        height: 26px;
    }

    .editor-content :deep(.ProseMirror) {
        min-height: 150px;
    }

    .link-modal {
        min-width: 280px;
        padding: var(--spacing-md);
    }
}
</style>
