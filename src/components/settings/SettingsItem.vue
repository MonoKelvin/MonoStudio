<template>
  <div class="settings-item" :class="{ 'has-description': description }">
    <div class="settings-item-header">
      <div class="settings-item-content">
        <span class="settings-item-title">{{ title }}</span>
        <span v-if="description" class="settings-item-description">{{ description }}</span>
      </div>
      <div class="settings-item-control">
        <slot></slot>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'SettingsItem',
  props: {
    title: {
      type: String,
      required: true
    },
    description: {
      type: String,
      default: ''
    }
  }
};
</script>

<style scoped>
.settings-item {
  padding: 16px;
}

.settings-item-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
}

.settings-item-content {
  display: flex;
  flex-direction: column;
  gap: 4px;
  flex: 1;
  min-width: 0;
}

.settings-item.has-description .settings-item-content {
  justify-content: center;
}

.settings-item-title {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
}

.settings-item-description {
  font-size: 13px;
  color: var(--text-muted);
  line-height: 1.4;
}

.settings-item-control {
  display: flex;
  align-items: center;
  flex-shrink: 0;
}
</style>