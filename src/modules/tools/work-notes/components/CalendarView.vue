<template>
    <div class="calendar-view">
        <!-- 日历主体 -->
        <div class="calendar-body">
            <FullCalendar :options="calendarOptions" ref="fullCalendar" />
        </div>

        <!-- 搜索和笔记列表区域 -->
        <div class="notes-section">
            <!-- 搜索框 -->
            <div class="search-bar">
                <svg class="search-icon" width="14" height="14" viewBox="0 0 24 24" fill="none"
                    stroke="currentColor" stroke-width="2">
                    <circle cx="11" cy="11" r="8"></circle>
                    <line x1="21" y1="21" x2="16.65" y2="16.65"></line>
                </svg>
                <input :value="searchQuery" @input="handleSearchInput" type="text" class="search-input" placeholder="搜索笔记..." />
                <button v-if="searchQuery" class="clear-btn" @click="clearSearch">
                    <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor"
                        stroke-width="2">
                        <line x1="18" y1="6" x2="6" y2="18"></line>
                        <line x1="6" y1="6" x2="18" y2="18"></line>
                    </svg>
                </button>
            </div>

            <!-- 标签过滤 -->
            <div class="filter-tags" v-if="allTags.length > 0">
                <button v-for="tag in allTags" :key="tag" class="filter-tag"
                    :class="{ active: selectedTags.includes(tag) }" @click="toggleTag(tag)">
                    {{ tag }}
                </button>
            </div>

            <!-- 笔记列表 -->
            <div class="notes-list">
                <div class="notes-list-header" v-if="selectedDate">
                    <span class="date-text">{{ formatSelectedDate }}</span>
                    <span class="notes-count">{{ selectedDateNotes.length }} 条笔记</span>
                </div>
                
                <div class="notes-list-content" v-if="selectedDateNotes.length > 0">
                    <div v-for="note in selectedDateNotes" :key="note.id" 
                         class="note-item"
                         @click="$emit('open-detail', note)">
                        <div class="note-item-header">
                            <span v-if="note.important" class="important-indicator">
                                <svg width="12" height="12" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor" stroke-width="2">
                                    <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
                                </svg>
                            </span>
                            <span class="note-item-title">{{ note.title || '无标题' }}</span>
                            <div class="note-item-actions">
                                <button class="icon-btn edit-btn" @click.stop="$emit('edit-note', note)" title="编辑">
                                    <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                        <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
                                        <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
                                    </svg>
                                </button>
                                <button class="icon-btn delete-btn" @click.stop="$emit('delete-note', note)" title="删除">
                                    <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                        <polyline points="3 6 5 6 21 6"></polyline>
                                        <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path>
                                    </svg>
                                </button>
                            </div>
                        </div>
                        <div class="note-item-content">{{ note.content.substring(0, 80) }}{{ note.content.length > 80 ? '...' : '' }}</div>
                        <div class="note-item-footer">
                            <span class="note-item-time">{{ formatNoteTime(note.createdAt) }}</span>
                            <span v-if="note.tag" class="tag-badge">{{ note.tag }}</span>
                        </div>
                    </div>
                </div>
                
                <div class="empty-notes" v-else>
                    <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
                        <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path>
                        <polyline points="14 2 14 8 20 8"></polyline>
                    </svg>
                    <p>这一天没有笔记</p>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, watch, onMounted } from 'vue';
import NoteCard from './NoteCard.vue';
import FullCalendar from '@fullcalendar/vue3';
import dayGridPlugin from '@fullcalendar/daygrid';
import timeGridPlugin from '@fullcalendar/timegrid';
import listPlugin from '@fullcalendar/list';
import interactionPlugin from '@fullcalendar/interaction';

// Props
const props = defineProps({
    currentDate: {
        type: Date,
        required: true
    },
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

// Emits
const emit = defineEmits(['prev-month', 'next-month', 'select-date', 'open-detail', 'edit-note', 'delete-note', 'add-note', 'update:searchQuery', 'update:selectedTags']);

// Reactive state
const fullCalendar = ref(null);
const currentView = ref('dayGridMonth');

// View options
const viewOptions = [
    { label: '月', value: 'dayGridMonth' },
    { label: '周', value: 'timeGridWeek' },
    { label: '日', value: 'timeGridDay' },
    { label: '列表', value: 'listMonth' }
];

// Computed properties
const currentMonthYear = computed(() => {
    return props.currentDate.toLocaleDateString('zh-CN', {
        year: 'numeric',
        month: 'long'
    });
});

const calendarOptions = computed(() => {
    return {
        plugins: [dayGridPlugin, timeGridPlugin, listPlugin, interactionPlugin],
        initialView: currentView.value,
        locale: 'zh-cn',
        headerToolbar: {
            left: 'prev,next today',
            center: 'title',
            right: 'dayGridMonth,timeGridWeek,timeGridDay,listMonth'
        },
        navLinks: true,
        selectable: true,
        events: calendarEvents.value,
        dateClick: handleDateClick,
        eventClick: handleEventClick,
        dayMaxEvents: 3,
        slotMinTime: '08:00',
        slotMaxTime: '20:00',
        allDaySlot: true,
        eventDidMount: function (info) {
            const eventEl = info.el;
            eventEl.style.borderRadius = '4px';
            eventEl.style.fontSize = '11px';
            eventEl.style.padding = '2px 4px';
        },
        datesSet: handleDatesSet,
        height: 'auto'
    };
});

const calendarEvents = computed(() => {
    let filtered = [...props.notes];

    if (props.searchQuery) {
        const query = props.searchQuery.toLowerCase();
        filtered = filtered.filter(note => {
            return (note.title && note.title.toLowerCase().includes(query)) ||
                note.content.toLowerCase().includes(query) ||
                (note.tag && note.tag.toLowerCase().includes(query));
        });
    }

    if (props.selectedTags && props.selectedTags.length > 0) {
        filtered = filtered.filter(note => {
            return note.tag && props.selectedTags.includes(note.tag);
        });
    }

    return filtered.map(note => {
        return {
            id: note.id,
            title: note.title,
            start: note.createdAt,
            end: note.createdAt,
            allDay: true,
            extendedProps: {
                type: note.important ? 'important' : note.tag ? 'tagged' : 'normal',
                note: note
            }
        };
    });
});

const selectedDateNotes = computed(() => {
    if (!props.selectedDate) return [];
    return props.notes.filter(note => note.createdAt.split('T')[0] === props.selectedDate);
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

// Methods
const prevMonth = () => {
    if (fullCalendar.value) {
        fullCalendar.value.getApi().prev();
    }
};

const nextMonth = () => {
    if (fullCalendar.value) {
        fullCalendar.value.getApi().next();
    }
};

const goToToday = () => {
    if (fullCalendar.value) {
        fullCalendar.value.getApi().today();
        emit('select-date', new Date().toISOString().split('T')[0]);
    }
};

const switchView = (view) => {
    currentView.value = view;
    if (fullCalendar.value) {
        fullCalendar.value.getApi().changeView(view);
    }
};

const handleDateClick = (info) => {
    const dateString = info.dateStr;
    emit('select-date', dateString);
};

const handleEventClick = (info) => {
    const note = info.event.extendedProps.note;
    emit('open-detail', note);
};

const handleDatesSet = (info) => {
    // 更新 currentDate 以同步外部状态
    const newDate = info.view.currentStart;
    emit('prev-month', newDate);
};

const handleSearchInput = (event) => {
    const value = event.target.value;
    emit('update:searchQuery', value);
};

const clearSearch = () => {
    emit('update:searchQuery', '');
};

const formatNoteTime = (dateString) => {
    const date = new Date(dateString);
    return date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
};

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
</script>

<style scoped>
.calendar-view {
    height: 100%;
    min-height: 0;
    display: flex;
    flex-direction: column;
    background: var(--background-primary);
    border-radius: var(--radius-lg);
    overflow: hidden;
    box-shadow: var(--shadow-card);
}

.calendar-body {
    flex: 1;
    min-height: 0;
    overflow: auto;
    background: var(--background-secondary);
}

.calendar-body :deep(.fc) {
    height: 100% !important;
    font-family: inherit;
}

.calendar-body :deep(.fc-toolbar) {
    padding: var(--spacing-md) var(--spacing-lg);
    margin: 0;
    background: var(--background-secondary);
    border-bottom: 1px solid var(--border-color);
}

.calendar-body :deep(.fc-toolbar-title) {
    font-size: var(--font-size-lg) !important;
    font-weight: var(--font-weight-semibold) !important;
    color: var(--text-primary) !important;
}

.calendar-body :deep(.fc-button) {
    background: var(--background-tertiary) !important;
    border: 1px solid var(--border-color) !important;
    color: var(--text-primary) !important;
    font-size: var(--font-size-sm) !important;
    font-weight: var(--font-weight-medium) !important;
    padding: var(--spacing-xs) var(--spacing-sm) !important;
    border-radius: var(--radius-sm) !important;
    box-shadow: none !important;
    transition: var(--transition-fast);
}

.calendar-body :deep(.fc-button:hover) {
    background: var(--background-hover) !important;
    border-color: var(--accent-color) !important;
}

.calendar-body :deep(.fc-button-active) {
    background: var(--accent-color) !important;
    border-color: var(--accent-color) !important;
    color: white !important;
}

.calendar-body :deep(.fc-theme-standard .fc-scrollgrid) {
    border: none !important;
    background: var(--background-secondary) !important;
}

.calendar-body :deep(.fc-theme-standard td),
.calendar-body :deep(.fc-theme-standard th) {
    border-color: var(--border-color) !important;
}

.calendar-body :deep(.fc-col-header-cell) {
    background: var(--background-tertiary) !important;
    border-bottom: 1px solid var(--border-color) !important;
    padding: 0 !important;
}

.calendar-body :deep(.fc-col-header-cell-cushion) {
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    text-transform: uppercase;
    letter-spacing: 0.5px;
    padding: var(--spacing-sm) var(--spacing-xs) !important;
    display: block;
    text-align: center;
}

.calendar-body :deep(.fc-daygrid-day) {
    min-height: 80px !important;
}

.calendar-body :deep(.fc-daygrid-day:hover) {
    background: var(--background-hover) !important;
}

.calendar-body :deep(.fc-daygrid-day-number) {
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    padding: var(--spacing-xs) var(--spacing-xs);
    display: block;
    text-align: right;
}

.calendar-body :deep(.fc-day-today .fc-daygrid-day-number) {
    background: var(--accent-color);
    color: white;
    font-weight: var(--font-weight-bold);
    border-radius: 50%;
    width: 24px;
    height: 24px;
    line-height: 24px;
    text-align: center;
    margin: 4px;
}

.calendar-body :deep(.fc-day-other .fc-daygrid-day-number) {
    color: var(--text-tertiary);
}

.calendar-body :deep(.fc-daygrid-event) {
    background: transparent !important;
    border: none !important;
    padding: 0 !important;
    margin: 2px 4px !important;
}

.calendar-body :deep(.fc-daygrid-event-content) {
    display: flex !important;
    align-items: center !important;
    padding: 2px 6px !important;
    background: var(--background-tertiary) !important;
    border-radius: var(--radius-sm) !important;
    border-left: 2px solid var(--accent-color) !important;
}

.calendar-body :deep(.fc-daygrid-event.important .fc-daygrid-event-content) {
    border-left-color: var(--warning-color) !important;
}

.calendar-body :deep(.fc-daygrid-event.tagged .fc-daygrid-event-content) {
    border-left-color: var(--success) !important;
}

.calendar-body :deep(.fc-daygrid-more-link) {
    font-size: var(--font-size-xs);
    color: var(--accent-color) !important;
    font-weight: var(--font-weight-medium);
    display: block;
    padding: 2px 6px;
}

.calendar-body :deep(.fc-list) {
    background: var(--background-secondary) !important;
    border: none !important;
}

.calendar-body :deep(.fc-list-day-cushion) {
    background: var(--background-tertiary) !important;
    color: var(--text-primary) !important;
    font-weight: var(--font-weight-semibold);
    padding: var(--spacing-sm) var(--spacing-lg) !important;
}

.calendar-body :deep(.fc-list-event) {
    background: var(--background-secondary) !important;
    border: 1px solid var(--border-color) !important;
    border-radius: var(--radius-md) !important;
    margin: var(--spacing-xs) var(--spacing-md) !important;
    padding: var(--spacing-sm) !important;
}

.calendar-body :deep(.fc-list-event:hover) {
    background: var(--background-hover) !important;
    border-color: var(--accent-color) !important;
}

.calendar-body :deep(.fc-list-event-title) {
    color: var(--text-primary) !important;
    font-weight: var(--font-weight-semibold);
}

.calendar-body :deep(.fc-list-event-time) {
    color: var(--text-secondary) !important;
}

.calendar-body :deep(.fc-timegrid) {
    background: var(--background-secondary) !important;
}

.calendar-body :deep(.fc-timegrid-slot) {
    border-bottom: 1px solid var(--border-color) !important;
    height: 40px !important;
}

.calendar-body :deep(.fc-timegrid-axis) {
    background: var(--background-tertiary) !important;
    border-right: 1px solid var(--border-color) !important;
}

.calendar-body :deep(.fc-timegrid-event) {
    background: var(--accent-color) !important;
    border: none !important;
    border-radius: var(--radius-sm) !important;
}

.calendar-body :deep(.fc-timegrid-event.important) {
    background: var(--warning-color) !important;
}

.calendar-body :deep(.fc-timegrid-event.tagged) {
    background: var(--success) !important;
}

.notes-section {
    flex-shrink: 0;
    display: flex;
    flex-direction: column;
    background: var(--background-secondary);
    border-top: 1px solid var(--border-color);
    max-height: 320px;
}

.search-bar {
    position: relative;
    display: flex;
    align-items: center;
    padding: var(--spacing-sm) var(--spacing-lg);
    border-bottom: 1px solid var(--border-color);
}

.search-icon {
    position: absolute;
    left: var(--spacing-xl);
    color: var(--text-tertiary);
    pointer-events: none;
}

.search-input {
    width: 100%;
    padding: var(--spacing-xs) var(--spacing-xl) var(--spacing-xs) var(--spacing-2xl);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    font-size: var(--font-size-sm);
    background: var(--background-tertiary);
    color: var(--text-primary);
    transition: var(--transition-fast);
}

.search-input:focus {
    outline: none;
    border-color: var(--accent-color);
    background: var(--background-secondary);
}

.search-input::placeholder {
    color: var(--text-tertiary);
}

.clear-btn {
    position: absolute;
    right: var(--spacing-xl);
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

.clear-btn:hover {
    color: var(--text-secondary);
    background: var(--background-hover);
}

.filter-tags {
    display: flex;
    flex-wrap: wrap;
    gap: var(--spacing-xs);
    padding: var(--spacing-sm) var(--spacing-lg);
    border-bottom: 1px solid var(--border-color);
}

.filter-tag {
    padding: var(--spacing-xs) var(--spacing-sm);
    font-size: var(--font-size-xs);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    background: var(--background-tertiary);
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
    padding: var(--spacing-sm) var(--spacing-lg);
    background: var(--background-tertiary);
    border-bottom: 1px solid var(--border-color);
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
    background: var(--background-tertiary);
    border-radius: var(--radius-md);
    cursor: pointer;
    transition: var(--transition-fast);
    border: 1px solid transparent;
}

.note-item:hover {
    background: var(--background-hover);
    border-color: var(--border-color);
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
    background: var(--background-secondary);
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
    color: var(--text-secondary);
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

@media (max-width: 768px) {
    .calendar-body :deep(.fc-toolbar) {
        flex-direction: column;
        gap: var(--spacing-sm);
        padding: var(--spacing-sm) var(--spacing-md);
    }

    .calendar-body :deep(.fc-toolbar-title) {
        font-size: var(--font-size-md) !important;
    }

    .calendar-body :deep(.fc-daygrid-day) {
        min-height: 60px !important;
    }

    .notes-section {
        max-height: 280px;
    }
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
    padding: 6px 12px;
    background: var(--bg-secondary);
    border-bottom: 1px solid var(--border-color);
}

.date-text {
    font-size: 12px;
    font-weight: 600;
    color: var(--text-primary);
}

.notes-count {
    font-size: 10px;
    color: var(--text-tertiary);
}

.notes-list-content {
    flex: 1;
    overflow-y: auto;
    padding: 4px;
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

.note-item {
    padding: 6px 8px;
    margin-bottom: 2px;
    background: var(--bg-secondary);
    border-radius: 4px;
    cursor: pointer;
}

.note-item:hover {
    background: var(--bg-tertiary);
}

.note-item-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 3px;
}

.note-item-title {
    font-size: 12px;
    font-weight: 500;
    color: var(--text-primary);
    flex: 1;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.note-item-actions {
    display: flex;
    gap: 2px;
    opacity: 0;
    transition: opacity 0.2s;
}

.note-item:hover .note-item-actions {
    opacity: 1;
}

.icon-btn {
    padding: 2px;
    border: none;
    background: transparent;
    cursor: pointer;
    color: var(--text-tertiary);
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 2px;
}

.icon-btn:hover {
    background: var(--bg-primary);
    color: var(--text-primary);
}

.note-item-content {
    font-size: 11px;
    line-height: 1.3;
    color: var(--text-secondary);
    margin-bottom: 3px;
    overflow: hidden;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
}

.note-item-footer {
    display: flex;
    align-items: center;
    gap: 6px;
}

.note-item-time {
    font-size: 10px;
    color: var(--text-tertiary);
}

.important-badge {
    font-size: 9px;
    padding: 1px 4px;
    background: rgba(245, 158, 11, 0.15);
    color: var(--warning-color, #f59e0b);
    border-radius: 2px;
}

.tag-badge {
    font-size: 9px;
    padding: 1px 4px;
    background: var(--success-color-light);
    color: var(--success-color);
    border-radius: 2px;
}

.empty-notes {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: 24px;
    color: var(--text-tertiary);
}

.empty-notes svg {
    margin-bottom: 8px;
    opacity: 0.5;
}

.empty-notes p {
    margin: 0;
    font-size: 12px;
}

@media (max-width: 768px) {
    .calendar-body :deep(.fc-toolbar) {
        flex-direction: column;
        gap: var(--spacing-sm);
        padding: var(--spacing-sm) var(--spacing-md);
    }

    .calendar-body :deep(.fc-toolbar-title) {
        font-size: var(--font-size-md) !important;
    }

    .calendar-body :deep(.fc-daygrid-day) {
        min-height: 60px !important;
    }

    .notes-section {
        max-height: 280px;
    }
}
</style>
