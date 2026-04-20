<template>
  <main class="content app-panel">
    <section class="content-header app-panel-section">
      <h2>{{ currentTool.name }}</h2>
      <p>{{ currentTool.description }}</p>
    </section>
    <section class="content-body app-panel-section">
      <keep-alive>
        <component
          :is="currentTool.component"
          :tool-name="currentTool.name"
          :tool-description="currentTool.description"
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
    themeMode: { type: String, required: true },
    themeOptions: { type: Array, required: true },
    currentAccentColor: { type: String, default: '#8b5cf6' },
    openDevTools: { type: Boolean, default: false }
  },
};
</script>
