<template>
    <div class="calendar-view">
        <FullCalendar :options="calendarOptions" ref="fullCalendar" />
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
        eventDidMount: function (info) {
            const eventEl = info.el;
            eventEl.style.borderRadius = '6px';
            eventEl.style.fontSize = '11px';
            eventEl.style.padding = '2px 4px';
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
            backgroundColor: note.important ? 'var(--warning-color)' : 'var(--accent-color)',
            borderColor: note.important ? 'var(--warning-color)' : 'var(--accent-color)',
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
}

.calendar-view :deep(.fc) {
    --fc-border-color: var(--border-color);
    --fc-button-bg-color: var(--bg-elevated);
    --fc-button-border-color: var(--border-color);
    --fc-button-text-color: var(--text-primary);
    --fc-button-hover-bg-color: var(--bg-hover);
    --fc-button-hover-border-color: var(--border-color);
    --fc-button-active-bg-color: var(--accent-color);
    --fc-button-active-border-color: var(--accent-color);
    --fc-event-bg-color: var(--accent-color);
    --fc-event-border-color: var(--accent-color);
    --fc-event-text-color: #ffffff;
    --fc-today-bg-color: color-mix(in srgb, var(--accent-color) 8%, transparent);
    --fc-page-bg-color: transparent;
    --fc-neutral-bg-color: var(--bg-soft);
    --fc-list-event-hover-bg-color: var(--bg-hover);
    font-family: inherit;
}

.calendar-view :deep(.fc-toolbar) {
    flex-wrap: wrap;
    gap: 12px;
    margin-bottom: 20px;
}

.calendar-view :deep(.fc-toolbar-title) {
    font-size: 18px !important;
    font-weight: 600 !important;
    color: var(--text-primary) !important;
    letter-spacing: -0.3px;
}

.calendar-view :deep(.fc-button) {
    background: var(--bg-elevated) !important;
    border: 1px solid var(--border-color) !important;
    color: var(--text-primary) !important;
    font-size: 12px !important;
    font-weight: 500 !important;
    padding: 8px 14px !important;
    border-radius: 8px !important;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.04) !important;
    transition: all 0.2s ease !important;
}

.calendar-view :deep(.fc-button:hover) {
    background: var(--bg-hover) !important;
    border-color: var(--border-color) !important;
    transform: translateY(-1px);
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.08) !important;
}

.calendar-view :deep(.fc-button:active) {
    transform: translateY(0);
}

.calendar-view :deep(.fc-button-primary:not(:disabled).fc-button-active),
.calendar-view :deep(.fc-button-primary:not(:disabled):active) {
    background: var(--accent-color) !important;
    border-color: var(--accent-color) !important;
    color: white !important;
}

.calendar-view :deep(.fc-button-group) {
    gap: 4px;
}

.calendar-view :deep(.fc-button-group .fc-button) {
    border-radius: 6px !important;
}

.calendar-view :deep(.fc-button-group .fc-button:first-child) {
    border-radius: 6px 0 0 6px !important;
}

.calendar-view :deep(.fc-button-group .fc-button:last-child) {
    border-radius: 0 6px 6px 0 !important;
}

.calendar-view :deep(.fc-scrollgrid) {
    border: 1px solid var(--border-color) !important;
    border-radius: 12px;
    overflow: hidden;
}

.calendar-view :deep(.fc-scrollgrid-section-header td) {
    border-bottom: 1px solid var(--border-color) !important;
}

.calendar-view :deep(.fc-col-header-cell) {
    background: var(--bg-soft) !important;
    padding: 0 !important;
}

.calendar-view :deep(.fc-col-header-cell-cushion) {
    font-size: 11px;
    font-weight: 600;
    color: var(--text-secondary);
    text-transform: uppercase;
    letter-spacing: 0.5px;
    padding: 12px 8px !important;
    display: block;
    text-align: center;
}

.calendar-view :deep(.fc-daygrid) {
    padding: 8px;
}

.calendar-view :deep(.fc-daygrid-day) {
    min-height: 80px !important;
    padding: 4px;
    box-sizing: border-box;
}

.calendar-view :deep(.fc-daygrid-day:hover) {
    background: var(--bg-soft);
    border-radius: 8px;
}

.calendar-view :deep(.fc-daygrid-day-frame) {
    padding: 4px;
    display: flex;
    flex-direction: column;
    height: 100%;
}

.calendar-view :deep(.fc-daygrid-day-number) {
    font-size: 13px;
    font-weight: 500;
    color: var(--text-primary);
    padding: 4px 8px !important;
    display: inline-block;
    text-align: center;
    text-decoration: none !important;
    border-radius: 6px;
    transition: all 0.15s ease;
    margin-bottom: 4px;
}

.calendar-view :deep(.fc-daygrid-day:hover .fc-daygrid-day-number) {
    background: var(--bg-hover);
}

.calendar-view :deep(.fc-day-today .fc-daygrid-day-number) {
    background: var(--accent-color);
    color: white;
    font-weight: 600;
}

.calendar-view :deep(.fc-daygrid-day.fc-day-selected) {
    background: transparent !important;
}

.calendar-view :deep(.fc-daygrid-day.fc-day-selected:hover) {
    background: var(--bg-soft) !important;
}

.calendar-view :deep(.fc-daygrid-day.fc-day-selected .fc-daygrid-day-number) {
    background: var(--accent-color);
    color: white;
}

.calendar-view :deep(.fc-day-other .fc-daygrid-day-number) {
    color: var(--text-tertiary);
}

.calendar-view :deep(.fc-daygrid-event) {
    background: transparent !important;
    border: none !important;
    padding: 0 !important;
    margin: 2px 0 !important;
}

.calendar-view :deep(.fc-daygrid-event-content) {
    display: flex !important;
    align-items: center !important;
    padding: 5px 8px !important;
    border-radius: 6px !important;
    border-left: 3px solid var(--accent-color) !important;
    background: var(--bg-elevated) !important;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.06) !important;
    transition: all 0.15s ease;
    overflow: hidden;
}

.calendar-view :deep(.fc-daygrid-event:hover .fc-daygrid-event-content) {
    transform: translateX(2px);
    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.1) !important;
}

.calendar-view :deep(.fc-daygrid-event-title) {
    font-size: 11px !important;
    font-weight: 500;
    color: var(--text-primary);
    line-height: 1.3;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    flex: 1;
}

.calendar-view :deep(.fc-daygrid-event.event-important .fc-daygrid-event-content) {
    border-left-color: var(--warning-color) !important;
    background: color-mix(in srgb, var(--warning-color) 10%, var(--bg-elevated)) !important;
}

.calendar-view :deep(.fc-daygrid-event.event-tagged .fc-daygrid-event-content) {
    border-left-color: var(--success) !important;
    background: color-mix(in srgb, var(--success) 10%, var(--bg-elevated)) !important;
}

.calendar-view :deep(.fc-daygrid-more-link) {
    font-size: 11px;
    font-weight: 500;
    color: var(--accent-color) !important;
    padding: 4px 8px;
    margin-top: 4px;
    display: inline-block;
    background: var(--bg-elevated);
    border-radius: 6px;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.04);
    transition: all 0.15s ease;
}

.calendar-view :deep(.fc-daygrid-more-link:hover) {
    background: var(--accent-color);
    color: white !important;
}

.calendar-view :deep(.fc-list) {
    background: var(--bg-elevated) !important;
    border: 1px solid var(--border-color) !important;
    border-radius: 12px;
    overflow: hidden;
}

.calendar-view :deep(.fc-list-day-cushion) {
    background: var(--bg-soft) !important;
    color: var(--text-primary) !important;
    font-weight: 600;
    padding: 12px 16px !important;
}

.calendar-view :deep(.fc-list-event) {
    transition: background-color 0.15s ease;
}

.calendar-view :deep(.fc-list-event:hover) {
    background: var(--bg-soft) !important;
}

.calendar-view :deep(.fc-list-event-time) {
    color: var(--text-secondary) !important;
    font-weight: 500;
    padding: 8px 12px !important;
}

.calendar-view :deep(.fc-list-event-title) {
    color: var(--text-primary) !important;
    font-weight: 500;
    padding: 8px 12px !important;
}

.calendar-view :deep(.fc-timegrid) {
    background: var(--bg-elevated) !important;
}

.calendar-view :deep(.fc-timegrid-axis) {
    background: var(--bg-soft) !important;
    border-right: 1px solid var(--border-color) !important;
}

.calendar-view :deep(.fc-timegrid-axis-cushion) {
    font-size: 11px;
    font-weight: 500;
    color: var(--text-secondary);
    padding: 8px !important;
}

.calendar-view :deep(.fc-timegrid-event) {
    background: var(--accent-color) !important;
    border: none !important;
    border-radius: 6px !important;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
    transition: all 0.15s ease;
}

.calendar-view :deep(.fc-timegrid-event:hover) {
    transform: scale(1.02);
    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.15);
}

.calendar-view :deep(.fc-timegrid-event.important) {
    background: var(--warning-color) !important;
}

.calendar-view :deep(.fc-timegrid-event.tagged) {
    background: var(--success) !important;
}

.calendar-view :deep(.fc-timegrid-event-title) {
    font-size: 11px;
    font-weight: 500;
    color: white;
    padding: 4px 6px;
}

.calendar-view :deep(.fc-timegrid-event-time) {
    font-size: 10px;
    font-weight: 500;
    color: rgba(255, 255, 255, 0.85);
    padding: 0 6px;
}
</style>
