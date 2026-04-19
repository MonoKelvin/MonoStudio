<template>
  <BasePanel class="panel guid-panel">
    <section class="guid-toolbar">
      <div class="guid-options">
        <BaseCheckbox v-model="options.uppercase">大写</BaseCheckbox>
        <BaseCheckbox v-model="options.hyphens">连字符</BaseCheckbox>
        <BaseCheckbox v-model="options.braces">花括号</BaseCheckbox>
      </div>
      <div class="guid-batch">
        <label>批量数量</label>
        <BaseSpinbox v-model="batchCount" :min="1" :max="50" />
        <BaseButton @click="generateBatch">批量生成</BaseButton>
      </div>
    </section>
    <section class="result-card guid-result">
      <div class="guid-result-head">
        <small class="guid-result-label">当前 GUID</small>
        <div class="guid-result-head-actions">
          <BaseButton variant="primary" @click="generateGuid">生成新 GUID</BaseButton>
        </div>
      </div>
      <div class="guid-result-main guid-result-code-wrap">
        <code class="guid-result-code">{{ generatedGuid }}</code>
        <BaseButton
          class="guid-copy-icon-btn"
          :class="{ 'is-success': copySuccess, 'is-recovering': copyRecovering }"
          size="sm"
          v-tooltip:top="'复制当前 GUID'"
          @click="copyToClipboard(generatedGuid)"
        >
          <img :src="copySuccess ? successIcon : copyIcon" alt="" aria-hidden="true" />
        </BaseButton>
      </div>
    </section>
    <section class="history guid-history">
      <div class="history-head">
        <div class="guid-history-title-wrap">
          <h3>历史记录</h3>
          <small class="guid-history-count">{{ guidList.length }}</small>
        </div>
        <BaseButton class="guid-clear-icon-btn" size="sm" v-tooltip:top="'清空历史'" @click="guidList = []">
          <img :src="clearIcon" alt="" aria-hidden="true" />
        </BaseButton>
      </div>
      <ul>
        <li v-for="(guid, index) in guidList" :key="getHistoryKey(guid, index)">
          <code>{{ guid }}</code>
          <BaseButton
            class="guid-copy-icon-btn guid-history-copy-btn"
            :class="{ 'is-success': isHistoryCopySuccess(guid, index), 'is-recovering': isHistoryCopyRecovering(guid, index) }"
            size="sm"
            v-tooltip:top="'复制该项 GUID'"
            @click="copyHistoryItem(guid, index)"
          >
            <img :src="isHistoryCopySuccess(guid, index) ? successIcon : copyIcon" alt="" aria-hidden="true" />
          </BaseButton>
        </li>
      </ul>
    </section>
  </BasePanel>
</template>

<script>
import copyIcon from '../../../assets/icons/copy.svg';
import successIcon from '../../../assets/icons/check-success.svg';
import clearIcon from '../../../assets/icons/delete.svg';

export default {
  name: 'GuidGeneratorToolView',
  data() {
    return {
      generatedGuid: '',
      guidList: [],
      copyIcon,
      successIcon,
      clearIcon,
      copySuccess: false,
      copyRecovering: false,
      copySuccessTimer: null,
      historyCopyStates: {},
      historyCopyTimers: {},
      batchCount: 5,
      options: { uppercase: false, hyphens: true, braces: false }
    };
  },
  beforeUnmount() {
    if (this.copySuccessTimer) {
      clearTimeout(this.copySuccessTimer);
      this.copySuccessTimer = null;
    }
    Object.values(this.historyCopyTimers).forEach((timerId) => {
      clearTimeout(timerId);
    });
    this.historyCopyTimers = {};
  },
  mounted() { this.generateGuid(); },
  methods: {
    createUuidV4() {
      const bytes = crypto.getRandomValues(new Uint8Array(16));
      bytes[6] = (bytes[6] & 0x0f) | 0x40;
      bytes[8] = (bytes[8] & 0x3f) | 0x80;
      const hex = [...bytes].map((b) => b.toString(16).padStart(2, '0')).join('');
      return `${hex.slice(0, 8)}-${hex.slice(8, 12)}-${hex.slice(12, 16)}-${hex.slice(16, 20)}-${hex.slice(20)}`;
    },
    formatGuid(rawGuid) {
      let guid = rawGuid;
      if (!this.options.hyphens) guid = guid.replaceAll('-', '');
      if (this.options.uppercase) guid = guid.toUpperCase();
      if (this.options.braces) guid = `{${guid}}`;
      return guid;
    },
    generateGuid() {
      this.generatedGuid = this.formatGuid(this.createUuidV4());
      this.guidList.unshift(this.generatedGuid);
    },
    generateBatch() {
      const count = Math.min(Math.max(Number(this.batchCount) || 1, 1), 50);
      for (let i = 0; i < count; i += 1) this.generateGuid();
    },
    async copyToClipboard(guid) {
      try {
        await navigator.clipboard.writeText(guid);
        this.copySuccess = false;
        this.copyRecovering = false;
        requestAnimationFrame(() => {
          this.copySuccess = true;
        });
        if (this.copySuccessTimer) clearTimeout(this.copySuccessTimer);
        this.copySuccessTimer = setTimeout(() => {
          this.copySuccess = false;
          this.copyRecovering = true;
          this.copySuccessTimer = setTimeout(() => {
            this.copyRecovering = false;
            this.copySuccessTimer = null;
          }, 240);
        }, 1500);
      } catch {
        // ignore clipboard errors to keep panel clean
      }
    },
    getHistoryKey(guid, index) {
      return `${guid}-${index}`;
    },
    isHistoryCopySuccess(guid, index) {
      return this.historyCopyStates[this.getHistoryKey(guid, index)]?.success === true;
    },
    isHistoryCopyRecovering(guid, index) {
      return this.historyCopyStates[this.getHistoryKey(guid, index)]?.recovering === true;
    },
    async copyHistoryItem(guid, index) {
      const itemKey = this.getHistoryKey(guid, index);
      try {
        await navigator.clipboard.writeText(guid);
        this.historyCopyStates[itemKey] = { success: false, recovering: false };
        requestAnimationFrame(() => {
          this.historyCopyStates[itemKey] = { success: true, recovering: false };
        });
        if (this.historyCopyTimers[itemKey]) clearTimeout(this.historyCopyTimers[itemKey]);
        this.historyCopyTimers[itemKey] = setTimeout(() => {
          this.historyCopyStates[itemKey] = { success: false, recovering: true };
          this.historyCopyTimers[itemKey] = setTimeout(() => {
            this.historyCopyStates[itemKey] = { success: false, recovering: false };
            delete this.historyCopyTimers[itemKey];
          }, 240);
        }, 1500);
      } catch {
        // ignore clipboard errors
      }
    }
  }
};
</script>
