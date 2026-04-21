<template>
    <BasePanel class="work-notes-panel">
        <div class="work-notes-container">
            <div class="work-notes-header">
                <div class="header-left">
                    <h2>{{ toolName }}</h2>
                    <span class="notes-count">{{ filteredNotes.length }} 条笔记</span>
                </div>
                <div class="header-actions">
                    <div class="view-toggle">
                        <button class="view-btn" :class="{ active: currentView === 'calendar' }"
                            @click="currentView = 'calendar'" title="日历视图">
                            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                                stroke-width="2">
                                <rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect>
                                <line x1="16" y1="2" x2="16" y2="6"></line>
                                <line x1="8" y1="2" x2="8" y2="6"></line>
                                <line x1="3" y1="10" x2="21" y2="10"></line>
                            </svg>
                        </button>
                        <button class="view-btn" :class="{ active: currentView === 'list' }"
                            @click="currentView = 'list'" title="列表视图">
                            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                                stroke-width="2">
                                <line x1="8" y1="6" x2="21" y2="6"></line>
                                <line x1="8" y1="12" x2="21" y2="12"></line>
                                <line x1="8" y1="18" x2="21" y2="18"></line>
                                <line x1="3" y1="6" x2="3.01" y2="6"></line>
                                <line x1="3" y1="12" x2="3.01" y2="12"></line>
                                <line x1="3" y1="18" x2="3.01" y2="18"></line>
                            </svg>
                        </button>
                    </div>
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

            <div class="search-filter-bar">
                <div class="search-box">
                    <svg class="search-icon" width="16" height="16" viewBox="0 0 24 24" fill="none"
                        stroke="currentColor" stroke-width="2">
                        <circle cx="11" cy="11" r="8"></circle>
                        <line x1="21" y1="21" x2="16.65" y2="16.65"></line>
                    </svg>
                    <input v-model="searchQuery" type="text" class="search-input" placeholder="搜索笔记..." />
                    <button v-if="searchQuery" class="clear-btn" @click="searchQuery = ''">
                        <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                            stroke-width="2">
                            <line x1="18" y1="6" x2="6" y2="18"></line>
                            <line x1="6" y1="6" x2="18" y2="18"></line>
                        </svg>
                    </button>
                </div>
                <div class="filter-tags" v-if="allTags.length > 0">
                    <button v-for="tag in allTags" :key="tag" class="filter-tag"
                        :class="{ active: selectedTags.includes(tag) }" @click="toggleTag(tag)">
                        {{ tag }}
                    </button>
                </div>
            </div>

            <!-- Inline editor -->
            <NoteEditor v-if="isEditorOpen" :is-open="isEditorOpen" :editing-note="editingNote"
                :selected-date="selectedDate" :all-tags="allTags" @cancel="toggleEditor" @save="saveNote" />

            <div class="content-area">
                <CalendarView v-if="currentView === 'calendar'" :selected-date="selectedDate"
                    :current-date="currentDate" :notes="notes" @prev-month="prevMonth" @next-month="nextMonth"
                    @select-date="selectDate" @open-detail="openDetail" @edit-note="toggleEditor"
                    @delete-note="confirmDelete" @add-note="openEditorForDate" />

                <ListView v-else :notes="notes" :search-query="searchQuery" :selected-tags="selectedTags"
                    @open-detail="openDetail" @edit-note="toggleEditor" @delete-note="confirmDelete" />
            </div>

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
            </div>

            <!-- Detail view modal -->
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

            <!-- Delete confirmation modal -->
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

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import CalendarView from './components/CalendarView.vue';
import ListView from './components/ListView.vue';
import NoteEditor from './components/NoteEditor.vue';

export default {
    name: 'WorkNotesToolView',
    components: {
        BaseButton,
        CalendarView,
        ListView,
        NoteEditor
    },
    props: {
        toolName: {
            type: String,
            default: '工作手记'
        }
    },
    data() {
        return {
            notes: [],
            searchQuery: '',
            selectedTags: [],
            currentView: 'calendar',
            currentDate: new Date(),
            selectedDate: null,
            isEditorOpen: false,
            showDetail: false,
            showDeleteConfirm: false,
            editingNote: null,
            detailNote: null,
            deleteTargetNote: null,
            allTags: []
        };
    },
    watch: {
        notes: {
            handler() {
                if (!this.notes || this.notes.length === 0) {
                    this.allTags = [];
                    return;
                }
                const tags = new Set();
                this.notes.forEach(note => {
                    if (note.tag) {
                        tags.add(note.tag);
                    }
                });
                this.allTags = Array.from(tags).sort();
            },
            immediate: true
        }
    },
    computed: {
        filteredNotes() {
            if (!this.notes || this.notes.length === 0) {
                return [];
            }

            let result = [...this.notes];

            if (this.searchQuery) {
                const query = this.searchQuery.toLowerCase();
                result = result.filter(note => {
                    return (note.title && note.title.toLowerCase().includes(query)) ||
                        note.content.toLowerCase().includes(query) ||
                        (note.tag && note.tag.toLowerCase().includes(query));
                });
            }

            if (this.selectedTags && this.selectedTags.length > 0) {
                result = result.filter(note => {
                    return note.tag && this.selectedTags.includes(note.tag);
                });
            }

            return result.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
        },
        importantNotesCount() {
            return this.notes.filter(note => note.important).length;
        },
        todayNotesCount() {
            const today = new Date().toISOString().split('T')[0];
            return this.notes.filter(note => note.createdAt.split('T')[0] === today).length;
        },
        formatDetailDate() {
            if (!this.detailNote) return '';
            return new Date(this.detailNote.createdAt).toLocaleDateString('zh-CN', {
                year: 'numeric',
                month: 'long',
                day: 'numeric',
                weekday: 'long'
            });
        },
        formatDetailCreatedAt() {
            if (!this.detailNote) return '';
            return new Date(this.detailNote.createdAt).toLocaleString('zh-CN', {
                year: 'numeric',
                month: '2-digit',
                day: '2-digit',
                hour: '2-digit',
                minute: '2-digit'
            });
        },
        formatDetailUpdatedAt() {
            if (!this.detailNote) return '';
            return new Date(this.detailNote.updatedAt).toLocaleString('zh-CN', {
                year: 'numeric',
                month: '2-digit',
                day: '2-digit',
                hour: '2-digit',
                minute: '2-digit'
            });
        }
    },
    mounted() {
        this.loadNotes();
        this.selectedDate = new Date().toISOString().split('T')[0];
    },
    methods: {
        loadNotes() {
            const saved = localStorage.getItem('workNotes');
            if (saved) {
                this.notes = JSON.parse(saved);
            }
        },
        saveNotes() {
            localStorage.setItem('workNotes', JSON.stringify(this.notes));
        },
        prevMonth() {
            // 创建新的Date对象，避免直接修改原对象
            const newDate = new Date(this.currentDate.getTime());
            newDate.setMonth(newDate.getMonth() - 1);
            this.currentDate = newDate;
        },
        nextMonth() {
            // 创建新的Date对象，避免直接修改原对象
            const newDate = new Date(this.currentDate.getTime());
            newDate.setMonth(newDate.getMonth() + 1);
            this.currentDate = newDate;
        },
        selectDate(dateString) {
            if (this.selectedDate !== dateString) {
                this.selectedDate = dateString;
            }
        },
        toggleTag(tag) {
            const index = this.selectedTags.indexOf(tag);
            if (index === -1) {
                this.selectedTags.push(tag);
            } else {
                this.selectedTags.splice(index, 1);
            }
        },
        toggleEditor(note = null) {
            if (this.isEditorOpen && !note) {
                this.isEditorOpen = false;
                this.editingNote = null;
                return;
            }

            this.editingNote = note;
            this.showDetail = false;
            this.isEditorOpen = true;
        },
        openEditorForDate(date) {
            this.selectedDate = date;
            this.toggleEditor(null);
        },
        saveNote(noteData) {
            if (!noteData.content.trim()) return;

            const now = new Date().toISOString();

            if (this.editingNote) {
                const index = this.notes.findIndex(n => n.id === this.editingNote.id);
                if (index !== -1) {
                    this.notes[index].title = noteData.title;
                    this.notes[index].content = noteData.content;
                    this.notes[index].tag = noteData.tag;
                    this.notes[index].createdAt = noteData.date ? `${noteData.date}T${new Date().toLocaleTimeString('en-US', { hour12: false })}` : now;
                    this.notes[index].important = noteData.important;
                    this.notes[index].updatedAt = now;
                }
            } else {
                this.notes.push({
                    id: Date.now().toString(),
                    title: noteData.title,
                    content: noteData.content,
                    tag: noteData.tag,
                    createdAt: noteData.date ? `${noteData.date}T${new Date().toLocaleTimeString('en-US', { hour12: false })}` : now,
                    updatedAt: now,
                    important: noteData.important
                });
            }

            this.saveNotes();
            this.isEditorOpen = false;
            this.editingNote = null;
        },
        openDetail(note) {
            this.detailNote = note;
            this.showDetail = true;
        },
        closeDetail() {
            this.showDetail = false;
            this.detailNote = null;
        },
        confirmDelete(note) {
            this.deleteTargetNote = note;
            this.showDeleteConfirm = true;
            this.showDetail = false;
            this.isEditorOpen = false;
        },
        cancelDelete() {
            this.showDeleteConfirm = false;
            this.deleteTargetNote = null;
        },
        executeDelete() {
            if (this.deleteTargetNote) {
                this.notes = this.notes.filter(n => n.id !== this.deleteTargetNote.id);
                this.saveNotes();
            }
            this.cancelDelete();
        }
    }
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
    gap: 8px;
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
    color: var(--text-primary);
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
    color: var(--text-primary);
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
