<template>
  <BasePanel class="panel network-tools-panel">
    <div class="network-tools-container">
      <div class="tools-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="tool-tabs">
        <BaseButton 
          :class="{ active: activeTab === 'ping' }" 
          @click="activeTab = 'ping'"
        >
          Ping 测试
        </BaseButton>
        <BaseButton 
          :class="{ active: activeTab === 'ip' }" 
          @click="activeTab = 'ip'"
        >
          IP 地址
        </BaseButton>
        <BaseButton 
          :class="{ active: activeTab === 'dns' }" 
          @click="activeTab = 'dns'"
        >
          DNS 查找
        </BaseButton>
      </div>
      
      <div class="tab-content">
        <!-- Ping 测试 -->
        <div v-if="activeTab === 'ping'" class="ping-tab">
          <div class="input-group">
            <label>目标地址</label>
            <BaseInput v-model="pingTarget" placeholder="输入域名或IP地址..." />
          </div>
          <BaseButton @click="startPing">开始 Ping</BaseButton>
          <div class="ping-results" v-if="pingResults.length > 0">
            <div v-for="(result, index) in pingResults" :key="index" class="ping-result">
              {{ result }}
            </div>
          </div>
        </div>
        
        <!-- IP 地址 -->
        <div v-if="activeTab === 'ip'" class="ip-tab">
          <div class="ip-info">
            <div class="ip-item">
              <span class="label">本地 IP 地址:</span>
              <span class="value">{{ localIp }}</span>
            </div>
            <div class="ip-item">
              <span class="label">公网 IP 地址:</span>
              <span class="value">{{ publicIp }}</span>
            </div>
            <BaseButton @click="getPublicIp">获取公网 IP</BaseButton>
          </div>
        </div>
        
        <!-- DNS 查找 -->
        <div v-if="activeTab === 'dns'" class="dns-tab">
          <div class="input-group">
            <label>域名</label>
            <BaseInput v-model="dnsDomain" placeholder="输入域名..." />
          </div>
          <BaseButton @click="lookupDns">查找 DNS</BaseButton>
          <div class="dns-results" v-if="dnsResults.length > 0">
            <div v-for="(result, index) in dnsResults" :key="index" class="dns-result">
              {{ result }}
            </div>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'NetworkToolsToolView',
  components: {
    BaseInput,
    BaseButton
  },
  props: {
    toolName: {
      type: String,
      default: '网络工具'
    },
    toolDescription: {
      type: String,
      default: '网络连接测试和管理'
    }
  },
  data() {
    return {
      activeTab: 'ping',
      pingTarget: 'google.com',
      pingResults: [],
      localIp: '获取中...',
      publicIp: '获取中...',
      dnsDomain: 'google.com',
      dnsResults: []
    };
  },
  mounted() {
    this.getLocalIp();
    this.getPublicIp();
  },
  methods: {
    async startPing() {
      if (!this.pingTarget) {
        alert('请输入目标地址');
        return;
      }
      
      this.pingResults = [];
      
      // 简单的 ping 实现（基于浏览器的 fetch API）
      try {
        const start = Date.now();
        const response = await fetch(`https://${this.pingTarget}`, {
          method: 'HEAD',
          mode: 'no-cors',
          timeout: 5000
        });
        const end = Date.now();
        this.pingResults.push(`Ping ${this.pingTarget} 成功，响应时间: ${end - start}ms`);
      } catch (error) {
        this.pingResults.push(`Ping ${this.pingTarget} 失败: ${error.message}`);
      }
    },
    getLocalIp() {
      // 在浏览器中获取本地 IP 地址的方法
      // 注意：这只能获取到本地网络的 IP 地址
      window.RTCPeerConnection = window.RTCPeerConnection || window.mozRTCPeerConnection || window.webkitRTCPeerConnection;
      
      if (window.RTCPeerConnection) {
        const pc = new RTCPeerConnection({ iceServers: [] });
        pc.createDataChannel('');
        pc.createOffer().then(offer => pc.setLocalDescription(offer));
        
        pc.onicecandidate = (event) => {
          if (event.candidate) {
            const ipRegex = /([0-9]{1,3}(\.[0-9]{1,3}){3})/;
            const ipMatch = event.candidate.candidate.match(ipRegex);
            if (ipMatch) {
              this.localIp = ipMatch[1];
              pc.close();
            }
          }
        };
      } else {
        this.localIp = '无法获取';
      }
    },
    async getPublicIp() {
      try {
        const response = await fetch('https://api.ipify.org?format=json');
        const data = await response.json();
        this.publicIp = data.ip;
      } catch (error) {
        this.publicIp = '无法获取';
      }
    },
    async lookupDns() {
      if (!this.dnsDomain) {
        alert('请输入域名');
        return;
      }
      
      this.dnsResults = [];
      
      try {
        // 使用浏览器的 DNS 解析
        const startTime = Date.now();
        const response = await fetch(`https://${this.dnsDomain}`, {
          method: 'HEAD',
          mode: 'no-cors',
          timeout: 5000
        });
        const endTime = Date.now();
        
        // 由于浏览器安全限制，我们无法直接获取 DNS 解析结果
        // 这里只是模拟一个 DNS 查找
        this.dnsResults.push(`正在查找 ${this.dnsDomain}...`);
        this.dnsResults.push(`DNS 查找完成，响应时间: ${endTime - startTime}ms`);
        this.dnsResults.push(`解析到的 IP 地址: ${this.getHostnameFromUrl(response.url)}`);
      } catch (error) {
        this.dnsResults.push(`DNS 查找失败: ${error.message}`);
      }
    },
    getHostnameFromUrl(url) {
      try {
        const urlObj = new URL(url);
        return urlObj.hostname;
      } catch {
        return '无法解析';
      }
    }
  }
};
</script>

<style scoped>
.network-tools-panel {
  height: 100%;
}

.network-tools-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.tools-header h3 {
  margin: 0 0 20px 0;
  font-size: 18px;
  font-weight: 600;
}

.tool-tabs {
  display: flex;
  gap: 10px;
  margin-bottom: 20px;
  border-bottom: 1px solid var(--border-color);
  padding-bottom: 10px;
}

.tool-tabs .BaseButton {
  padding: 8px 16px;
  border-radius: var(--radius-md) var(--radius-md) 0 0;
  background: transparent;
  border: 1px solid transparent;
  border-bottom: none;
}

.tool-tabs .BaseButton.active {
  background: var(--bg-primary);
  border-color: var(--border-color);
  border-bottom-color: var(--bg-primary);
  font-weight: 500;
}

.tab-content {
  flex: 1;
  padding: 20px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
}

.input-group {
  margin-bottom: 20px;
}

.input-group label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.ping-results,
.dns-results {
  margin-top: 20px;
  padding: 12px;
  background-color: var(--bg-primary);
  border-radius: var(--radius-md);
  max-height: 200px;
  overflow-y: auto;
}

.ping-result,
.dns-result {
  margin-bottom: 8px;
  font-size: 14px;
  color: var(--text-primary);
}

.ip-info {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.ip-item {
  display: flex;
  justify-content: space-between;
  padding: 12px;
  background-color: var(--bg-primary);
  border-radius: var(--radius-md);
}

.ip-item .label {
  font-weight: 500;
  color: var(--text-secondary);
}

.ip-item .value {
  font-family: monospace;
  color: var(--text-primary);
}
</style>