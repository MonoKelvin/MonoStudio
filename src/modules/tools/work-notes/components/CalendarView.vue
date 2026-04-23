<template>
    <div class="calendar-view" ref="calendarContainer">
        <FullCalendar :options="calendarOptions" ref="fullCalendar" />
    </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted, watch, nextTick } from 'vue';
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
const calendarContainer = ref(null);
const calendarHeight = ref('auto');

const calendarOptions = computed(() => {
    return {
        plugins: [dayGridPlugin, timeGridPlugin, listPlugin, interactionPlugin],
        initialView: 'dayGridMonth',
        locale: zhCnLocale,
        headerToolbar: {
            left: 'prev,next',
            center: 'title today',
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
        height: calendarHeight.value,
        // 自定义日期格式，只显示数字
        dayCellContent: function(info) {
            return {
                html: `<span class="fc-daygrid-day-number">${info.date.getDate()}</span>`
            };
        }
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
    // 视图变化时重新计算高度
    nextTick(() => {
        calculateCalendarHeight();
    });
};

const calculateCalendarHeight = () => {
    if (!calendarContainer.value) return;
    
    const container = calendarContainer.value;
    const containerRect = container.getBoundingClientRect();
    
    // 计算可用高度：容器高度 - 工具栏高度 - 底部间距
    const toolbar = container.querySelector('.fc-toolbar');
    const toolbarHeight = toolbar ? toolbar.offsetHeight : 0;
    const padding = 16; // 上下内边距
    const bottomSpace = 8; // 底部8px间距
    
    // 确保有足够的高度显示完整的一个月
    // 考虑到一个月最多可能有6行日期，每行高度80px
    const minRequiredHeight = 6 * 80 + 150; // 6行 * 每行80px + 150px头部和间距
    const availableHeight = Math.max(containerRect.height - toolbarHeight - padding - bottomSpace, minRequiredHeight);
    
    calendarHeight.value = `${availableHeight}px`;
};

// 监听窗口大小变化
const handleResize = () => {
    nextTick(() => {
        calculateCalendarHeight();
    });
};

onMounted(() => {
    nextTick(() => {
        calculateCalendarHeight();
    });
    window.addEventListener('resize', handleResize);
});

// 组件卸载时清理事件监听
onUnmounted(() => {
    window.removeEventListener('resize', handleResize);
});</script>

<style scoped>
.calendar-view {
    height: 100%;
    min-height: 600px;
    display: flex;
    flex-direction: column;
    position: relative;
    overflow: hidden;
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
    overflow: hidden;
    height: 100%;
}

.calendar-view :deep(.fc-toolbar) {
    flex-wrap: nowrap;
    justify-content: space-between;
    align-items: center;
    gap: 12px;
    margin-bottom: 10px;
    overflow: hidden;
}

.calendar-view :deep(.fc-toolbar-title) {
    font-size: 16px !important;
    font-weight: 600 !important;
    color: var(--text-primary) !important;
    letter-spacing: -0.3px;
    margin: 0 !important;
    flex-shrink: 0;
}

.calendar-view :deep(.fc-toolbar-chunk) {
    display: flex;
    align-items: center;
    gap: 8px;
}

.calendar-view :deep(.fc-button) {
    background: var(--bg-elevated) !important;
    border: 1px solid var(--border-color) !important;
    color: var(--text-primary) !important;
    font-size: 12px !important;
    font-weight: 500 !important;
    padding: 6px 10px !important;
    border-radius: 6px !important;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.04) !important;
    transition: all 0.2s ease !important;
}

.calendar-view :deep(.fc-button.fc-today-button) {
    background: var(--accent-color) !important;
    border-color: var(--accent-color) !important;
    color: white !important;
    margin: 0 4px;
}

.calendar-view :deep(.fc-button.fc-today-button:hover) {
    background: color-mix(in srgb, var(--accent-color) 90%, white) !important;
    border-color: var(--accent-color) !important;
}

.calendar-view :deep(.fc-button:hover) {
    background: var(--bg-hover) !important;
    border-color: var(--border-color) !important;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.08) !important;
}

.calendar-view :deep(.fc-button-primary:not(:disabled).fc-button-active),
.calendar-view :deep(.fc-button-primary:not(:disabled):active) {
    background: var(--accent-color) !important;
    border-color: var(--accent-color) !important;
    color: white !important;
}

.calendar-view :deep(.fc-button-group) {
    display: flex;
    border-radius: 6px;
    background: transparent !important;
    border: none !important;
    overflow: hidden;
}

.calendar-view :deep(.fc-button-group .fc-button) {
    border-radius: 0 !important;
    border: 1px solid var(--border-color) !important;
    margin: 0 !important;
}

.calendar-view :deep(.fc-button-group .fc-button:first-child) {
    border-radius: 6px 0 0 6px !important;
    border-right: none !important;
}

.calendar-view :deep(.fc-button-group .fc-button:not(:first-child):not(:last-child)) {
    border-radius: 0 !important;
    border-left: none !important;
    border-right: none !important;
}

.calendar-view :deep(.fc-button-group .fc-button:last-child) {
    border-radius: 0 6px 6px 0 !important;
    border-left: none !important;
}

.calendar-view :deep(.fc-scrollgrid) {
    border: 1px solid var(--border-color) !important;
    border-radius: 12px !important;
    overflow: hidden !important;
    width: 100% !important;
    max-width: 100% !important;
    height: calc(100% - 60px) !important;
}

.calendar-view :deep(.fc-scrollgrid-section-header) {
    border-top-left-radius: 12px !important;
    border-top-right-radius: 12px !important;
    overflow: hidden !important;
    width: 100% !important;
}

.calendar-view :deep(.fc-scrollgrid-section-body) {
    width: 100% !important;
    overflow: hidden !important;
    height: calc(100% - 40px) !important;
}

.calendar-view :deep(.fc-scrollgrid-section-footer) {
    border-bottom-left-radius: 12px !important;
    border-bottom-right-radius: 12px !important;
    overflow: hidden !important;
    width: 100% !important;
}

.calendar-view :deep(.fc-scrollgrid-section-header td) {
    border-bottom: 1px solid var(--border-color) !important;
    text-align: center !important;
    padding: 0 !important;
}

.calendar-view :deep(.fc-col-header-cell) {
    background: var(--bg-soft) !important;
    padding: 0 !important;
    text-align: center !important;
    width: calc(100% / 7) !important;
}

.calendar-view :deep(.fc-col-header-cell-cushion) {
    font-size: 11px;
    font-weight: 600;
    color: var(--text-secondary);
    text-transform: uppercase;
    letter-spacing: 0.5px;
    padding: 12px 4px !important;
    display: block;
    text-align: center !important;
    margin: 0 auto !important;
    width: 100% !important;
    box-sizing: border-box !important;
}

.calendar-view :deep(.fc-daygrid-day) {
    min-height: 80px !important;
    padding: 10px 6px !important;
    box-sizing: border-box !important;
    text-align: center !important;
    white-space: normal !important;
    overflow: visible !important;
    width: calc(100% / 7) !important;
}

.calendar-view :deep(.fc-daygrid-day:hover) {
    background: var(--bg-soft);
}

/* 基础日期数字样式 */
.calendar-view :deep(.fc-daygrid-day-number) {
    font-size: 12px !important;
    font-weight: 500 !important;
    color: var(--text-primary);
    text-align: center !important;
    text-decoration: none !important;
    transition: all 0.15s ease;
    width: 24px;
    height: 24px;
    line-height: 24px;
    display: inline-flex !important;
    align-items: center !important;
    justify-content: center !important;
    margin: 0 auto 8px !important;
    white-space: nowrap !important;
    text-overflow: ellipsis !important;
    z-index: 1 !important;
    position: relative !important;
    background: transparent !important;
    border-radius: 0 !important;
}

/* 移除日期中的"日"字 */
.calendar-view :deep(.fc-daygrid-day-number)::after {
    content: '' !important;
}

/* 确保日期数字和事件内容不重叠 */
.calendar-view :deep(.fc-daygrid-day-top) {
    margin-bottom: 8px !important;
    padding-bottom: 4px !important;
    display: block !important;
    height: 32px !important;
    line-height: 32px !important;
}

/* 确保日历主体内容宽度正确 */
.calendar-view :deep(.fc-daygrid-body) {
    width: 100% !important;
    overflow: hidden !important;
}

/* 确保表格宽度正确 */
.calendar-view :deep(.fc-scrollgrid table) {
    width: 100% !important;
    table-layout: fixed !important;
}

/* 日期数字 hover 效果 */
.calendar-view :deep(.fc-daygrid-day:hover .fc-daygrid-day-number) {
    background: var(--bg-hover);
    border-radius: 50%;
}

/* 今天日期的高亮效果 */
.calendar-view :deep(.fc-day-today .fc-daygrid-day-number) {
    background: var(--accent-color) !important;
    color: white !important;
    font-weight: 600 !important;
    border-radius: 50% !important;
    box-shadow: 0 0 0 2px rgba(0, 0, 0, 0.1) !important;
}

/* 选中日期的效果 */
.calendar-view :deep(.fc-daygrid-day.fc-day-selected) {
    background: var(--bg-soft) !important;
}

.calendar-view :deep(.fc-daygrid-day.fc-day-selected:hover) {
    background: var(--bg-soft) !important;
}

.calendar-view :deep(.fc-daygrid-day.fc-day-selected .fc-daygrid-day-number) {
    background: var(--accent-color);
    color: white;
    border-radius: 50%;
}

.calendar-view :deep(.fc-day-other .fc-daygrid-day-number) {
    color: var(--text-tertiary);
}

/* 事件样式 */
.calendar-view :deep(.fc-daygrid-event) {
    background: transparent !important;
    border: none !important;
    padding: 0 !important;
    margin: 4px 0 !important;
    width: 100% !important;
    box-sizing: border-box !important;
    z-index: 2 !important;
    position: relative !important;
}

.calendar-view :deep(.fc-daygrid-event .fc-daygrid-event-content) {
    border-radius: 4px !important;
    padding: 3px 5px !important;
    margin: 0 !important;
    width: 100% !important;
    box-sizing: border-box !important;
    white-space: nowrap !important;
    overflow: hidden !important;
    text-overflow: ellipsis !important;
    border-left: 3px solid var(--accent-color) !important;
    background: color-mix(in srgb, var(--accent-color) 10%, var(--bg-elevated)) !important;
    font-size: 10px !important;
    line-height: 1.2 !important;
    height: auto !important;
    min-height: 20px !important;
}

.calendar-view :deep(.fc-daygrid-event:hover .fc-daygrid-event-content) {
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.08) !important;
}

.calendar-view :deep(.fc-daygrid-event.event-important .fc-daygrid-event-content) {
    border-left-color: var(--warning-color) !important;
    background: color-mix(in srgb, var(--warning-color) 10%, var(--bg-elevated)) !important;
}

.calendar-view :deep(.fc-daygrid-event.event-tagged .fc-daygrid-event-content) {
    border-left-color: var(--success) !important;
    background: color-mix(in srgb, var(--success) 10%, var(--bg-elevated)) !important;
}

/* 周视图和日视图样式 */
.calendar-view :deep(.fc-timegrid) {
    background: var(--bg-elevated) !important;
    border-radius: 12px !important;
    overflow: hidden !important;
    border: 1px solid var(--border-color) !important;
    width: 100% !important;
    max-width: 100% !important;
    height: calc(100% - 60px) !important;
}

.calendar-view :deep(.fc-timegrid-axis) {
    background: var(--bg-soft) !important;
    border-right: 1px solid var(--border-color) !important;
    min-width: 60px !important;
}

.calendar-view :deep(.fc-timegrid-axis-cushion) {
    font-size: 11px;
    font-weight: 500;
    color: var(--text-secondary);
    padding: 8px !important;
    text-align: center !important;
}

/* 确保时间网格内容不溢出 */
.calendar-view :deep(.fc-timegrid-body) {
    width: 100% !important;
    overflow: hidden !important;
}

.calendar-view :deep(.fc-timegrid-cols) {
    width: calc(100% - 60px) !important;
}

.calendar-view :deep(.fc-timegrid-col) {
    width: calc(100% / 7) !important;
    padding: 0 !important;
}

/* 优化周视图和日视图的单元格 */
.calendar-view :deep(.fc-timegrid-slot) {
    border-bottom: 1px solid var(--border-color) !important;
    min-height: 40px !important;
}

.calendar-view :deep(.fc-timegrid-slot-label) {
    font-size: 10px !important;
    color: var(--text-secondary) !important;
    padding: 2px 4px !important;
}

.calendar-view :deep(.fc-timegrid-event) {
    background: var(--accent-color) !important;
    border: none !important;
    border-radius: 6px !important;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
    transition: all 0.15s ease;
    margin: 2px 0 !important;
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

/* 列表视图样式 */
.calendar-view :deep(.fc-list-table) {
    border-radius: 12px !important;
    overflow: hidden !important;
    border: 1px solid var(--border-color) !important;
    height: calc(100% - 60px) !important;
}

.calendar-view :deep(.fc-list-day-cushion) {
    background: var(--bg-soft) !important;
    color: var(--text-primary) !important;
    font-weight: 600;
    padding: 12px 16px !important;
}

.calendar-view :deep(.fc-list-event) {
    transition: background-color 0.15s ease;
    padding: 8px 0 !important;
}

.calendar-view :deep(.fc-list-event:hover) {
    background: var(--bg-soft) !important;
}

.calendar-view :deep(.fc-list-event-time) {
    color: var(--text-secondary) !important;
    font-weight: 500;
    padding: 8px 12px !important;
    font-size: 11px !important;
}

.calendar-view :deep(.fc-list-event-title) {
    color: var(--text-primary) !important;
    font-weight: 500;
    padding: 8px 12px !important;
    font-size: 11px !important;
}

/* 确保日历控件不出现滚动条 */
.calendar-view :deep(.fc-scroller) {
    overflow: hidden !important;
}

.calendar-view :deep(.fc-daygrid-body) {
    overflow: hidden !important;
}

.calendar-view :deep(.fc-timegrid-body) {
    overflow: hidden !important;
}

.calendar-view :deep(.fc-list-body) {
    overflow: hidden !important;
}
</style>
