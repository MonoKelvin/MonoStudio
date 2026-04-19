<template>
  <BasePanel class="panel base64-encoder-panel">
    <div class="base64-encoder-container">
      <div class="encoder-header">
        <h3>{{ toolName }}</h3>
        <div class="action-buttons">
          <BaseButton size="sm" @click="encode">编码</BaseButton>
          <BaseButton size="sm" @click="decode">解码</BaseButton>
          <BaseButton size="sm" @click="clear">清空</BaseButton>
        </div>
      </div>
      
      <div class="input-section">
        <label>输入文本</label>
        <BaseTextarea 
          v-model="inputText" 
          placeholder="请输入要编码或解码的文本..."
          :rows="6"
        />
      </div>
      
      <div class="output-section">
        <div class="output-header">
          <label>输出结果</label>
          <BaseButton size="sm" @click="copyOutput" :disabled="!outputText">
            复制
          </BaseButton>
        </div>
        <BaseTextarea 
          v-model="outputText" 
          placeholder="编码或解码结果将显示在这里..."
          :rows="6"
          readonly
        />
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseTextarea from '../../../components/base/BaseTextarea.vue';
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'Base64EncoderToolView',
  components: {
    BaseTextarea,
    BaseButton
  },
  props: {
    toolName: {
      type: String,
      default: 'Base64 工具'
    },
    toolDescription: {
      type: String,
      default: 'Base64 编码和解码'
    }
  },
  data() {
    return {
      inputText: '',
      outputText: ''
    };
  },
  methods: {
    encode() {
      if (!this.inputText) {
        this.outputText = '';
        return;
      }
      try {
        this.outputText = btoa(unescape(encodeURIComponent(this.inputText)));
      } catch (error) {
        this.outputText = '编码错误: ' + error.message;
      }
    },
    decode() {
      if (!this.inputText) {
        this.outputText = '';
        return;
      }
      try {
        this.outputText = decodeURIComponent(escape(atob(this.inputText)));
      } catch (error) {
        this.outputText = '解码错误: ' + error.message;
      }
    },
    clear() {
      this.inputText = '';
      this.outputText = '';
    },
    copyOutput() {
      if (this.outputText) {
        navigator.clipboard.writeText(this.outputText).then(() => {
          alert('已复制到剪贴板');
        }).catch(err => {
          console.error('复制失败:', err);
        });
      }
    }
  }
};
</script>

<style scoped>
.base64-encoder-panel {
  height: 100%;
}

.base64-encoder-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.encoder-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.encoder-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.action-buttons {
  display: flex;
  gap: 10px;
}

.input-section,
.output-section {
  margin-bottom: 20px;
  flex: 1;
}

.input-section label,
.output-section label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.output-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
}

.output-header label {
  margin: 0;
}

.BaseTextarea {
  width: 100%;
  box-sizing: border-box;
}
</style>