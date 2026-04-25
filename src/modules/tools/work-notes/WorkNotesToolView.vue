<template>
    <BasePanel class="work-notes-panel">
        <div class="work-notes-container">
            <div class="work-notes-header">
                <div class="header-left">
                    <div class="view-toggle" role="tablist" aria-label="视图切换">
                        <button class="view-btn" :class="{ active: currentView === 'calendar' }"
                            @click="currentView = 'calendar'" type="button">
                            日历
                        </button>
                        <button class="view-btn" :class="{ active: currentView === 'list' }"
                            @click="currentView = 'list'" type="button">
                            列表
                        </button>
                    </div>
                </div>
                <div class="header-actions">
                    <div class="header-buttons" v-if="!isEditorOpen">
                        <BaseButton @click="exportNotes" size="sm" :disabled="notes.length === 0">
                            <img src="@/assets/icons/export.svg" alt="导出" class="icon header-icon" />
                            导出
                        </BaseButton>
                        <BaseButton @click="toggleEditor(null)" :variant="isEditorOpen ? 'danger' : 'primary'"
                            size="sm">
                            <img v-if="!isEditorOpen" src="@/assets/icons/add.svg" alt="新建" class="icon" />
                            <img v-else src="@/assets/icons/cancel.svg" alt="取消" class="icon" />
                            {{ isEditorOpen ? '取消' : '新建笔记' }}
                        </BaseButton>
                    </div>
                    <div class="header-buttons" v-else>
                        <BaseButton @click="handleCancelEditor" size="sm">
                            <img src="@/assets/icons/cancel.svg" alt="取消" class="icon" />
                            取消
                        </BaseButton>
                        <BaseButton @click="handleSaveNote" size="sm" variant="primary" :disabled="!canSaveNote">
                            保存
                        </BaseButton>
                    </div>
                </div>
            </div>

            <NoteEditor v-if="isEditorOpen" ref="noteEditorRef" :is-open="isEditorOpen" :editing-note="editingNote"
                :selected-date="selectedDate" :all-tags="allTags" />

            <div class="main-content" v-if="!isEditorOpen">
                <div class="content-area">
                    <CalendarView v-if="currentView === 'calendar'" :selected-date="selectedDate"
                        :current-date="currentDate" :notes="notes" :search-query="searchQuery"
                        :selected-tags="selectedTags" :all-tags="allTags" @prev-month="handleDateChange"
                        @select-date="selectDate" @open-detail="openDetail" @edit-note="toggleEditor"
                        @delete-note="confirmDelete" @add-note="openEditorForDate" />

                    <ListView v-else :notes="notes" :search-query="searchQuery" :selected-tags="selectedTags"
                        @open-detail="openDetail" @edit-note="toggleEditor" @delete-note="confirmDelete" />
                </div>
            </div>

            <NotesDrawer v-if="!isEditorOpen && currentView === 'calendar'" :selected-date="selectedDate" :notes="notes"
                :search-query="searchQuery" :selected-tags="selectedTags" :all-tags="allTags" @open-detail="openDetail"
                @edit-note="toggleEditor" @delete-note="confirmDelete" @add-note="openEditorForDate"
                @update:searchQuery="searchQuery = $event" @update:selectedTags="selectedTags = $event" />

            <div class="modal-overlay" v-if="showDetail" @click="closeDetail">
                <div class="detail-modal" @click.stop>
                    <div class="detail-header">
                        <div class="detail-meta">
                            <span class="detail-date">{{ formatDetailDate }}</span>
                            <span v-if="detailNote?.tag" class="detail-tag">{{ detailNote.tag }}</span>
                            <span v-if="detailNote?.important" class="detail-important">
                                <img src="@/assets/icons/star.svg" alt="重要" class="icon" />
                                重要
                            </span>
                        </div>
                        <button class="close-btn" @click="closeDetail">
                            <img src="@/assets/icons/cancel.svg" alt="关闭" class="icon" />
                        </button>
                    </div>
                    <div class="detail-body">
                        <h2 class="detail-title">{{ detailNote?.title || '无标题' }}</h2>
                        <div class="detail-content">{{ detailContentText }}</div>
                    </div>
                    <div class="detail-footer">
                        <div class="detail-info">
                            <span>创建于 {{ formatDetailCreatedAt }}</span>
                            <span v-if="detailNote?.updatedAt !== detailNote?.createdAt">· 更新于 {{ formatDetailUpdatedAt
                            }}</span>
                        </div>
                        <div class="detail-actions">
                            <BaseButton @click="toggleEditor(detailNote)">编辑</BaseButton>
                            <BaseButton variant="danger" @click="confirmDelete(detailNote)">删除</BaseButton>
                        </div>
                    </div>
                </div>
            </div>

            <div class="modal-overlay" v-if="showDeleteConfirm" @click="cancelDelete">
                <div class="confirm-modal" @click.stop>
                    <div class="confirm-icon">
                        <img src="@/assets/icons/warning.svg" alt="警告" class="icon" />
                    </div>
                    <h3>确认删除</h3>
                    <p>确定要删除这条笔记吗？此操作无法撤销。</p>
                    <div class="confirm-actions">
                        <BaseButton @click="cancelDelete">取消</BaseButton>
                        <BaseButton variant="danger" @click="executeDelete">删除</BaseButton>
                    </div>
                </div>
            </div>


        </div>
    </BasePanel>
</template>

<script setup>
import { ref, computed, watch, onMounted } from 'vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import CalendarView from './components/CalendarView.vue';
import ListView from './components/ListView.vue';
import NotesDrawer from './components/NotesDrawer.vue';
import NoteEditor from './components/NoteEditor.vue';
import noteService from './services/NoteService.js';
import { NoteModel } from './services/NoteModel.js';
import { NoteQueryService } from './services/NoteQueryService.js';

const notes = ref([]);
const searchQuery = ref('');
const selectedTags = ref([]);
const currentView = ref('calendar');
const currentDate = ref(new Date());
const selectedDate = ref(null);
const isEditorOpen = ref(false);
const showDetail = ref(false);
const showDeleteConfirm = ref(false);
const editingNote = ref(null);
const detailNote = ref(null);
const deleteTargetNote = ref(null);
const allTags = ref([]);
const noteEditorRef = ref(null);

const canSaveNote = computed(() => noteEditorRef.value?.canSave ?? false);

const formatDetailDate = computed(() => {
    if (!detailNote.value) return '';
    return new Date(detailNote.value.createdAt).toLocaleDateString('zh-CN', {
        year: 'numeric',
        month: 'long',
        day: 'numeric',
        weekday: 'long'
    });
});

const formatDetailCreatedAt = computed(() => {
    if (!detailNote.value) return '';
    return new Date(detailNote.value.createdAt).toLocaleString('zh-CN', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit'
    });
});

const formatDetailUpdatedAt = computed(() => {
    if (!detailNote.value) return '';
    return new Date(detailNote.value.updatedAt).toLocaleString('zh-CN', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit'
    });
});

const detailContentText = computed(() => {
    if (!detailNote.value) return '';
    return NoteModel.getPlainText(detailNote.value.content || '');
});

watch(notes, () => {
    if (!notes.value || notes.value.length === 0) {
        allTags.value = [];
        return;
    }
    const tags = new Set();
    notes.value.forEach(note => {
        if (note.tag) {
            tags.add(note.tag);
        }
    });
    allTags.value = Array.from(tags).sort();
}, { immediate: true });

onMounted(async () => {
    await noteService.init();
    await loadNotes();
    selectedDate.value = new Date().toISOString().split('T')[0];
});

const loadNotes = async () => {
    notes.value = await noteService.getAllNotes();
};

const refreshNotes = async () => {
    await loadNotes();
};

const handleDateChange = (newDate) => {
    if (newDate instanceof Date) {
        currentDate.value = newDate;
    }
};

const selectDate = (dateString) => {
    if (selectedDate.value !== dateString) {
        selectedDate.value = dateString;
    }
};

const toggleEditor = (note = null) => {
    if (isEditorOpen.value && !note) {
        isEditorOpen.value = false;
        editingNote.value = null;
        return;
    }

    if (isEditorOpen.value && note) {
        editingNote.value = note;
        return;
    }

    editingNote.value = note;
    showDetail.value = false;
    isEditorOpen.value = true;
};

const handleCancelEditor = () => {
    isEditorOpen.value = false;
    editingNote.value = null;
};

const handleSaveNote = async () => {
    if (!noteEditorRef.value) return;
    const noteData = noteEditorRef.value.getNoteData();
    const plainContent = NoteModel.getPlainText(noteData.content || '');
    if (!plainContent.trim()) return;

    const existing = editingNote.value || null;
    const fallbackDate = existing?.createdAt || new Date().toISOString();
    const nextCreatedAt = composeDateTime(noteData.date, fallbackDate);
    const payload = {
        ...existing,
        title: noteData.title,
        content: noteData.content,
        tag: noteData.tag,
        createdAt: nextCreatedAt,
        important: noteData.important
    };

    await noteService.saveNote(payload);
    await refreshNotes();
    isEditorOpen.value = false;
    editingNote.value = null;
};

const openEditorForDate = (date) => {
    selectedDate.value = date;
    toggleEditor(null);
};

const composeDateTime = (dateValue, fallbackDateTime) => {
    if (!dateValue) return fallbackDateTime;
    const now = new Date();
    const hh = String(now.getHours()).padStart(2, '0');
    const mm = String(now.getMinutes()).padStart(2, '0');
    const ss = String(now.getSeconds()).padStart(2, '0');
    return `${dateValue}T${hh}:${mm}:${ss}`;
};

const saveNote = async (noteData) => {
    const plainContent = NoteModel.getPlainText(noteData.content || '');
    if (!plainContent.trim()) return;

    const existing = editingNote.value || null;
    const fallbackDate = existing?.createdAt || new Date().toISOString();
    const nextCreatedAt = composeDateTime(noteData.date, fallbackDate);
    const payload = {
        ...existing,
        title: noteData.title,
        content: noteData.content,
        tag: noteData.tag,
        createdAt: nextCreatedAt,
        important: noteData.important
    };

    await noteService.saveNote(payload);
    await refreshNotes();
    isEditorOpen.value = false;
    editingNote.value = null;
};

const openDetail = (note) => {
    detailNote.value = note;
    showDetail.value = true;
};

const closeDetail = () => {
    showDetail.value = false;
    detailNote.value = null;
};

const confirmDelete = (note) => {
    deleteTargetNote.value = note;
    showDeleteConfirm.value = true;
};

const cancelDelete = () => {
    showDeleteConfirm.value = false;
    deleteTargetNote.value = null;
};

const executeDelete = async () => {
    if (deleteTargetNote.value) {
        await noteService.deleteNote(deleteTargetNote.value.id);
        await refreshNotes();
        showDetail.value = false;
        detailNote.value = null;
    }
    showDeleteConfirm.value = false;
    deleteTargetNote.value = null;
};

const exportNotes = () => {
    const data = JSON.stringify(notes.value, null, 2);
    const blob = new Blob([data], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `work-notes-${new Date().toISOString().split('T')[0]}.json`;
    a.click();
    URL.revokeObjectURL(url);
};
</script>

<style scoped>
.header-icon {
    width: 14px;
    height: 14px;
}

.work-notes-panel {
    height: 100%;
    display: flex;
    flex-direction: column;
    background: var(--bg-primary);
}

.work-notes-container {
    flex: 1;
    min-height: 0;
    display: flex;
    flex-direction: column;
    padding: var(--spacing-md);
    gap: var(--spacing-md);
    /* 移除 overflow: hidden 避免裁剪按钮圆角 */
    position: relative;
}

.work-notes-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    flex-shrink: 0;
    padding-bottom: var(--spacing-sm);
    border-bottom: 1px solid var(--border-color);
}

.header-left {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
}

.header-actions {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
}

.view-toggle {
    display: flex;
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    padding: 2px;
}

.view-btn {
    padding: 6px 16px;
    border: none;
    background: transparent;
    color: var(--text-secondary);
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-medium);
    cursor: pointer;
    border-radius: calc(var(--radius-md) - 2px);
    transition: all 0.2s ease;
}

.view-btn:hover {
    color: var(--text-primary);
}

.view-btn.active {
    background: var(--bg-primary);
    color: var(--text-primary);
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
}

.header-buttons {
    display: flex;
    gap: var(--spacing-sm);
}

.main-content {
    flex: 1;
    min-height: 0;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
}

.content-area {
    flex: 1;
    min-height: 0;
    display: flex;
    flex-direction: column;
    position: relative;
    background: var(--bg-secondary);
    border-radius: var(--radius-lg);
}

.modal-overlay {
    position: fixed;
    inset: 0;
    background: rgba(0, 0, 0, 0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
    backdrop-filter: blur(4px);
}

.detail-modal {
    background: var(--bg-primary);
    border-radius: var(--radius-lg);
    width: 90%;
    max-width: 500px;
    max-height: 80vh;
    display: flex;
    flex-direction: column;
    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
}

.detail-header {
    display: flex;
    justify-content: space-between;
    align-items: flex-start;
    padding: var(--spacing-lg);
    border-bottom: 1px solid var(--border-color);
}

.detail-meta {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    flex-wrap: wrap;
}

.detail-date {
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
}

.detail-tag {
    font-size: var(--font-size-xs);
    padding: 2px 8px;
    background: var(--accent-color);
    color: white;
    border-radius: var(--radius-sm);
}

.detail-important {
    display: flex;
    align-items: center;
    gap: 4px;
    font-size: var(--font-size-xs);
    color: var(--warning-color);
}

.close-btn {
    padding: var(--spacing-xs);
    border: none;
    background: transparent;
    cursor: pointer;
    color: var(--text-tertiary);
    border-radius: var(--radius-sm);
    transition: all 0.2s ease;
}

.close-btn:hover {
    background: var(--bg-hover);
    color: var(--text-primary);
}

.detail-body {
    flex: 1;
    padding: var(--spacing-lg);
    overflow-y: auto;
}

.detail-title {
    font-size: var(--font-size-xl);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    margin: 0 0 var(--spacing-md) 0;
}

.detail-content {
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
    line-height: 1.6;
    white-space: pre-wrap;
}

.detail-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: var(--spacing-lg);
    border-top: 1px solid var(--border-color);
}

.detail-info {
    display: flex;
    gap: var(--spacing-sm);
    font-size: var(--font-size-xs);
    color: var(--text-tertiary);
}

.detail-actions {
    display: flex;
    gap: var(--spacing-sm);
}

.confirm-modal {
    background: var(--bg-primary);
    border-radius: var(--radius-lg);
    width: 90%;
    max-width: 360px;
    padding: var(--spacing-xl);
    text-align: center;
    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
}

.confirm-icon {
    color: var(--warning-color);
    margin-bottom: var(--spacing-md);
}

.confirm-modal h3 {
    font-size: var(--font-size-lg);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    margin: 0 0 var(--spacing-sm) 0;
}

.confirm-modal p {
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
    margin: 0 0 var(--spacing-lg) 0;
}

.confirm-actions {
    display: flex;
    gap: var(--spacing-sm);
    justify-content: center;
}

@media (max-width: 768px) {
    .work-notes-header {
        flex-direction: column;
        align-items: flex-start;
        gap: var(--spacing-sm);
    }

    .header-actions {
        width: 100%;
        justify-content: space-between;
    }

    .quick-stats {
        flex-wrap: wrap;
    }
}
</style>
