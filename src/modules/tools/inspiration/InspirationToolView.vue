<template>
  <BasePanel class="panel inspiration-panel">
    <div class="inspiration-container">
      <div class="inspiration-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="addInspiration">添加灵感</BaseButton>
      </div>
      
      <div class="search-section">
        <BaseInput v-model="searchQuery" placeholder="搜索灵感..." />
      </div>
      
      <div class="inspiration-list" v-if="filteredInspirations.length > 0">
        <div v-for="inspiration in filteredInspirations" :key="inspiration.id" class="inspiration-item">
          <div class="inspiration-content">
            <div class="inspiration-text">{{ inspiration.content }}</div>
            <div class="inspiration-meta">
              <span class="inspiration-time">{{ formatDate(inspiration.createdAt) }}</span>
              <div class="inspiration-actions">
                <BaseButton size="sm" @click="editInspiration(inspiration)">编辑</BaseButton>
                <BaseButton size="sm" variant="danger" @click="deleteInspiration(inspiration.id)">删除</BaseButton>
              </div>
            </div>
          </div>
        </div>
      </div>
      
      <div class="no-inspirations" v-else>
        <div class="no-inspirations-icon">💡</div>
        <p>还没有灵感，点击"添加灵感"开始捕捉你的想法</p>
      </div>
      
      <!-- 添加/编辑灵感对话框 -->
      <div class="modal-overlay" v-if="showModal" @click="closeModal">
        <div class="modal-content" @click.stop>
          <div class="modal-header">
            <h4>{{ isEditing ? '编辑灵感' : '添加灵感' }}</h4>
            <button class="close-button" @click="closeModal">×</button>
          </div>
          <div class="modal-body">
            <BaseTextarea v-model="modalContent" placeholder="输入你的灵感..." :rows="6" />
          </div>
          <div class="modal-footer">
            <BaseButton @click="closeModal">取消</BaseButton>
            <BaseButton @click="saveInspiration" :disabled="!modalContent.trim()">保存</BaseButton>
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
  name: 'InspirationToolView',
  components: {
    BaseButton,
    BaseInput,
    BaseTextarea
  },
  props: {
    toolName: {
      type: String,
      default: '灵感碎片'
    },
    toolDescription: {
      type: String,
      default: '捕捉转瞬即逝的想法'
    }
  },
  data() {
    return {
      inspirations: [],
      searchQuery: '',
      showModal: false,
      isEditing: false,
      editingId: null,
      modalContent: ''
    };
  },
  computed: {
    filteredInspirations() {
      if (!this.searchQuery) {
        return this.inspirations.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
      }
      return this.inspirations
        .filter(inspiration => inspiration.content.toLowerCase().includes(this.searchQuery.toLowerCase()))
        .sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
    }
  },
  mounted() {
    this.loadInspirations();
  },
  methods: {
    loadInspirations() {
      const saved = localStorage.getItem('inspirations');
      if (saved) {
        this.inspirations = JSON.parse(saved);
      }
    },
    saveInspirations() {
      localStorage.setItem('inspirations', JSON.stringify(this.inspirations));
    },
    addInspiration() {
      this.isEditing = false;
      this.editingId = null;
      this.modalContent = '';
      this.showModal = true;
    },
    editInspiration(inspiration) {
      this.isEditing = true;
      this.editingId = inspiration.id;
      this.modalContent = inspiration.content;
      this.showModal = true;
    },
    saveInspiration() {
      if (!this.modalContent.trim()) return;
      
      if (this.isEditing) {
        const index = this.inspirations.findIndex(insp => insp.id === this.editingId);
        if (index !== -1) {
          this.inspirations[index].content = this.modalContent;
          this.inspirations[index].updatedAt = new Date().toISOString();
        }
      } else {
        this.inspirations.push({
          id: Date.now().toString(),
          content: this.modalContent,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString()
        });
      }
      
      this.saveInspirations();
      this.closeModal();
    },
    deleteInspiration(id) {
      if (confirm('确定要删除这个灵感吗？')) {
        this.inspirations = this.inspirations.filter(insp => insp.id !== id);
        this.saveInspirations();
      }
    },
    closeModal() {
      this.showModal = false;
      this.isEditing = false;
      this.editingId = null;
      this.modalContent = '';
    },
    formatDate(dateString) {
      const date = new Date(dateString);
      return date.toLocaleString('zh-CN', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit'
      });
    }
  }
};
</script>

<style scoped>
.inspiration-panel {
  height: 100%;
}

.inspiration-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.inspiration-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.inspiration-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.search-section {
  margin-bottom: 20px;
}

.inspiration-list {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 12px;
  overflow-y: auto;
}

.inspiration-item {
  padding: 16px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  box-shadow: 0 1px 2px rgba(0, 0, 0, 0.05);
  transition: transform 0.2s ease, box-shadow 0.2s ease;
}

.inspiration-item:hover {
  transform: translateY(-1px);
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.inspiration-content {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.inspiration-text {
  font-size: 14px;
  line-height: 1.5;
  color: var(--text-primary);
  white-space: pre-wrap;
  word-break: break-word;
}

.inspiration-meta {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 12px;
  color: var(--text-tertiary);
}

.inspiration-actions {
  display: flex;
  gap: 8px;
}

.no-inspirations {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  color: var(--text-secondary);
  gap: 16px;
}

.no-inspirations-icon {
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