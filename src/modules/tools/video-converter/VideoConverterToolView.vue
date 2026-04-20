<template>
  <BasePanel class="panel video-converter-panel">
    <div class="video-converter-container">
      <div class="video-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="video-upload-section">
        <label class="upload-label">
          <input type="file" accept="video/*" @change="handleFileUpload" />
          <span class="upload-icon">🎬</span>
          <span class="upload-text" v-if="!selectedFile">点击或拖入视频文件</span>
          <span class="upload-text" v-else>已选择：{{ selectedFile.name }}</span>
          <span class="upload-hint">支持 mp4 / avi / mov / wmv</span>
        </label>
        <BaseButton v-if="selectedFile" @click="clearFile">移除文件</BaseButton>
      </div>
      
      <div class="video-options" v-if="selectedFile">
        <div class="option-group">
          <label>目标格式</label>
          <BaseSelect v-model="targetFormat" :options="formatOptions" />
        </div>
        
        <div class="option-group">
          <label>视频质量</label>
          <div class="quality-slider">
            <input type="range" v-model.number="quality" min="1" max="10" step="1" />
            <span>{{ quality }}/10</span>
          </div>
        </div>
        
        <div class="option-group">
          <label>分辨率</label>
          <BaseSelect v-model="resolution" :options="resolutionOptions" />
        </div>
        
        <BaseButton @click="convertVideo">转换并下载</BaseButton>
      </div>
      
      <div class="video-info" v-else>
        <p>上传视频文件以进行格式转换</p>
        <p>支持将视频文件转换为不同格式和分辨率</p>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';

export default {
  name: 'VideoConverterToolView',
  components: {
    BaseButton,
    BaseSelect
  },
  props: {
    toolName: {
      type: String,
      default: '视频转换'
    },
    toolDescription: {
      type: String,
      default: '视频格式转换和处理'
    }
  },
  data() {
    return {
      selectedFile: null,
      targetFormat: 'mp4',
      quality: 7,
      resolution: '1920x1080',
      formatOptions: [
        { value: 'mp4', label: 'MP4' },
        { value: 'avi', label: 'AVI' },
        { value: 'mov', label: 'MOV' },
        { value: 'wmv', label: 'WMV' }
      ],
      resolutionOptions: [
        { value: '1920x1080', label: '1080p (1920x1080)' },
        { value: '1280x720', label: '720p (1280x720)' },
        { value: '854x480', label: '480p (854x480)' },
        { value: '640x360', label: '360p (640x360)' }
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
    convertVideo() {
      if (!this.selectedFile) return;
      
      // 模拟视频转换
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
.video-converter-panel {
  height: 100%;
}

.video-converter-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.video-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.video-upload-section {
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

.video-options {
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

.video-info {
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