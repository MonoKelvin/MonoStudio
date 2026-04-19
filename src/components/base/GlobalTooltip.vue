<template>
  <Teleport to="body">
    <Transition name="tooltip-fade">
      <div
        v-if="tooltipState.visible.value"
        ref="tooltipEl"
        class="global-tooltip"
        :class="actualPlacement"
        :style="tooltipStyle"
      >
        {{ tooltipState.content.value }}
      </div>
    </Transition>
  </Teleport>
</template>

<script>
import { computed, ref, watch, nextTick } from 'vue';
import { tooltipState } from '../../directives/tooltip.js';

const OFFSET = 12;

export default {
  name: 'GlobalTooltip',
  setup() {
    const tooltipEl = ref(null);
    const actualHeight = ref(30);

    watch(() => tooltipState.visible.value, (visible) => {
      if (visible) {
        nextTick(() => {
          if (tooltipEl.value) {
            actualHeight.value = tooltipEl.value.offsetHeight || 30;
          }
        });
      }
    });

    const actualPlacement = computed(() => {
      return 'global-tooltip-' + tooltipState.placement.value;
    });

    const tooltipStyle = computed(() => {
      const x = tooltipState.x.value;
      const y = tooltipState.y.value;
      const placement = tooltipState.placement.value;
      const vw = window.innerWidth;

      let finalPlacement = placement;

      if (placement === 'top') {
        if (y - OFFSET - actualHeight.value < 10) {
          finalPlacement = 'bottom';
        }
      } else if (placement === 'bottom') {
        if (y + OFFSET + actualHeight.value > window.innerHeight - 10) {
          finalPlacement = 'top';
        }
      }

      let left, top, transform;

      switch (finalPlacement) {
        case 'top':
          left = x;
          top = y - OFFSET;
          transform = 'translate(-50%, -100%)';
          break;
        case 'bottom':
          left = x;
          top = y + OFFSET;
          transform = 'translate(-50%, 0)';
          break;
        case 'left':
          left = x - OFFSET;
          top = y;
          transform = 'translate(-100%, -50%)';
          break;
        case 'right':
          left = x + OFFSET;
          top = y;
          transform = 'translate(0, -50%)';
          break;
        default:
          left = x;
          top = y - OFFSET;
          transform = 'translate(-50%, -100%)';
      }

      return {
        left: `${left}px`,
        top: `${top}px`,
        transform,
        maxWidth: `${vw - 20}px`
      };
    });

    return {
      tooltipEl,
      tooltipState,
      tooltipStyle,
      actualPlacement
    };
  }
};
</script>