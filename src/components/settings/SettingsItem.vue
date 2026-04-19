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