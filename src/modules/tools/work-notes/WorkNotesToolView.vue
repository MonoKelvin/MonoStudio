<template>
    <BasePanel class="work-notes-panel">
        <div class="work-notes-container">
            <!-- 头部区域 -->
            <div class="work-notes-header">
                <div class="header-left">
                    <h2>{{ toolName }}</h2>
                    <span class="notes-count">{{ filteredNotes.length }} 条笔记</span>
                </div>
                <div class="header-actions">
                    <!-- 操作按钮 -->
                    <div class="header-buttons">
                        <BaseButton @click="exportNotes" size="sm" :disabled="notes.length === 0">
                            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                                <polyline points="7 10 12 15 17 10"></polyline>
                                <line x1="12" y1="15" x2="12" y2="3"></line>
                            </svg>
                            导出
                        </BaseButton>
                        <BaseButton @click="toggleEditor(null)" variant="primary">
                            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                                stroke-width="2">
                                <line x1="12" y1="5" x2="12" y2="19"></line>
                                <line x1="5" y1="12" x2="19" y2="12"></line>
                            </svg>
                            {{ isEditorOpen ? '取消' : '新建笔记' }}
                        </BaseButton>
                    </div>
                </div>
            </div>

            <!-- 内联编辑器 -->
            <NoteEditor v-if="isEditorOpen" :is-open="isEditorOpen" :editing-note="editingNote"
                :selected-date="selectedDate" :all-tags="allTags" @cancel="toggleEditor" @save="saveNote" />

            <!-- 内容区域 -->
            <div class="content-area">
                <CalendarView v-if="currentView === 'calendar'" :selected-date="selectedDate"
                    :current-date="currentDate" :notes="notes" :search-query="searchQuery"
                    :selected-tags="selectedTags" :all-tags="allTags"
                    @prev-month="handleDateChange" @select-date="selectDate"
                    @open-detail="openDetail" @edit-note="toggleEditor"
                    @delete-note="confirmDelete" @add-note="openEditorForDate"
                    @update:searchQuery="searchQuery = $event"
                    @update:selectedTags="selectedTags = $event" />

                <ListView v-else :notes="notes" :search-query="searchQuery" :selected-tags="selectedTags"
                    @open-detail="openDetail" @edit-note="toggleEditor" @delete-note="confirmDelete" />
            </div>

            <!-- 快速统计 -->
            <div class="quick-stats" v-if="notes.length > 0">
                <div class="stat-item">
                    <span class="stat-value">{{ notes.length }}</span>
                    <span class="stat-label">总笔记</span>
                </div>
                <div class="stat-item">
                    <span class="stat-value">{{ importantNotesCount }}</span>
                    <span class="stat-label">重要</span>
                </div>
                <div class="stat-item">
                    <span class="stat-value">{{ todayNotesCount }}</span>
                    <span class="stat-label">今日</span>
                </div>
                <div class="stat-item">
                    <span class="stat-value">{{ allTags.length }}</span>
                    <span class="stat-label">标签</span>
                </div>
            </div>

            <!-- 详情查看模态框 -->
            <div class="modal-overlay" v-if="showDetail" @click="closeDetail">
                <div class="detail-modal" @click.stop>
                    <div class="detail-header">
                        <div class="detail-meta">
                            <span class="detail-date">{{ formatDetailDate }}</span>
                            <span v-if="detailNote?.tag" class="detail-tag">{{ detailNote.tag }}</span>
                            <span v-if="detailNote?.important" class="detail-important">
                                <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor"
                                    stroke="currentColor" stroke-width="2">
                                    <polygon
                                        points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2">
                                    </polygon>
                                </svg>
                                重要
                            </span>
                        </div>
                        <button class="close-btn" @click="closeDetail">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                                stroke-width="2">
                                <line x1="18" y1="6" x2="6" y2="18"></line>
                                <line x1="6" y1="6" x2="18" y2="18"></line>
                            </svg>
                        </button>
                    </div>
                    <div class="detail-body">
                        <h2 class="detail-title">{{ detailNote?.title || '无标题' }}</h2>
                        <div class="detail-content">{{ detailNote?.content }}</div>
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

            <!-- 删除确认模态框 -->
            <div class="modal-overlay" v-if="showDeleteConfirm" @click="cancelDelete">
                <div class="confirm-modal" @click.stop>
                    <div class="confirm-icon">
                        <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                            stroke-width="1.5">
                            <circle cx="12" cy="12" r="10"></circle>
                            <line x1="12" y1="8" x2="12" y2="12"></line>
                            <line x1="12" y1="16" x2="12.01" y2="16"></line>
                        </svg>
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
import { ref, computed, watch, onMounted, nextTick } from 'vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import CalendarView from './components/CalendarView.vue';
import ListView from './components/ListView.vue';
import NoteEditor from './components/NoteEditor.vue';

// Props
const props = defineProps({
    toolName: {
        type: String,
        default: '工作手记'
    }
});

// Reactive state
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

// Computed properties
const filteredNotes = computed(() => {
    if (!notes.value || notes.value.length === 0) {
        return [];
    }

    let result = [...notes.value];

    if (searchQuery.value) {
        const query = searchQuery.value.toLowerCase();
        result = result.filter(note => {
            return (note.title && note.title.toLowerCase().includes(query)) ||
                note.content.toLowerCase().includes(query) ||
                (note.tag && note.tag.toLowerCase().includes(query));
        });
    }

    if (selectedTags.value && selectedTags.value.length > 0) {
        result = result.filter(note => {
            return note.tag && selectedTags.value.includes(note.tag);
        });
    }

    return result.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
});

const importantNotesCount = computed(() => {
    return notes.value.filter(note => note.important).length;
});

const todayNotesCount = computed(() => {
    const today = new Date().toISOString().split('T')[0];
    return notes.value.filter(note => note.createdAt.split('T')[0] === today).length;
});

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

// Watchers
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

// Lifecycle
onMounted(() => {
    loadNotes();
    selectedDate.value = new Date().toISOString().split('T')[0];
});

// Methods
const loadNotes = () => {
    const saved = localStorage.getItem('workNotes');
    if (saved) {
        notes.value = JSON.parse(saved);
    }
};

const saveNotes = () => {
    localStorage.setItem('workNotes', JSON.stringify(notes.value));
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

    editingNote.value = note;
    showDetail.value = false;
    isEditorOpen.value = true;
};

const openEditorForDate = (date) => {
    selectedDate.value = date;
    toggleEditor(null);
};

const saveNote = (noteData) => {
    if (!noteData.content.trim()) return;

    const now = new Date().toISOString();

    if (editingNote.value) {
        const index = notes.value.findIndex(n => n.id === editingNote.value.id);
        if (index !== -1) {
            notes.value[index].title = noteData.title;
            notes.value[index].content = noteData.content;
            notes.value[index].tag = noteData.tag;
            notes.value[index].createdAt = noteData.date ? `${noteData.date}T${new Date().toLocaleTimeString('en-US', { hour12: false })}` : now;
            notes.value[index].important = noteData.important;
            notes.value[index].updatedAt = now;
        }
    } else {
        notes.value.push({
            id: Date.now().toString(),
            title: noteData.title,
            content: noteData.content,
            tag: noteData.tag,
            createdAt: noteData.date ? `${noteData.date}T${new Date().toLocaleTimeString('en-US', { hour12: false })}` : now,
            updatedAt: now,
            important: noteData.important
        });
    }

    saveNotes();
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
    showDetail.value = false;
    isEditorOpen.value = false;
};

const cancelDelete = () => {
    showDeleteConfirm.value = false;
    deleteTargetNote.value = null;
};

const executeDelete = () => {
    if (deleteTargetNote.value) {
        notes.value = notes.value.filter(n => n.id !== deleteTargetNote.value.id);
        saveNotes();
    }
    cancelDelete();
};

const exportNotes = () => {
    const exportData = {
        notes: notes.value,
        exportDate: new Date().toISOString(),
        version: '1.0'
    };
    
    const blob = new Blob([JSON.stringify(exportData, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `work-notes-${new Date().toISOString().split('T')[0]}.json`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
};
</script>

<style scoped>
.work-notes-panel {
    height: 100%;
    overflow: hidden;
}

.work-notes-container {
    height: 100%;
    display: flex;
    flex-direction: column;
    padding: 16px;
    gap: 12px;
}

/* 响应式设计 */
@media (max-width: 768px) {
    .work-notes-container {
        padding: 12px;
        gap: 8px;
    }
    
    .work-notes-header {
        flex-direction: column;
        align-items: flex-start;
        gap: 12px;
        padding-bottom: 8px;
    }
    
    .header-actions {
        width: 100%;
        flex-direction: column;
        align-items: stretch;
        gap: 8px;
    }
    
    .header-buttons {
        width: 100%;
        justify-content: space-between;
    }
    
    .search-filter-bar {
        gap: 8px;
    }
    
    .filter-tags {
        flex-wrap: wrap;
        gap: 4px;
    }
    
    .content-area {
        padding: 8px;
    }
    
    .quick-stats {
        flex-wrap: wrap;
        gap: 8px;
    }
    
    .stat-item {
        flex: 1;
        min-width: 80px;
        padding: 6px 8px;
    }
    
    .detail-modal {
        max-width: 95vw;
        max-height: 90vh;
    }
    
    .detail-body {
        padding: 16px;
    }
    
    .category-manager-modal {
        max-width: 95vw;
        max-height: 90vh;
    }
    
    .modal-body {
        padding: 16px;
    }
}

.work-notes-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding-bottom: 12px;
    border-bottom: 1px solid var(--border-color);
}

.header-left {
    display: flex;
    align-items: baseline;
    gap: 8px;
}

.header-left h2 {
    margin: 0;
    font-size: 18px;
    font-weight: 600;
    color: var(--text-primary);
}

.notes-count {
    font-size: 12px;
    color: var(--text-secondary);
}

.header-actions {
    display: flex;
    align-items: center;
    gap: 12px;
}

.header-buttons {
    display: flex;
    align-items: center;
    gap: 8px;
}

.header-buttons :deep(.base-button:disabled) {
    cursor: not-allowed;
    opacity: 0.6;
}

.header-buttons :deep(.base-button:disabled:hover) {
    transform: none;
    box-shadow: none;
}

.view-toggle {
    display: flex;
    background: var(--bg-secondary);
    border-radius: 6px;
    padding: 2px;
}

.view-btn {
    padding: 4px 8px;
    border: none;
    background: transparent;
    border-radius: 4px;
    cursor: pointer;
    color: var(--text-secondary);
    display: flex;
    align-items: center;
    justify-content: center;
    transition: all 0.2s ease;
}

.view-btn:hover {
    color: var(--text-primary);
    background: var(--bg-tertiary);
}

.view-btn.active {
    background: var(--bg-primary);
    color: var(--accent-color);
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1);
    transform: translateY(-1px);
}

.search-filter-bar {
    display: flex;
    flex-direction: column;
    gap: 8px;
}

.search-box {
    position: relative;
    display: flex;
    align-items: center;
}

.search-icon {
    position: absolute;
    left: 10px;
    color: var(--text-tertiary);
}

.search-input {
    width: 100%;
    padding: 8px 32px;
    border: 1px solid var(--border-color);
    border-radius: 8px;
    font-size: 13px;
    background: var(--bg-secondary);
    color: var(--text-primary);
    transition: all 0.2s ease;
}

.search-input:focus {
    outline: none;
    border-color: var(--accent-color);
    background: var(--bg-primary);
    box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.1);
}

.search-input::placeholder {
    color: var(--text-tertiary);
}

.clear-btn {
    position: absolute;
    right: 8px;
    padding: 3px;
    border: none;
    background: transparent;
    cursor: pointer;
    color: var(--text-tertiary);
    display: flex;
    align-items: center;
    justify-content: center;
    transition: all 0.2s ease;
}

.clear-btn:hover {
    color: var(--text-secondary);
    background: var(--bg-tertiary);
    border-radius: 4px;
}



.filter-tags {
    display: flex;
    flex-wrap: wrap;
    gap: 4px;
}

.filter-tag {
    padding: 3px 8px;
    font-size: 11px;
    border: 1px solid var(--border-color);
    border-radius: 12px;
    background: var(--bg-secondary);
    color: var(--text-secondary);
    cursor: pointer;
    transition: all 0.2s ease;
}

.filter-tag:hover {
    border-color: var(--accent-color);
    color: var(--accent-color);
    transform: scale(1.05);
}

.filter-tag.active {
    background: var(--accent-color);
    border-color: var(--accent-color);
    color: white;
}

.content-area {
    flex: 1;
    overflow: hidden;
    border-radius: 8px;
    background: var(--bg-secondary);
    padding: 12px;
    box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.05);
}

.quick-stats {
    display: flex;
    gap: 12px;
    padding-top: 12px;
    border-top: 1px solid var(--border-color);
}

.stat-item {
    display: flex;
    flex-direction: column;
    gap: 2px;
    padding: 8px 12px;
    background: var(--bg-secondary);
    border-radius: 6px;
    min-width: 60px;
    text-align: center;
    transition: all 0.2s ease;
}

.stat-item:hover {
    background: var(--bg-tertiary);
    transform: translateY(-2px);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.stat-value {
    font-size: 16px;
    font-weight: 600;
    color: var(--text-primary);
}

.stat-label {
    font-size: 10px;
    color: var(--text-tertiary);
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

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
    padding: 20px;
    backdrop-filter: blur(4px);
}

.detail-modal {
    background: var(--bg-primary);
    border-radius: 12px;
    width: 100%;
    max-width: 560px;
    max-height: 80vh;
    display: flex;
    flex-direction: column;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
    animation: modalSlideIn 0.3s ease forwards;
}

.detail-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 16px 20px;
    border-bottom: 1px solid var(--border-color);
}

.detail-header h3 {
    margin: 0;
    font-size: 16px;
    font-weight: 600;
    color: var(--text-primary);
}

.detail-meta {
    display: flex;
    align-items: center;
    gap: 8px;
}

.detail-date {
    font-size: 12px;
    color: var(--text-secondary);
}

.detail-tag {
    font-size: 11px;
    padding: 2px 6px;
    background: var(--success-color-light);
    color: var(--success-color);
    border-radius: 4px;
}

.detail-important {
    display: flex;
    align-items: center;
    gap: 3px;
    font-size: 11px;
    color: var(--warning-color, #f59e0b);
}

.close-btn {
    padding: 4px;
    border: none;
    background: transparent;
    cursor: pointer;
    color: var(--text-secondary);
    border-radius: 4px;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: all 0.2s ease;
}

.close-btn:hover {
    background: var(--bg-secondary);
    color: var(--text-primary);
    transform: scale(1.1);
}

.detail-body {
    flex: 1;
    padding: 20px;
    overflow-y: auto;
    display: flex;
    flex-direction: column;
    gap: 12px;
}

.detail-title {
    margin: 0 0 12px 0;
    font-size: 18px;
    font-weight: 600;
    color: #111827;
}

.detail-content {
    font-size: 14px;
    line-height: 1.7;
    color: var(--text-primary);
    white-space: pre-wrap;
    word-break: break-word;
}

.detail-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 16px 20px;
    border-top: 1px solid var(--border-color);
}

.detail-info {
    font-size: 11px;
    color: var(--text-tertiary);
    display: flex;
    gap: 4px;
}

.detail-actions {
    display: flex;
    gap: 8px;
}

.confirm-modal {
    background: var(--bg-primary);
    border-radius: 12px;
    width: 100%;
    max-width: 360px;
    padding: 24px;
    text-align: center;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
    animation: modalSlideIn 0.3s ease forwards;
}

.confirm-icon {
    color: var(--danger-color);
    margin-bottom: 12px;
    animation: pulse 2s infinite;
}

.confirm-modal h3 {
    margin: 0 0 8px 0;
    font-size: 16px;
    font-weight: 600;
    color: #111827;
}

.confirm-modal p {
    margin: 0 0 20px 0;
    font-size: 13px;
    color: var(--text-secondary);
}

.confirm-actions {
    display: flex;
    gap: 10px;
    justify-content: center;
}

@keyframes modalSlideIn {
    from {
        opacity: 0;
        transform: translateY(-20px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

@keyframes pulse {

    0%,
    100% {
        transform: scale(1);
    }

    50% {
        transform: scale(1.05);
    }
}


</style>
