<template>
  <BasePanel class="panel process-manager-panel">
    <section class="placeholder-content">
      <div class="placeholder-icon">⚙️</div>
      <h3>{{ toolName }}</h3>
      <p>{{ toolDescription }}</p>
      <p class="placeholder-note">功能开发中，敬请期待...</p>
    </section>
  </BasePanel>
</template>

<script>
export default {
  name: 'ProcessManagerToolView',
  props: {
    toolName: {
      type: String,
      default: '进程管理'
    },
    toolDescription: {
      type: String,
      default: '查看和管理系统进程'
    }
  }
};
</script>

<style scoped>
.process-manager-panel {
  height: 100%;
}
</style>