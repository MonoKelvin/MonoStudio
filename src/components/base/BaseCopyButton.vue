<template>
    <BaseButton @click="handleCopy" :class="{ 'is-success': copySuccess, 'is-recovering': copyRecovering }"
        :disabled="disabled || !textToCopy">
        <img :src="copySuccess ? successIcon : copyIcon" class="copy-icon" alt="复制">
        {{ copySuccess ? successText : buttonText }}
    </BaseButton>
</template>

<script>
import { ref } from 'vue';
import BaseButton from '../../components/base/BaseButton.vue';
import copyIcon from '../../assets/icons/copy.svg';
import successIcon from '../../assets/icons/check-success.svg';

export default {
    name: 'BaseCopyButton',
    components: {
        BaseButton
    },
    props: {
        textToCopy: {
            type: String,
            default: ''
        },
        buttonText: {
            type: String,
            default: ''
        },
        successText: {
            type: String,
            default: ''
        },
        disabled: {
            type: Boolean,
            default: false
        }
    },
    emits: ['copy', 'copy-success', 'copy-error'],
    setup(props, { emit }) {
        const copySuccess = ref(false);
        const copyRecovering = ref(false);
        const copySuccessTimer = ref(null);

        const handleCopy = async () => {
            if (!props.textToCopy || props.disabled) return;

            emit('copy');

            try {
                await navigator.clipboard.writeText(props.textToCopy);
                console.log('复制成功:', props.textToCopy);
                emit('copy-success');
                showSuccessAnimation();
            } catch (err) {
                console.error('复制失败:', err);
                emit('copy-error', err);
            }
        };

        const showSuccessAnimation = () => {
            copySuccess.value = false;
            copyRecovering.value = false;
            requestAnimationFrame(() => {
                copySuccess.value = true;
            });
            if (copySuccessTimer.value) clearTimeout(copySuccessTimer.value);
            copySuccessTimer.value = setTimeout(() => {
                copySuccess.value = false;
                copyRecovering.value = true;
                copySuccessTimer.value = setTimeout(() => {
                    copyRecovering.value = false;
                    copySuccessTimer.value = null;
                }, 240);
            }, 1500);
        };

        return {
            copySuccess,
            copyRecovering,
            copyIcon,
            successIcon,
            handleCopy
        };
    }
};
</script>

<style scoped>
.copy-icon {
    width: 16px;
    height: 16px;
    flex-shrink: 0;
    filter: brightness(0) invert(1);
    opacity: 0.8;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

.copy-button:hover .copy-icon {
    opacity: 1;
}

.copy-button.is-success .copy-icon {
    filter: none;
    opacity: 1;
    transform: scale(1.1);
}

.copy-button.is-recovering .copy-icon {
    filter: brightness(0) invert(1);
    opacity: 0.8;
    transform: scale(0.8);
}

[data-theme="dark"] .copy-icon {
    filter: brightness(0) invert(1);
}
</style>
