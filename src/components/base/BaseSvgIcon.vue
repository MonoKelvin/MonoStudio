<template>
  <div 
    class="base-svg-icon"
    :class="{ 'base-svg-icon-spin': spin }"
    :style="iconStyle"
    v-html="processedSvg"
  ></div>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
  icon: {
    type: String,
    required: true
  },
  size: {
    type: [String, Number],
    default: '16px'
  },
  color: {
    type: String,
    default: 'var(--text-primary)'
  },
  spin: {
    type: Boolean,
    default: false
  }
});

const iconStyle = computed(() => {
  const size = typeof props.size === 'number' ? `${props.size}px` : props.size;
  return {
    width: size,
    height: size,
    color: props.color
  };
});

const processedSvg = computed(() => {
  if (!props.icon) return '';
  const size = typeof props.size === 'number' ? props.size : parseInt(props.size);
  let svg = props.icon;
  svg = svg.replace(/width="[^"]*"/, `width="${size}"`);
  svg = svg.replace(/height="[^"]*"/, `height="${size}"`);
  return svg;
});
</script>

<style scoped>
.base-svg-icon {
  display: inline-block;
  vertical-align: middle;
  transition: all 0.2s ease;
}

.base-svg-icon :deep(svg) {
  width: 100%;
  height: 100%;
}

.base-svg-icon :deep(svg path) {
  fill: currentColor;
}

.base-svg-icon-spin {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}
</style>