<template>
  <button class="ui-btn" :class="[`ui-btn--${variant}`, `ui-btn--${size}`, { 'is-block': block }]" v-bind="$attrs">
    <slot />
  </button>
</template>

<script>
export default {
  name: 'BaseButton',
  inheritAttrs: false,
  props: {
    variant: { type: String, default: 'default' },
    size: { type: String, default: 'md' },
    block: { type: Boolean, default: false }
  }
};
</script>

<style scoped>
.ui-btn {
  box-sizing: border-box;
  font: inherit;
  line-height: 1;
  height: 34px;
  border: 1px solid color-mix(in srgb, var(--bg-elevated) 82%, var(--text-secondary) 18%);
  border-radius: 6px;
  padding: 0 11px;
  color: var(--text-primary);
  background: color-mix(in srgb, var(--bg-elevated) 78%, var(--hover-bg) 22%);
  cursor: pointer;
  transition: background-color 0.18s ease, border-color 0.18s ease, color 0.18s ease;
}

.ui-btn:hover {
  background: color-mix(in srgb, var(--hover-bg) 82%, var(--bg-elevated) 18%);
  border-color: color-mix(in srgb, var(--hover-bg) 65%, var(--text-secondary) 35%);
}

.ui-btn:active,
.ui-btn.active {
  background: color-mix(in srgb, var(--accent-color) 85%, var(--bg-elevated) 15%);
  border-color: color-mix(in srgb, var(--accent-color) 60%, var(--text-secondary) 40%);
  box-shadow: none;
}

.ui-btn--primary {
  color: #fff;
  background: color-mix(in srgb, var(--accent-color) 84%, #0b0d10 16%);
  border-color: color-mix(in srgb, var(--accent-color) 70%, #0b0d10 30%);
}

.ui-btn--primary:hover {
  background: color-mix(in srgb, var(--accent-color) 90%, #0b0d10 10%);
}

.ui-btn--primary:active {
  background: color-mix(in srgb, var(--accent-color) 78%, #0b0d10 22%);
  border-color: color-mix(in srgb, var(--accent-color) 58%, #0b0d10 42%);
  box-shadow: none;
}

.ui-btn--danger {
  color: #fff;
  background: color-mix(in srgb, var(--danger) 84%, #0b0d10 16%);
  border-color: color-mix(in srgb, var(--danger) 68%, #0b0d10 32%);
}

.ui-btn--danger:hover {
  background: color-mix(in srgb, var(--danger) 90%, #0b0d10 10%);
}

.ui-btn--danger:active {
  background: color-mix(in srgb, var(--danger) 78%, #0b0d10 22%);
  border-color: color-mix(in srgb, var(--danger) 54%, #0b0d10 46%);
  box-shadow: none;
}

.ui-btn--sm {
  height: 28px;
  padding: 0 9px;
  border-radius: 5px;
  font-size: 12px;
}

.ui-btn--lg {
  height: 40px;
  padding: 0 13px;
  border-radius: 7px;
}

.ui-btn.is-block {
  width: 100%;
}

.ui-btn:disabled {
  cursor: not-allowed;
  opacity: 0.5;
  background: var(--bg-secondary);
  border-color: var(--border-color);
  color: var(--text-tertiary);
  pointer-events: none;
  user-select: none;
  box-shadow: none !important;
  transform: none !important;
}

.ui-btn:disabled:hover,
.ui-btn:disabled:focus,
.ui-btn:disabled:active,
.ui-btn:disabled.active {
  background: var(--bg-secondary);
  border-color: var(--border-color);
  color: var(--text-tertiary);
  box-shadow: none !important;
  transform: none !important;
}

.ui-btn--primary:disabled {
  background: var(--bg-secondary);
  border-color: var(--border-color);
  color: var(--text-tertiary);
}

.ui-btn--primary:disabled:hover,
.ui-btn--primary:disabled:focus,
.ui-btn--primary:disabled:active,
.ui-btn--primary:disabled.active {
  background: var(--bg-secondary);
  border-color: var(--border-color);
  color: var(--text-tertiary);
}

.ui-btn--danger:disabled {
  background: var(--bg-secondary);
  border-color: var(--border-color);
  color: var(--text-tertiary);
}

.ui-btn--danger:disabled:hover,
.ui-btn--danger:disabled:focus,
.ui-btn--danger:disabled:active,
.ui-btn--danger:disabled.active {
  background: var(--bg-secondary);
  border-color: var(--border-color);
  color: var(--text-tertiary);
}
</style>
