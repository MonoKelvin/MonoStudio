<template>
  <label class="ui-switch-wrap">
    <input class="ui-switch-input" type="checkbox" v-bind="$attrs" />
    <span class="ui-switch-track">
      <span class="ui-switch-thumb"></span>
    </span>
  </label>
</template>

<script>
export default {
  name: 'BaseSwitch',
  inheritAttrs: false
};
</script>

<style scoped>
.ui-switch-wrap {
  position: relative;
  display: inline-block;
  width: 44px;
  height: 24px;
  cursor: pointer;
}

.ui-switch-input {
  opacity: 0;
  width: 0;
  height: 0;
}

.ui-switch-track {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: var(--input-bg);
  border: 1px solid var(--border-color);
  border-radius: 12px;
  transition: all 0.2s ease;
}

.ui-switch-track::before {
  position: absolute;
  content: "";
  height: 16px;
  width: 16px;
  left: 3px;
  bottom: 3px;
  background: var(--text-primary);
  border-radius: 50%;
  transition: all 0.2s ease;
}

.ui-switch-input:checked + .ui-switch-track {
  background: var(--accent-color);
  border-color: var(--accent-color);
}

.ui-switch-input:checked + .ui-switch-track::before {
  transform: translateX(20px);
  background: white;
}

.ui-switch-input:focus + .ui-switch-track {
  box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.2);
}

.ui-switch-input:disabled + .ui-switch-track {
  opacity: 0.6;
  cursor: not-allowed;
}

.ui-switch-input:disabled + .ui-switch-track::before {
  background: var(--text-secondary);
}
</style>