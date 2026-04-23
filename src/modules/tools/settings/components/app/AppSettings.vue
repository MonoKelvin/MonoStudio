<template>
    <div class="settings-section">
        <h3>应用行为</h3>
        <p>控制应用的基本行为和功能</p>
        <div class="settings-group">
            <SettingsItem title="开发者工具" description="控制开发者工具的显示">
                <BaseSwitch :checked="localDevToolsOpen" @change="onDevToolsChange" />
            </SettingsItem>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../../components/base/BaseButton.vue';
import BaseSwitch from '../../../../../components/base/BaseSwitch.vue';
import SettingsItem from '../../../../../components/settings/SettingsItem.vue';

export default {
    name: 'AppSettings',
    components: {
        BaseButton,
        BaseSwitch,
        SettingsItem
    },
    props: {
        openDevTools: {
            type: Boolean,
            default: false
        }
    },
    emits: ['change-dev-tools'],
    data() {
        return {
            localDevToolsOpen: false
        };
    },
    watch: {
        openDevTools: {
            immediate: true,
            handler(newVal) {
                this.localDevToolsOpen = newVal;
            }
        }
    },
    methods: {
        onDevToolsChange(newVal) {
            this.localDevToolsOpen = newVal;
            this.$emit('change-dev-tools', newVal);
        }
    }
};
</script>

<style scoped>
.settings-group {
    margin-bottom: 32px;
}

.settings-control-group {
    display: flex;
    align-items: center;
    gap: 12px;
    width: 100%;
    justify-content: space-between;
}

.settings-control-sub {
    flex: 1;
    max-width: 150px;
    margin-left: auto;
}
</style>
