<template>
    <div class="gojuon-card" @dblclick="$emit('dblclick', char)">
        <div class="card-content">
            <div class="kana-section">
                <div class="main-kana" :class="activeKana">
                    {{ activeKana === 'hiragana' ? char.hiragana : char.katakana }}
                </div>
                <div class="secondary-kana" :class="activeKana === 'hiragana' ? 'katakana' : 'hiragana'">
                    {{ activeKana === 'hiragana' ? char.katakana : char.hiragana }}
                </div>
            </div>
            <div class="romaji">{{ char.romaji }}</div>
        </div>
    </div>
</template>

<script setup>
const props = defineProps({
    char: {
        type: Object,
        required: true
    },
    activeKana: {
        type: String,
        default: 'hiragana'
    }
});

defineEmits(['dblclick']);
</script>

<style scoped>
.gojuon-card {
    background: transparent;
    border-radius: var(--radius-lg);
    padding: var(--spacing-xs);
    transition: all var(--transition-fast);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-sm);
    min-width: 0;
    min-height: 80px;
    border: 1px solid transparent;
    position: relative;
    cursor: pointer;
}

.gojuon-card:hover {
    transform: translateY(-2px);
    border-color: var(--accent-primary);
    background: var(--bg-soft);
}

.card-content {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 12px;
    max-width: 100%;
}

.kana-section {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 2px;
    padding: 2px;
    transition: all var(--transition-fast);
}

.main-kana {
    font-size: 20px;
    font-weight: 700;
    line-height: 1;
    transition: all var(--transition-fast);
    color: var(--text-primary);
}

.secondary-kana {
    font-size: 14px;
    font-weight: 500;
    opacity: 0.6;
    transition: all var(--transition-fast);
    color: var(--text-primary);
}

.romaji {
    font-size: 16px;
    font-weight: 500;
    color: var(--text-primary);
    letter-spacing: 0.5px;
    transition: all var(--transition-fast);
    min-width: 28px;
    text-align: center;
}

@media (max-width: 768px) {
    .gojuon-card {
        flex-direction: column;
        text-align: center;
        min-width: 100px;
        min-height: 70px;
    }

    .card-content {
        flex-direction: column;
        gap: 4px;
    }

    .main-kana {
        font-size: 24px;
    }

    .romaji {
        font-size: 14px;
    }
}
</style>
