<template>
  <div class="search-input-container">
    <span class="search-prefix-icon" aria-hidden="true">
      <img :src="searchIcon" alt="搜索" />
    </span>
    <BaseInput 
      v-model="localValue" 
      class="search-input search-input-with-prefix"
      :placeholder="placeholder"
      @input="handleInput"
      v-bind="$attrs"
    />
    <BaseButton v-if="localValue" class="search-clear-btn" size="sm"
      @click="handleClear">
      ✕
    </BaseButton>
  </div>
</template>

<script>
import BaseInput from './BaseInput.vue';
import BaseButton from './BaseButton.vue';
import searchIcon from '../../assets/icons/search-16.svg';

export default {
  name: 'SearchInput',
  components: {
    BaseInput,
    BaseButton
  },
  props: {
    modelValue: {
      type: String,
      default: ''
    },
    placeholder: {
      type: String,
      default: '搜索...'
    }
  },
  emits: ['update:modelValue', 'input'],
  data() {
    return {
      localValue: this.modelValue,
      searchIcon
    };
  },
  watch: {
    modelValue: {
      handler(newValue) {
        this.localValue = newValue;
      },
      immediate: true
    }
  },
  methods: {
    handleInput(event) {
      const value = event.target.value;
      this.localValue = value;
      this.$emit('update:modelValue', value);
      this.$emit('input', event);
    },
    handleClear() {
      this.localValue = '';
      this.$emit('update:modelValue', '');
      // 触发input事件，确保父组件重新筛选
      this.$emit('input', { target: { value: '' } });
    }
  }
};
</script>

<style scoped>
.search-input-container {
  position: relative;
  display: flex;
  align-items: center;
  width: 100%;
}

.search-prefix-icon {
  position: absolute;
  left: 10px;
  display: inline-flex;
  color: var(--text-secondary);
  pointer-events: none;
}

.search-prefix-icon img {
  width: 14px;
  height: 14px;
  filter: brightness(0) invert(0.75);
}

.search-input-with-prefix {
  padding-left: 30px !important;
  padding-right: 34px !important;
}

.search-clear-btn {
  position: absolute;
  right: 6px;
  width: 22px;
  height: 22px;
  min-width: 22px;
  min-height: 22px;
  padding: 0;
  border-radius: 999px;
  font-size: 11px;
  line-height: 1;
  display: flex;
  align-items: center;
  justify-content: center;
}
</style>