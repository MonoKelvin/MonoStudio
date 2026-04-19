<template>
  <BasePanel class="panel system-info-panel">
    <div class="system-info-container">
      <div class="info-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="refreshInfo">刷新信息</BaseButton>
      </div>
      
      <div class="info-sections">
        <!-- 操作系统信息 -->
        <div class="info-section">
          <h4>操作系统</h4>
          <div class="info-grid">
            <div class="info-item">
              <span class="label">类型:</span>
              <span class="value">{{ systemInfo.os.type }}</span>
            </div>
            <div class="info-item">
              <span class="label">版本:</span>
              <span class="value">{{ systemInfo.os.version }}</span>
            </div>
            <div class="info-item">
              <span class="label">架构:</span>
              <span class="value">{{ systemInfo.os.arch }}</span>
            </div>
            <div class="info-item">
              <span class="label">平台:</span>
              <span class="value">{{ systemInfo.os.platform }}</span>
            </div>
          </div>
        </div>
        
        <!-- 硬件信息 -->
        <div class="info-section">
          <h4>硬件信息</h4>
          <div class="info-grid">
            <div class="info-item">
              <span class="label">CPU:</span>
              <span class="value">{{ systemInfo.hardware.cpu }}</span>
            </div>
            <div class="info-item">
              <span class="label">内存:</span>
              <span class="value">{{ systemInfo.hardware.memory }}</span>
            </div>
            <div class="info-item">
              <span class="label">屏幕分辨率:</span>
              <span class="value">{{ systemInfo.hardware.screen }}</span>
            </div>
          </div>
        </div>
        
        <!-- 应用信息 -->
        <div class="info-section">
          <h4>应用信息</h4>
          <div class="info-grid">
            <div class="info-item">
              <span class="label">版本:</span>
              <span class="value">{{ systemInfo.app.version }}</span>
            </div>
            <div class="info-item">
              <span class="label">应用路径:</span>
              <span class="value">{{ systemInfo.app.path }}</span>
            </div>
            <div class="info-item">
              <span class="label">Electron 版本:</span>
              <span class="value">{{ systemInfo.app.electronVersion }}</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'SystemInfoToolView',
  components: {
    BaseButton
  },
  props: {
    toolName: {
      type: String,
      default: '系统信息'
    },
    toolDescription: {
      type: String,
      default: '查看系统硬件和软件信息'
    }
  },
  data() {
    return {
      systemInfo: {
        os: {
          type: '获取中...',
          version: '获取中...',
          arch: '获取中...',
          platform: '获取中...'
        },
        hardware: {
          cpu: '获取中...',
          memory: '获取中...',
          screen: '获取中...'
        },
        app: {
          version: '获取中...',
          path: '获取中...',
          electronVersion: '获取中...'
        }
      }
    };
  },
  mounted() {
    this.getSystemInfo();
  },
  methods: {
    getSystemInfo() {
      // 在浏览器环境中获取系统信息
      // 注意：在实际的 Electron 应用中，我们会使用主进程的 API 来获取更详细的系统信息
      
      // 操作系统信息
      this.systemInfo.os.type = navigator.platform;
      this.systemInfo.os.version = navigator.appVersion;
      this.systemInfo.os.arch = navigator.platform.includes('64') ? '64位' : '32位';
      this.systemInfo.os.platform = navigator.userAgent;
      
      // 硬件信息
      this.systemInfo.hardware.cpu = '未知 (浏览器环境)';
      this.systemInfo.hardware.memory = '未知 (浏览器环境)';
      this.systemInfo.hardware.screen = `${window.screen.width}x${window.screen.height}`;
      
      // 应用信息
      this.systemInfo.app.version = '1.0.0';
      this.systemInfo.app.path = '未知 (浏览器环境)';
      this.systemInfo.app.electronVersion = '未知 (浏览器环境)';
    },
    refreshInfo() {
      this.getSystemInfo();
    }
  }
};
</script>

<style scoped>
.system-info-panel {
  height: 100%;
}

.system-info-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.info-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.info-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.info-sections {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 20px;
  overflow-y: auto;
}

.info-section {
  background-color: var(--bg-secondary);
  padding: 16px;
  border-radius: var(--radius-md);
}

.info-section h4 {
  margin: 0 0 12px 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 12px;
}

.info-item {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.info-item .label {
  font-size: 12px;
  color: var(--text-tertiary);
}

.info-item .value {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
  word-break: break-all;
}
</style>