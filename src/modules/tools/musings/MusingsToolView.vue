<template>
  <BasePanel class="panel musings-panel">
    <div class="musings-container">
      <div class="musings-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="addMusing">添加随想</BaseButton>
      </div>
      
      <div class="search-section">
        <BaseInput v-model="searchQuery" placeholder="搜索随想..." />
      </div>
      
      <div class="tags-section" v-if="allTags.length > 0">
        <div class="tags-header">
          <h4>标签</h4>
        </div>
        <div class="tags-list">
          <BaseButton 
            v-for="tag in allTags" 
            :key="tag" 
            size="sm" 
            :variant="selectedTags.includes(tag) ? 'primary' : 'secondary'"
            @click="toggleTag(tag)"
          >
            {{ tag }}
          </BaseButton>
        </div>
      </div>
      
      <div class="musings-list" v-if="filteredMusings.length > 0">
        <div v-for="musing in filteredMusings" :key="musing.id" class="musing-item">
          <div class="musing-content">
            <div class="musing-title" v-if="musing.title">{{ musing.title }}</div>
            <div class="musing-text">{{ musing.content }}</div>
            <div class="musing-meta">
              <div class="musing-tags">
                <span v-for="tag in musing.tags" :key="tag" class="tag">{{ tag }}</span>
              </div>
              <div class="musing-info">
                <span class="musing-time">{{ formatDate(musing.createdAt) }}</span>
                <div class="musing-actions">
                  <BaseButton size="sm" @click="editMusing(musing)">编辑</BaseButton>
                  <BaseButton size="sm" variant="danger" @click="deleteMusing(musing.id)">删除</BaseButton>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
      
      <div class="no-musings" v-else>
        <div class="no-musings-icon">🌙</div>
        <p>还没有随想，点击"添加随想"开始记录</p>
      </div>
      
      <!-- 添加/编辑随想对话框 -->
      <div class="modal-overlay" v-if="showModal" @click="closeModal">
        <div class="modal-content" @click.stop>
          <div class="modal-header">
            <h4>{{ isEditing ? '编辑随想' : '添加随想' }}</h4>
            <button class="close-button" @click="closeModal">×</button>
          </div>
          <div class="modal-body">
            <BaseInput v-model="modalTitle" placeholder="随想标题（可选）" />
            <BaseTextarea v-model="modalContent" placeholder="随想内容..." :rows="8" />
            <BaseInput v-model="modalTags" placeholder="标签，用逗号分隔" />
          </div>
          <div class="modal-footer">
            <BaseButton @click="closeModal">取消</BaseButton>
            <BaseButton @click="saveMusing" :disabled="!modalContent.trim()">保存</BaseButton>
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
  name: 'MusingsToolView',
  components: {
    BaseButton,
    BaseInput,
    BaseTextarea
  },
  props: {
    toolName: {
      type: String,
      default: '随想录'
    },
    toolDescription: {
      type: String,
      default: '随性的思考与感悟'
    }
  },
  data() {
    return {
      musings: [],
      searchQuery: '',
      selectedTags: [],
      showModal: false,
      isEditing: false,
      editingId: null,
      modalTitle: '',
      modalContent: '',
      modalTags: ''
    };
  },
  computed: {
    allTags() {
      const tags = new Set();
      this.musings.forEach(musing => {
        musing.tags.forEach(tag => tags.add(tag));
      });
      return Array.from(tags);
    },
    filteredMusings() {
      let result = this.musings;
      
      // 按标签筛选
      if (this.selectedTags.length > 0) {
        result = result.filter(musing => {
          return this.selectedTags.some(tag => musing.tags.includes(tag));
        });
      }
      
      // 按搜索词筛选
      if (this.searchQuery) {
        const query = this.searchQuery.toLowerCase();
        result = result.filter(musing => {
          const content = (musing.title || '') + ' ' + musing.content + ' ' + musing.tags.join(' ');
          return content.toLowerCase().includes(query);
        });
      }
      
      // 按时间排序
      return result.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
    }
  },
  mounted() {
    this.loadMusings();
  },
  methods: {
    async loadMusings() {
      try {
        const data = await window.electronAPI.userData.getMusings();
        this.musings = Array.isArray(data) ? data : [];
      } catch (e) {
        console.error('Failed to load musings:', e);
        this.musings = [];
      }
    },
    async saveMusings() {
      try {
        await window.electronAPI.userData.saveMusings(this.musings);
      } catch (e) {
        console.error('Failed to save musings:', e);
      }
    },
    addMusing() {
      this.isEditing = false;
      this.editingId = null;
      this.modalTitle = '';
      this.modalContent = '';
      this.modalTags = '';
      this.showModal = true;
    },
    editMusing(musing) {
      this.isEditing = true;
      this.editingId = musing.id;
      this.modalTitle = musing.title || '';
      this.modalContent = musing.content;
      this.modalTags = musing.tags.join(', ');
      this.showModal = true;
    },
    saveMusing() {
      if (!this.modalContent.trim()) return;
      
      // 处理标签
      const tags = this.modalTags
        .split(',')
        .map(tag => tag.trim())
        .filter(tag => tag);
      
      if (this.isEditing) {
        const index = this.musings.findIndex(musing => musing.id === this.editingId);
        if (index !== -1) {
          this.musings[index].title = this.modalTitle;
          this.musings[index].content = this.modalContent;
          this.musings[index].tags = tags;
          this.musings[index].updatedAt = new Date().toISOString();
        }
      } else {
        this.musings.push({
          id: Date.now().toString(),
          title: this.modalTitle,
          content: this.modalContent,
          tags: tags,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString()
        });
      }
      
      this.saveMusings();
      this.closeModal();
    },
    deleteMusing(id) {
      if (confirm('确定要删除这个随想吗？')) {
        this.musings = this.musings.filter(musing => musing.id !== id);
        this.saveMusings();
      }
    },
    closeModal() {
      this.showModal = false;
      this.isEditing = false;
      this.editingId = null;
      this.modalTitle = '';
      this.modalContent = '';
      this.modalTags = '';
    },
    toggleTag(tag) {
      const index = this.selectedTags.indexOf(tag);
      if (index === -1) {
        this.selectedTags.push(tag);
      } else {
        this.selectedTags.splice(index, 1);
      }
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
.musings-panel {
  height: 100%;
}

.musings-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.musings-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.musings-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.search-section {
  margin-bottom: 20px;
}

.tags-section {
  margin-bottom: 20px;
}

.tags-header h4 {
  margin: 0 0 12px 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.tags-list {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.musings-list {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 12px;
  overflow-y: auto;
}

.musing-item {
  padding: 16px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  box-shadow: 0 1px 2px rgba(0, 0, 0, 0.05);
  transition: transform 0.2s ease, box-shadow 0.2s ease;
}

.musing-item:hover {
  transform: translateY(-1px);
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.musing-content {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.musing-title {
  font-size: 14px;
  font-weight: 600;
  color: var(--text-primary);
}

.musing-text {
  font-size: 14px;
  line-height: 1.5;
  color: var(--text-primary);
  white-space: pre-wrap;
  word-break: break-word;
}

.musing-meta {
  display: flex;
  flex-direction: column;
  gap: 8px;
  margin-top: 4px;
}

.musing-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}

.tag {
  font-size: 12px;
  padding: 2px 8px;
  background-color: var(--bg-primary);
  border-radius: var(--radius-sm);
  color: var(--text-secondary);
}

.musing-info {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 12px;
  color: var(--text-tertiary);
}

.musing-actions {
  display: flex;
  gap: 8px;
}

.no-musings {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  color: var(--text-secondary);
  gap: 16px;
}

.no-musings-icon {
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