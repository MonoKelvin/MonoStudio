<template>
  <div class="info-item">
    <div class="info-label">{{ label }}</div>
    <div class="info-value" :class="{ 'value-multiline': isMultiline }">
      <slot v-if="$slots.default" />
      <span v-else :title="value" class="value-text">
        {{ formattedValue }}
      </span>
    </div>
  </div>
</template>

<script>
export default {
  name: 'InfoItem',
  props: {
    label: {
      type: String,
      required: true
    },
    value: {
      type: [String, Number],
      default: ''
    },
    status: {
      type: String,
      default: null
    }
  },
  computed: {
    isMultiline() {
      return this.value && typeof this.value === 'string' && this.value.length > 50
    },
    formattedValue() {
      if (this.value === null || this.value === undefined) {
        return '无'
      }
      if (typeof this.value === 'number') {
        return this.value.toLocaleString()
      }
      return this.value
    }
  }
}
</script>

<style scoped>
.info-item {
  display: flex;
  align-items: flex-start;
  gap: 12px;
  padding: 8px 0;
}

.info-label {
  font-size: 14px;
  color: var(--text-secondary);
  min-width: 120px;
  flex-shrink: 0;
}

.info-value {
  flex: 1;
  font-size: 14px;
  color: var(--text-primary);
  word-break: break-all;
}

.value-text {
  display: inline-block;
  max-width: 100%;
}

.value-multiline {
  white-space: pre-wrap;
  word-break: break-word;
}
</style>