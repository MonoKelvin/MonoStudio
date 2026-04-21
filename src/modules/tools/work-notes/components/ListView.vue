<template>
  <div class="list-view">
    <div class="notes-list" v-if="filteredNotes.length > 0">
      <div
        v-for="note in filteredNotes"
        :key="note.id"
        class="note-list-item"
        :class="{ 'important': note.important }"
        @click="$emit('open-detail', note)"
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
        <div class="note-list-actions">
          <button class="action-btn" @click.stop="$emit('edit-note', note)" title="编辑">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
              <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
            </svg>
          </button>
          <button class="action-btn danger" @click.stop="$emit('delete-note', note)" title="删除">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <polyline points="3 6 5 6 21 6"></polyline>
              <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path>
            </svg>
          </button>
        </div>
      </div>
    </div>
    <div class="empty-state" v-else>
      <div class="empty-icon">
        <svg width="48" height="48" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
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
</template>

<script>
export default {
  name: 'ListView',
  props: {
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
    }
  },
  emits: ['open-detail', 'edit-note', 'delete-note'],
  computed: {
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
    }
  },
  methods: {
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
    }
  }
};
</script>

<style scoped>
.list-view {
  height: 100%;
  overflow-y: auto;
}

.notes-list {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.note-list-item {
  display: flex;
  gap: 10px;
  padding: 12px;
  background: var(--bg-secondary);
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s ease;
  border: 1px solid transparent;
}

.note-list-item:hover {
  border-color: var(--accent-color);
  transform: translateX(2px);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.note-list-item.important {
  border-left: 3px solid var(--warning-color, #f59e0b);
  background: rgba(245, 158, 11, 0.05);
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
  margin-bottom: 4px;
}

.note-list-title-row {
  display: flex;
  align-items: center;
  gap: 4px;
  flex: 1;
  min-width: 0;
}

.important-star {
  color: var(--warning-color, #f59e0b);
  display: flex;
  flex-shrink: 0;
}

.note-list-title {
  font-size: 13px;
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
  font-size: 11px;
  color: var(--text-tertiary);
  flex-shrink: 0;
}

.note-list-text {
  margin: 0;
  font-size: 12px;
  line-height: 1.4;
  color: var(--text-secondary);
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
  overflow: hidden;
}

.note-list-actions {
  display: flex;
  flex-direction: column;
  gap: 4px;
  flex-shrink: 0;
}

.action-btn {
  padding: 3px;
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
  transform: scale(1.1);
}

.action-btn.danger:hover {
  background: var(--danger-color-light);
  color: var(--danger-color);
}

.empty-state {
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 8px;
  color: var(--text-tertiary);
  text-align: center;
}

.empty-icon {
  opacity: 0.5;
  animation: float 3s ease-in-out infinite;
}

.empty-state h3 {
  margin: 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.empty-state p {
  margin: 0;
  font-size: 12px;
}

@keyframes float {
  0%, 100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-10px);
  }
}
</style>