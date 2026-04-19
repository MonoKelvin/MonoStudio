<template>
  <div class="settings-section">
    <h3>关于 MonoStudio</h3>
    <p>MonoStudio是一个连开发者都不知道全部功能的神奇工具箱。软件源码全部由 AI 赛博打工人生成，开发者负责取餐。鼠鼠我呀，真的一行代码都不想写了～ (￣ω￣)</p>
    <div class="settings-group">
      <SettingsItem title="项目地址">
        <a href="https://github.com/MonoKelvin" target="_blank" class="settings-link" @click.prevent="openExternal('https://github.com/MonoKelvin')">GitHub</a>
      </SettingsItem>
      <SettingsItem title="开发人员">
        <div class="developers-list">
          <a href="https://trae.io" target="_blank" class="developer-link" @click.prevent="openExternal('https://trae.io')">Trae</a>
          <span class="developer-separator">,</span>
          <a href="https://cursor.sh" target="_blank" class="developer-link" @click.prevent="openExternal('https://cursor.sh')">Cursor</a>
          <span class="developer-separator">,</span>
          <a href="https://deepseek.com" target="_blank" class="developer-link" @click.prevent="openExternal('https://deepseek.com')">DeepSeek</a>
          <span class="developer-separator">,</span>
          <span class="developer-self">混入其中的🐁</span>
        </div>
      </SettingsItem>
      <SettingsItem title="版权信息">
        <span class="settings-value">© 2026 MonoStudio. 保留所有权利</span>
      </SettingsItem>
      <SettingsItem title="隐私政策">
        <span class="privacy-link-text">查看<a href="https://www.freeprivacypolicy.com/live/4a9b3112-1f97-4ae0-af5c-1814f236198e" class="settings-link" @click.prevent="openPrivacyPanel">《{{ isZh ? '隐私政策' : 'Privacy Policy' }}》</a></span>
      </SettingsItem>
      <SettingsItem title="赞助支持">
        <a href="#sponsor" class="settings-link" @click.prevent="openSponsorPanel">点击投喂🐷</a>
      </SettingsItem>
    </div>
  </div>
</template>

<script>
import SettingsItem from '../../../../components/settings/SettingsItem.vue';

export default {
  name: 'AboutSettings',
  components: {
    SettingsItem
  },
  computed: {
    isZh() {
      const lang = navigator.language || navigator.userLanguage || 'en';
      return lang.toLowerCase().startsWith('zh');
    }
  },
  methods: {
    openExternal(url) {
      try {
        if (window.electronAPI?.shell?.openExternal) {
          window.electronAPI.shell.openExternal(url);
        } else if (window.shell?.openExternal) {
          window.shell.openExternal(url);
        } else {
          window.open(url, '_blank');
        }
      } catch (error) {
        console.error('Failed to open external link:', error);
        window.open(url, '_blank');
      }
    },
    openPrivacyPanel() {
      this.$emit('open-privacy-panel');
    },
    openSponsorPanel() {
      this.$emit('open-sponsor-panel');
    }
  }
};
</script>