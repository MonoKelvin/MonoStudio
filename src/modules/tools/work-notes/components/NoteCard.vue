<template>
  <div class="note-card" :class="{ 'important': note.important }" @click="$emit('click')">
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
    <p class="note-card-content">{{ previewText(note) }}</p>
    <div class="note-card-footer">
      <span class="note-time">{{ formatNoteTime(note.createdAt) }}</span>
      <div class="note-card-actions">
        <button class="action-btn edit-btn" @click.stop="$emit('edit')" title="编辑">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
            <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
          </svg>
        </button>
        <button class="action-btn delete-btn" @click.stop="$emit('delete')" title="删除">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <polyline points="3 6 5 6 21 6"></polyline>
            <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path>
          </svg>
        </button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { NoteQueryService } from '../services/NoteQueryService.js';

// Props
const props = defineProps({
  note: {
    type: Object,
    required: true
  }
});

// Emits
const emit = defineEmits(['click', 'edit', 'delete']);

// Methods
const formatNoteTime = (dateString) => {
  const date = new Date(dateString);
  return date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
};

const previewText = (note) => {
  return NoteQueryService.toPreviewText(note, 100);
};
</script>

<style scoped>
.note-card {
  padding: 8px;
  background: var(--bg-secondary);
  border-radius: 6px;
  cursor: pointer;
  border: 1px solid transparent;
}

.note-card.important {
  border-left: 3px solid var(--warning-color, #f59e0b);
  background: rgba(245, 158, 11, 0.05);
}

.note-card-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 3px;
}

.note-card-title-row {
  display: flex;
  align-items: center;
  gap: 3px;
  flex: 1;
}

.important-star {
  color: var(--warning-color, #f59e0b);
  display: flex;
}

.note-title {
  font-size: 12px;
  font-weight: 500;
  color: var(--text-primary);
  line-height: 1.3;
}

.note-tag-badge {
  font-size: 9px;
  padding: 1px 4px;
  background: var(--success-color-light);
  color: var(--success-color);
  border-radius: 3px;
  flex-shrink: 0;
}

.note-card-content {
  margin: 0;
  font-size: 11px;
  line-height: 1.3;
  color: var(--text-secondary);
  max-height: 40px;
  overflow: hidden;
  display: -webkit-box;
  -webkit-line-clamp: 3;
  -webkit-box-orient: vertical;
}

.note-card-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-top: 4px;
}

.note-time {
  font-size: 10px;
  color: var(--text-tertiary);
}

.note-card-actions {
  display: flex;
  gap: 3px;
}

.action-btn {
  padding: 2px;
  border: none;
  background: transparent;
  cursor: pointer;
  border-radius: 3px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.edit-btn {
  color: var(--primary-color);
}

.delete-btn {
  color: var(--danger-color);
}
</style>