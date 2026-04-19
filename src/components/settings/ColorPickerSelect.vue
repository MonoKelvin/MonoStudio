<template>
  <div class="ui-color-picker-select" ref="rootRef">
    <button type="button" class="ui-color-picker-trigger" @click="toggleOpen">
      <div class="ui-color-preview" :style="{ backgroundColor: modelValue }"></div>
      <img :src="arrowIcon" alt="" class="ui-color-picker-arrow" :class="{ open: isOpen }" />
    </button>
    <transition name="ui-color-picker-fade">
      <div v-if="isOpen" class="ui-color-picker-dropdown" @mousedown.prevent>
        <div class="ui-color-grid">
          <button
            v-for="(color, index) in colors"
            :key="index"
            class="ui-color-option"
            :class="{ active: modelValue === color.value }"
            @click="selectColor(color.value)"
            v-tooltip:top="color.label"
          >
            <div class="ui-color-preview" :style="{ backgroundColor: color.value }"></div>
          </button>
        </div>
      </div>
    </transition>
  </div>
</template>

<script>
import arrowIcon from '../../assets/icons/chevron-down.svg';

export default {
  name: 'ColorPickerSelect',
  props: {
    modelValue: {
      type: String,
      default: '#3b82f6'
    },
    colors: {
      type: Array,
      default: () => []
    }
  },
  emits: ['update:modelValue'],
  data() {
    return {
      isOpen: false,
      arrowIcon
    };
  },
  methods: {
    toggleOpen() {
      this.isOpen = !this.isOpen;
    },
    selectColor(color) {
      this.$emit('update:modelValue', color);
      this.isOpen = false;
    },
    handleOutsideClick(event) {
      if (!this.$refs.rootRef) return;
      if (!this.$refs.rootRef.contains(event.target)) {
        this.isOpen = false;
      }
    }
  },
  mounted() {
    document.addEventListener('mousedown', this.handleOutsideClick);
  },
  beforeUnmount() {
    document.removeEventListener('mousedown', this.handleOutsideClick);
  }
};
</script>

