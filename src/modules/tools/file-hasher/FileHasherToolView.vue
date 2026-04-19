<template>
  <BasePanel class="panel file-hasher-panel">
    <div class="file-hasher-container">
      <div class="hasher-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="selectFile">选择文件</BaseButton>
      </div>
      
      <div class="file-info" v-if="selectedFile">
        <div class="file-name">{{ selectedFile.name }}</div>
        <div class="file-size">{{ formatFileSize(selectedFile.size) }}</div>
      </div>
      
      <div class="hash-options">
        <label>哈希算法</label>
        <BaseSelect v-model="selectedAlgorithm" :options="hashAlgorithms" />
      </div>
      
      <div class="hash-result" v-if="hashResult">
        <div class="result-header">
          <label>{{ selectedAlgorithm }} 哈希值</label>
          <BaseButton size="sm" @click="copyHash" :disabled="!hashResult">
            复制
          </BaseButton>
        </div>
        <BaseInput v-model="hashResult" readonly />
      </div>
      
      <div class="status-message" v-if="statusMessage">
        {{ statusMessage }}
      </div>
      
      <input type="file" ref="fileInput" style="display: none" @change="handleFileSelect" />
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';

export default {
  name: 'FileHasherToolView',
  components: {
    BaseButton,
    BaseInput,
    BaseSelect
  },
  props: {
    toolName: {
      type: String,
      default: '文件哈希'
    },
    toolDescription: {
      type: String,
      default: '计算文件的哈希值'
    }
  },
  data() {
    return {
      selectedFile: null,
      hashAlgorithms: [
        { label: 'MD5', value: 'MD5' },
        { label: 'SHA-1', value: 'SHA-1' },
        { label: 'SHA-256', value: 'SHA-256' },
        { label: 'SHA-512', value: 'SHA-512' }
      ],
      selectedAlgorithm: 'SHA-256',
      hashResult: '',
      statusMessage: ''
    };
  },
  methods: {
    selectFile() {
      this.$refs.fileInput.click();
    },
    handleFileSelect(event) {
      const file = event.target.files[0];
      if (file) {
        this.selectedFile = file;
        this.calculateHash(file);
      }
    },
    async calculateHash(file) {
      this.statusMessage = '正在计算哈希值...';
      this.hashResult = '';
      
      try {
        const hash = await this.computeHash(file, this.selectedAlgorithm);
        this.hashResult = hash;
        this.statusMessage = '哈希值计算完成';
      } catch (error) {
        this.statusMessage = '计算哈希值失败: ' + error.message;
        this.hashResult = '';
      }
    },
    computeHash(file, algorithm) {
      return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = async (e) => {
          try {
            const arrayBuffer = e.target.result;
            const hashBuffer = await crypto.subtle.digest(algorithm, arrayBuffer);
            const hashArray = Array.from(new Uint8Array(hashBuffer));
            const hashHex = hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
            resolve(hashHex);
          } catch (error) {
            reject(error);
          }
        };
        reader.onerror = () => {
          reject(new Error('文件读取失败'));
        };
        reader.readAsArrayBuffer(file);
      });
    },
    formatFileSize(bytes) {
      if (bytes === 0) return '0 B';
      const k = 1024;
      const sizes = ['B', 'KB', 'MB', 'GB'];
      const i = Math.floor(Math.log(bytes) / Math.log(k));
      return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    },
    copyHash() {
      if (this.hashResult) {
        navigator.clipboard.writeText(this.hashResult).then(() => {
          alert('已复制到剪贴板');
        }).catch(err => {
          console.error('复制失败:', err);
        });
      }
    }
  },
  watch: {
    selectedAlgorithm() {
      if (this.selectedFile) {
        this.calculateHash(this.selectedFile);
      }
    }
  }
};
</script>

<style scoped>
.file-hasher-panel {
  height: 100%;
}

.file-hasher-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.hasher-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.hasher-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.file-info {
  margin-bottom: 20px;
  padding: 12px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
}

.file-name {
  font-weight: 500;
  color: var(--text-primary);
  margin-bottom: 4px;
}

.file-size {
  font-size: 14px;
  color: var(--text-secondary);
}

.hash-options {
  margin-bottom: 20px;
}

.hash-options label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.hash-result {
  margin-bottom: 20px;
}

.result-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
}

.result-header label {
  font-weight: 500;
  color: var(--text-primary);
}

.status-message {
  margin-top: auto;
  padding: 12px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  font-size: 14px;
  color: var(--text-secondary);
  text-align: center;
}
</style>