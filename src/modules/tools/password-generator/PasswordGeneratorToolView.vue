<template>
    <BasePanel class="panel password-generator-panel">
        <div class="password-generator-container">
            <div class="password-result">
                <div class="result-header">
                    <label>生成的密码</label>
                    <BaseButton variant="primary" @click="generatePassword">生成密码</BaseButton>
                </div>
                <div class="password-display">
                    <code class="password-text">{{ generatedPassword }}</code>
                    <BaseCopyButton :text-to-copy="generatedPassword" class="copy-button" size="sm" :disabled="!generatedPassword" :button-text="''" :success-text="''" />

                </div>
                <div class="password-strength">
                    <span>密码强度: </span>
                    <span class="strength-text" :class="'strength-' + passwordStrength">
                        {{ passwordStrengthText }}
                    </span>
                </div>
            </div>

            <div class="generator-options">
                <div class="settings-group">
                    <div class="settings-group-header" @click="toggleBasicSettings" :aria-expanded="basicSettingsOpen">
                        <label>基础设置</label>
                        <span class="toggle-icon">▶</span>
                    </div>

                    <div v-if="basicSettingsOpen" class="settings-group-content">
                        <div class="settings-group-horizontal">
                            <label>密码长度</label>
                            <BaseSpinbox v-model="passwordLength" :min="4" :max="64" />
                        </div>

                        <div class="option-group">
                            <label>字符类型</label>
                            <div class="checkbox-group checkbox-group-flow">
                                <BaseCheckbox v-model="options.uppercase">大写字母 (A-Z)</BaseCheckbox>
                                <BaseCheckbox v-model="options.lowercase">小写字母 (a-z)</BaseCheckbox>
                                <BaseCheckbox v-model="options.numbers">数字 (0-9)</BaseCheckbox>
                                <BaseCheckbox v-model="options.symbols">特殊字符 (!@#$%^&*)</BaseCheckbox>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="settings-group advanced-settings">
                    <div class="settings-group-header" @click="toggleAdvancedSettings"
                        :aria-expanded="advancedSettingsOpen">
                        <label>高级设置</label>
                        <span class="toggle-icon">▶</span>
                    </div>

                    <div v-if="advancedSettingsOpen" class="settings-group-content">
                        <div class="option-group">
                            <label>密码模式</label>
                            <div class="radio-group">
                                <BaseCheckbox type="radio" name="passwordMode" v-model="options.mode" :value="'random'">
                                    随机
                                </BaseCheckbox>
                                <BaseTooltip placement="top" :delay="0">
                                    <template #default>
                                        <BaseCheckbox type="radio" name="passwordMode" v-model="options.mode"
                                            :value="'memorable'">
                                            易记
                                        </BaseCheckbox>
                                    </template>
                                    <template #content>
                                        易记模式使用随机英文单词组合，便于记忆和输入。例如：apple-battery-horse
                                    </template>
                                </BaseTooltip>
                            </div>
                        </div>

                        <div class="option-group">
                            <label>密码规则</label>
                            <div class="checkbox-group checkbox-group-left">
                                <BaseCheckbox v-model="options.excludeSimilar">排除相似字符 (如 1, l, I, 0, O)</BaseCheckbox>
                                <BaseCheckbox v-model="options.avoidConsecutive">避免连续字符 (如 AAA, 111)</BaseCheckbox>
                            </div>
                        </div>

                        <div class="option-group">
                            <label>特殊字符集</label>
                            <BaseInput v-model="options.customSymbols" placeholder="输入自定义特殊字符" />
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </BasePanel>
    <BasePromptDialog v-model:visible="dialogVisible" :type="dialogType" :title="dialogTitle" :message="dialogMessage"
        :confirm-text="dialogConfirmText" :cancel-text="dialogCancelText" :show-cancel="dialogShowCancel"
        :close-on-overlay="dialogCloseOnOverlay" :enable-sound="dialogEnableSound" @confirm="handleDialogConfirm"
        @cancel="handleDialogCancel" />
</template>

<script>
import { ref, computed, onMounted } from 'vue';
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseCopyButton from '../../../components/base/BaseCopyButton.vue';
import BaseCheckbox from '../../../components/base/BaseCheckbox.vue';
import BaseSpinbox from '../../../components/base/BaseSpinbox.vue';
import BasePromptDialog from '../../../components/base/BasePromptDialog.vue';
import BaseTooltip from '../../../components/base/BaseTooltip.vue';
import { useDialog } from '../../../composables/useDialog.js';

export default {
    name: 'PasswordGeneratorToolView',
    emits: ['change-theme', 'change-accent-color', 'change-dev-tools', 'unlocked', 'locked'],
    components: {
        BaseInput,
        BaseButton,
        BaseCopyButton,
        BaseCheckbox,
        BaseSpinbox,
        BasePromptDialog,
        BaseTooltip
    },
    setup() {
        const dialog = useDialog();

        const generatedPassword = ref('');
        const passwordLength = ref(12);
        const basicSettingsOpen = ref(true);
        const advancedSettingsOpen = ref(true);
        const options = ref({
            uppercase: true,
            lowercase: true,
            numbers: true,
            symbols: true,
            mode: 'random',
            excludeSimilar: false,
            avoidConsecutive: false,
            customSymbols: ''
        });

        const passwordStrength = computed(() => {
            if (!generatedPassword.value) return 'weak';
            const length = generatedPassword.value.length;
            let strength = 0;

            if (length >= 8) strength += 1;
            if (length >= 12) strength += 1;
            if (options.value.uppercase && /[A-Z]/.test(generatedPassword.value)) strength += 1;
            if (options.value.lowercase && /[a-z]/.test(generatedPassword.value)) strength += 1;
            if (options.value.numbers && /[0-9]/.test(generatedPassword.value)) strength += 1;

            if (options.value.symbols) {
                let symbolSet = '!@#$%^&*(),.?":{}|<>';
                // 确保自定义特殊字符集被正确使用
                const customSymbols = options.value.customSymbols;
                if (customSymbols && customSymbols.trim()) {
                    symbolSet = customSymbols;
                }
                // 检查密码中是否包含自定义特殊字符
                const password = generatedPassword.value;
                const hasSymbol = symbolSet.split('').some(symbol => password.includes(symbol));
                if (hasSymbol) strength += 1;
            }

            if (strength >= 5) return 'strong';
            if (strength >= 3) return 'medium';
            return 'weak';
        });

        const passwordStrengthText = computed(() => {
            switch (passwordStrength.value) {
                case 'strong': return '强';
                case 'medium': return '中';
                case 'weak': return '弱';
                default: return '弱';
            }
        });

        const generatePassword = () => {
            if (options.value.mode === 'memorable') {
                generateMemorablePassword();
                return;
            }

            let symbolSet = '!@#$%^&*(),.?":{}|<>';
            // 确保自定义特殊字符集被正确使用
            const customSymbols = options.value.customSymbols;
            if (customSymbols && customSymbols.trim()) {
                symbolSet = customSymbols;
            }

            const charset = {
                uppercase: 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                lowercase: 'abcdefghijklmnopqrstuvwxyz',
                numbers: '0123456789',
                symbols: symbolSet
            };

            let availableChars = [];
            if (options.value.uppercase) availableChars = availableChars.concat([...charset.uppercase]);
            if (options.value.lowercase) availableChars = availableChars.concat([...charset.lowercase]);
            if (options.value.numbers) availableChars = availableChars.concat([...charset.numbers]);
            if (options.value.symbols) availableChars = availableChars.concat([...symbolSet]);

            if (options.value.excludeSimilar) {
                availableChars = availableChars.filter(char => !/[1lI0O]/.test(char));
            }

            if (!availableChars.length) {
                dialog.alert('请至少选择一种字符类型', '提示', 'warning').catch(() => { });
                return;
            }

            let password = '';
            let lastChar = '';
            for (let i = 0; i < passwordLength.value; i++) {
                let randomIndex = Math.floor(Math.random() * availableChars.length);
                let currentChar = availableChars[randomIndex];

                if (options.value.avoidConsecutive && currentChar === lastChar) {
                    randomIndex = (randomIndex + 1) % availableChars.length;
                    currentChar = availableChars[randomIndex];
                }

                password += currentChar;
                lastChar = currentChar;
            }

            generatedPassword.value = password;
        };

        const generateMemorablePassword = () => {
            const words = [
                'apple', 'banana', 'cherry', 'date', 'elderberry', 'fig', 'grape', 'honeydew',
                'kiwi', 'lemon', 'mango', 'nectarine', 'orange', 'peach', 'quince', 'raspberry',
                'strawberry', 'tangerine', 'watermelon', 'apricot', 'blueberry', 'cantaloupe',
                'dragonfruit', 'eggplant', 'guava', 'papaya', 'pineapple', 'starfruit'
            ];

            const numbers = '0123456789';
            let symbolSet = '!@#$%^&*';
            // 确保自定义特殊字符集被正确使用
            const customSymbols = options.value.customSymbols;
            if (customSymbols && customSymbols.trim()) {
                symbolSet = customSymbols;
            }

            // 检查是否至少选择了一种字符类型
            const hasAnyType = options.value.uppercase || options.value.lowercase || options.value.numbers || options.value.symbols;
            if (!hasAnyType) {
                dialog.alert('请至少选择一种字符类型', '提示', 'warning').catch(() => { });
                return;
            }

            let password = '';
            const wordCount = Math.ceil(passwordLength.value / 4);

            for (let i = 0; i < wordCount; i++) {
                const randomWord = words[Math.floor(Math.random() * words.length)];
                password += randomWord.charAt(0).toUpperCase() + randomWord.slice(1);
            }

            if (options.value.numbers) {
                const numbersArray = [...numbers];
                const randomNumber = numbersArray[Math.floor(Math.random() * numbersArray.length)];
                password = password.slice(0, -1) + randomNumber + password.slice(-1);
            }

            if (options.value.symbols) {
                const symbolsArray = [...symbolSet];
                const randomSymbol = symbolsArray[Math.floor(Math.random() * symbolsArray.length)];
                password = password.slice(0, 1) + randomSymbol + password.slice(1);
            }

            generatedPassword.value = password.slice(0, passwordLength.value);
        };



        const toggleBasicSettings = () => {
            basicSettingsOpen.value = !basicSettingsOpen.value;
        };

        const toggleAdvancedSettings = () => {
            advancedSettingsOpen.value = !advancedSettingsOpen.value;
        };

        onMounted(() => {
            generatePassword();
        });

        return {
            ...dialog,
            generatedPassword,
            passwordLength,
            basicSettingsOpen,
            advancedSettingsOpen,
            options,
            passwordStrength,
            passwordStrengthText,
            generatePassword,
            generateMemorablePassword,
            toggleBasicSettings,
            toggleAdvancedSettings
        };
    }
};
</script>

<style scoped>
.password-generator-panel {
    height: 100%;
}

.password-generator-container {
    padding: 20px 28px 20px 20px;
    display: flex;
    flex-direction: column;
    gap: 24px;
}

.password-result {
    display: flex;
    flex-direction: column;
    gap: 12px;
    padding: 16px;
    background-color: var(--background-secondary);
    border-radius: 8px;
    border: 1px solid var(--border-color);
}

.result-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.result-header label {
    font-weight: 500;
    color: var(--text-primary);
    font-size: 14px;
}



.password-display {
    display: flex;
    align-items: center;
    gap: 12px;
    background: var(--background-tertiary);
    padding: 8px 16px;
    border-radius: 6px;
    border: 1px solid var(--border-color);
}

.password-text {
    flex: 1;
    font-family: 'Courier New', monospace;
    font-size: 16px;
    font-weight: 600;
    color: var(--text-primary);
    word-break: break-all;
    background: transparent;
    border: none;
    outline: none;
    padding: 0;
    margin: 0;
}

.copy-button {
    flex-shrink: 0;
    width: 32px;
    height: 32px;
    padding: 0;
    border: none;
    background: transparent;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    border-radius: 6px;
    transition: all 0.2s ease;
}

.copy-button:hover:not(:disabled) {
    background: var(--background-hover);
}

.copy-button:disabled {
    opacity: 0.5;
    cursor: not-allowed;
}

.copy-button img {
    width: 18px;
    height: 18px;
    filter: brightness(0) invert(1);
    opacity: 0.8;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

.copy-button:hover:not(:disabled) img {
    opacity: 1;
}

.copy-button.is-success img {
    filter: none;
    opacity: 1;
    transform: scale(1.1);
}

.copy-button.is-recovering img {
    filter: brightness(0) invert(1);
    opacity: 0.8;
    transform: scale(0.8);
}

.password-strength {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 13px;
    color: var(--text-secondary);
}

.strength-text {
    font-weight: 600;
}

.strength-text.strength-strong {
    color: var(--success);
}

.strength-text.strength-medium {
    color: var(--warning);
}

.strength-text.strength-weak {
    color: var(--danger);
}

.generator-options {
    display: flex;
    flex-direction: column;
    gap: 16px;
}

.settings-group {
    border: 1px solid var(--border-color);
    border-radius: 8px;
    overflow: hidden;
    background: var(--background-secondary);
}

.settings-group.advanced-settings {
    border-color: var(--border-color);
}

.settings-group-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 12px 16px;
    background: var(--background-tertiary);
    cursor: pointer;
    user-select: none;
    transition: background 0.2s ease;
}

.settings-group-header:hover {
    background: var(--background-hover);
}

.settings-group-header label {
    font-weight: 500;
    color: var(--text-primary);
    font-size: 14px;
    cursor: pointer;
}

.toggle-icon {
    font-size: 12px;
    color: var(--text-secondary);
    transition: transform 0.3s ease;
}

.settings-group-header[aria-expanded="true"] .toggle-icon {
    transform: rotate(90deg);
}

.settings-group-content {
    padding: 16px;
    display: flex;
    flex-direction: column;
    gap: 16px;
}

.settings-group-horizontal {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 12px;
}

.settings-group-horizontal label {
    font-weight: 500;
    color: var(--text-primary);
    font-size: 14px;
    flex-shrink: 0;
}

.option-group {
    display: flex;
    flex-direction: column;
    gap: 12px;
}

.option-group label {
    font-weight: 500;
    color: var(--text-primary);
    font-size: 14px;
}

.option-hint {
    font-size: 12px;
    color: var(--text-secondary);
    margin-top: 4px;
}

.checkbox-group {
    display: flex;
    flex-direction: column;
    gap: 8px;
}

.checkbox-group-flow {
    display: flex;
    flex-direction: row;
    flex-wrap: nowrap;
    gap: 16px;
    align-items: center;
    justify-content: space-between;
    width: 100%;
}

.checkbox-group-left {
    display: flex;
    flex-direction: row;
    flex-wrap: nowrap;
    gap: 16px;
    align-items: center;
    justify-content: flex-start;
}

.radio-group {
    display: flex;
    gap: 16px;
    justify-content: flex-start;
}
</style>
