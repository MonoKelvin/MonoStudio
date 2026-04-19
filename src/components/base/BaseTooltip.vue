<template>
  <div class="ui-tooltip-wrap" @mouseenter="showTooltip" @mouseleave="hideTooltip" @focus="showTooltip" @blur="hideTooltip">
    <slot />
    <Transition name="tooltip-fade">
      <div v-if="visible && content" ref="tooltipRef" class="ui-tooltip" :class="`ui-tooltip-${placement}`" :style="tooltipStyle">
        {{ content }}
      </div>
    </Transition>
  </div>
</template>

<script>
import { ref, computed, onMounted, onBeforeUnmount } from 'vue';

export default {
  name: 'BaseTooltip',
  props: {
    content: { type: String, default: '' },
    placement: { type: String, default: 'top' },
    delay: { type: Number, default: 200 },
    maxWidth: { type: Number, default: 300 }
  },
  setup(props) {
    const visible = ref(false);
    const tooltipRef = ref(null);
    let showTimer = null;

    const tooltipStyle = computed(() => ({
      maxWidth: `${props.maxWidth}px`
    }));

    function showTooltip() {
      clearTimeout(showTimer);
      showTimer = setTimeout(() => {
        visible.value = true;
      }, props.delay);
    }

    function hideTooltip() {
      clearTimeout(showTimer);
      visible.value = false;
    }

    onBeforeUnmount(() => {
      clearTimeout(showTimer);
    });

    return {
      visible,
      tooltipRef,
      tooltipStyle,
      showTooltip,
      hideTooltip
    };
  }
};
</script>

<style scoped>
.ui-tooltip-wrap {
  position: relative;
  display: inline-block;
}

.ui-tooltip {
  position: absolute;
  z-index: 9999;
  padding: 6px 10px;
  background: var(--bg-tooltip, #1f2937);
  color: var(--text-tooltip, #f9fafb);
  font-size: 12px;
  line-height: 1.4;
  border-radius: 6px;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
  white-space: normal;
  word-wrap: break-word;
  pointer-events: none;
}

.ui-tooltip-top {
  bottom: calc(100% + 8px);
  left: 50%;
  transform: translateX(-50%);
}

.ui-tooltip-bottom {
  top: calc(100% + 8px);
  left: 50%;
  transform: translateX(-50%);
}

.ui-tooltip-left {
  right: calc(100% + 8px);
  top: 50%;
  transform: translateY(-50%);
}

.ui-tooltip-right {
  left: calc(100% + 8px);
  top: 50%;
  transform: translateY(-50%);
}

.tooltip-fade-enter-active,
.tooltip-fade-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}

.tooltip-fade-enter-from,
.tooltip-fade-leave-to {
  opacity: 0;
  transform: translateY(-4px) scale(0.95);
}

.ui-tooltip-top.tooltip-fade-enter-from,
.ui-tooltip-top.tooltip-fade-leave-to {
  transform: translateX(-50%) translateY(4px);
}

.ui-tooltip-bottom.tooltip-fade-enter-from,
.ui-tooltip-bottom.tooltip-fade-leave-to {
  transform: translateX(-50%) translateY(-4px);
}

.ui-tooltip-left.tooltip-fade-enter-from,
.ui-tooltip-left.tooltip-fade-leave-to {
  transform: translateY(-50%) translateX(4px);
}

.ui-tooltip-right.tooltip-fade-enter-from,
.ui-tooltip-right.tooltip-fade-leave-to {
  transform: translateY(-50%) translateX(-4px);
}
</style>