<template>
  <BasePanel class="panel audio-tools-panel">
    <section class="placeholder-content">
      <div class="placeholder-icon">🎵</div>
      <h3>{{ toolName }}</h3>
      <p>{{ toolDescription }}</p>
      <p class="placeholder-note">功能开发中，敬请期待...</p>
    </section>
  </BasePanel>
</template>

<script>
export default {
  name: 'AudioToolsToolView',
  props: {
    toolName: {
      type: String,
      default: '音频工具'
    },
    toolDescription: {
      type: String,
      default: '音频格式转换和编辑'
    }
  }
};
</script>

<style scoped>
.audio-tools-panel {
  height: 100%;
}
</style>