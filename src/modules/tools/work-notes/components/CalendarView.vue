<template>
    <div class="calendar-view">
        <!-- 日历头部 -->
        <div class="calendar-header">
            <button class="header-btn today-btn" @click="goToToday" title="回到今天">
                <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect>
                    <line x1="16" y1="2" x2="16" y2="6"></line>
                    <line x1="8" y1="2" x2="8" y2="6"></line>
                    <line x1="3" y1="10" x2="21" y2="10"></line>
                </svg>
            </button>
            <div class="header-nav">
                <button class="header-btn" @click="prevMonth">
                    <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <polyline points="15 18 9 12 15 6"></polyline>
                    </svg>
                </button>
                <h3 class="header-title">{{ currentMonthYear }}</h3>
                <button class="header-btn" @click="nextMonth">
                    <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <polyline points="9 18 15 12 9 6"></polyline>
                    </svg>
                </button>
            </div>
            <button class="header-btn today-text-btn" @click="goToToday" title="今天">
                <span class="today-text">今天</span>
            </button>
        </div>

        <!-- 视图切换 -->
        <div class="view-toggle">
            <button v-for="view in viewOptions" :key="view.value" class="view-btn"
                :class="{ active: currentView === view.value }" @click="switchView(view.value)">
                {{ view.label }}
            </button>
        </div>

        <!-- 日历主体 -->
        <div class="calendar-body">
            <FullCalendar :options="calendarOptions" ref="fullCalendar" />
        </div>

        <!-- 选中日期的笔记 -->
        <div class="selected-date-notes" v-if="selectedDate">
            <div class="date-notes-header">
                <h4>{{ formatSelectedDate }}</h4>
                <div class="header-actions">
                    <span class="notes-badge">{{ selectedDateNotes.length }}</span>
                    <BaseButton size="sm" @click="$emit('add-note', selectedDate)">添加笔记</BaseButton>
                </div>
            </div>
            <div class="date-notes-list" v-if="selectedDateNotes.length > 0">
                <NoteCard v-for="note in selectedDateNotes" :key="note.id" :note="note"
                    @click="$emit('open-detail', note)" @edit="$emit('edit-note', note)"
                    @delete="$emit('delete-note', note)" />
            </div>
            <div class="no-notes-for-date" v-else>
                <div class="empty-state">
                    <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5"
                        stroke-linecap="round" stroke-linejoin="round">
                        <rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect>
                        <line x1="16" y1="2" x2="16" y2="6"></line>
                        <line x1="8" y1="2" x2="8" y2="6"></line>
                        <line x1="3" y1="10" x2="21" y2="10"></line>
                    </svg>
                    <p>这一天没有笔记</p>
                    <BaseButton size="sm" @click="$emit('add-note', selectedDate)">添加笔记</BaseButton>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../components/base/BaseButton.vue';
import NoteCard from './NoteCard.vue';
import FullCalendar from '@fullcalendar/vue3';
import dayGridPlugin from '@fullcalendar/daygrid';
import timeGridPlugin from '@fullcalendar/timegrid';
import listPlugin from '@fullcalendar/list';
import interactionPlugin from '@fullcalendar/interaction';

export default {
    name: 'CalendarView',
    components: {
        BaseButton,
        NoteCard,
        FullCalendar
    },
    props: {
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
        }
    },
    emits: ['prev-month', 'next-month', 'select-date', 'open-detail', 'edit-note', 'delete-note', 'add-note'],
    data() {
        return {
            calendarRef: null,
            currentView: 'dayGridMonth',
            viewOptions: [
                { label: '月', value: 'dayGridMonth' },
                { label: '周', value: 'timeGridWeek' },
                { label: '日', value: 'timeGridDay' },
                { label: '列表', value: 'listMonth' }
            ]
        };
    },
    computed: {
        currentMonthYear() {
            return this.currentDate.toLocaleDateString('zh-CN', {
                year: 'numeric',
                month: 'long'
            });
        },
        calendarOptions() {
            return {
                plugins: [dayGridPlugin, timeGridPlugin, listPlugin, interactionPlugin],
                initialView: this.currentView,
                locale: 'zh-cn',
                headerToolbar: false,
                navLinks: true,
                selectable: true,
                events: this.calendarEvents,
                dateClick: this.handleDateClick,
                eventClick: this.handleEventClick,
                dayMaxEvents: 3,
                slotMinTime: '08:00',
                slotMaxTime: '20:00',
                allDaySlot: true,
                eventDidMount: function (info) {
                    const eventEl = info.el;
                    eventEl.style.borderRadius = '6px';
                    eventEl.style.fontSize = '12px';
                    eventEl.style.padding = '3px 6px';
                    eventEl.style.margin = '2px 0';
                    eventEl.style.transition = 'all 0.2s ease';
                },
                dayCellContent: function (arg) {
                    return {
                        html: `<span class="fc-day-number">${arg.dayNumberText}</span>`
                    };
                },
                eventContent: function (arg) {
                    return {
                        html: `<span class="event-dot ${arg.event.extendedProps.type}"></span><span class="event-title">${arg.event.title}</span>`
                    };
                },
                datesSet: this.handleDatesSet,
                height: 'auto'
            };
        },
        calendarEvents() {
            return this.notes.map(note => {
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
        },
        selectedDateNotes() {
            if (!this.selectedDate) return [];
            return this.notes.filter(note => note.createdAt.split('T')[0] === this.selectedDate);
        },
        formatSelectedDate() {
            if (!this.selectedDate) return '';
            const date = new Date(this.selectedDate);
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
        }
    },
    mounted() {
        this.calendarRef = this.$refs.fullCalendar;
    },
    methods: {
        prevMonth() {
            if (this.calendarRef) {
                this.calendarRef.getApi().prev();
            }
        },
        nextMonth() {
            if (this.calendarRef) {
                this.calendarRef.getApi().next();
            }
        },
        goToToday() {
            if (this.calendarRef) {
                this.calendarRef.getApi().today();
                this.$emit('select-date', new Date().toISOString().split('T')[0]);
            }
        },
        switchView(view) {
            this.currentView = view;
            if (this.calendarRef) {
                this.calendarRef.getApi().changeView(view);
            }
        },
        handleDateClick(info) {
            const dateString = info.dateStr;
            this.$emit('select-date', dateString);
        },
        handleEventClick(info) {
            const note = info.event.extendedProps.note;
            this.$emit('open-detail', note);
        },
        handleDatesSet(info) {
        }
    }
};
</script>

<style scoped>
.calendar-view {
    height: 100%;
    min-height: 0;
    display: flex;
    flex-direction: column;
    background: #ffffff;
    border-radius: 12px;
    overflow: hidden;
}

.calendar-header {
    flex-shrink: 0;
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 14px 18px;
    background: #3b82f6;
    color: white;
}

.header-nav {
    display: flex;
    align-items: center;
    gap: 6px;
}

.header-btn {
    background: rgba(255, 255, 255, 0.15);
    border: none;
    color: white;
    cursor: pointer;
    padding: 8px;
    border-radius: 8px;
    transition: all 0.25s ease;
    display: flex;
    align-items: center;
    justify-content: center;
}

.header-btn:hover {
    background: rgba(255, 255, 255, 0.25);
    transform: scale(1.08);
}

.header-btn:active {
    transform: scale(0.95);
}

.today-btn {
    padding: 8px 10px;
}

.today-text-btn {
    padding: 8px 12px;
}

.today-text {
    font-size: 13px;
    font-weight: 600;
}

.header-title {
    font-size: 17px;
    font-weight: 700;
    margin: 0;
    letter-spacing: 0.3px;
    min-width: 160px;
    text-align: center;
}

.view-toggle {
    flex-shrink: 0;
    display: flex;
    gap: 5px;
    padding: 10px 18px;
    background: #ffffff;
    border-bottom: 1px solid #e2e8f0;
}

.view-btn {
    background: transparent;
    border: 1px solid #e2e8f0;
    color: #64748b;
    font-size: 12px;
    font-weight: 500;
    padding: 6px 14px;
    border-radius: 18px;
    cursor: pointer;
    transition: all 0.25s ease;
    font-family: inherit;
}

.view-btn:hover {
    background: #f1f5f9;
    color: #1e293b;
    border-color: #3b82f6;
}

.view-btn.active {
    background: #3b82f6;
    color: white;
    border-color: #3b82f6;
}

.calendar-body {
    flex: 1;
    min-height: 0;
    overflow: auto;
    background: #ffffff;
}

.calendar-body :deep(.fc) {
    height: 100% !important;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
}

.calendar-body :deep(.fc-theme-standard) {
    background: #ffffff !important;
}

.calendar-body :deep(.fc-theme-standard .fc-scrollgrid) {
    border: none !important;
    background: #ffffff !important;
}

.calendar-body :deep(.fc-theme-standard td),
.calendar-body :deep(.fc-theme-standard th) {
    border-color: #e2e8f0 !important;
}

.calendar-body :deep(.fc-daygrid-day) {
    transition: all 0.25s ease;
}

.calendar-body :deep(.fc-daygrid-day:hover) {
    background: #f1f5f9 !important;
}

.calendar-body :deep(.fc-daygrid-day-number) {
    font-size: 13px;
    font-weight: 600;
    color: #1e293b;
    padding: 6px 8px;
    border-radius: 50%;
    min-width: 28px;
    height: 28px;
    text-align: center;
    display: inline-flex;
    align-items: center;
    justify-content: center;
    transition: all 0.15s ease;
    margin: 2px;
}

.calendar-body :deep(.fc-day-today .fc-daygrid-day-number) {
    background: #3b82f6;
    color: white;
    font-weight: 700;
    transform: scale(1.05);
}

.calendar-body :deep(.fc-daygrid-event) {
    background: transparent !important;
    border: none !important;
    padding: 0 !important;
    margin: 2px 0 !important;
    transition: all 0.15s ease;
}

.calendar-body :deep(.fc-daygrid-event:hover) {
    transform: translateX(4px);
}

.event-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    display: inline-block;
    margin-right: 4px;
    vertical-align: middle;
    transition: all 0.15s ease;
}

.calendar-body :deep(.fc-daygrid-event:hover .event-dot) {
    transform: scale(1.2);
}

.event-dot.important {
    background: #f59e0b;
}

.event-dot.tagged {
    background: #10b981;
}

.event-dot.normal {
    background: #3b82f6;
}

.event-title {
    font-size: 11px;
    color: #1e293b;
    font-weight: 500;
    line-height: 1.4;
}

.calendar-body :deep(.fc-daygrid-more-link) {
    font-size: 10px;
    color: #3b82f6 !important;
    font-weight: 600;
    background: #f1f5f9 !important;
    border-radius: 8px !important;
    padding: 2px 5px !important;
}

.calendar-body :deep(.fc-col-header-cell) {
    background: #f1f5f9 !important;
    border-bottom: 1px solid #e2e8f0 !important;
}

.calendar-body :deep(.fc-col-header-cell-cushion) {
    font-size: 11px;
    font-weight: 600;
    color: #64748b;
    text-transform: uppercase;
    letter-spacing: 0.5px;
    padding: 10px 0;
}

.calendar-body :deep(.fc-scrollgrid tbody tr) {
    border-bottom: 1px solid #e2e8f0;
}

.calendar-body :deep(.fc-scrollgrid tbody tr td) {
    border-right: 1px solid #e2e8f0;
}

.calendar-body :deep(.fc-list-event) {
    border-radius: 8px;
    margin: 6px 0;
    transition: all 0.15s ease;
    border: 1px solid #e2e8f0 !important;
    background: #ffffff !important;
}

.calendar-body :deep(.fc-list-event:hover) {
    border-color: #3b82f6 !important;
}

.calendar-body :deep(.fc-list-event-title) {
    font-weight: 600;
    color: #1e293b;
}

.calendar-body :deep(.fc-list-event-time) {
    color: #64748b;
    font-size: 11px;
}

.calendar-body :deep(.fc-list-day-text) {
    font-weight: 700;
    color: #3b82f6;
    font-size: 13px;
}

.selected-date-notes {
    flex-shrink: 0;
    max-height: 300px;
    display: flex;
    flex-direction: column;
    gap: 10px;
    overflow: hidden;
    background: #ffffff;
    padding: 14px 18px;
}

.date-notes-header {
    flex-shrink: 0;
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding-bottom: 10px;
    border-bottom: 1px solid #e2e8f0;
}

.date-notes-header h4 {
    margin: 0;
    font-size: 15px;
    font-weight: 700;
    color: #1e293b;
    font-family: inherit;
}

.header-actions {
    display: flex;
    align-items: center;
    gap: 8px;
}

.notes-badge {
    font-size: 10px;
    padding: 4px 8px;
    background: #3b82f6;
    color: white;
    border-radius: 12px;
    font-weight: 600;
}

.date-notes-list {
    flex: 1;
    overflow-y: auto;
    display: flex;
    flex-direction: column;
    gap: 8px;
    padding-right: 5px;
}

.date-notes-list::-webkit-scrollbar {
    width: 5px;
}

.date-notes-list::-webkit-scrollbar-track {
    background: #f1f5f9;
    border-radius: 3px;
}

.date-notes-list::-webkit-scrollbar-thumb {
    background: #64748b;
    border-radius: 3px;
}

.date-notes-list::-webkit-scrollbar-thumb:hover {
    background: #3b82f6;
}

.no-notes-for-date {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    min-height: 150px;
}

.empty-state {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 16px;
    text-align: center;
    color: #64748b;
    padding: 40px 24px;
    background: #f1f5f9;
    border-radius: 12px;
    max-width: 320px;
    width: 100%;
    transition: all 0.25s ease;
    border: 1px solid #e2e8f0;
}

.empty-state:hover {
    border-color: #3b82f6;
}

.empty-state svg {
    opacity: 0.6;
    transition: all 0.25s ease;
}

.empty-state:hover svg {
    opacity: 0.8;
    transform: scale(1.1) rotate(5deg);
}

.empty-state p {
    margin: 0;
    font-size: 14px;
    font-weight: 500;
    line-height: 1.6;
}

@media (max-width: 768px) {
    .calendar-header {
        padding: 12px 15px;
    }

    .header-title {
        font-size: 15px;
        min-width: 140px;
    }

    .view-toggle {
        padding: 8px 15px;
    }

    .view-btn {
        padding: 4px 10px;
        font-size: 11px;
    }

    .selected-date-notes {
        padding: 12px 15px;
        max-height: 250px;
    }

    .date-notes-header h4 {
        font-size: 14px;
    }

    .calendar-body :deep(.fc-daygrid-day-number) {
        font-size: 11px;
        min-width: 24px;
        padding: 4px;
    }

    .event-title {
        font-size: 9px;
    }

    .empty-state {
        padding: 24px 16px;
    }

    .empty-state svg {
        width: 40px;
        height: 40px;
    }
}

@media (prefers-color-scheme: dark) {
    .calendar-view {
        background: #1e293b;
    }

    .calendar-header {
        background: #3b82f6;
    }

    .view-toggle {
        background: #1e293b;
        border-bottom: 1px solid #475569;
    }

    .view-btn {
        background: transparent;
        border: 1px solid #475569;
        color: #94a3b8;
    }

    .view-btn:hover {
        background: #334155;
        color: #f8fafc;
        border-color: #3b82f6;
    }

    .view-btn.active {
        background: #3b82f6;
        color: white;
        border-color: #3b82f6;
    }

    .calendar-body {
        background: #1e293b;
    }

    .calendar-body :deep(.fc) {
        height: 100% !important;
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    }

    .calendar-body :deep(.fc-theme-standard) {
        background: #1e293b !important;
    }

    .calendar-body :deep(.fc-theme-standard .fc-scrollgrid) {
        border: none !important;
        background: #1e293b !important;
    }

    .calendar-body :deep(.fc-theme-standard td),
    .calendar-body :deep(.fc-theme-standard th) {
        border-color: #475569 !important;
    }

    .calendar-body :deep(.fc-daygrid-day) {
        background: #1e293b !important;
    }

    .calendar-body :deep(.fc-daygrid-day:hover) {
        background: #334155 !important;
    }

    .calendar-body :deep(.fc-daygrid-day-number) {
        color: #f8fafc;
    }

    .calendar-body :deep(.fc-day-today .fc-daygrid-day-number) {
        background: #3b82f6;
        color: white;
    }

    .calendar-body :deep(.fc-col-header-cell) {
        background: #334155 !important;
        border-bottom: 1px solid #475569 !important;
    }

    .calendar-body :deep(.fc-col-header-cell-cushion) {
        color: #94a3b8;
    }

    .calendar-body :deep(.fc-scrollgrid tbody tr) {
        border-bottom: 1px solid #475569;
    }

    .calendar-body :deep(.fc-scrollgrid tbody tr td) {
        border-right: 1px solid #475569;
    }

    .calendar-body :deep(.fc-list-event) {
        background: #334155 !important;
        border: 1px solid #475569 !important;
    }

    .calendar-body :deep(.fc-list-event:hover) {
        border-color: #3b82f6 !important;
    }

    .calendar-body :deep(.fc-list-event-title) {
        color: #f8fafc;
    }

    .calendar-body :deep(.fc-list-event-time) {
        color: #94a3b8;
    }

    .calendar-body :deep(.fc-list-day-text) {
        color: #60a5fa;
    }

    .calendar-body :deep(.fc-daygrid-more-link) {
        background: #334155 !important;
        color: #60a5fa !important;
    }

    .calendar-body :deep(.fc-daygrid-more-link:hover) {
        background: #3b82f6 !important;
        color: white !important;
    }

    .event-title {
        color: #f8fafc;
    }

    .selected-date-notes {
        background: #1e293b;
    }

    .date-notes-header {
        border-bottom: 1px solid #475569;
    }

    .date-notes-header h4 {
        color: #f8fafc;
    }

    .date-notes-list::-webkit-scrollbar-track {
        background: #334155;
    }

    .date-notes-list::-webkit-scrollbar-thumb {
        background: #94a3b8;
    }

    .date-notes-list::-webkit-scrollbar-thumb:hover {
        background: #3b82f6;
    }

    .empty-state {
        background: #334155;
        color: #94a3b8;
        border: 1px solid #475569;
    }

    .empty-state:hover {
        border-color: #3b82f6;
    }

    .empty-state svg {
        color: #60a5fa;
    }
}
</style>
