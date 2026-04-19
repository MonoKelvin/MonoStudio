<template>
  <BasePanel class="panel work-notes-panel">
    <div class="work-notes-container">
      <div class="work-notes-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="addNote">添加日志</BaseButton>
      </div>
      
      <div class="search-section">
        <BaseInput v-model="searchQuery" placeholder="搜索日志..." />
      </div>
      
      <div class="work-notes-list" v-if="groupedNotes.length > 0">
        <div v-for="(notes, date) in groupedNotes" :key="date" class="notes-group">
          <div class="group-header">
            <h4>{{ formatDateHeader(date) }}</h4>
            <span class="note-count">{{ notes.length }} 条</span>
          </div>
          <div class="notes-items">
            <div v-for="note in notes" :key="note.id" class="note-item">
              <div class="note-content">
                <div class="note-title" v-if="note.title">{{ note.title }}</div>
                <div class="note-text">{{ note.content }}</div>
                <div class="note-meta">
                  <span class="note-time">{{ formatTime(note.createdAt) }}</span>
                  <div class="note-actions">
                    <BaseButton size="sm" @click="editNote(note)">编辑</BaseButton>
                    <BaseButton size="sm" variant="danger" @click="deleteNote(note.id)">删除</BaseButton>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
      
      <div class="no-notes" v-else>
        <div class="no-notes-icon">📝</div>
        <p>还没有工作日志，点击"添加日志"开始记录</p>
      </div>
      
      <!-- 添加/编辑日志对话框 -->
      <div class="modal-overlay" v-if="showModal" @click="closeModal">
        <div class="modal-content" @click.stop>
          <div class="modal-header">
            <h4>{{ isEditing ? '编辑日志' : '添加日志' }}</h4>
            <button class="close-button" @click="closeModal">×</button>
          </div>
          <div class="modal-body">
            <BaseInput v-model="modalTitle" placeholder="日志标题（可选）" />
            <BaseTextarea v-model="modalContent" placeholder="日志内容..." :rows="8" />
          </div>
          <div class="modal-footer">
            <BaseButton @click="closeModal">取消</BaseButton>
            <BaseButton @click="saveNote" :disabled="!modalContent.trim()">保存</BaseButton>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseTextarea from '../../../components/base/BaseTextarea.vue';

export default {
  name: 'WorkNotesToolView',
  components: {
    BaseButton,
    BaseInput,
    BaseTextarea
  },
  props: {
    toolName: {
      type: String,
      default: '工作手记'
    },
    toolDescription: {
      type: String,
      default: '记录工作中的点滴'
    }
  },
  data() {
    return {
      notes: [],
      searchQuery: '',
      showModal: false,
      isEditing: false,
      editingId: null,
      modalTitle: '',
      modalContent: ''
    };
  },
  computed: {
    filteredNotes() {
      if (!this.searchQuery) {
        return this.notes.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
      }
      return this.notes
        .filter(note => {
          const content = (note.title || '') + ' ' + note.content;
          return content.toLowerCase().includes(this.searchQuery.toLowerCase());
        })
        .sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
    },
    groupedNotes() {
      const grouped = {};
      this.filteredNotes.forEach(note => {
        const date = new Date(note.createdAt).toISOString().split('T')[0];
        if (!grouped[date]) {
          grouped[date] = [];
        }
        grouped[date].push(note);
      });
      return grouped;
    }
  },
  mounted() {
    this.loadNotes();
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
    addNote() {
      this.isEditing = false;
      this.editingId = null;
      this.modalTitle = '';
      this.modalContent = '';
      this.showModal = true;
    },
    editNote(note) {
      this.isEditing = true;
      this.editingId = note.id;
      this.modalTitle = note.title || '';
      this.modalContent = note.content;
      this.showModal = true;
    },
    saveNote() {
      if (!this.modalContent.trim()) return;
      
      if (this.isEditing) {
        const index = this.notes.findIndex(note => note.id === this.editingId);
        if (index !== -1) {
          this.notes[index].title = this.modalTitle;
          this.notes[index].content = this.modalContent;
          this.notes[index].updatedAt = new Date().toISOString();
        }
      } else {
        this.notes.push({
          id: Date.now().toString(),
          title: this.modalTitle,
          content: this.modalContent,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString()
        });
      }
      
      this.saveNotes();
      this.closeModal();
    },
    deleteNote(id) {
      if (confirm('确定要删除这个日志吗？')) {
        this.notes = this.notes.filter(note => note.id !== id);
        this.saveNotes();
      }
    },
    closeModal() {
      this.showModal = false;
      this.isEditing = false;
      this.editingId = null;
      this.modalTitle = '';
      this.modalContent = '';
    },
    formatDateHeader(dateString) {
      const date = new Date(dateString);
      const today = new Date();
      const yesterday = new Date(today);
      yesterday.setDate(yesterday.getDate() - 1);
      
      if (date.toDateString() === today.toDateString()) {
        return '今天';
      } else if (date.toDateString() === yesterday.toDateString()) {
        return '昨天';
      } else {
        return date.toLocaleDateString('zh-CN', {
          year: 'numeric',
          month: '2-digit',
          day: '2-digit'
        });
      }
    },
    formatTime(dateString) {
      const date = new Date(dateString);
      return date.toLocaleTimeString('zh-CN', {
        hour: '2-digit',
        minute: '2-digit'
      });
    }
  }
};
</script>

<style scoped>
.work-notes-panel {
  height: 100%;
}

.work-notes-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.work-notes-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.work-notes-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.search-section {
  margin-bottom: 20px;
}

.work-notes-list {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 20px;
  overflow-y: auto;
}

.notes-group {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.group-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 8px 0;
  border-bottom: 1px solid var(--border-color);
}

.group-header h4 {
  margin: 0;
  font-size: 14px;
  font-weight: 600;
  color: var(--text-secondary);
}

.note-count {
  font-size: 12px;
  color: var(--text-tertiary);
  background-color: var(--bg-secondary);
  padding: 2px 8px;
  border-radius: var(--radius-sm);
}

.notes-items {
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.note-item {
  padding: 16px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  box-shadow: 0 1px 2px rgba(0, 0, 0, 0.05);
  transition: transform 0.2s ease, box-shadow 0.2s ease;
}

.note-item:hover {
  transform: translateY(-1px);
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.note-content {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.note-title {
  font-size: 14px;
  font-weight: 600;
  color: var(--text-primary);
}

.note-text {
  font-size: 14px;
  line-height: 1.5;
  color: var(--text-primary);
  white-space: pre-wrap;
  word-break: break-word;
}

.note-meta {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 12px;
  color: var(--text-tertiary);
  margin-top: 4px;
}

.note-actions {
  display: flex;
  gap: 8px;
}

.no-notes {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  color: var(--text-secondary);
  gap: 16px;
}

.no-notes-icon {
  font-size: 48px;
}

.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
}

.modal-content {
  background-color: var(--bg-primary);
  border-radius: var(--radius-md);
  width: 90%;
  max-width: 500px;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 20px;
  border-bottom: 1px solid var(--border-color);
}

.modal-header h4 {
  margin: 0;
  font-size: 16px;
  font-weight: 600;
}

.close-button {
  background: none;
  border: none;
  font-size: 24px;
  cursor: pointer;
  color: var(--text-secondary);
  padding: 0;
  width: 24px;
  height: 24px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.modal-body {
  padding: 20px;
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.modal-footer {
  display: flex;
  justify-content: flex-end;
  gap: 10px;
  padding: 16px 20px;
  border-top: 1px solid var(--border-color);
  background-color: var(--bg-secondary);
  border-bottom-left-radius: var(--radius-md);
  border-bottom-right-radius: var(--radius-md);
}
</style>