<template>
  <div class="settings-section">
    <h3>应用行为</h3>
    <p>窗口与交互相关设置。</p>
    <div class="settings-group">
      <SettingsItem title="开发者工具" description="控制开发者工具的显示">
        <BaseSwitch :checked="localDevToolsOpen" @change="onDevToolsChange" />
      </SettingsItem>
    </div>
  </div>
</template>

<script>
import SettingsItem from '../../../../components/settings/SettingsItem.vue';
import BaseSwitch from '../../../../components/base/BaseSwitch.vue';

export default {
  name: 'AppSettings',
  components: {
    SettingsItem,
    BaseSwitch
  },
  props: {
    openDevTools: {
      type: Boolean,
      default: false
    }
  },
  emits: ['change-dev-tools'],
  data() {
    return {
      localDevToolsOpen: false
    };
  },
  watch: {
    openDevTools: {
      immediate: true,
      handler(val) {
        this.localDevToolsOpen = val;
      }
    }
  },
  mounted() {
    this.syncDevToolsState();
    this.listenDevToolsState();
  },
  methods: {
    async syncDevToolsState() {
      if (window.electronAPI?.devtools?.isOpen) {
        this.localDevToolsOpen = await window.electronAPI.devtools.isOpen();
      }
    },
    listenDevToolsState() {
      if (window.electronAPI?.devtools?.onStateChanged) {
        window.electronAPI.devtools.onStateChanged((isOpen) => {
          this.localDevToolsOpen = isOpen;
        });
      }
    },
    onDevToolsChange(event) {
      this.localDevToolsOpen = event.target.checked;
      this.$emit('change-dev-tools', event.target.checked);
    }
  }
};
</script>