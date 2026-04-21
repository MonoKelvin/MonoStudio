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
    <p class="note-card-content">{{ note.content.substring(0, 100) }}{{ note.content.length > 100 ? '...' : '' }}</p>
    <div class="note-card-footer">
      <span class="note-time">{{ formatNoteTime(note.createdAt) }}</span>
      <div class="note-card-actions">
        <button class="action-btn" @click.stop="$emit('edit')" title="编辑">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"></path>
            <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"></path>
          </svg>
        </button>
        <button class="action-btn danger" @click.stop="$emit('delete')" title="删除">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <polyline points="3 6 5 6 21 6"></polyline>
            <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"></path>
          </svg>
        </button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'NoteCard',
  props: {
    note: {
      type: Object,
      required: true
    }
  },
  emits: ['click', 'edit', 'delete'],
  methods: {
    formatNoteTime(dateString) {
      const date = new Date(dateString);
      return date.toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });
    }
  }
};
</script>

<style scoped>
.note-card {
  padding: 10px;
  background: var(--bg-secondary);
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s ease;
  border: 1px solid transparent;
}

.note-card:hover {
  border-color: var(--accent-color);
  transform: translateY(-1px);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.note-card.important {
  border-left: 3px solid var(--warning-color, #f59e0b);
  background: rgba(245, 158, 11, 0.05);
}

.note-card-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  margin-bottom: 4px;
}

.note-card-title-row {
  display: flex;
  align-items: center;
  gap: 4px;
  flex: 1;
}

.important-star {
  color: var(--warning-color, #f59e0b);
  display: flex;
  animation: twinkle 2s infinite;
}

.note-title {
  font-size: 13px;
  font-weight: 500;
  color: var(--text-primary);
}

.note-tag-badge {
  font-size: 10px;
  padding: 2px 6px;
  background: var(--success-color-light);
  color: var(--success-color);
  border-radius: 4px;
  flex-shrink: 0;
}

.note-card-content {
  margin: 0;
  font-size: 12px;
  line-height: 1.4;
  color: var(--text-secondary);
}

.note-card-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-top: 6px;
}

.note-time {
  font-size: 11px;
  color: var(--text-tertiary);
}

.note-card-actions {
  display: flex;
  gap: 4px;
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

@keyframes twinkle {
  0%, 100% {
    opacity: 1;
  }
  50% {
    opacity: 0.6;
  }
}
</style>