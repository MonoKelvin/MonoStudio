<template>
    <div class="notes-drawer-wrapper">
        <div class="notes-drawer" :class="{ expanded: isNotesExpanded }">
            <div class="drawer-handle" @click="toggleNotes">
                <div class="handle-bar"></div>
            </div>
            <div class="notes-section">
                <div class="notes-header">
                    <div class="stats-section">
                        <div class="stat-item">
                            <span class="stat-label">总笔记</span>
                            <span class="stat-value">{{ notes.length }}</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">今日</span>
                            <span class="stat-value">{{ todayNotesCount }}</span>
                        </div>
                        <div class="stat-item">
                            <span class="stat-label">重要</span>
                            <span class="stat-value">{{ importantNotesCount }}</span>
                        </div>
                    </div>
                    <div class="search-bar">
                        <SearchInput v-model="localSearchQuery" placeholder="搜索笔记..." @input="handleSearchInput" />
                    </div>
                </div>
                <div class="filter-tags" v-if="allTags.length > 0">
                    <button v-for="tag in allTags" :key="tag" class="filter-tag"
                        :class="{ active: selectedTags.includes(tag) }" @click="toggleTag(tag)">
                        {{ tag }}
                    </button>
                </div>
                <div class="notes-list">
                    <div class="notes-list-header" v-if="selectedDate">
                        <span class="date-text">{{ formatSelectedDate }}</span>
                        <div class="notes-list-header-actions">
                            <span class="notes-count">{{ selectedDateNotes.length }} 条笔记</span>
                        </div>
                    </div>
                    <div class="notes-list-content" v-if="selectedDateNotes.length > 0">
                        <div v-for="note in selectedDateNotes" :key="note.id" class="note-item"
                            @click="$emit('open-detail', note)">
                            <div class="note-item-header">
                                <span v-if="note.important" class="important-indicator">
                                    <svg width="12" height="12" viewBox="0 0 24 24" fill="currentColor"
                                        stroke="currentColor" stroke-width="2">
                                        <polygon
                                            points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2">
                                        </polygon>
                                    </svg>
                                </span>
                                <span class="note-item-title">{{ note.title || '无标题' }}</span>
                                <div class="note-item-actions">
                                    <button class="icon-btn edit-btn" @click.stop="$emit('edit-note', note)" title="编辑">
                                        <svg width="12" height="12" viewBox="0 0 24 24" fill="none"
                                            stroke="currentColor" stroke-width="2">
                                            <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7">
                                            </path>
                                            <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z">
                                            </path>
                                        </svg>
                                    </button>
                                    <button class="icon-btn delete-btn" @click.stop="$emit('delete-note', note)"
                                        title="删除">
                                        <svg width="12" height="12" viewBox="0 0 24 24" fill="none"
                                            stroke="currentColor" stroke-width="2">
                                            <polyline points="3 6 5 6 21 6"></polyline>
                                            <path
                                                d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2">
                                            </path>
                                        </svg>
                                        height="12" />
                                    </button>
                                </div>
                            </div>
                            <div class="note-item-content">{{ previewText(note) }}</div>
                            <div class="note-item-footer">
                                <span class="note-item-time">{{ formatNoteTime(note.createdAt) }}</span>
                                <span v-if="note.tag" class="tag-badge">{{ note.tag }}</span>
                            </div>
                        </div>
                    </div>
                    <div class="empty-notes" v-else>
                        <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                            stroke-width="1.5">
                            <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path>
                            <polyline points="14 2 14 8 20 8"></polyline>
                        </svg>
                        <p>这一天没有笔记</p>
                        <button class="empty-add-btn" type="button" @click="$emit('add-note', selectedDate)">
                            新建第一条
                        </button>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, watch } from 'vue';
import SearchInput from '../../../../components/base/SearchInput.vue';
import { NoteQueryService } from '../services/NoteQueryService.js';

const props = defineProps({
    selectedDate: {
        type: String,
        default: null
    },
    notes: {
        type: Array,
        default: () => []
    },
    searchQuery: {
        type: String,
        default: ''
    },
    selectedTags: {
        type: Array,
        default: () => []
    },
    allTags: {
        type: Array,
        default: () => []
    }
});

const emit = defineEmits(['open-detail', 'edit-note', 'delete-note', 'add-note', 'update:searchQuery', 'update:selectedTags']);

const isNotesExpanded = ref(false);
const localSearchQuery = ref('');

const selectedDateNotes = computed(() => {
    if (!props.selectedDate) return [];
    const filtered = NoteQueryService.filterNotes(props.notes, props.searchQuery, props.selectedTags);
    return filtered.filter(note => NoteQueryService.toDateKey(note.createdAt) === props.selectedDate);
});

const formatSelectedDate = computed(() => {
    if (!props.selectedDate) return '';
    const date = new Date(props.selectedDate);
    const today = new Date();
    const yesterday = new Date(today);
    yesterday.setDate(yesterday.getDate() - 1);

    const dateStr = date.toDateString();
    const todayStr = today.toDateString();
    const yesterdayStr = yesterday.toDateString();

    if (dateStr === todayStr) {
        return '今天';
    } else if (dateStr === yesterdayStr) {
        return '昨天';
    } else {
        return date.toLocaleDateString('zh-CN', {
            month: 'long',
            day: 'numeric',
            weekday: 'long'
        });
    }
});

const todayNotesCount = computed(() => {
    const today = new Date().toDateString();
    return props.notes.filter(note => {
        const noteDate = new Date(note.createdAt).toDateString();
        return noteDate === today;
    }).length;
});

const importantNotesCount = computed(() => {
    return props.notes.filter(note => note.important).length;
});

const toggleTag = (tag) => {
    const newTags = [...props.selectedTags];
    const index = newTags.indexOf(tag);
    if (index === -1) {
        newTags.push(tag);
    } else {
        newTags.splice(index, 1);
    }
    emit('update:selectedTags', newTags);
};

const toggleNotes = () => {
    isNotesExpanded.value = !isNotesExpanded.value;
};

const previewText = (note) => {
    return NoteQueryService.toPreviewText(note, 80);
};

const formatNoteTime = (dateString) => {
    const date = new Date(dateString);
    return date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
};

const handleSearchInput = () => {
    emit('update:searchQuery', localSearchQuery.value);
};

watch(() => props.searchQuery, (newValue) => {
    localSearchQuery.value = newValue;
});
</script>

<style scoped>
.notes-drawer-wrapper {
    position: absolute;
    left: 0;
    right: 0;
    bottom: 0;
    pointer-events: none;
    z-index: 10;
}

.notes-drawer {
    position: relative;
    left: 10px;
    width: calc(100% - 20px);
    bottom: 0;
    background: var(--bg-soft);
    height: 42px;
    overflow: hidden;
    border-radius: 22px 22px 0 0;
    box-shadow: 0 -4px 12px color-mix(in srgb, var(--bg-primary) 15%, transparent);
    pointer-events: auto;
    transition: height 0.6s cubic-bezier(0.2, 0.8, 0.2, 1), box-shadow 0.4s ease 0.2s;
}

.notes-drawer.expanded {
    height: 400px;
    box-shadow: 0 -14px 36px color-mix(in srgb, var(--bg-primary) 70%, transparent);
}

.notes-section {
    display: none;
}

.notes-drawer.expanded .notes-section {
    display: block;
}

.notes-section-enter-active,
.notes-section-leave-active {
    transition: all 0.6s cubic-bezier(0.1, 0.9, 0.2, 1);
}

.notes-section-enter-from {
    opacity: 0;
    transform: translateY(20px);
}

.notes-section-leave-to {
    opacity: 0;
    transform: translateY(20px);
}

.notes-drawer::before {
    display: none;
}

.drawer-handle {
    height: 42px;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-sm);
    cursor: pointer;
    background: var(--bg-hover);
    border-bottom: 1px solid var(--border-color);
    transition: background-color 0.2s ease;
}

.drawer-handle:hover {
    background: var(--bg-hover);
}

.handle-bar {
    width: 44px;
    height: 4px;
    background: var(--text-muted);
    border-radius: 2px;
    transition: all 0.2s ease;
}

.drawer-handle:hover .handle-bar {
    background: var(--accent-color);
    width: 50px;
}

.drawer-label {
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-medium);
    color: var(--text-secondary);
    transition: color 0.2s ease;
}

.drawer-handle:hover .drawer-label {
    color: var(--accent-color);
}

.notes-section {
    flex: 1;
    display: flex;
    flex-direction: column;
    max-height: 424px;
    overflow: hidden;
}

.notes-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 4px 12px;
    border-bottom: 1px solid var(--border-color);
    background: var(--bg-soft);
}

.stats-section {
    display: flex;
    gap: 20px;
}

.stat-item {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.stat-label {
    font-size: 12px;
    color: var(--text-tertiary);
}

.stat-value {
    font-size: 16px;
    font-weight: 600;
    color: var(--text-primary);
}

.search-bar {
    flex: 0 0 300px;
    position: relative;
    display: flex;
    align-items: center;
}

.filter-tags {
    display: flex;
    flex-wrap: wrap;
    gap: var(--spacing-xs);
    padding: var(--spacing-sm) var(--spacing-md);
    border-bottom: 1px solid var(--border-color);
}

.filter-tag {
    padding: var(--spacing-xs) var(--spacing-sm);
    font-size: var(--font-size-xs);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    background: var(--bg-tertiary);
    color: var(--text-secondary);
    cursor: pointer;
    transition: var(--transition-fast);
}

.filter-tag:hover {
    border-color: var(--accent-color);
    color: var(--accent-color);
}

.filter-tag.active {
    background: var(--accent-color);
    border-color: var(--accent-color);
    color: white;
}

.notes-list {
    flex: 1;
    display: flex;
    flex-direction: column;
    overflow: hidden;
}

.notes-list-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: var(--spacing-sm) var(--spacing-md);
    background: var(--bg-elevated);
    border-bottom: 1px solid var(--border-color);
}

.notes-list-header-actions {
    display: flex;
    align-items: center;
    gap: 8px;
}

.date-text {
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
}

.notes-count {
    font-size: var(--font-size-xs);
    color: var(--text-tertiary);
}

.notes-list-content {
    flex: 1;
    overflow-y: auto;
    padding: var(--spacing-xs);
}

.notes-list-content::-webkit-scrollbar {
    width: 4px;
}

.notes-list-content::-webkit-scrollbar-track {
    background: transparent;
}

.notes-list-content::-webkit-scrollbar-thumb {
    background: var(--border-color);
    border-radius: 2px;
}

.notes-list-content::-webkit-scrollbar-thumb:hover {
    background: var(--accent-color);
}

.note-item {
    padding: var(--spacing-sm);
    margin-bottom: var(--spacing-xs);
    background: var(--bg-elevated);
    border-radius: 10px;
    cursor: pointer;
    transition: var(--transition-fast);
    border: 1px solid var(--border-color);
}

.note-item:hover {
    background: var(--bg-hover);
    border-color: var(--accent-color);
    transform: translateY(-1px);
}

.note-item-header {
    display: flex;
    justify-content: space-between;
    align-items: flex-start;
    margin-bottom: var(--spacing-xs);
    gap: var(--spacing-xs);
}

.important-indicator {
    color: var(--warning-color);
    flex-shrink: 0;
    margin-top: 1px;
}

.note-item-title {
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-medium);
    color: var(--text-primary);
    flex: 1;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.note-item-actions {
    display: flex;
    gap: var(--spacing-xs);
    opacity: 0;
    transition: var(--transition-fast);
}

.note-item:hover .note-item-actions {
    opacity: 1;
}

.icon-btn {
    padding: var(--spacing-xs);
    border: none;
    background: transparent;
    cursor: pointer;
    color: var(--text-tertiary);
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: var(--radius-sm);
    transition: var(--transition-fast);
}

.icon-btn:hover {
    background: var(--bg-secondary);
    color: var(--text-primary);
}

.edit-btn:hover {
    color: var(--accent-color);
}

.delete-btn:hover {
    color: var(--danger);
}

.note-item-content {
    font-size: var(--font-size-xs);
    line-height: 1.4;
    color: var(--text-tertiary);
    margin-bottom: var(--spacing-xs);
    overflow: hidden;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
}

.note-item-footer {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
}

.note-item-time {
    font-size: var(--font-size-xs);
    color: var(--text-tertiary);
}

.tag-badge {
    font-size: var(--font-size-xs);
    padding: 1px var(--spacing-xs);
    background: var(--success);
    color: white;
    border-radius: var(--radius-sm);
    font-weight: var(--font-weight-medium);
}

.empty-notes {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: var(--spacing-2xl);
    color: var(--text-tertiary);
}

.empty-notes svg {
    margin-bottom: var(--spacing-sm);
    opacity: 0.5;
}

.empty-notes p {
    margin: 0;
    font-size: var(--font-size-sm);
}

.empty-add-btn {
    margin-top: 8px;
    border: none;
    border-radius: 8px;
    padding: 6px 10px;
    cursor: pointer;
    background: var(--bg-tertiary);
    color: var(--text-primary);
}

.empty-add-btn:hover {
    background: var(--bg-hover);
}

@media (max-width: 768px) {
    .notes-drawer {
        left: 6px;
        right: 6px;
    }

    .notes-section {
        max-height: 373px;
    }
}
</style>
