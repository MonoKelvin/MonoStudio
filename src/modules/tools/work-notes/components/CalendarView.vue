<template>
    <div class="calendar-view">
        <div class="calendar-body">
            <FullCalendar :options="calendarOptions" ref="fullCalendar" />
        </div>
    </div>
</template>

<script setup>
import { ref, computed } from 'vue';
import FullCalendar from '@fullcalendar/vue3';
import dayGridPlugin from '@fullcalendar/daygrid';
import timeGridPlugin from '@fullcalendar/timegrid';
import listPlugin from '@fullcalendar/list';
import interactionPlugin from '@fullcalendar/interaction';
import zhCnLocale from '@fullcalendar/core/locales/zh-cn';
import { NoteQueryService } from '../services/NoteQueryService.js';

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

const emit = defineEmits(['prev-month', 'next-month', 'select-date', 'open-detail', 'edit-note', 'delete-note', 'add-note']);

const fullCalendar = ref(null);

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

const handleDateClick = (info) => {
    const dateString = info.dateStr;
    emit('select-date', dateString);
};

const handleEventClick = (info) => {
    const note = info.event.extendedProps.note;
    emit('open-detail', note);
};

const handleDatesSet = (info) => {
    const newDate = info.view.currentStart;
    emit('prev-month', newDate);
};
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
    position: relative;
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
    border: 1px solid var(--border-color) !important;
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
    color: var(--text-primary);
    transform: translateY(-1px);
}

.calendar-body :deep(.fc-daygrid-body) {
    padding: var(--spacing-xs);
}

.calendar-body :deep(.fc-daygrid-day) {
    min-height: 60px !important;
    max-height: 60px !important;
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
    padding: 4px 8px !important;
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
    font-size: 11px !important;
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
    border: 1px solid var(--border-color) !important;
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
    transition: background-color 0.2s ease;
}

.calendar-body :deep(.fc-list-event:hover) {
    background: var(--bg-hover) !important;
}

.calendar-body :deep(.fc-list-event-time) {
    color: var(--text-secondary) !important;
    font-weight: var(--font-weight-medium);
}

.calendar-body :deep(.fc-list-event-title) {
    color: var(--text-primary) !important;
    font-weight: var(--font-weight-medium);
}

.calendar-body :deep(.fc-timegrid) {
    background: var(--bg-primary) !important;
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
</style>
