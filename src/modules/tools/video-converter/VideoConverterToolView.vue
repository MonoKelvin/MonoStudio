<template>
  <BasePanel class="panel video-converter-panel">
    <section class="placeholder-content">
      <div class="placeholder-icon">🎬</div>
      <h3>{{ toolName }}</h3>
      <p>{{ toolDescription }}</p>
      <p class="placeholder-note">功能开发中，敬请期待...</p>
    </section>
  </BasePanel>
</template>

<script>
export default {
  name: 'VideoConverterToolView',
  props: {
    toolName: {
      type: String,
      default: '视频转换'
    },
    toolDescription: {
      type: String,
      default: '视频格式转换和处理'
    }
  }
};
</script>

<style scoped>
.video-converter-panel {
  height: 100%;
}
</style>