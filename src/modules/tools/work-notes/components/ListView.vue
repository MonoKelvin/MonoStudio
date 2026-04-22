<template>
  <div class="list-view">
    <!-- 列表头部 -->
    <div class="list-header" v-if="filteredNotes.length > 0">
      <div class="list-info">
        <span class="notes-count">{{ filteredNotes.length }} 条笔记</span>
        <span class="sort-info">按时间排序：最新优先</span>
      </div>
    </div>
    
    <!-- 笔记列表 -->
    <div class="notes-list" v-if="filteredNotes.length > 0">
      <div
        v-for="(note, index) in filteredNotes"
        :key="note.id"
        class="note-list-item"
        :class="{ 'important': note.important, 'fade-in': true }"
        :style="{ animationDelay: `${index * 0.05}s` }"
        @click="$emit('open-detail', note)"
      >
        <div class="note-list-indicator" :class="{ 'important': note.important }"></div>
        <div class="note-list-content">
          <div class="note-list-header">
            <div class="note-list-title-row">
              <span v-if="note.important" class="important-star">
                <img src="@/assets/icons/star.svg" alt="重要" class="icon" />
              </span>
              <span class="note-list-title">{{ note.title || '无标题' }}</span>
              <span v-if="note.tag" class="note-list-tag">{{ note.tag }}</span>
            </div>
            <span class="note-list-date">{{ formatDateHeader(note.createdAt) }}</span>
          </div>
          <p class="note-list-text">{{ previewText(note) }}</p>
        </div>
        <div class="note-list-actions">
          <button class="action-btn" @click.stop="$emit('edit-note', note)" title="编辑">
            <img src="@/assets/icons/edit.svg" alt="编辑" class="icon" />
          </button>
          <button class="action-btn danger" @click.stop="$emit('delete-note', note)" title="删除">
            <img src="@/assets/icons/delete.svg" alt="删除" class="icon" />
          </button>
        </div>
      </div>
    </div>
    
    <!-- 空状态 -->
    <div class="empty-state" v-else>
      <div class="empty-icon">
        <img src="@/assets/icons/note.svg" alt="无笔记" class="empty-icon-img" />
      </div>
      <h3>{{ searchQuery || selectedTags.length > 0 ? '没有找到匹配的笔记' : '还没有任何笔记' }}</h3>
      <p>{{ searchQuery || selectedTags.length > 0 ? '尝试调整搜索条件' : '点击"新建笔记"开始记录' }}</p>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue';
import { NoteQueryService } from '../services/NoteQueryService.js';

// Props
const props = defineProps({
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
});

// Emits
const emit = defineEmits(['open-detail', 'edit-note', 'delete-note']);

// Computed properties
const filteredNotes = computed(() => {
  return NoteQueryService.filterNotes(props.notes, props.searchQuery, props.selectedTags);
});

// Methods
const formatDateHeader = (dateString) => {
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
};

const previewText = (note) => {
  return NoteQueryService.toPreviewText(note, 150);
};
</script>

<style scoped>
.empty-icon-img {
  width: 48px;
  height: 48px;
  display: block;
}

.list-view {
  height: 100%;
  overflow-y: auto;
}

.list-header {
  padding: 12px;
  background: var(--bg-secondary);
  border-radius: 8px;
  margin-bottom: 12px;
  border: 1px solid var(--border-color);
}

.list-info {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.notes-count {
  font-size: 12px;
  font-weight: 600;
  color: var(--text-primary);
}

.sort-info {
  font-size: 11px;
  color: var(--text-tertiary);
}

.notes-list {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.fade-in {
  animation: fadeIn 0.5s ease forwards;
  opacity: 0;
  transform: translateY(10px);
}

@keyframes fadeIn {
  to {
    opacity: 1;
    transform: translateY(0);
  }
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