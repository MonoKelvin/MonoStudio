<template>
    <div class="settings-section">
        <h3>个性化</h3>
        <p>调整主题与外观风格。</p>
        <div class="settings-group">
            <SettingsItem title="主题模式" description="选择浅色、深色或跟随系统">
                <BaseSelect :model-value="themeMode" :options="themeOptions"
                    @update:modelValue="$emit('change-theme', $event)" />
            </SettingsItem>
            <SettingsItem title="主题色" description="选择软件的主要强调色">
                <ColorPickerSelect :model-value="currentAccentColor" :colors="themeColors"
                    @update:modelValue="$emit('change-accent-color', $event)" />
            </SettingsItem>
        </div>
    </div>
</template>

<script>
import SettingsItem from '../../../../../components/settings/SettingsItem.vue';
import ColorPickerSelect from '../../../../../components/settings/ColorPickerSelect.vue';
import BaseSelect from '../../../../../components/base/BaseSelect.vue';

export default {
    name: 'PersonalizationSettings',
    components: {
        SettingsItem,
        ColorPickerSelect,
        BaseSelect
    },
    props: {
        themeMode: {
            type: String,
            default: 'system'
        },
        themeOptions: {
            type: Array,
            default: () => []
        },
        currentAccentColor: {
            type: String,
            default: '#8b5cf6'
        }
    },
    emits: ['change-theme', 'change-accent-color'],
    data() {
        return {
            themeColors: [
                { label: '宝石蓝', value: '#3b82f6' },
                { label: '紫水晶', value: '#8b5cf6' },
                { label: '玫瑰红', value: '#ec4899' },
                { label: '翡翠绿', value: '#10b981' },
                { label: '琥珀橙', value: '#f59e0b' },
                { label: '玛瑙红', value: '#ef4444' },
                { label: '青金石', value: '#06b6d4' },
                { label: '柠檬黄', value: '#facc15' }
            ]
        };
    }
};
</script>
