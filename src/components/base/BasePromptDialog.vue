<template>
    <Teleport to="body">
        <Transition name="prompt-fade">
            <div v-if="visible" class="prompt-overlay" @click="handleOverlayClick">
                <Transition name="prompt-scale">
                    <div v-if="visible" class="prompt-container" :class="promptClasses" @click.stop>
                        <div class="prompt-header">
                            <div class="prompt-icon" :class="`prompt-icon-${type}`">
                                <component :is="iconComponent" />
                            </div>
                            <h3 class="prompt-title">{{ title }}</h3>
                        </div>
                        <div class="prompt-body">
                            <slot>
                                <p>{{ message }}</p>
                            </slot>
                        </div>
                        <div class="prompt-footer">
                            <BaseButton v-if="showCancel" class="prompt-btn-cancel" @click="handleCancel">
                                {{ cancelText }}
                            </BaseButton>
                            <BaseButton class="prompt-btn-confirm" :class="confirmButtonClass" @click="handleConfirm">
                                {{ confirmText }}
                            </BaseButton>
                        </div>
                    </div>
                </Transition>
            </div>
        </Transition>
    </Teleport>
</template>

<script>
import BaseButton from './BaseButton.vue';

const ICONS = {
    info: {
        template: `
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
        <circle cx="12" cy="12" r="10" stroke="currentColor" stroke-width="2"/>
        <path d="M12 16V11" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <circle cx="12" cy="8" r="1" fill="currentColor"/>
      </svg>
    `
    },
    warning: {
        template: `
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M12 9V13" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <path d="M12 17V17.01" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <path d="M12 2C6.48 2 2 6.48 2 12C2 17.52 6.48 22 12 22C17.52 22 22 17.52 22 12C22 6.48 17.52 2 12 2ZM12 20C7.59 20 4 16.41 4 12C4 7.59 7.59 4 12 4C16.41 4 20 7.59 20 12C20 16.41 16.41 20 12 20Z" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
      </svg>
    `
    },
    debug: {
        template: `
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M12 2L2 7L12 12L22 7L12 2Z" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
        <path d="M2 17L12 12L22 17" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
        <path d="M2 7V17" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
        <path d="M22 7V17" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
        <path d="M12 12V22" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
      </svg>
    `
    },
    error: {
        template: `
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M18 6L6 18" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <path d="M6 6L18 18" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
      </svg>
    `
    },
    critical: {
        template: `
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M12 2C6.48 2 2 6.48 2 12C2 17.52 6.48 22 12 22C17.52 22 22 17.52 22 12C22 6.48 17.52 2 12 2ZM12 20C7.59 20 4 16.41 4 12C4 7.59 7.59 4 12 4C16.41 4 20 7.59 20 12C20 16.41 16.41 20 12 20Z" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
        <path d="M12 8V12" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
        <path d="M12 16V16.01" stroke="currentColor" stroke-width="2" stroke-linecap="round"/>
      </svg>
    `
    }
};

export default {
    name: 'BasePromptDialog',
    components: {
        BaseButton
    },
    props: {
        visible: {
            type: Boolean,
            default: false
        },
        type: {
            type: String,
            default: 'info',
            validator: (value) => ['info', 'warning', 'debug', 'error', 'critical'].includes(value)
        },
        title: {
            type: String,
            default: '提示'
        },
        message: {
            type: String,
            default: ''
        },
        confirmText: {
            type: String,
            default: '确定'
        },
        cancelText: {
            type: String,
            default: '取消'
        },
        showCancel: {
            type: Boolean,
            default: false
        },
        closeOnOverlay: {
            type: Boolean,
            default: true
        },
        enableSound: {
            type: Boolean,
            default: true
        }
    },
    emits: ['update:visible', 'confirm', 'cancel'],
    computed: {
        iconComponent() {
            return ICONS[this.type] || ICONS.info;
        },
        promptClasses() {
            return [`prompt-${this.type}`];
        },
        confirmButtonClass() {
            return `prompt-btn-${this.type}`;
        }
    },
    watch: {
        visible(newVal) {
            if (newVal && this.enableSound) {
                this.playSound();
            }
        }
    },
    methods: {
        playSound() {
            try {
                const audioContext = new (window.AudioContext || window.webkitAudioContext)();
                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();

                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);

                const frequencies = {
                    info: 800,
                    warning: 600,
                    debug: 1000,
                    error: 400,
                    critical: 300
                };

                oscillator.frequency.value = frequencies[this.type] || 800;
                oscillator.type = 'sine';
                gainNode.gain.setValueAtTime(0.08, audioContext.currentTime);
                gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.15);

                oscillator.start(audioContext.currentTime);
                oscillator.stop(audioContext.currentTime + 0.15);
            } catch (e) {
                console.warn('Failed to play sound:', e);
            }
        },
        handleConfirm() {
            this.$emit('confirm');
            this.$emit('update:visible', false);
        },
        handleCancel() {
            this.$emit('cancel');
            this.$emit('update:visible', false);
        },
        handleOverlayClick() {
            if (this.closeOnOverlay) {
                this.$emit('cancel');
                this.$emit('update:visible', false);
            }
        }
    }
};
</script>

<style scoped>
.prompt-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.6);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 10000;
    padding: 24px;
}

.prompt-container {
    background: var(--background-primary);
    border-radius: 12px;
    box-shadow:
        0 20px 25px -5px rgba(0, 0, 0, 0.1),
        0 10px 10px -5px rgba(0, 0, 0, 0.04);
    max-width: 400px;
    width: 100%;
    overflow: hidden;
    border: 1px solid var(--border-color);
}

.prompt-header {
    display: flex;
    align-items: center;
    gap: 12px;
    padding: 20px 20px 16px;
    border-bottom: 1px solid var(--border-color);
}

.prompt-icon {
    flex-shrink: 0;
    width: 32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 8px;
    color: var(--text-primary);
}

.prompt-icon-info {
    color: var(--info-color);
    background: color-mix(in srgb, var(--info-color) 10%, transparent);
}

.prompt-icon-warning {
    color: var(--warning-color);
    background: color-mix(in srgb, var(--warning-color) 10%, transparent);
}

.prompt-icon-debug {
    color: var(--debug-color);
    background: color-mix(in srgb, var(--debug-color) 10%, transparent);
}

.prompt-icon-error {
    color: var(--error-color);
    background: color-mix(in srgb, var(--error-color) 10%, transparent);
}

.prompt-icon-critical {
    color: var(--critical-color);
    background: color-mix(in srgb, var(--critical-color) 10%, transparent);
}

.prompt-icon svg {
    width: 100%;
    height: 100%;
    color: currentColor;
}

.prompt-title {
    flex: 1;
    margin: 0;
    font-size: 16px;
    font-weight: 600;
    color: var(--text-primary);
}

.prompt-body {
    padding: 16px 20px;
    color: var(--text-secondary);
    font-size: 14px;
    line-height: 1.5;
    min-height: 60px;
    display: flex;
    align-items: center;
}

.prompt-body p {
    margin: 0;
    text-align: left;
}

.prompt-footer {
    display: flex;
    justify-content: flex-end;
    gap: 12px;
    padding: 16px 20px;
    border-top: 1px solid var(--border-color);
    background: var(--background-secondary);
}

.prompt-btn-cancel {
    background: var(--background-tertiary);
    color: var(--text-primary);
    border: 1px solid var(--border-color);
    padding: 8px 16px;
    font-size: 14px;
    font-weight: 500;
    border-radius: 6px;
    transition: all 0.2s ease;
}

.prompt-btn-cancel:hover {
    background: var(--background-hover);
    border-color: var(--accent-color);
}

.prompt-btn-confirm {
    background: var(--accent-color);
    color: white;
    border: none;
    padding: 8px 16px;
    font-size: 14px;
    font-weight: 500;
    border-radius: 6px;
    transition: all 0.2s ease;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.prompt-btn-confirm:hover {
    background: var(--accent-active);
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.15);
}

.prompt-btn-info {
    background: var(--info-color);
}

.prompt-btn-info:hover {
    background: var(--info-active);
}

.prompt-btn-warning {
    background: var(--warning-color);
}

.prompt-btn-warning:hover {
    background: var(--warning-active);
}

.prompt-btn-debug {
    background: var(--debug-color);
}

.prompt-btn-debug:hover {
    background: var(--debug-color);
    opacity: 0.9;
}

.prompt-btn-error {
    background: var(--error-color);
}

.prompt-btn-error:hover {
    background: var(--error-active);
}

.prompt-btn-critical {
    background: var(--critical-color);
}

.prompt-btn-critical:hover {
    background: var(--critical-color);
    opacity: 0.9;
}

.prompt-fade-enter-active,
.prompt-fade-leave-active {
    transition: opacity 0.2s ease;
}

.prompt-fade-enter-from,
.prompt-fade-leave-to {
    opacity: 0;
}

.prompt-scale-enter-active,
.prompt-scale-leave-active {
    transition: transform 0.2s ease;
}

.prompt-scale-enter-from,
.prompt-scale-leave-to {
    transform: scale(0.95);
}

@media (prefers-color-scheme: dark) {
    .prompt-container {
        box-shadow:
            0 20px 25px -5px rgba(0, 0, 0, 0.3),
            0 10px 10px -5px rgba(0, 0, 0, 0.2);
    }
}
</style>
