<template>
  <section class="ui-panel" :class="{ glass }">
    <slot />
  </section>
</template>

<script>
export default {
  name: 'BasePanel',
  props: {
    glass: { type: Boolean, default: true }
  }
};
</script>

<style scoped>
.ui-panel {
  border-radius: 8px;
  background: color-mix(in srgb, var(--bg-elevated) 88%, transparent);
  box-shadow: none;
}

.ui-panel.glass {
  backdrop-filter: blur(8px) saturate(1.06);
}
</style>
