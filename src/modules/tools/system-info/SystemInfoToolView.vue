<template>
  <BasePanel class="panel system-info-panel">
    <div class="system-info-container">
      <div class="info-header">
        <h3>{{ toolName }}</h3>
        <div class="header-actions">
          <BaseButton @click="copySystemInfo" class="copy-button">
            <img src="../../../assets/icons/copy.svg" class="copy-icon" alt="复制">
            复制
          </BaseButton>
          <BaseButton @click="refreshInfo" class="refresh-button" :disabled="isLoading" v-if="!autoRefreshEnabled">
            <span class="refresh-icon" :class="{ 'rotating': isLoading }">↻</span>
            {{ isLoading ? '获取中...' : '刷新' }}
          </BaseButton>
          <BaseButton @click="toggleAutoRefresh" class="auto-refresh-button" :class="{ 'auto-refresh-enabled': autoRefreshEnabled }">
            {{ autoRefreshEnabled ? '自动刷新' : '手动刷新' }}
          </BaseButton>
        </div>
      </div>
      
      <div class="info-sections">
        <!-- 系统概览（常用信息） -->
        <div class="info-section overview-section">
          <div class="section-header">
            <div class="section-icon system-icon">⚙️</div>
            <h4>系统概览</h4>
          </div>
          <div class="info-grid overview-grid">
            <InfoCard 
              icon="🖥️" 
              label="操作系统" 
              :value="systemInfo.os.type" 
              :subtitle="systemInfo.os.version" 
              :highlighted="systemInfo.os.type !== '未知'"
              icon-class="os-icon"
            />
            <InfoCard 
              icon="💻" 
              label="CPU" 
              :value="systemInfo.hardware.cpu" 
              :subtitle="systemInfo.hardware.cpuCores + '核心'" 
              :highlighted="systemInfo.hardware.cpu !== '未知'"
              icon-class="cpu-icon"
            />
            <InfoCard 
              icon="🧠" 
              label="内存" 
              :value="systemInfo.hardware.memory" 
              :subtitle="systemInfo.hardware.freeMemory + '可用'" 
              :highlighted="systemInfo.hardware.memory !== '未知'"
              icon-class="memory-icon"
            />
            <InfoCard 
              icon="🖼️" 
              label="屏幕分辨率" 
              :value="systemInfo.hardware.screen" 
              :subtitle="systemInfo.hardware.pixelDensity + 'DPI'" 
              :highlighted="systemInfo.hardware.screen !== '未知'"
              icon-class="screen-icon"
            />
            <InfoCard 
              icon="🔋" 
              label="电池状态" 
              :value="systemInfo.hardware.battery" 
              :highlighted="systemInfo.hardware.battery !== '未知'"
              icon-class="battery-icon"
            />
            <InfoCard 
              icon="🎮" 
              label="GPU" 
              :value="systemInfo.gpu.model" 
              :subtitle="systemInfo.gpu.vendor" 
              :highlighted="systemInfo.gpu.model !== '未知'"
              icon-class="gpu-icon"
            />
            <InfoCard 
              icon="📡" 
              label="网络状态" 
              :value="systemInfo.network.online ? '在线' : '离线'" 
              :subtitle="systemInfo.network.connectionType" 
              :highlighted="systemInfo.network.online"
              icon-class="network-icon"
            />
            <InfoCard 
              icon="⏰" 
              label="运行时间" 
              :value="systemInfo.os.uptime" 
              :subtitle="'启动于 ' + systemBootTime" 
              :highlighted="systemInfo.os.uptime !== '未知'"
              icon-class="uptime-icon"
            />
          </div>
        </div>
        
        <!-- 操作系统信息 -->
        <div class="info-section">
          <div class="section-header">
            <div class="section-icon os-section-icon">🖥️</div>
            <h4>操作系统</h4>
          </div>
          <OsInfo 
            :type="systemInfo.os.type"
            :version="systemInfo.os.version"
            :arch="systemInfo.os.arch"
            :platform="systemInfo.os.platform"
            :release="systemInfo.os.release"
            :hostname="systemInfo.os.hostname"
            :uptime="systemInfo.os.uptime"
            :boot-time="systemBootTime"
          />
        </div>
        
        <!-- 硬件信息 -->
        <div class="info-section">
          <div class="section-header">
            <div class="section-icon hardware-icon">🔧</div>
            <h4>硬件信息</h4>
          </div>
          <HardwareInfo 
            :cpu="systemInfo.hardware.cpu"
            :cpu-cores="systemInfo.hardware.cpuCores"
            :memory="systemInfo.hardware.memory"
            :free-memory="systemInfo.hardware.freeMemory"
            :battery="systemInfo.hardware.battery"
            :screen="systemInfo.hardware.screen"
            :pixel-density="systemInfo.hardware.pixelDensity"
            :color-depth="systemInfo.hardware.colorDepth"
          />
        </div>
        
        <!-- 存储信息 -->
        <div class="info-section">
          <div class="section-header">
            <div class="section-icon storage-icon">💾</div>
            <h4>存储信息</h4>
          </div>
          <StorageInfo :drives="systemInfo.storage.drives" />
        </div>
        
        <!-- 网络信息 -->
        <div class="info-section">
          <div class="section-header">
            <div class="section-icon network-icon">📡</div>
            <h4>网络信息</h4>
          </div>
          <NetworkInfo 
            :online="systemInfo.network.online"
            :connection-type="systemInfo.network.connectionType"
            :local-ip="systemInfo.network.localIp"
            :public-ip="systemInfo.network.publicIp"
            :ping="systemInfo.network.ping"
            :dns-servers="systemInfo.network.dnsServers"
          />
        </div>
        
        <!-- GPU信息 -->
        <div class="info-section">
          <div class="section-header">
            <div class="section-icon gpu-icon">�</div>
            <h4>GPU信息</h4>
          </div>
          <div class="info-grid">
            <div class="info-item">
              <span class="label">GPU型号:</span>
              <span class="value">{{ systemInfo.gpu.model || '未知' }}</span>
            </div>
            <div class="info-item">
              <span class="label">GPU供应商:</span>
              <span class="value">{{ systemInfo.gpu.vendor || '未知' }}</span>
            </div>
            <div class="info-item">
              <span class="label">WebGL版本:</span>
              <span class="value">{{ systemInfo.gpu.webglVersion || '未知' }}</span>
            </div>
          </div>
        </div>
        
        <!-- 应用信息 -->
        <div class="info-section">
          <div class="section-header">
            <div class="section-icon app-icon">📱</div>
            <h4>应用信息</h4>
          </div>
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
            <div class="info-item">
              <span class="label">Node.js 版本:</span>
              <span class="value">{{ systemInfo.app.nodeVersion || '未知' }}</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import InfoCard from './components/InfoCard.vue';
import OsInfo from './components/OsInfo.vue';
import HardwareInfo from './components/HardwareInfo.vue';
import StorageInfo from './components/StorageInfo.vue';
import NetworkInfo from './components/NetworkInfo.vue';
import { nextTick } from 'vue';

export default {
  name: 'SystemInfoToolView',
  components: {
    BaseButton,
    InfoCard,
    OsInfo,
    HardwareInfo,
    StorageInfo,
    NetworkInfo
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
          type: '未知',
          version: '未知',
          arch: '未知',
          platform: '未知',
          release: '未知',
          hostname: '未知',
          uptime: '未知'
        },
        hardware: {
          cpu: '未知',
          cpuCores: '未知',
          memory: '未知',
          freeMemory: '未知',
          screen: '未知',
          pixelDensity: '未知',
          colorDepth: '未知',
          battery: '未知'
        },
        gpu: {
          model: '未知',
          vendor: '未知',
          webglVersion: '未知'
        },
        browser: {
          name: '未知',
          version: '未知',
          userAgent: '未知'
        },
        network: {
          online: false,
          connectionType: '未知',
          localIp: '未知',
          publicIp: '未知',
          ping: '未知',
          dnsServers: '未知'
        },
        storage: {
          drives: []
        },
        app: {
          version: '未知',
          path: '未知',
          electronVersion: '未知',
          nodeVersion: '未知'
        }
      },
      isLoading: false,
      lastRefreshTime: 0,
      refreshInterval: 5000, // 5秒内不重复刷新
      autoRefreshInterval: null, // 自动刷新定时器
      autoRefreshEnabled: true, // 是否启用自动刷新
      isVisible: true, // 组件是否可见
      dataCache: {
        systemInfo: null,
        timestamp: 0,
        cacheDuration: 10000 // 缓存有效期10秒
      }
    };
  },
  computed: {
    systemBootTime() {
      try {
        // 从系统运行时间计算启动时间
        // 注意：这里需要解析uptime字符串，格式如"2小时 30分钟"
        const uptimeStr = this.systemInfo.os.uptime;
        if (uptimeStr && uptimeStr !== '未知') {
          // 解析运行时间字符串
          const hoursMatch = uptimeStr.match(/(\d+)小时/);
          const minutesMatch = uptimeStr.match(/(\d+)分钟/);
          
          let uptimeSeconds = 0;
          if (hoursMatch) {
            uptimeSeconds += parseInt(hoursMatch[1]) * 3600;
          }
          if (minutesMatch) {
            uptimeSeconds += parseInt(minutesMatch[1]) * 60;
          }
          
          if (uptimeSeconds > 0) {
            const bootTime = new Date(Date.now() - uptimeSeconds * 1000);
            return bootTime.toLocaleString();
          }
        }
        return '未知';
      } catch (error) {
        console.error('计算启动时间失败:', error);
        return '未知';
      }
    }
  },
  mounted() {
    this.getSystemInfo();
    // 启动自动刷新
    this.startAutoRefresh();
    // 添加页面可见性检测
    this.handleVisibilityChange = () => {
      this.isVisible = !document.hidden;
      if (this.isVisible && this.autoRefreshEnabled) {
        this.refreshInfo();
      }
    };
    document.addEventListener('visibilitychange', this.handleVisibilityChange);
  },
  beforeUnmount() {
    // 清除自动刷新定时器
    this.stopAutoRefresh();
    // 移除页面可见性监听
    if (this.handleVisibilityChange) {
      document.removeEventListener('visibilitychange', this.handleVisibilityChange);
    }
  },
  methods: {
    startAutoRefresh() {
      if (this.autoRefreshEnabled) {
        // 使用 requestAnimationFrame 实现更平滑的自动刷新
        const refresh = () => {
          if (this.autoRefreshEnabled) {
            // 每30秒刷新一次
            setTimeout(() => {
              this.refreshInfo();
              requestAnimationFrame(refresh);
            }, 30000);
          }
        };
        requestAnimationFrame(refresh);
      }
    },
    stopAutoRefresh() {
      if (this.autoRefreshInterval) {
        clearInterval(this.autoRefreshInterval);
        this.autoRefreshInterval = null;
      }
    },
    toggleAutoRefresh() {
      this.autoRefreshEnabled = !this.autoRefreshEnabled;
      if (this.autoRefreshEnabled) {
        this.startAutoRefresh();
      } else {
        this.stopAutoRefresh();
      }
    },
    async getSystemInfo() {
      // 检查缓存是否有效
      const now = Date.now();
      if (this.dataCache.systemInfo && (now - this.dataCache.timestamp) < this.dataCache.cacheDuration) {
        // 使用缓存数据
        this.systemInfo = JSON.parse(JSON.stringify(this.dataCache.systemInfo));
        this.isLoading = false;
        return;
      }
      
      // 检查是否在 Electron 环境中
      const isElectron = window.electronAPI !== undefined;
      
      this.isLoading = true;
      
      try {
        // 操作系统信息
        if (isElectron && window.electronAPI.systemInfo) {
          // 在 Electron 环境中使用主进程 API 获取更详细的系统信息
          const info = await window.electronAPI.systemInfo.getSystemInfo();
          
          // 使用 nextTick 确保 DOM 更新不会阻塞渲染
          await nextTick();
          
          // 操作系统信息
          this.systemInfo.os.type = info.os.type || navigator.platform;
          this.systemInfo.os.version = info.os.version || navigator.appVersion;
          this.systemInfo.os.arch = info.os.arch || (navigator.platform.includes('64') ? '64位' : '32位');
          this.systemInfo.os.platform = info.os.platform || navigator.userAgent;
          this.systemInfo.os.release = info.os.release || '未知';
          this.systemInfo.os.hostname = info.os.hostname || '未知';
          this.systemInfo.os.uptime = info.os.uptime || '未知';
          
          await nextTick();
          
          // 硬件信息
          this.systemInfo.hardware.cpu = info.hardware.cpu || '未知';
          this.systemInfo.hardware.cpuCores = info.hardware.cpuCores || '未知';
          this.systemInfo.hardware.memory = info.hardware.memory || '未知';
          this.systemInfo.hardware.freeMemory = info.hardware.freeMemory || '未知';
          this.systemInfo.hardware.battery = info.hardware.battery || '未知';
          
          await nextTick();
          
          // 屏幕信息（异步获取）
          this.systemInfo.hardware.screen = info.hardware.screen || `${window.screen.width}x${window.screen.height}`;
          this.systemInfo.hardware.pixelDensity = `${window.devicePixelRatio}x`;
          this.systemInfo.hardware.colorDepth = `${window.screen.colorDepth}位`;
          
          await nextTick();
          
          // 网络信息
          this.systemInfo.network.online = true; // 假设在Electron环境中网络是在线的
          this.systemInfo.network.connectionType = '未知';
          this.systemInfo.network.localIp = info.network?.localIp || '未知';
          this.systemInfo.network.publicIp = info.network?.publicIp || '未知';
          this.systemInfo.network.ping = info.network?.ping || '未知';
          this.systemInfo.network.dnsServers = info.network?.dnsServers || '未知';
          
          await nextTick();
          
          // 存储信息
          this.systemInfo.storage.drives = info.storage?.drives || [];
          
          await nextTick();
          
          // 应用信息
          this.systemInfo.app.version = info.app.version || '1.0.0';
          this.systemInfo.app.path = info.app.path || '未知';
          this.systemInfo.app.electronVersion = info.app.electronVersion || '未知';
          this.systemInfo.app.nodeVersion = info.app.nodeVersion || '未知';
          
          await nextTick();
          
          // GPU信息（异步获取）
          this.systemInfo.gpu.model = info.gpu?.model || '未知';
          this.systemInfo.gpu.vendor = info.gpu?.vendor || '未知';
          this.systemInfo.gpu.webglVersion = info.gpu?.webglVersion || '未知';
        } else {
          // 在浏览器环境中获取系统信息
          await this.getBrowserSystemInfo();
        }
        
        // 更新缓存
        this.dataCache.systemInfo = JSON.parse(JSON.stringify(this.systemInfo));
        this.dataCache.timestamp = now;
      } catch (error) {
        console.error('获取系统信息失败:', error);
        await this.getBrowserSystemInfo();
      } finally {
        await nextTick();
        this.isLoading = false;
      }
    },
    async getBrowserSystemInfo() {
      try {
        this.isLoading = true;
        
        // 操作系统信息
        this.systemInfo.os.type = navigator.platform;
        this.systemInfo.os.version = navigator.appVersion;
        this.systemInfo.os.arch = navigator.platform.includes('64') ? '64位' : '32位';
        this.systemInfo.os.platform = navigator.userAgent;
        this.systemInfo.os.release = '未知 (浏览器环境)';
        this.systemInfo.os.hostname = '未知 (浏览器环境)';
        this.systemInfo.os.uptime = '未知 (浏览器环境)';
        
        await nextTick();
        
        // 硬件信息
        this.systemInfo.hardware.cpu = '未知 (浏览器环境)';
        this.systemInfo.hardware.cpuCores = '未知 (浏览器环境)';
        this.systemInfo.hardware.memory = '未知 (浏览器环境)';
        this.systemInfo.hardware.freeMemory = '未知 (浏览器环境)';
        this.systemInfo.hardware.battery = '未知 (浏览器环境)';
        
        await nextTick();
        
        // 屏幕信息
        try {
          this.systemInfo.hardware.screen = `${window.screen.width}x${window.screen.height}`;
          this.systemInfo.hardware.pixelDensity = `${window.devicePixelRatio}x`;
          this.systemInfo.hardware.colorDepth = `${window.screen.colorDepth}位`;
        } catch (error) {
          console.error('获取屏幕信息失败:', error);
          this.systemInfo.hardware.screen = '未知';
          this.systemInfo.hardware.pixelDensity = '未知';
          this.systemInfo.hardware.colorDepth = '未知';
        }
        
        await nextTick();
        
        // 浏览器信息（异步解析）
        const browserInfo = await this.parseBrowserInfo();
        this.systemInfo.browser.name = browserInfo.name;
        this.systemInfo.browser.version = browserInfo.version;
        this.systemInfo.browser.userAgent = navigator.userAgent;
        
        await nextTick();
        
        // GPU信息（异步获取）
        const gpuInfo = await this.getGpuInfo();
        this.systemInfo.gpu.model = gpuInfo.model;
        this.systemInfo.gpu.vendor = gpuInfo.vendor;
        this.systemInfo.gpu.webglVersion = gpuInfo.webglVersion;
        
        await nextTick();
        
        // 网络信息
        this.systemInfo.network.online = navigator.onLine;
        this.systemInfo.network.connectionType = this.getConnectionType();
        this.systemInfo.network.localIp = '未知 (浏览器环境)';
        this.systemInfo.network.publicIp = '未知 (浏览器环境)';
        this.systemInfo.network.ping = '未知 (浏览器环境)';
        this.systemInfo.network.dnsServers = '未知 (浏览器环境)';
        
        await nextTick();
        
        // 存储信息
        this.systemInfo.storage.drives = [];
        
        await nextTick();
        
        // 应用信息
        this.systemInfo.app.version = '1.0.0';
        this.systemInfo.app.path = '未知 (浏览器环境)';
        this.systemInfo.app.electronVersion = '未知 (浏览器环境)';
        this.systemInfo.app.nodeVersion = '未知 (浏览器环境)';
        
        await nextTick();
      } catch (error) {
        console.error('获取浏览器系统信息失败:', error);
      } finally {
        await nextTick();
        this.isLoading = false;
      }
    },
    // 解析浏览器信息（异步）
    parseBrowserInfo() {
      return new Promise(resolve => {
        setTimeout(() => {
          const userAgent = navigator.userAgent;
          let browserName = '未知';
          let browserVersion = '未知';
          
          try {
            if (userAgent.includes('Chrome') && !userAgent.includes('Edg')) {
              browserName = 'Chrome';
              const match = userAgent.match(/Chrome\/(\d+\.\d+)/);
              browserVersion = match ? match[1] : '未知';
            } else if (userAgent.includes('Firefox')) {
              browserName = 'Firefox';
              const match = userAgent.match(/Firefox\/(\d+\.\d+)/);
              browserVersion = match ? match[1] : '未知';
            } else if (userAgent.includes('Safari') && !userAgent.includes('Chrome')) {
              browserName = 'Safari';
              const match = userAgent.match(/Version\/(\d+\.\d+)/);
              browserVersion = match ? match[1] : '未知';
            } else if (userAgent.includes('Edge')) {
              browserName = 'Edge';
              const match = userAgent.match(/Edge\/(\d+\.\d+)/);
              browserVersion = match ? match[1] : '未知';
            } else if (userAgent.includes('MSIE') || userAgent.includes('Trident/')) {
              browserName = 'Internet Explorer';
              const match = userAgent.match(/(MSIE|rv):(\d+\.\d+)/);
              browserVersion = match ? match[2] : '未知';
            }
          } catch (error) {
            console.error('解析浏览器信息失败:', error);
          }
          
          resolve({ name: browserName, version: browserVersion });
        }, 0);
      });
    },
    // 获取GPU信息（异步）
    getGpuInfo() {
      return new Promise(resolve => {
        setTimeout(() => {
          let model = '未知';
          let vendor = '未知';
          let webglVersion = '未知';
          
          try {
            const canvas = document.createElement('canvas');
            const gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
            if (gl) {
              const debugInfo = gl.getExtension('WEBGL_debug_renderer_info');
              if (debugInfo) {
                vendor = gl.getParameter(debugInfo.UNMASKED_VENDOR_WEBGL) || '未知';
                model = gl.getParameter(debugInfo.UNMASKED_RENDERER_WEBGL) || '未知';
              }
              webglVersion = gl.getParameter(gl.VERSION) || '未知';
            }
          } catch (error) {
            console.error('获取GPU信息失败:', error);
          }
          
          resolve({ model, vendor, webglVersion });
        }, 0);
      });
    },
    getConnectionType() {
      // 尝试获取网络连接类型
      if (navigator.connection) {
        return navigator.connection.effectiveType || '未知';
      }
      return '未知';
    },
    refreshInfo() {
      if (!this.isVisible) {
        return; // 组件不可见时不刷新
      }
      const now = Date.now();
      if (now - this.lastRefreshTime < this.refreshInterval) {
        return; // 防止频繁刷新
      }
      this.lastRefreshTime = now;
      this.getSystemInfo();
    },
    copySystemInfo() {
      // 生成系统信息文本
      let infoText = `系统信息\n`;
      infoText += `================\n`;
      infoText += `操作系统: ${this.systemInfo.os.type} ${this.systemInfo.os.version}\n`;
      infoText += `CPU: ${this.systemInfo.hardware.cpu} (${this.systemInfo.hardware.cpuCores}核心)\n`;
      infoText += `内存: ${this.systemInfo.hardware.memory}\n`;
      infoText += `可用内存: ${this.systemInfo.hardware.freeMemory}\n`;
      infoText += `屏幕分辨率: ${this.systemInfo.hardware.screen}\n`;
      infoText += `电池状态: ${this.systemInfo.hardware.battery}\n`;
      infoText += `本地IP: ${this.systemInfo.network.localIp}\n`;
      infoText += `公网IP: ${this.systemInfo.network.publicIp}\n`;
      infoText += `网络延迟: ${this.systemInfo.network.ping}\n`;
      infoText += `启动时间: ${this.systemBootTime}\n`;
      
      // 复制到剪贴板
      navigator.clipboard.writeText(infoText).then(() => {
        // 可以添加一个提示
        console.log('系统信息已复制到剪贴板');
      }).catch(err => {
        console.error('复制失败:', err);
      });
    },
  }
};
</script>

<style scoped>
.system-info-panel {
  height: 100%;
}

.system-info-container {
  padding: 16px;
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 16px;
  background-color: var(--surface-1, #f9fafb);
}

.info-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
  padding-bottom: 12px;
  border-bottom: 1px solid var(--border-color, #e5e7eb);
}

.header-actions {
  display: flex;
  gap: 8px;
  align-items: center;
}

.copy-button {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 8px 16px;
  background-color: var(--surface-2, #ffffff);
  border: 1px solid var(--border-color, #e5e7eb);
  border-radius: var(--radius-md, 6px);
  font-size: 13px;
  font-weight: 400;
  color: var(--text-primary, #111827);
  cursor: pointer;
  transition: all 0.2s ease;
  box-shadow: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
}

.copy-button:hover {
  background-color: var(--surface-3, #f3f4f6);
  border-color: var(--accent-color, #3b82f6);
  transform: translateY(-1px);
}

.copy-icon {
  width: 16px;
  height: 16px;
  margin-right: 6px;
  flex-shrink: 0;
  filter: brightness(0);
}

[data-theme="dark"] .copy-icon {
  filter: brightness(0) invert(1);
}

.auto-refresh-button {
  position: relative;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  padding: 8px 16px;
  background-color: var(--surface-2, #ffffff);
  border: 1px solid var(--border-color, #e5e7eb);
  border-radius: var(--radius-md, 6px);
  font-size: 13px;
  font-weight: 400;
  color: var(--text-primary, #111827);
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
}

.auto-refresh-button:hover {
  background-color: var(--surface-3, #f3f4f6);
  border-color: var(--accent-color, #3b82f6);
  transform: translateY(-1px);
}

.auto-refresh-button.auto-refresh-enabled {
  background: linear-gradient(135deg, var(--accent-color, #3b82f6) 0%, #2563eb 50%, var(--accent-color, #3b82f6) 100%);
  background-size: 200% 200%;
  color: white;
  border-color: transparent;
  animation: gradientShift 3s ease infinite, refreshPulse 2s ease-in-out infinite;
}

.auto-refresh-button.auto-refresh-enabled:hover {
  background: linear-gradient(135deg, #2563eb 0%, #1d4ed8 50%, #2563eb 100%);
  background-size: 200% 200%;
  animation: gradientShift 2s ease infinite, refreshPulse 1.5s ease-in-out infinite;
  border-color: transparent;
}

@keyframes gradientShift {
  0% {
    background-position: 0% 50%;
  }
  50% {
    background-position: 100% 50%;
  }
  100% {
    background-position: 0% 50%;
  }
}

@keyframes refreshPulse {
  0%, 100% {
    box-shadow: 0 0 0 0 rgba(59, 130, 246, 0.4);
  }
  50% {
    box-shadow: 0 0 0 8px rgba(59, 130, 246, 0);
  }
}

.info-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
  color: var(--text-primary, #111827);
  letter-spacing: -0.01em;
}

.refresh-button {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 8px 16px;
  background-color: var(--surface-2, #ffffff);
  border: 1px solid var(--border-color, #e5e7eb);
  border-radius: var(--radius-md, 6px);
  font-size: 13px;
  font-weight: 500;
  color: var(--text-primary, #111827);
  cursor: pointer;
  transition: all 0.2s ease;
  box-shadow: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
}

.refresh-button:hover:not(:disabled) {
  background-color: var(--surface-3, #f3f4f6);
  border-color: var(--accent-color, #3b82f6);
  transform: translateY(-1px);
}

.refresh-button:disabled {
  background-color: var(--surface-3, #f3f4f6);
  color: var(--text-secondary, #4b5563);
  cursor: not-allowed;
  transform: none;
  border-color: var(--border-color, #e5e7eb);
}

.refresh-icon {
  font-size: 14px;
  transition: transform 0.3s ease;
}

.refresh-button:hover .refresh-icon:not(.rotating) {
  transform: rotate(180deg);
}

.refresh-icon.rotating {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}

.info-sections {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 16px;
  overflow-y: auto;
  padding-right: 8px;
}

.info-sections::-webkit-scrollbar {
  width: 6px;
}

.info-sections::-webkit-scrollbar-track {
  background: var(--surface-2, #ffffff);
  border-radius: 3px;
}

.info-sections::-webkit-scrollbar-thumb {
  background: var(--border-color, #e5e7eb);
  border-radius: 3px;
}

.info-sections::-webkit-scrollbar-thumb:hover {
  background: var(--text-tertiary, #9ca3af);
}

.info-section {
  background-color: var(--surface-2, #ffffff);
  padding: 16px;
  border-radius: var(--radius-md, 8px);
  box-shadow: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
  border: 1px solid var(--border-color, #e5e7eb);
}

.info-section:hover {
  box-shadow: 0 2px 4px 0 rgba(0, 0, 0, 0.1);
}

.section-header {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 12px;
  padding-bottom: 10px;
  border-bottom: 1px solid var(--border-color, #e5e7eb);
}

.section-icon {
  font-size: 18px;
  width: 36px;
  height: 36px;
  display: flex;
  align-items: center;
  justify-content: center;
  background-color: var(--surface-3, #f3f4f6);
  border-radius: var(--radius-sm, 4px);
  color: var(--accent-color, #3b82f6);
  flex-shrink: 0;
}

.section-header h4 {
  margin: 0;
  font-size: 15px;
  font-weight: 600;
  color: var(--text-primary, #111827);
}

/* 概览网格 */
.overview-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 12px;
}

/* 信息网格 */
.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 12px;
}

.info-item {
  display: flex;
  flex-direction: column;
  gap: 6px;
  padding: 12px;
  background-color: var(--surface-1, #f9fafb);
  border-radius: var(--radius-sm, 4px);
  border: 1px solid var(--border-color, #e5e7eb);
  transition: all 0.2s ease;
}

.info-item:hover {
  border-color: var(--accent-color, #3b82f6);
}

.info-item .label {
  font-size: 12px;
  font-weight: 500;
  color: var(--text-secondary, #4b5563);
  text-transform: uppercase;
  letter-spacing: 0.03em;
}

.info-item .value {
  font-size: 13px;
  font-weight: 500;
  color: var(--text-primary, #111827);
  word-break: break-all;
  line-height: 1.4;
  white-space: normal;
  overflow: visible;
  text-overflow: clip;
}

/* 响应式设计 */
@media (max-width: 768px) {
  .system-info-container {
    padding: 16px;
    gap: 16px;
  }
  
  .info-header {
    padding-bottom: 12px;
  }
  
  .info-header h3 {
    font-size: 20px;
  }
  
  .refresh-button {
    padding: 8px 16px;
    font-size: 13px;
  }
  
  .info-section {
    padding: 20px;
  }
  
  .section-header {
    gap: 12px;
    margin-bottom: 16px;
  }
  
  .section-icon {
    font-size: 20px;
    width: 40px;
    height: 40px;
  }
  
  .section-header h4 {
    font-size: 16px;
  }
  
  .overview-grid {
    grid-template-columns: 1fr;
    gap: 16px;
  }
  
  .info-grid {
    grid-template-columns: 1fr;
    gap: 16px;
  }
  
  .info-item {
    padding: 14px;
  }
}
</style>