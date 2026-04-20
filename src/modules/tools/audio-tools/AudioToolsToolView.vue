<template>
  <BasePanel class="panel audio-tools-panel">
    <div class="audio-tools-container">
      <div class="audio-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="audio-upload-section">
        <label class="upload-label">
          <input type="file" accept="audio/*" @change="handleFileUpload" />
          <span class="upload-icon">🎵</span>
          <span class="upload-text" v-if="!selectedFile">点击或拖入音频文件</span>
          <span class="upload-text" v-else>已选择：{{ selectedFile.name }}</span>
          <span class="upload-hint">支持 mp3 / wav / ogg / flac</span>
        </label>
        <BaseButton v-if="selectedFile" @click="clearFile">移除文件</BaseButton>
      </div>
      
      <div class="audio-options" v-if="selectedFile">
        <div class="option-group">
          <label>目标格式</label>
          <BaseSelect v-model="targetFormat" :options="formatOptions" />
        </div>
        
        <div class="option-group">
          <label>音频质量</label>
          <div class="quality-slider">
            <input type="range" v-model.number="quality" min="1" max="10" step="1" />
            <span>{{ quality }}/10</span>
          </div>
        </div>
        
        <BaseButton @click="convertAudio">转换并下载</BaseButton>
      </div>
      
      <div class="audio-info" v-else>
        <p>上传音频文件以进行格式转换</p>
        <p>支持将音频文件转换为不同格式</p>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';

export default {
  name: 'AudioToolsToolView',
  components: {
    BaseButton,
    BaseSelect
  },
  props: {
    toolName: {
      type: String,
      default: '音频工具'
    },
    toolDescription: {
      type: String,
      default: '音频格式转换和编辑'
    }
  },
  data() {
    return {
      selectedFile: null,
      targetFormat: 'mp3',
      quality: 7,
      formatOptions: [
        { value: 'mp3', label: 'MP3' },
        { value: 'wav', label: 'WAV' },
        { value: 'ogg', label: 'OGG' },
        { value: 'flac', label: 'FLAC' }
      ]
    };
  },
  methods: {
    handleFileUpload(event) {
      const file = event.target.files[0];
      if (file) {
        this.selectedFile = file;
      }
    },
    clearFile() {
      this.selectedFile = null;
    },
    convertAudio() {
      if (!this.selectedFile) return;
      
      // 模拟音频转换
      setTimeout(() => {
        // 创建下载链接
        const link = document.createElement('a');
        link.href = URL.createObjectURL(this.selectedFile);
        link.download = `${this.selectedFile.name.replace(/\.[^.]+$/, '')}.${this.targetFormat}`;
        link.click();
      }, 1000);
    }
  }
};
</script>

<style scoped>
.audio-tools-panel {
  height: 100%;
}

.audio-tools-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.audio-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.audio-upload-section {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.upload-label {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 40px;
  border: 2px dashed var(--border-color);
  border-radius: var(--radius-md);
  cursor: pointer;
  transition: all 0.2s ease;
  background-color: var(--bg-secondary);
}

.upload-label:hover {
  border-color: var(--accent-color);
  background-color: var(--bg-tertiary);
}

.upload-icon {
  font-size: 48px;
  margin-bottom: 12px;
}

.upload-text {
  font-size: 16px;
  font-weight: 500;
  color: var(--text-primary);
  margin-bottom: 8px;
}

.upload-hint {
  font-size: 14px;
  color: var(--text-secondary);
}

input[type="file"] {
  display: none;
}

.audio-options {
  background-color: var(--bg-secondary);
  padding: 20px;
  border-radius: var(--radius-md);
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.option-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.option-group label {
  font-weight: 500;
  color: var(--text-primary);
  font-size: 14px;
}

.quality-slider {
  display: flex;
  align-items: center;
  gap: 12px;
}

.quality-slider input {
  flex: 1;
}

.audio-info {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  color: var(--text-secondary);
  gap: 12px;
  background-color: var(--bg-secondary);
  padding: 40px;
  border-radius: var(--radius-md);
}
</style>