<template>
    <div v-if="visible" class="privacy-panel-overlay" @click="close">
        <div class="privacy-panel" @click.stop>
            <div class="privacy-panel-header">
                <h4>{{ isZh ? '隐私政策' : 'Privacy Policy' }}</h4>
                <button class="close-btn" @click="close">×</button>
            </div>
            <div class="privacy-panel-body" v-html="currentPrivacyPolicy"></div>
        </div>
    </div>
</template>

<script>
import privacyPolicyZhHTML from '../../../../../assets/files/privacy_policy_zh.html?raw';
import privacyPolicyEnHTML from '../../../../../assets/files/privacy_policy_en.html?raw';

export default {
    name: 'PrivacyPanel',
    props: {
        visible: {
            type: Boolean,
            default: false
        }
    },
    emits: ['update:visible'],
    data() {
        return {
            privacyPolicyZhHTML: privacyPolicyZhHTML,
            privacyPolicyEnHTML: privacyPolicyEnHTML
        };
    },
    computed: {
        isZh() {
            const lang = navigator.language || navigator.userLanguage || 'en';
            return lang.toLowerCase().startsWith('zh');
        },
        currentPrivacyPolicy() {
            return this.isZh ? this.privacyPolicyZhHTML : this.privacyPolicyEnHTML;
        }
    },
    methods: {
        close() {
            this.$emit('update:visible', false);
        }
    }
};
</script>
