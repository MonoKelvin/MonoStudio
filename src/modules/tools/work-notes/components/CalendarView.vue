<template>
    <div class="calendar-view">
        <!-- 日历主体 -->
        <div class="calendar-body">
            <FullCalendar :options="calendarOptions" ref="fullCalendar" />
        </div>

        <div class="drawer-backdrop" :class="{ visible: isNotesExpanded }" @click="toggleNotes"></div>

        <!-- 笔记列表抽屉 -->
        <div class="notes-drawer" :class="{ expanded: isNotesExpanded }">
            <!-- 抽屉触发器 -->
            <div class="drawer-handle" @click="toggleNotes">
                <div class="handle-bar"></div>
            </div>

            <!-- 搜索和笔记列表区域 -->
            <div class="notes-section">
                <!-- 搜索框 -->
                <div class="search-bar">
                    <img class="search-icon" src="../../../../assets/icons/search-16.svg" alt="搜索" />
                    <BaseInput v-model="localSearchQuery" class="search-input" placeholder="搜索笔记..."
                        @input="handleSearchInput" />
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
                                            <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
                                            <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
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
import FullCalendar from '@fullcalendar/vue3';
import dayGridPlugin from '@fullcalendar/daygrid';
import timeGridPlugin from '@fullcalendar/timegrid';
import listPlugin from '@fullcalendar/list';
import interactionPlugin from '@fullcalendar/interaction';
import zhCnLocale from '@fullcalendar/core/locales/zh-cn';
import { NoteQueryService } from '../services/NoteQueryService.js';
import BaseInput from '../../../../components/base/BaseInput.vue';

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
const isNotesExpanded = ref(false);
const localSearchQuery = ref('');

// View options
// Computed properties
const calendarOptions = computed(() => {
    return {
        plugins: [dayGridPlugin, timeGridPlugin, listPlugin, interactionPlugin],
        initialView: 'dayGridMonth',
        locale: zhCnLocale,
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
            eventEl.style.borderRadius = '10px';
            eventEl.style.fontSize = '11px';
            eventEl.style.padding = '3px 6px';
            eventEl.classList.add(`event-${info.event.extendedProps.type}`);
        },
        datesSet: handleDatesSet,
        height: 'auto'
    };
});

const calendarEvents = computed(() => {
    const filtered = NoteQueryService.filterNotes(props.notes, props.searchQuery, props.selectedTags);

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

// Methods
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

const toggleNotes = () => {
    isNotesExpanded.value = !isNotesExpanded.value;
};

const previewText = (note) => {
    return NoteQueryService.toPreviewText(note, 80);
};

const handleSearchInput = () => {
    emit('update:searchQuery', localSearchQuery.value);
};

const clearSearch = () => {
    localSearchQuery.value = '';
    emit('update:searchQuery', '');
};

// Watch for searchQuery changes from parent
watch(() => props.searchQuery, (newValue) => {
    localSearchQuery.value = newValue;
});
</script>

<style scoped>
.calendar-view {
    height: 100%;
    min-height: 0;
    display: flex;
    flex-direction: column;
    position: relative;
    border-radius: 12px;
    overflow: hidden;
}

.calendar-body {
    flex: 1;
    min-height: 0;
    overflow: hidden;
    background: var(--bg-primary);
    padding: 14px;
    padding-bottom: 120px;
}

.calendar-body :deep(.fc-toolbar) {
    background: transparent;
    border: none;
}

.calendar-body :deep(.fc-toolbar-title) {
    font-size: 18px !important;
    font-weight: var(--font-weight-semibold) !important;
    color: var(--text-primary) !important;
    letter-spacing: -0.3px;
    transition: color 0.2s ease;
}

.calendar-body :deep(.fc-toolbar-title:hover) {
    color: var(--accent-color) !important;
    text-decoration: none !important;
}

.calendar-body :deep(.fc-button) {
    background: color-mix(in srgb, var(--bg-tertiary) 90%, white 10%) !important;
    border: none !important;
    color: var(--text-primary) !important;
    font-size: var(--font-size-sm) !important;
    font-weight: var(--font-weight-medium) !important;
    padding: 6px 13px !important;
    border-radius: 10px !important;
    box-shadow: none !important;
    transition: all 0.2s ease !important;
    transform: translateY(0);
}

.calendar-body :deep(.fc-button:hover) {
    background: var(--bg-hover) !important;
    transform: translateY(-1px);
    box-shadow: 0 4px 10px rgba(15, 23, 42, 0.1) !important;
}

.calendar-body :deep(.fc-button:active) {
    transform: translateY(0) scale(0.98);
}

.calendar-body :deep(.fc-button:focus) {
    box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.3) !important;
}

.calendar-body :deep(.fc-button-primary:not(:disabled).fc-button-active),
.calendar-body :deep(.fc-button-primary:not(:disabled):active) {
    background: var(--accent-color) !important;
    border-color: var(--accent-color) !important;
    color: white !important;
    font-weight: 600 !important;
}

.calendar-body :deep(.fc-button-group) {
    gap: 2px;
}

.calendar-body :deep(.fc-button-group .fc-button) {
    border-radius: 0 !important;
}

.calendar-body :deep(.fc-button-group .fc-button:first-child) {
    border-radius: var(--radius-md) 0 0 var(--radius-md) !important;
}

.calendar-body :deep(.fc-button-group .fc-button:last-child) {
    border-radius: 0 var(--radius-md) var(--radius-md) 0 !important;
}

.calendar-body :deep(.fc-theme-standard .fc-scrollgrid) {
    border: none !important;
    border-radius: 12px;
    overflow: hidden;
    background: var(--bg-primary) !important;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
}

.calendar-body :deep(.fc-theme-standard td),
.calendar-body :deep(.fc-theme-standard th) {
    border-color: var(--border-color) !important;
    border-width: 1px !important;
    padding: 0 !important;
}

.calendar-body :deep(.fc-scrollgrid-section) {
    border: none !important;
}

.calendar-body :deep(.fc-scrollgrid-section-header) {
    border-bottom: 1px solid var(--border-color) !important;
}

.calendar-body :deep(.fc-scrollgrid-section-body) {
    border-top: none !important;
}

.calendar-body :deep(.fc-col-header-cell) {
    background: var(--bg-elevated) !important;
    border-bottom: 1px solid var(--border-color) !important;
    padding: 0 !important;
    transition: background-color 0.2s ease;
}

.calendar-body :deep(.fc-col-header-cell:hover) {
    background: var(--bg-hover) !important;
}

.calendar-body :deep(.fc-col-header-cell-cushion) {
    font-size: var(--font-size-xs);
    font-weight: var(--font-weight-semibold);
    color: var(--text-secondary);
    text-transform: uppercase;
    letter-spacing: 0.5px;
    padding: var(--spacing-sm) var(--spacing-xs) !important;
    display: block;
    text-align: center;
    transition: all 0.2s ease;
}

.calendar-body :deep(.fc-col-header-cell:hover .fc-col-header-cell-cushion) {
    color: var(--accent-color);
    transform: translateY(-1px);
}

.calendar-body :deep(.fc-daygrid-body) {
    padding: var(--spacing-xs);
}

.calendar-body :deep(.fc-daygrid-day) {
    min-height: 50px !important;
    max-height: 50px !important;
    transition: all 0.2s ease;
    border-radius: 8px;
    margin: 2px;
    cursor: pointer;
}

.calendar-body :deep(.fc-daygrid-day:hover) {
    background: var(--bg-hover) !important;
    transform: translateY(-1px);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}

.calendar-body :deep(.fc-daygrid-day-frame) {
    padding: var(--spacing-xs);
    height: 100%;
    display: flex;
    flex-direction: column;
    overflow: hidden;
}

.calendar-body :deep(.fc-daygrid-day-number) {
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    padding: var(--spacing-xs);
    display: inline-block;
    text-align: center;
    transition: all 0.2s ease;
    border-radius: 50%;
    width: 28px;
    height: 28px;
    line-height: 28px;
    margin: 0 auto;
    text-decoration: none !important;
}

.calendar-body :deep(.fc-day-today) {
    background: var(--bg-primary) !important;
}

.calendar-body :deep(.fc-day-today .fc-daygrid-day-number) {
    background: var(--accent-color);
    color: white;
    font-weight: var(--font-weight-bold);
    box-shadow: 0 2px 8px rgba(59, 130, 246, 0.4);
    transform: scale(1.1);
}

.calendar-body :deep(.fc-daygrid-day.fc-day-selected) {
    background: color-mix(in srgb, var(--accent-color) 20%, transparent) !important;
    border-radius: 8px;
}

.calendar-body :deep(.fc-daygrid-day.fc-day-selected .fc-daygrid-day-number) {
    background: var(--accent-color);
    color: white;
    box-shadow: 0 2px 8px rgba(59, 130, 246, 0.4);
}

.calendar-body :deep(.fc-day-other .fc-daygrid-day-number) {
    color: var(--text-tertiary);
    opacity: 0.7;
}

.calendar-body :deep(.fc-day-other:hover .fc-daygrid-day-number) {
    color: var(--text-secondary);
    opacity: 1;
}

.calendar-body :deep(.fc-daygrid-event) {
    background: transparent !important;
    border: none !important;
    padding: 0 !important;
    margin: 2px 4px !important;
    transition: all 0.2s ease;
    border-radius: 8px;
    overflow: hidden;
    white-space: nowrap;
}

.calendar-body :deep(.fc-daygrid-event:hover) {
    transform: translateY(-1px);
}

.calendar-body :deep(.fc-daygrid-event-content) {
    display: flex !important;
    align-items: center !important;
    padding: 3px 6px !important;
    background: var(--bg-elevated) !important;
    border-radius: 6px !important;
    border-left: 3px solid var(--accent-color) !important;
    transition: all 0.2s ease;
    overflow: hidden;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
    white-space: nowrap;
    text-overflow: ellipsis;
}

.calendar-body :deep(.fc-daygrid-event-title) {
    font-size: 10px !important;
    font-weight: var(--font-weight-medium);
    color: var(--text-primary);
    line-height: 1.2;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    flex: 1;
    margin: 0 !important;
}

.calendar-body :deep(.fc-daygrid-event:hover .fc-daygrid-event-content) {
    background: var(--bg-hover) !important;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
    transform: translateX(2px);
}

.calendar-body :deep(.fc-daygrid-event.event-important .fc-daygrid-event-content) {
    border-left-color: var(--warning-color) !important;
    background: rgba(245, 158, 11, 0.1) !important;
}

.calendar-body :deep(.fc-daygrid-event.event-tagged .fc-daygrid-event-content) {
    border-left-color: var(--success) !important;
    background: rgba(16, 185, 129, 0.1) !important;
}

.calendar-body :deep(.fc-daygrid-event-title) {
    font-size: 10px !important;
    font-weight: var(--font-weight-medium);
    color: var(--text-primary);
    line-height: 1.2;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    flex: 1;
    margin: 0 !important;
}

.calendar-body :deep(.fc-daygrid-more-link) {
    font-size: var(--font-size-xs);
    color: var(--accent-color) !important;
    font-weight: var(--font-weight-medium);
    display: inline-flex;
    align-items: center;
    padding: 4px 12px;
    margin: 4px;
    background: var(--bg-elevated) !important;
    border: 1px solid var(--accent-color) !important;
    border-radius: var(--radius-md) !important;
    transition: all 0.2s ease;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
}

.calendar-body :deep(.fc-daygrid-more-link:hover) {
    background: var(--accent-color) !important;
    color: white !important;
    transform: translateY(-1px);
    box-shadow: 0 4px 12px rgba(59, 130, 246, 0.3);
}

.calendar-body :deep(.fc-list) {
    background: var(--bg-primary) !important;
    border: none !important;
    border-radius: var(--radius-lg);
    overflow: hidden;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
}

.calendar-body :deep(.fc-list-day-cushion) {
    background: var(--bg-elevated) !important;
    color: var(--text-primary) !important;
    font-weight: var(--font-weight-semibold);
    padding: var(--spacing-sm) var(--spacing-lg) !important;
    border-bottom: 1px solid var(--border-color) !important;
    transition: background-color 0.2s ease;
}

.calendar-body :deep(.fc-list-day-cushion:hover) {
    background: var(--bg-hover) !important;
}

.calendar-body :deep(.fc-list-event) {
    background: var(--bg-elevated) !important;
    border: 1px solid var(--border-color) !important;
    border-radius: var(--radius-md) !important;
    margin: var(--spacing-xs) var(--spacing-md) !important;
    padding: var(--spacing-sm) !important;
    transition: all 0.2s ease;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
}

.calendar-body :deep(.fc-list-event:hover) {
    background: var(--bg-hover) !important;
    border-color: var(--accent-color) !important;
    transform: translateX(4px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}

.calendar-body :deep(.fc-list-event-title) {
    color: var(--text-primary) !important;
    font-weight: var(--font-weight-semibold);
    font-size: var(--font-size-sm);
}

.calendar-body :deep(.fc-list-event-time) {
    color: var(--text-secondary) !important;
    font-size: var(--font-size-xs);
    background: var(--bg-tertiary) !important;
    padding: 2px 6px !important;
    border-radius: var(--radius-sm) !important;
    margin-right: var(--spacing-xs) !important;
}

.calendar-body :deep(.fc-timegrid) {
    background: var(--bg-primary) !important;
    border: none !important;
    border-radius: var(--radius-lg);
    overflow: hidden;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
}

.calendar-body :deep(.fc-timegrid-slot) {
    border-bottom: 1px solid var(--border-color) !important;
    height: 45px !important;
    transition: background-color 0.2s ease;
}

.calendar-body :deep(.fc-timegrid-slot:hover) {
    background: var(--bg-hover) !important;
}

.calendar-body :deep(.fc-timegrid-axis) {
    background: var(--bg-elevated) !important;
    border-right: 1px solid var(--border-color) !important;
    transition: background-color 0.2s ease;
}

.calendar-body :deep(.fc-timegrid-axis:hover) {
    background: var(--bg-hover) !important;
}

.calendar-body :deep(.fc-timegrid-axis-cushion) {
    font-size: var(--font-size-xs);
    font-weight: var(--font-weight-medium);
    color: var(--text-secondary);
    padding: var(--spacing-xs) !important;
}

.calendar-body :deep(.fc-timegrid-event) {
    background: var(--accent-color) !important;
    border: none !important;
    border-radius: var(--radius-md) !important;
    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.1);
    transition: all 0.2s ease;
    overflow: hidden;
}

.calendar-body :deep(.fc-timegrid-event:hover) {
    transform: scale(1.02);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
    z-index: 10 !important;
}

.calendar-body :deep(.fc-timegrid-event.important) {
    background: var(--warning-color) !important;
}

.calendar-body :deep(.fc-timegrid-event.tagged) {
    background: var(--success) !important;
}

.calendar-body :deep(.fc-timegrid-event-title) {
    font-size: var(--font-size-xs);
    font-weight: var(--font-weight-medium);
    color: white;
    padding: var(--spacing-xs);
    line-height: 1.3;
}

.calendar-body :deep(.fc-timegrid-event-time) {
    font-size: var(--font-size-xs);
    font-weight: var(--font-weight-medium);
    color: rgba(255, 255, 255, 0.9);
    padding: 0 var(--spacing-xs);
}

.notes-drawer {
    position: absolute;
    left: 10px;
    right: 10px;
    bottom: 10px;
    background: var(--bg-soft);
    transition: max-height 0.5s cubic-bezier(0.22, 1, 0.36, 1), box-shadow 0.25s ease;
    max-height: 42px;
    overflow: hidden;
    z-index: 60;
    border-radius: 14px;
    box-shadow: 0 -4px 12px color-mix(in srgb, var(--bg-primary) 15%, transparent);
}

.notes-drawer.expanded {
    max-height: 360px;
    box-shadow: 0 -10px 36px var(--bg-primary);
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
    max-height: 318px;
    overflow: hidden;
}

.search-bar {
    position: relative;
    display: flex;
    align-items: center;
    padding: var(--spacing-md);
    border-bottom: 1px solid var(--border-color);
    background: var(--bg-soft);
}

.search-icon {
    position: absolute;
    left: calc(var(--spacing-md) + var(--spacing-sm));
    width: 16px;
    height: 16px;
    pointer-events: none;
    transition: opacity 0.2s ease;
    opacity: 0.6;
}

.search-input {
    width: 100%;
    padding-left: 40px;
    padding-right: 36px;
    font-size: var(--font-size-sm);
}

.search-input:focus+.search-icon,
.search-input:not(:placeholder-shown)~.search-icon {
    opacity: 1;
}

.clear-btn {
    position: absolute;
    right: calc(var(--spacing-md) + var(--spacing-xs));
    padding: var(--spacing-xs);
    border: none;
    background: transparent;
    cursor: pointer;
    color: var(--text-tertiary);
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: var(--radius-sm);
    transition: all 0.2s ease;
}

.clear-btn:hover {
    color: var(--text-primary);
    background: var(--bg-hover);
    transform: scale(1.1);
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

.drawer-backdrop {
    position: absolute;
    inset: 0;
    pointer-events: none;
    opacity: 0;
    transition: opacity 0.2s ease;
    background: color-mix(in srgb, var(--bg-primary) 30%, transparent);
    z-index: 15;
}

.drawer-backdrop.visible {
    opacity: 1;
    pointer-events: auto;
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

    .notes-drawer {
        left: 6px;
        right: 6px;
        bottom: 6px;
    }

    .notes-section {
        max-height: 280px;
    }
}
</style>
