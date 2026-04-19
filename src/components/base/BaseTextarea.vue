<template>
  <div class="ui-textarea" :class="{ disabled }">
    <textarea
      v-model="value"
      :placeholder="placeholder"
      :rows="rows"
      :disabled="disabled"
      @input="$emit('input', value)"
      @focus="$emit('focus')"
      @blur="$emit('blur')"
    />
  </div>
</template>

<script>
export default {
  name: 'BaseTextarea',
  props: {
    value: {
      type: String,
      default: ''
    },
    placeholder: {
      type: String,
      default: ''
    },
    rows: {
      type: Number,
      default: 3
    },
    disabled: {
      type: Boolean,
      default: false
    },
    readonly: {
      type: Boolean,
      default: false
    }
  },
  emits: ['input', 'focus', 'blur']
};
</script>

<style scoped>
.ui-textarea {
  position: relative;
  width: 100%;
}

.ui-textarea textarea {
  width: 100%;
  min-height: 100px;
  padding: 10px 12px;
  border: 1px solid var(--border-color);
  border-radius: var(--radius-md);
  background: var(--bg-primary);
  color: var(--text-primary);
  font-size: var(--font-size-base);
  font-family: var(--font-family);
  resize: vertical;
  transition: all var(--transition-fast);
  box-sizing: border-box;
}

.ui-textarea textarea:focus {
  outline: none;
  border-color: var(--accent-color);
  box-shadow: 0 0 0 3px rgba(139, 92, 246, 0.1);
}

.ui-textarea textarea::placeholder {
  color: var(--text-tertiary);
}

.ui-textarea.disabled textarea {
  background: var(--bg-secondary);
  color: var(--text-tertiary);
  cursor: not-allowed;
}

.ui-textarea textarea:read-only {
  background: var(--bg-secondary);
  cursor: default;
}
</style>