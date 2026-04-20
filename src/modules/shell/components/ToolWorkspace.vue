<template>
  <main class="content app-panel">
    <section class="content-header app-panel-section">
      <h2>{{ displayToolName }}</h2>
      <p>{{ displayToolDescription }}</p>
    </section>
    <section class="content-body app-panel-section">
      <keep-alive>
        <component
          :is="currentTool.component"
          :tool-name="displayToolName"
          :tool-description="displayToolDescription"
          :theme-mode="themeMode"
          :theme-options="themeOptions"
          :current-accent-color="currentAccentColor"
          @change-theme="$emit('change-theme', $event)"
          @change-accent-color="$emit('change-accent-color', $event)"
          @change-dev-tools="$emit('change-dev-tools', $event)"
          @unlocked="currentTool.isLockGate && $emit('unlock-personal-life')"
        />
      </keep-alive>
    </section>
  </main>
</template>

<script>
export default {
  name: 'ToolWorkspace',
  props: {
    currentTool: { type: Object, required: true },
    pendingPersonalLifeTool: { type: String, default: null },
    tools: { type: Array, required: true },
    themeMode: { type: String, required: true },
    themeOptions: { type: Array, required: true },
    currentAccentColor: { type: String, default: '#8b5cf6' },
    openDevTools: { type: Boolean, default: false }
  },
  computed: {
    displayToolName() {
      if (this.currentTool.isLockGate && this.pendingPersonalLifeTool) {
        const pendingTool = this.tools.find(tool => tool.id === this.pendingPersonalLifeTool);
        return pendingTool ? pendingTool.name : this.currentTool.name;
      }
      return this.currentTool.name;
    },
    displayToolDescription() {
      if (this.currentTool.isLockGate && this.pendingPersonalLifeTool) {
        const pendingTool = this.tools.find(tool => tool.id === this.pendingPersonalLifeTool);
        return pendingTool ? pendingTool.description : this.currentTool.description;
      }
      return this.currentTool.description;
    }
  }
};
</script>
