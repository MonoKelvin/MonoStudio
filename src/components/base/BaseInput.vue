<template>
  <input 
    class="ui-input" 
    :value="value" 
    @input="$emit('input', $event.target.value)"
    v-bind="$attrs" 
  />
</template>

<script>
export default {
  name: 'BaseInput',
  props: {
    value: {
      type: String,
      default: ''
    }
  },
  emits: ['input'],
  inheritAttrs: false
};
</script>

<style scoped>
.ui-input {
  box-sizing: border-box;
  font: inherit;
  line-height: 1;
  width: 100%;
  height: 34px;
  border: 0;
  border-radius: 6px;
  padding: 0 11px;
  color: var(--text-primary);
  background: var(--surface-2);
  box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--border-color) 36%, transparent);
  transition: box-shadow 0.2s ease, background-color 0.2s ease;
}

.ui-input:focus {
  outline: none;
  background: color-mix(in srgb, var(--surface-2) 86%, var(--accent-color) 14%);
  box-shadow: 0 0 0 1px color-mix(in srgb, var(--accent-color) 45%, transparent);
}
</style>
