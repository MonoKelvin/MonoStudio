<template>
  <label class="ui-checkbox-wrap">
    <input class="ui-checkbox-input" type="checkbox" v-bind="$attrs" />
    <span class="ui-checkbox-box">
      <img :src="checkIcon" alt="" aria-hidden="true" />
    </span>
    <span v-if="$slots.default" class="ui-checkbox-label"><slot /></span>
  </label>
</template>

<script>
import checkIcon from '../../assets/icons/check.svg';

export default {
  name: 'BaseCheckbox',
  inheritAttrs: false,
  data() {
    return {
      checkIcon
    };
  }
};
</script>

<style scoped>
.ui-checkbox-wrap {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
  user-select: none;
}

.ui-checkbox-input {
  position: absolute;
  opacity: 0;
  pointer-events: none;
}

.ui-checkbox-box {
  width: 16px;
  height: 16px;
  border-radius: 4px;
  background: var(--surface-2);
  box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--border-color) 45%, transparent);
  display: inline-flex;
  align-items: center;
  justify-content: center;
  transition: background-color 0.2s ease, box-shadow 0.2s ease;
}

.ui-checkbox-box img {
  width: 11px;
  height: 11px;
  opacity: 0;
}

.ui-checkbox-input:checked + .ui-checkbox-box {
  background: color-mix(in srgb, var(--accent-color) 82%, #0b0d10 18%);
  box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--accent-color) 60%, transparent);
}

.ui-checkbox-input:checked + .ui-checkbox-box img {
  opacity: 1;
}

.ui-checkbox-input:focus-visible + .ui-checkbox-box {
  box-shadow:
    inset 0 0 0 1px color-mix(in srgb, var(--accent-color) 60%, transparent),
    0 0 0 1px color-mix(in srgb, var(--accent-color) 35%, transparent);
}

.ui-checkbox-label {
  font-size: 13px;
  color: var(--text-primary);
}
</style>
