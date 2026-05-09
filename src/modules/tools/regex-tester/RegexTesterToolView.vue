<template>
    <BasePanel class="regex-tester-panel">
        <div class="regex-tester-container">
            <div class="regex-input-section">
                <div class="section-header">
                    <label class="section-label">正则表达式</label>
                    <div class="regex-flags">
                        <BaseCheckbox v-model="flags.g" @change="debouncedTest">全局</BaseCheckbox>
                        <BaseCheckbox v-model="flags.i" @change="debouncedTest">忽略大小写</BaseCheckbox>
                        <BaseCheckbox v-model="flags.m" @change="debouncedTest">多行</BaseCheckbox>
                        <BaseCheckbox v-model="flags.s" @change="debouncedTest">跨行匹配</BaseCheckbox>
                    </div>
                </div>
                <div class="regex-input-wrapper" :class="{ 'has-error': regexError }">
                    <span class="regex-delimiter">/</span>
                    <input ref="regexInputRef" v-model="regexPattern" type="text" class="regex-input"
                        placeholder="输入正则表达式..." spellcheck="false" @input="onRegexInput" />
                    <span class="regex-delimiter">/</span>
                    <span class="regex-flags-display">{{ activeFlags }}</span>
                </div>
                <div v-if="regexError" class="error-message">
                    <BaseSvgIcon :icon="warningIcon" class="error-icon" />
                    {{ regexError }}
                </div>
            </div>

            <div class="test-input-section">
                <div class="section-header">
                    <label class="section-label">测试文本</label>
                    <span class="char-count">{{ testText.length }} 字符</span>
                </div>
                <div class="test-text-wrapper">
                    <div class="highlight-layer" v-html="highlightedText" ref="highlightRef"></div>
                    <textarea ref="textareaRef" v-model="testText" class="test-textarea" placeholder="输入要测试的文本..."
                        spellcheck="false" @input="onTextInput" @scroll="syncScroll"></textarea>
                </div>
            </div>

            <div class="results-section">
                <div class="section-header">
                    <label class="section-label">匹配结果</label>
                    <span class="match-summary" v-if="matches.length > 0">
                        {{ matches.length }} 个匹配
                        <span class="char-summary">({{ totalMatchChars }} 字符)</span>
                    </span>
                </div>

                <div v-if="regexError" class="results-empty">
                    <p>请修正正则表达式错误</p>
                </div>
                <div v-else-if="!regexPattern" class="results-empty">
                    <p>输入正则表达式开始匹配</p>
                </div>
                <div v-else-if="matches.length === 0 && testText" class="results-empty">
                    <p>没有找到匹配项</p>
                </div>
                <div v-else-if="matches.length > 0" class="results-list">
                    <div v-for="(match, index) in matches" :key="index" class="result-item"
                        :class="{ 'selected': selectedMatchIndex === index }" @click="selectMatch(index)">
                        <div class="result-meta">
                            <span class="match-index">匹配 {{ index + 1 }}</span>
                            <span class="match-position">[{{ match.start }}-{{ match.end }}]</span>
                        </div>
                        <div class="match-content">{{ match.text || '(空字符串)' }}</div>
                        <div v-if="match.groups.length > 0" class="match-groups">
                            <div class="group-item" v-for="(group, gIndex) in match.groups" :key="gIndex">
                                <span class="group-label">组 {{ gIndex + 1 }}:</span>
                                <span class="group-value">{{ group.text || '(未匹配)' }}</span>
                                <span v-if="group.name" class="group-name">({{ group.name }})</span>
                            </div>
                        </div>
                        <div class="result-actions">
                            <BaseCopyButton :text-to-copy="match.text" button-text="复制" success-text="已复制"
                                @copy-success="onCopySuccess" />
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </BasePanel>
</template>

<script>
import { ref, computed, watch } from 'vue'
import BasePanel from '../../../components/base/BasePanel.vue'
import BaseCheckbox from '../../../components/base/BaseCheckbox.vue'
import BaseCopyButton from '../../../components/base/BaseCopyButton.vue'
import BaseSvgIcon from '../../../components/base/BaseSvgIcon.vue'
import { toastService } from '../../../common/services/toastService'

import warningIcon from '../../../assets/icons/warning.svg?raw'

export default {
    name: 'RegexTesterToolView',
    components: {
        BasePanel,
        BaseCheckbox,
        BaseCopyButton,
        BaseSvgIcon
    },
    props: {
        toolName: {
            type: String,
            default: '正则表达式测试'
        }
    },
    setup() {
        const regexInputRef = ref(null)
        const textareaRef = ref(null)
        const highlightRef = ref(null)
        const regexPattern = ref('')
        const testText = ref('')
        const flags = ref({
            g: true,
            i: false,
            m: false,
            s: false
        })
        const regexError = ref('')
        const selectedMatchIndex = ref(-1)
        const debounceTimer = ref(null)

        const activeFlags = computed(() => {
            return Object.entries(flags.value)
                .filter(([_, v]) => v)
                .map(([k]) => k)
                .join('')
        })

        const allMatchesText = computed(() => {
            return matches.value.map(m => m.text).join('\n')
        })

        const buildRegex = () => {
            if (!regexPattern.value) {
                regexError.value = ''
                return null
            }
            try {
                const regex = new RegExp(regexPattern.value, activeFlags.value)
                regexError.value = ''
                return regex
            } catch (error) {
                regexError.value = error.message
                return null
            }
        }

        const matches = computed(() => {
            const regex = buildRegex()
            if (!regex || !testText.value) {
                return []
            }

            const results = []
            let match

            if (flags.value.g) {
                while ((match = regex.exec(testText.value)) !== null) {
                    results.push({
                        text: match[0],
                        start: match.index,
                        end: match.index + match[0].length,
                        groups: match.slice(1).map((g, i) => ({
                            text: g,
                            index: i + 1,
                            name: match.groups ? Object.keys(match.groups)[i] : null
                        }))
                    })
                    if (match[0].length === 0) {
                        regex.lastIndex++
                    }
                }
            } else {
                match = regex.exec(testText.value)
                if (match) {
                    results.push({
                        text: match[0],
                        start: match.index,
                        end: match.index + match[0].length,
                        groups: match.slice(1).map((g, i) => ({
                            text: g,
                            index: i + 1,
                            name: match.groups ? Object.keys(match.groups)[i] : null
                        }))
                    })
                }
            }

            return results
        })

        const totalMatchChars = computed(() => {
            return matches.value.reduce((sum, m) => sum + m.text.length, 0)
        })

        const escapeHtml = (text) => {
            return text
                .replace(/&/g, '&amp;')
                .replace(/</g, '&lt;')
                .replace(/>/g, '&gt;')
                .replace(/"/g, '&quot;')
                .replace(/'/g, '&#039;')
                .replace(/\n/g, '<br>')
                .replace(/ /g, '&nbsp;')
        }

        const highlightedText = computed(() => {
            if (!testText.value || matches.value.length === 0) {
                return escapeHtml(testText.value)
            }

            let result = ''
            let lastIndex = 0

            for (const match of matches.value) {
                result += escapeHtml(testText.value.slice(lastIndex, match.start))
                result += `<mark class="highlight">${escapeHtml(match.text)}</mark>`
                lastIndex = match.end
            }
            result += escapeHtml(testText.value.slice(lastIndex))

            return result
        })

        const onRegexInput = () => {
            debouncedTest()
        }

        const onTextInput = () => {
            debouncedTest()
        }

        const debouncedTest = () => {
            if (debounceTimer.value) {
                clearTimeout(debounceTimer.value)
            }
            debounceTimer.value = setTimeout(() => {
                selectedMatchIndex.value = -1
            }, 150)
        }

        const syncScroll = () => {
            if (textareaRef.value && highlightRef.value) {
                highlightRef.value.scrollTop = textareaRef.value.scrollTop
                highlightRef.value.scrollLeft = textareaRef.value.scrollLeft
            }
        }

        const selectMatch = (index) => {
            selectedMatchIndex.value = index
        }

        const onCopySuccess = () => {
            toastService.success('已复制到剪贴板', '复制成功')
        }

        watch(testText, () => {
            selectedMatchIndex.value = -1
        })

        return {
            regexInputRef,
            textareaRef,
            highlightRef,
            regexPattern,
            testText,
            flags,
            regexError,
            matches,
            totalMatchChars,
            highlightedText,
            activeFlags,
            allMatchesText,
            selectedMatchIndex,
            onRegexInput,
            onTextInput,
            debouncedTest,
            syncScroll,
            selectMatch,
            onCopySuccess,
            warningIcon
        }
    }
}
</script>

<style scoped>
.regex-tester-panel {
    height: 100%;
    overflow: hidden;
}

.regex-tester-container {
    height: 100%;
    display: flex;
    flex-direction: column;
    padding: 20px;
    gap: 18px;
    overflow: hidden;
}

.regex-input-section,
.test-input-section,
.results-section {
    display: flex;
    flex-direction: column;
    gap: 10px;
}

.section-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 0 2px;
}

.section-label {
    font-size: 14px;
    font-weight: 500;
    color: var(--text-secondary);
}

.char-count {
    font-size: 12px;
    color: var(--text-tertiary);
}

.regex-flags {
    display: flex;
    gap: 16px;
    flex-wrap: wrap;
}

.regex-input-wrapper {
    display: flex;
    align-items: center;
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    padding: 0 12px;
    transition: border-color 0.2s;
}

.regex-input-wrapper:focus-within {
    border-color: var(--accent-color);
}

.regex-input-wrapper.has-error {
    border-color: var(--color-error, #ef4444);
}

.regex-delimiter {
    color: var(--text-tertiary);
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 14px;
    flex-shrink: 0;
}

.regex-input {
    flex: 1;
    border: none;
    outline: none;
    background: transparent;
    padding: 12px 8px;
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 14px;
    color: var(--text-primary);
    min-width: 0;
}

.regex-input::placeholder {
    color: var(--text-tertiary);
}

.regex-flags-display {
    color: var(--accent-color);
    font-family: monospace;
    font-size: 13px;
    min-width: 20px;
    flex-shrink: 0;
}

.error-message {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 12px;
    color: var(--color-error, #ef4444);
    padding: 8px 12px;
    background: color-mix(in srgb, var(--color-error, #ef4444) 10%, transparent);
    border-radius: 6px;
}

.error-icon {
    width: 14px;
    height: 14px;
    flex-shrink: 0;
}

.test-text-wrapper {
    position: relative;
    flex: 1;
    min-height: 93px;
    border: 1px solid var(--border-color);
    border-radius: 8px;
    overflow: hidden;
}

.highlight-layer {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    padding: 12px;
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 14px;
    line-height: 1.6;
    color: transparent;
    white-space: pre-wrap;
    word-break: break-word;
    overflow: hidden;
    pointer-events: none;
}

.highlight-layer :deep(.highlight) {
    background: color-mix(in srgb, var(--accent-color) 30%, transparent);
    border-radius: 3px;
    color: transparent;
}

.test-textarea {
    position: relative;
    width: 100%;
    height: 100%;
    min-height: 93px;
    padding: 12px;
    border: none;
    background: transparent;
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 14px;
    line-height: 1.6;
    color: var(--text-primary);
    resize: none;
    outline: none;
    background: var(--bg-secondary);
}

.test-textarea::placeholder {
    color: var(--text-tertiary);
}

.results-section {
    flex: 1;
    min-height: 0;
    overflow: hidden;
}

.match-summary {
    font-size: 13px;
    color: var(--text-secondary);
}

.char-summary {
    color: var(--text-tertiary);
    margin-left: 4px;
}

.results-empty {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    background: var(--bg-secondary);
    border-radius: 8px;
    color: var(--text-tertiary);
    font-size: 14px;
    min-height: 80px;
}

.results-list {
    flex: 1;
    overflow-y: auto;
    display: flex;
    flex-direction: column;
    gap: 10px;
}

.result-item {
    padding: 14px;
    background: var(--bg-secondary);
    border-radius: 10px;
    border: 1px solid transparent;
    cursor: pointer;
    transition: all 0.2s;
}

.result-item:hover {
    border-color: var(--border-color);
}

.result-item.selected {
    border-color: var(--accent-color);
    background: color-mix(in srgb, var(--accent-color) 5%, var(--bg-secondary));
}

.result-meta {
    display: flex;
    align-items: center;
    gap: 10px;
    margin-bottom: 10px;
}

.match-index {
    font-size: 13px;
    font-weight: 500;
    color: var(--text-primary);
}

.match-position {
    font-size: 11px;
    color: var(--text-tertiary);
    font-family: monospace;
}

.match-content {
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 13px;
    color: var(--text-primary);
    word-break: break-all;
    padding: 10px 12px;
    background: var(--bg-primary);
    border-radius: 6px;
    margin-bottom: 8px;
}

.match-groups {
    padding-top: 10px;
    border-top: 1px solid var(--border-color);
    display: flex;
    flex-direction: column;
    gap: 6px;
}

.group-item {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 12px;
}

.group-label {
    color: var(--text-tertiary);
}

.group-value {
    font-family: monospace;
    color: var(--text-secondary);
    padding: 2px 8px;
    background: var(--bg-primary);
    border-radius: 4px;
}

.group-name {
    color: var(--accent-color);
    font-size: 11px;
}

.result-actions {
    display: flex;
    justify-content: flex-end;
    margin-top: 10px;
}

@media (max-width: 600px) {
    .regex-tester-container {
        padding: 16px;
        gap: 16px;
    }

    .regex-flags {
        gap: 12px;
    }
}
</style>
