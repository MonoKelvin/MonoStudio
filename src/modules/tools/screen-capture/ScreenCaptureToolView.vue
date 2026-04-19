<template>
  <BasePanel class="panel screen-capture-panel">
    <div class="screen-capture-container">
      <div class="capture-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="capture-options">
        <div class="option-group">
          <label>截图类型</label>
          <div class="radio-group">
            <label class="radio-item">
              <input type="radio" v-model="captureType" value="fullscreen" />
              <span>全屏</span>
            </label>
            <label class="radio-item">
              <input type="radio" v-model="captureType" value="custom" />
              <span>自定义区域</span>
            </label>
          </div>
        </div>
        
        <BaseButton @click="startCapture" :disabled="isCapturing">
          {{ isCapturing ? '截图中...' : '开始截图' }}
        </BaseButton>
      </div>
      
      <div class="capture-result" v-if="capturedImage">
        <div class="result-header">
          <h4>截图结果</h4>
          <div class="result-actions">
            <BaseButton size="sm" @click="saveImage">保存图片</BaseButton>
            <BaseButton size="sm" @click="copyImage">复制到剪贴板</BaseButton>
            <BaseButton size="sm" @click="clearImage">清除</BaseButton>
          </div>
        </div>
        <div class="image-preview">
          <img :src="capturedImage" alt="截图" />
        </div>
      </div>
      
      <div class="capture-info" v-if="!capturedImage">
        <p>点击"开始截图"按钮开始截取屏幕内容</p>
        <p>支持全屏截图和自定义区域截图</p>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'ScreenCaptureToolView',
  components: {
    BaseButton
  },
  props: {
    toolName: {
      type: String,
      default: '屏幕截图'
    },
    toolDescription: {
      type: String,
      default: '截取屏幕和编辑图片'
    }
  },
  data() {
    return {
      captureType: 'fullscreen',
      capturedImage: null,
      isCapturing: false,
      mediaStream: null
    };
  },
  methods: {
    async startCapture() {
      try {
        this.isCapturing = true;
        
        // 使用 getDisplayMedia API 捕获屏幕
        this.mediaStream = await navigator.mediaDevices.getDisplayMedia({
          video: {
            cursor: 'always'
          },
          audio: false
        });
        
        // 创建视频元素来显示捕获的流
        const video = document.createElement('video');
        video.srcObject = this.mediaStream;
        video.onloadedmetadata = async () => {
          video.play();
          
          // 创建 canvas 来绘制视频帧
          const canvas = document.createElement('canvas');
          canvas.width = video.videoWidth;
          canvas.height = video.videoHeight;
          const ctx = canvas.getContext('2d');
          
          // 绘制当前视频帧到 canvas
          ctx.drawImage(video, 0, 0, canvas.width, canvas.height);
          
          // 将 canvas 转换为图片 URL
          this.capturedImage = canvas.toDataURL('image/png');
          
          // 停止媒体流
          this.mediaStream.getTracks().forEach(track => track.stop());
          this.mediaStream = null;
          this.isCapturing = false;
        };
      } catch (error) {
        console.error('截图失败:', error);
        this.isCapturing = false;
        alert('截图失败: ' + error.message);
      }
    },
    saveImage() {
      if (!this.capturedImage) return;
      
      // 创建下载链接
      const link = document.createElement('a');
      link.href = this.capturedImage;
      link.download = `screenshot_${new Date().toISOString().slice(0, 19).replace(/[-:]/g, '')}.png`;
      link.click();
    },
    async copyImage() {
      if (!this.capturedImage) return;
      
      try {
        // 将图片 URL 转换为 Blob
        const response = await fetch(this.capturedImage);
        const blob = await response.blob();
        
        // 复制到剪贴板
        await navigator.clipboard.write([
          new ClipboardItem({
            'image/png': blob
          })
        ]);
        
        alert('已复制到剪贴板');
      } catch (error) {
        console.error('复制失败:', error);
        alert('复制失败: ' + error.message);
      }
    },
    clearImage() {
      this.capturedImage = null;
    }
  }
};
</script>

<style scoped>
.screen-capture-panel {
  height: 100%;
}

.screen-capture-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.capture-header h3 {
  margin: 0 0 20px 0;
  font-size: 18px;
  font-weight: 600;
}

.capture-options {
  margin-bottom: 20px;
  padding: 20px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
}

.option-group {
  margin-bottom: 16px;
}

.option-group label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.radio-group {
  display: flex;
  gap: 20px;
}

.radio-item {
  display: flex;
  align-items: center;
  gap: 6px;
  cursor: pointer;
}

.radio-item input[type="radio"] {
  cursor: pointer;
}

.capture-result {
  flex: 1;
  margin-top: 20px;
  padding: 20px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  display: flex;
  flex-direction: column;
}

.result-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 16px;
}

.result-header h4 {
  margin: 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.result-actions {
  display: flex;
  gap: 10px;
}

.image-preview {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 1px solid var(--border-color);
  border-radius: var(--radius-md);
  background-color: var(--bg-primary);
  overflow: auto;
}

.image-preview img {
  max-width: 100%;
  max-height: 100%;
  object-fit: contain;
}

.capture-info {
  flex: 1;
  margin-top: 20px;
  padding: 40px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  color: var(--text-secondary);
  gap: 12px;
}
</style>