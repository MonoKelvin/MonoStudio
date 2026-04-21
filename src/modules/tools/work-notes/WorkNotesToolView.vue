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
            <button
              class="view-btn"
              :class="{ active: currentView === 'calendar' }"
              @click="currentView = 'calendar'"
              title="日历视图"
            >
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <rect x="3" y="4" width="18" height="18" rx="2" ry="2"></rect>
                <line x1="16" y1="2" x2="16" y2="6"></line>
                <line x1="8" y1="2" x2="8" y2="6"></line>
                <line x1="3" y1="10" x2="21" y2="10"></line>
              </svg>
            </button>
            <button
              class="view-btn"
              :class="{ active: currentView === 'list' }"
              @click="currentView = 'list'"
              title="列表视图"
            >
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <line x1="8" y1="6" x2="21" y2="6"></line>
                <line x1="8" y1="12" x2="21" y2="12"></line>
                <line x1="8" y1="18" x2="21" y2="18"></line>
                <line x1="3" y1="6" x2="3.01" y2="6"></line>
                <line x1="3" y1="12" x2="3.01" y2="12"></line>
                <line x1="3" y1="18" x2="3.01" y2="18"></line>
              </svg>
            </button>
          </div>
          <BaseButton @click="openEditor(null)" variant="primary">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <line x1="12" y1="5" x2="12" y2="19"></line>
              <line x1="5" y1="12" x2="19" y2="12"></line>
            </svg>
            新建笔记
          </BaseButton>
        </div>
      </div>

      <div class="search-filter-bar">
        <div class="search-box">
          <svg class="search-icon" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <circle cx="11" cy="11" r="8"></circle>
            <line x1="21" y1="21" x2="16.65" y2="16.65"></line>
          </svg>
          <input
            v-model="searchQuery"
            type="text"
            class="search-input"
            placeholder="搜索笔记..."
          />
          <button v-if="searchQuery" class="clear-btn" @click="searchQuery = ''">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <line x1="18" y1="6" x2="6" y2="18"></line>
              <line x1="6" y1="6" x2="18" y2="18"></line>
            </svg>
          </button>
        </div>
        <div class="filter-tags" v-if="allTags.length > 0">
          <button
            v-for="tag in allTags"
            :key="tag"
            class="filter-tag"
            :class="{ active: selectedTags.includes(tag) }"
            @click="toggleTag(tag)"
          >
            {{ tag }}
          </button>
        </div>
      </div>

      <div class="content-area">
        <div v-if="currentView === 'calendar'" class="calendar-view">
          <div class="calendar-header">
            <button class="nav-btn" @click="prevMonth">
              <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <polyline points="15 18 9 12 15 6"></polyline>
              </svg>
            </button>
            <h3 class="current-month">{{ currentMonthYear }}</h3>
            <button class="nav-btn" @click="nextMonth">
              <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <polyline points="9 18 15 12 9 6"></polyline>
              </svg>
            </button>
          </div>
          <div class="calendar-grid">
            <div class="weekday" v-for="day in weekdays" :key="day">{{ day }}</div>
            <div
              v-for="(day, index) in calendarDays"
              :key="index"
              class="calendar-day"
              :class="{
                'other-month': !day.isCurrentMonth,
                'today': day.isToday,
                'has-notes': day.notes.length > 0,
                'selected': selectedDate === day.dateString
              }"
              @click="selectDate(day)"
            >
              <span class="day-number">{{ day.dayNumber }}</span>
              <div class="day-notes" v-if="day.notes.length > 0">
                <div
                  v-for="note in day.notes.slice(0, 3)"
                  :key="note.id"
                  class="day-note-dot"
                  :class="{ 'important': note.important, 'has-tag': note.tag }"
                ></div>
                <span v-if="day.notes.length > 3" class="more-indicator">+{{ day.notes.length - 3 }}</span>
              </div>
            </div>
          </div>
          <div class="selected-date-notes" v-if="selectedDate">
            <div class="date-notes-header">
              <h4>{{ formatSelectedDate }}</h4>
              <span class="notes-badge">{{ getNotesForDate(selectedDate).length }}</span>
            </div>
            <div class="date-notes-list" v-if="getNotesForDate(selectedDate).length > 0">
              <div
                v-for="note in getNotesForDate(selectedDate)"
                :key="note.id"
                class="note-card"
                :class="{ 'important': note.important }"
                @click="openDetail(note)"
              >
                <div class="note-card-header">
                  <div class="note-card-title-row">
                    <span v-if="note.important" class="important-star">
                      <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor" stroke-width="2">
                        <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
                      </svg>
                    </span>
                    <span class="note-title">{{ note.title || '无标题' }}</span>
                  </div>
                  <span v-if="note.tag" class="note-tag-badge">{{ note.tag }}</span>
                </div>
                <p class="note-card-content">{{ note.content.substring(0, 100) }}{{ note.content.length > 100 ? '...' : '' }}</p>
                <div class="note-card-footer">
                  <span class="note-time">{{ formatNoteTime(note.createdAt) }}</span>
                  <div class="note-card-actions">
                    <button class="action-btn" @click.stop="openEditor(note)" title="编辑">
                      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
                        <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
                      </svg>
                    </button>
                    <button class="action-btn danger" @click.stop="confirmDelete(note)" title="删除">
                      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <polyline points="3 6 5 6 21 6"></polyline>
                        <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path>
                      </svg>
                    </button>
                  </div>
                </div>
              </div>
            </div>
            <div class="no-notes-for-date" v-else>
              <p>这一天没有笔记</p>
              <BaseButton size="sm" @click="openEditorForDate(selectedDate)">添加笔记</BaseButton>
            </div>
          </div>
        </div>

        <div v-else class="list-view">
          <div class="notes-list" v-if="filteredNotes.length > 0">
            <div
              v-for="note in filteredNotes"
              :key="note.id"
              class="note-list-item"
              :class="{ 'important': note.important }"
              @click="openDetail(note)"
            >
              <div class="note-list-indicator" :class="{ 'important': note.important }"></div>
              <div class="note-list-content">
                <div class="note-list-header">
                  <div class="note-list-title-row">
                    <span v-if="note.important" class="important-star">
                      <svg width="12" height="12" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor" stroke-width="2">
                        <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
                      </svg>
                    </span>
                    <span class="note-list-title">{{ note.title || '无标题' }}</span>
                    <span v-if="note.tag" class="note-list-tag">{{ note.tag }}</span>
                  </div>
                  <span class="note-list-date">{{ formatDateHeader(note.createdAt) }}</span>
                </div>
                <p class="note-list-text">{{ note.content.substring(0, 150) }}{{ note.content.length > 150 ? '...' : '' }}</p>
              </div>
            </div>
          </div>
          <div class="empty-state" v-else>
            <div class="empty-icon">
              <svg width="64" height="64" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
                <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path>
                <polyline points="14 2 14 8 20 8"></polyline>
                <line x1="16" y1="13" x2="8" y2="13"></line>
                <line x1="16" y1="17" x2="8" y2="17"></line>
                <polyline points="10 9 9 9 8 9"></polyline>
              </svg>
            </div>
            <h3>{{ searchQuery || selectedTags.length > 0 ? '没有找到匹配的笔记' : '还没有任何笔记' }}</h3>
            <p>{{ searchQuery || selectedTags.length > 0 ? '尝试调整搜索条件' : '点击"新建笔记"开始记录' }}</p>
          </div>
        </div>
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
    </div>

    <div class="modal-overlay" v-if="showEditor" @click="closeEditor">
      <div class="editor-modal" @click.stop>
        <div class="editor-header">
          <h3>{{ editingNote ? '编辑笔记' : '新建笔记' }}</h3>
          <button class="close-btn" @click="closeEditor">
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <line x1="18" y1="6" x2="6" y2="18"></line>
              <line x1="6" y1="6" x2="18" y2="18"></line>
            </svg>
          </button>
        </div>
        <div class="editor-body">
          <div class="editor-meta">
            <input
              v-model="editorDate"
              type="date"
              class="date-input"
            />
            <input
              v-model="editorTag"
              type="text"
              class="tag-input"
              placeholder="添加标签"
              list="tag-suggestions"
            />
            <datalist id="tag-suggestions">
              <option v-for="tag in allTags" :key="tag" :value="tag"></option>
            </datalist>
          </div>
          <input
            v-model="editorTitle"
            type="text"
            class="title-input"
            placeholder="标题（可选）"
          />
          <textarea
            v-model="editorContent"
            class="content-textarea"
            placeholder="开始记录..."
          ></textarea>
        </div>
        <div class="editor-footer">
          <div class="editor-options">
            <label class="important-toggle" :class="{ active: editorImportant }">
              <input type="checkbox" v-model="editorImportant" />
              <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor" stroke-width="2">
                <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
              </svg>
              标记重要
            </label>
          </div>
          <div class="editor-actions">
            <BaseButton @click="closeEditor">取消</BaseButton>
            <BaseButton variant="primary" @click="saveNote" :disabled="!editorContent.trim()">保存</BaseButton>
          </div>
        </div>
      </div>
    </div>

    <div class="modal-overlay" v-if="showDetail" @click="closeDetail">
      <div class="detail-modal" @click.stop>
        <div class="detail-header">
          <div class="detail-meta">
            <span class="detail-date">{{ formatDetailDate }}</span>
            <span v-if="detailNote?.tag" class="detail-tag">{{ detailNote.tag }}</span>
            <span v-if="detailNote?.important" class="detail-important">
              <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor" stroke="currentColor" stroke-width="2">
                <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2"></polygon>
              </svg>
              重要
            </span>
          </div>
          <button class="close-btn" @click="closeDetail">
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
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
            <span v-if="detailNote?.updatedAt !== detailNote?.createdAt">· 更新于 {{ formatDetailUpdatedAt }}</span>
          </div>
          <div class="detail-actions">
            <BaseButton @click="openEditor(detailNote)">编辑</BaseButton>
            <BaseButton variant="danger" @click="confirmDelete(detailNote)">删除</BaseButton>
          </div>
        </div>
      </div>
    </div>

    <div class="modal-overlay" v-if="showDeleteConfirm" @click="cancelDelete">
      <div class="confirm-modal" @click.stop>
        <div class="confirm-icon">
          <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
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
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'WorkNotesToolView',
  components: {
    BaseButton
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
      showEditor: false,
      showDetail: false,
      showDeleteConfirm: false,
      editingNote: null,
      detailNote: null,
      deleteTargetNote: null,
      editorTitle: '',
      editorContent: '',
      editorTag: '',
      editorDate: '',
      editorImportant: false,
      weekdays: ['日', '一', '二', '三', '四', '五', '六']
    };
  },
  computed: {
    allTags() {
      const tags = new Set();
      this.notes.forEach(note => {
        if (note.tag) {
          tags.add(note.tag);
        }
      });
      return Array.from(tags).sort();
    },
    filteredNotes() {
      let result = [...this.notes];

      if (this.searchQuery) {
        const query = this.searchQuery.toLowerCase();
        result = result.filter(note => {
          return (note.title && note.title.toLowerCase().includes(query)) ||
                 note.content.toLowerCase().includes(query) ||
                 (note.tag && note.tag.toLowerCase().includes(query));
        });
      }

      if (this.selectedTags.length > 0) {
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
    currentMonthYear() {
      const year = this.currentDate.getFullYear();
      const month = this.currentDate.getMonth() + 1;
      return `${year}年${month}月`;
    },
    calendarDays() {
      const year = this.currentDate.getFullYear();
      const month = this.currentDate.getMonth();

      const firstDay = new Date(year, month, 1);
      const lastDay = new Date(year, month + 1, 0);

      const days = [];
      const today = new Date().toISOString().split('T')[0];

      const startPadding = firstDay.getDay();
      for (let i = startPadding - 1; i >= 0; i--) {
        const date = new Date(year, month, -i);
        const dateString = date.toISOString().split('T')[0];
        days.push({
          dayNumber: date.getDate(),
          dateString,
          isCurrentMonth: false,
          isToday: dateString === today,
          notes: this.getNotesForDate(dateString)
        });
      }

      for (let day = 1; day <= lastDay.getDate(); day++) {
        const date = new Date(year, month, day);
        const dateString = date.toISOString().split('T')[0];
        days.push({
          dayNumber: day,
          dateString,
          isCurrentMonth: true,
          isToday: dateString === today,
          notes: this.getNotesForDate(dateString)
        });
      }

      const endPadding = 42 - days.length;
      for (let i = 1; i <= endPadding; i++) {
        const date = new Date(year, month + 1, i);
        const dateString = date.toISOString().split('T')[0];
        days.push({
          dayNumber: i,
          dateString,
          isCurrentMonth: false,
          isToday: dateString === today,
          notes: this.getNotesForDate(dateString)
        });
      }

      return days;
    },
    formatSelectedDate() {
      if (!this.selectedDate) return '';
      const date = new Date(this.selectedDate);
      const today = new Date();
      const yesterday = new Date(today);
      yesterday.setDate(yesterday.getDate() - 1);

      if (date.toDateString() === today.toDateString()) {
        return '今天';
      } else if (date.toDateString() === yesterday.toDateString()) {
        return '昨天';
      } else {
        return date.toLocaleDateString('zh-CN', {
          month: 'long',
          day: 'numeric',
          weekday: 'long'
        });
      }
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
    getNotesForDate(dateString) {
      return this.notes.filter(note => note.createdAt.split('T')[0] === dateString);
    },
    prevMonth() {
      const newDate = new Date(this.currentDate);
      newDate.setMonth(newDate.getMonth() - 1);
      this.currentDate = newDate;
    },
    nextMonth() {
      const newDate = new Date(this.currentDate);
      newDate.setMonth(newDate.getMonth() + 1);
      this.currentDate = newDate;
    },
    selectDate(day) {
      this.selectedDate = day.dateString;
    },
    toggleTag(tag) {
      const index = this.selectedTags.indexOf(tag);
      if (index === -1) {
        this.selectedTags.push(tag);
      } else {
        this.selectedTags.splice(index, 1);
      }
    },
    openEditor(note = null) {
      this.editingNote = note;
      if (note) {
        this.editorTitle = note.title || '';
        this.editorContent = note.content;
        this.editorTag = note.tag || '';
        this.editorDate = note.createdAt.split('T')[0];
        this.editorImportant = note.important || false;
      } else {
        this.editorTitle = '';
        this.editorContent = '';
        this.editorTag = '';
        this.editorDate = this.selectedDate || new Date().toISOString().split('T')[0];
        this.editorImportant = false;
      }
      this.showDetail = false;
      this.showEditor = true;
    },
    openEditorForDate(date) {
      this.selectedDate = date;
      this.openEditor(null);
    },
    closeEditor() {
      this.showEditor = false;
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
    saveNote() {
      if (!this.editorContent.trim()) return;

      const now = new Date().toISOString();

      if (this.editingNote) {
        const index = this.notes.findIndex(n => n.id === this.editingNote.id);
        if (index !== -1) {
          this.notes[index].title = this.editorTitle;
          this.notes[index].content = this.editorContent;
          this.notes[index].tag = this.editorTag;
          this.notes[index].createdAt = this.editorDate ? `${this.editorDate}T${new Date().toLocaleTimeString('en-US', { hour12: false })}` : now;
          this.notes[index].important = this.editorImportant;
          this.notes[index].updatedAt = now;
        }
      } else {
        this.notes.push({
          id: Date.now().toString(),
          title: this.editorTitle,
          content: this.editorContent,
          tag: this.editorTag,
          createdAt: this.editorDate ? `${this.editorDate}T${new Date().toLocaleTimeString('en-US', { hour12: false })}` : now,
          updatedAt: now,
          important: this.editorImportant
        });
      }

      this.saveNotes();
      this.closeEditor();
    },
    confirmDelete(note) {
      this.deleteTargetNote = note;
      this.showDeleteConfirm = true;
      this.showDetail = false;
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
    },
    formatDateHeader(dateString) {
      const date = new Date(dateString);
      const today = new Date();
      const yesterday = new Date(today);
      yesterday.setDate(yesterday.getDate() - 1);

      if (date.toDateString() === today.toDateString()) {
        return '今天 ' + date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
      } else if (date.toDateString() === yesterday.toDateString()) {
        return '昨天 ' + date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
      } else {
        return date.toLocaleDateString('zh-CN', { month: '2-digit', day: '2-digit' }) + ' ' +
               date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
      }
    },
    formatNoteTime(dateString) {
      const date = new Date(dateString);
      return date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
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
  padding: 20px;
  gap: 16px;
}

.work-notes-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.header-left {
  display: flex;
  align-items: baseline;
  gap: 12px;
}

.header-left h2 {
  margin: 0;
  font-size: 20px;
  font-weight: 600;
  color: var(--text-primary);
}

.notes-count {
  font-size: 13px;
  color: var(--text-secondary);
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 12px;
}

.view-toggle {
  display: flex;
  background: var(--bg-secondary);
  border-radius: 6px;
  padding: 2px;
}

.view-btn {
  padding: 6px 10px;
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
}

.view-btn.active {
  background: var(--bg-primary);
  color: var(--accent-color);
  box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1);
}

.search-filter-bar {
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.search-box {
  position: relative;
  display: flex;
  align-items: center;
}

.search-icon {
  position: absolute;
  left: 12px;
  color: var(--text-tertiary);
}

.search-input {
  width: 100%;
  padding: 10px 36px;
  border: 1px solid var(--border-color);
  border-radius: 8px;
  font-size: 14px;
  background: var(--bg-secondary);
  color: var(--text-primary);
  transition: all 0.2s ease;
}

.search-input:focus {
  outline: none;
  border-color: var(--accent-color);
  background: var(--bg-primary);
}

.search-input::placeholder {
  color: var(--text-tertiary);
}

.clear-btn {
  position: absolute;
  right: 10px;
  padding: 4px;
  border: none;
  background: transparent;
  cursor: pointer;
  color: var(--text-tertiary);
  display: flex;
  align-items: center;
  justify-content: center;
}

.clear-btn:hover {
  color: var(--text-secondary);
}

.filter-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}

.filter-tag {
  padding: 4px 10px;
  font-size: 12px;
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
}

.filter-tag.active {
  background: var(--accent-color);
  border-color: var(--accent-color);
  color: white;
}

.content-area {
  flex: 1;
  overflow: hidden;
}

.calendar-view {
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.calendar-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.current-month {
  margin: 0;
  font-size: 16px;
  font-weight: 600;
  color: var(--text-primary);
}

.nav-btn {
  padding: 6px;
  border: none;
  background: var(--bg-secondary);
  border-radius: 6px;
  cursor: pointer;
  color: var(--text-secondary);
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.2s ease;
}

.nav-btn:hover {
  background: var(--bg-tertiary);
  color: var(--text-primary);
}

.calendar-grid {
  display: grid;
  grid-template-columns: repeat(7, 1fr);
  gap: 4px;
  background: var(--bg-secondary);
  border-radius: 8px;
  padding: 8px;
}

.weekday {
  text-align: center;
  font-size: 12px;
  font-weight: 500;
  color: var(--text-tertiary);
  padding: 6px 0;
}

.calendar-day {
  aspect-ratio: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 2px;
  border-radius: 6px;
  cursor: pointer;
  transition: all 0.2s ease;
  padding: 4px;
  min-height: 40px;
}

.calendar-day:hover {
  background: var(--bg-tertiary);
}

.calendar-day.other-month {
  opacity: 0.4;
}

.calendar-day.today {
  background: var(--accent-color);
  color: white;
}

.calendar-day.today .day-number {
  font-weight: 600;
}

.calendar-day.selected {
  background: var(--accent-color-light);
  border: 2px solid var(--accent-color);
}

.day-number {
  font-size: 13px;
  color: var(--text-primary);
}

.day-notes {
  display: flex;
  gap: 2px;
  align-items: center;
}

.day-note-dot {
  width: 4px;
  height: 4px;
  border-radius: 50%;
  background: var(--accent-color);
}

.day-note-dot.important {
  background: var(--warning-color, #f59e0b);
}

.day-note-dot.has-tag {
  background: var(--success-color, #10b981);
}

.more-indicator {
  font-size: 9px;
  color: var(--text-tertiary);
}

.selected-date-notes {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 10px;
  overflow: hidden;
}

.date-notes-header {
  display: flex;
  align-items: center;
  gap: 10px;
}

.date-notes-header h4 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: var(--text-primary);
}

.notes-badge {
  font-size: 11px;
  padding: 2px 8px;
  background: var(--accent-color);
  color: white;
  border-radius: 10px;
}

.date-notes-list {
  flex: 1;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.note-card {
  padding: 12px;
  background: var(--bg-secondary);
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s ease;
  border: 1px solid transparent;
}

.note-card:hover {
  border-color: var(--accent-color);
  transform: translateY(-1px);
}

.note-card.important {
  border-left: 3px solid var(--warning-color, #f59e0b);
}

.note-card-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 6px;
}

.note-card-title-row {
  display: flex;
  align-items: center;
  gap: 6px;
  flex: 1;
}

.important-star {
  color: var(--warning-color, #f59e0b);
  display: flex;
}

.note-title {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
}

.note-tag-badge {
  font-size: 11px;
  padding: 2px 6px;
  background: var(--success-color-light);
  color: var(--success-color);
  border-radius: 4px;
}

.note-card-content {
  margin: 0;
  font-size: 13px;
  line-height: 1.4;
  color: var(--text-secondary);
}

.note-card-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-top: 8px;
}

.note-time {
  font-size: 12px;
  color: var(--text-tertiary);
}

.note-card-actions {
  display: flex;
  gap: 4px;
}

.action-btn {
  padding: 4px;
  border: none;
  background: transparent;
  cursor: pointer;
  color: var(--text-tertiary);
  border-radius: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.2s ease;
}

.action-btn:hover {
  background: var(--bg-tertiary);
  color: var(--text-primary);
}

.action-btn.danger:hover {
  background: var(--danger-color-light);
  color: var(--danger-color);
}

.no-notes-for-date {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 12px;
  color: var(--text-tertiary);
}

.list-view {
  height: 100%;
  overflow-y: auto;
}

.notes-list {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.note-list-item {
  display: flex;
  gap: 12px;
  padding: 14px;
  background: var(--bg-secondary);
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s ease;
}

.note-list-item:hover {
  background: var(--bg-tertiary);
  transform: translateX(2px);
}

.note-list-indicator {
  width: 3px;
  border-radius: 2px;
  background: var(--accent-color);
  flex-shrink: 0;
}

.note-list-indicator.important {
  background: var(--warning-color, #f59e0b);
}

.note-list-content {
  flex: 1;
  min-width: 0;
}

.note-list-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 6px;
}

.note-list-title-row {
  display: flex;
  align-items: center;
  gap: 6px;
  flex: 1;
  min-width: 0;
}

.note-list-title {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.note-list-tag {
  font-size: 10px;
  padding: 2px 6px;
  background: var(--success-color-light);
  color: var(--success-color);
  border-radius: 4px;
  flex-shrink: 0;
}

.note-list-date {
  font-size: 12px;
  color: var(--text-tertiary);
  flex-shrink: 0;
}

.note-list-text {
  margin: 0;
  font-size: 13px;
  line-height: 1.4;
  color: var(--text-secondary);
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
  overflow: hidden;
}

.empty-state {
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 12px;
  color: var(--text-tertiary);
  text-align: center;
}

.empty-icon {
  opacity: 0.5;
}

.empty-state h3 {
  margin: 0;
  font-size: 16px;
  font-weight: 500;
  color: var(--text-secondary);
}

.empty-state p {
  margin: 0;
  font-size: 13px;
}

.quick-stats {
  display: flex;
  gap: 16px;
  padding-top: 12px;
  border-top: 1px solid var(--border-color);
}

.stat-item {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.stat-value {
  font-size: 18px;
  font-weight: 600;
  color: var(--text-primary);
}

.stat-label {
  font-size: 11px;
  color: var(--text-tertiary);
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
}

.editor-modal,
.detail-modal {
  background: var(--bg-primary);
  border-radius: 12px;
  width: 100%;
  max-width: 560px;
  max-height: 80vh;
  display: flex;
  flex-direction: column;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
}

.editor-header,
.detail-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 20px;
  border-bottom: 1px solid var(--border-color);
}

.editor-header h3,
.detail-header h3 {
  margin: 0;
  font-size: 16px;
  font-weight: 600;
  color: var(--text-primary);
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
}

.editor-body,
.detail-body {
  flex: 1;
  padding: 20px;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.editor-meta {
  display: flex;
  gap: 10px;
}

.date-input,
.tag-input {
  padding: 8px 12px;
  border: 1px solid var(--border-color);
  border-radius: 6px;
  font-size: 13px;
  background: var(--bg-secondary);
  color: var(--text-primary);
}

.date-input:focus,
.tag-input:focus,
.title-input:focus,
.content-textarea:focus {
  outline: none;
  border-color: var(--accent-color);
}

.tag-input {
  flex: 1;
}

.title-input {
  padding: 10px 12px;
  border: 1px solid var(--border-color);
  border-radius: 6px;
  font-size: 16px;
  font-weight: 500;
  background: var(--bg-secondary);
  color: var(--text-primary);
}

.content-textarea {
  flex: 1;
  min-height: 200px;
  padding: 12px;
  border: 1px solid var(--border-color);
  border-radius: 6px;
  font-size: 14px;
  line-height: 1.6;
  background: var(--bg-secondary);
  color: var(--text-primary);
  resize: none;
  font-family: inherit;
}

.editor-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 20px;
  border-top: 1px solid var(--border-color);
  gap: 12px;
}

.editor-options {
  display: flex;
  align-items: center;
}

.important-toggle {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 13px;
  color: var(--text-secondary);
  cursor: pointer;
  padding: 6px 10px;
  border-radius: 6px;
  transition: all 0.2s ease;
}

.important-toggle input {
  display: none;
}

.important-toggle:hover {
  background: var(--bg-secondary);
}

.important-toggle.active {
  color: var(--warning-color, #f59e0b);
  background: rgba(245, 158, 11, 0.1);
}

.editor-actions {
  display: flex;
  gap: 8px;
}

.detail-header {
  justify-content: space-between;
}

.detail-meta {
  display: flex;
  align-items: center;
  gap: 10px;
}

.detail-date {
  font-size: 13px;
  color: var(--text-secondary);
}

.detail-tag {
  font-size: 12px;
  padding: 3px 8px;
  background: var(--success-color-light);
  color: var(--success-color);
  border-radius: 4px;
}

.detail-important {
  display: flex;
  align-items: center;
  gap: 4px;
  font-size: 12px;
  color: var(--warning-color, #f59e0b);
}

.detail-title {
  margin: 0 0 12px 0;
  font-size: 20px;
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
  font-size: 12px;
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
}

.confirm-icon {
  color: var(--danger-color);
  margin-bottom: 12px;
}

.confirm-modal h3 {
  margin: 0 0 8px 0;
  font-size: 18px;
  font-weight: 600;
  color: var(--text-primary);
}

.confirm-modal p {
  margin: 0 0 20px 0;
  font-size: 14px;
  color: var(--text-secondary);
}

.confirm-actions {
  display: flex;
  gap: 10px;
  justify-content: center;
}
</style>
